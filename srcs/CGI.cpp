#include "CGI.hpp"

CGI::CGI(Request &req, ServerConfig &server) 
	: _req(req), _server(server), _statusCode(200), _timeout(30), _writtenBytes(0), _pid(-1), _readComplete(false) {
	_scriptPath = _req.getCompletPath();
	_pipeIn[0] = -1;
	_pipeIn[1] = -1;
	_pipeOut[0] = -1;
	_pipeOut[1] = -1;
}

CGI::~CGI() {}

void	CGI::addHTTPHeaders(std::vector<std::string> &env) {
	const std::map<std::string, std::string>	&headers = _req.getHttpHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++) {
		std::string	headerName = it->first;
		std::string	headerValue = it->second;
		if (headerName == "Content-Type" || headerName == "Content-Length")
			continue;
		if (headerName == "Transfer-Encoding")
			continue;

		std::string	envName = "HTTP_";
		for (size_t i = 0; i < headerName.size(); i++) {
			char	c = headerName[i];
			if (c == '-')
				envName += "_";
			else
				envName += std::toupper(c);
		}
		addEnv(env, envName, headerValue);
	}
}

void	CGI::parseOutput() {
	std::string	output(_output.begin(), _output.end());
	size_t		separatorPos = output.find("\r\n\r\n");
	size_t		separatorSize = 4;

	if (separatorPos == std::string::npos) {
		separatorPos = output.find("\n\n");
		separatorSize = 2;
	}
	if (separatorPos == std::string::npos) { 
		_body = output;
		return ;
	}

	std::string	headersSection = output.substr(0, separatorPos);
	size_t		bodyStart = separatorPos + separatorSize;
	if (bodyStart < output.size())
		_body = output.substr(bodyStart);

	std::istringstream	headerStream(headersSection);
	std::string			line;

	while (std::getline(headerStream, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line = line.substr(0, line.size() - 1);
		size_t	colonPos = line.find(':');
		if (colonPos == std::string::npos)
			continue ;

		std::string	headerName = line.substr(0, colonPos);
		std::string	headerValue = line.substr(colonPos + 1);

		size_t	valueStart = headerValue.find_first_not_of(" \t");
		if (valueStart != std::string::npos)
			headerValue = headerValue.substr(valueStart);
		else
			headerValue = "";

		size_t	valueEnd = headerValue.find_last_not_of(" \t\r");
		if (valueEnd != std::string::npos)
			headerValue = headerValue.substr(0, valueEnd + 1);
		else if (!headerValue.empty())
			headerValue = "";

		_cgiHeaders[headerName] = headerValue;
		if (headerName == "Status") {
			std::istringstream statusStream(headerValue);
			statusStream >> _statusCode;
		}
	}
}

void	CGI::executeScript(char **env) {
	std::string	interpreterPath = _server.cgi[_interpreter];
	std::string	scriptName = _scriptPath.substr(_scriptPath.find_last_of('/') + 1);
	char		**argv = new char*[3];
	argv[0] = new char[interpreterPath.length() + 1];
	argv[1] = new char[scriptName.length() + 1];

	std::strcpy(argv[0], interpreterPath.c_str());
	std::strcpy(argv[1], scriptName.c_str());
	argv[2] = NULL;

	execve(argv[0], argv, env);

	delete[] argv[0];
	delete[] argv[1];
	delete[] argv;
	exit(127);
}

bool	CGI::execute(const Request &req) {
	this->_interpreter = findInterpreter();
	if (this->_interpreter.empty()){
		return (false);
	}

	if (_server.cgi.find(this->_interpreter) == _server.cgi.end()){
		return (false);
	}
	std::string	interpreterPath = _server.cgi[_interpreter];
	if (access(interpreterPath.c_str(), X_OK) != 0){
		return (false);
	}
	if (open(_scriptPath.c_str(), O_RDONLY, O_EXCL) < 0){
		return (false);
	}

	char	**cgiEnv = setupEnv(req);
	if (processScript(cgiEnv)) {
		freeEnv(cgiEnv);
		parseOutput();
		return (true);
	}
	freeEnv(cgiEnv);

	return (false);
}

bool	CGI::processScript(char **env) {
	if (pipe(_pipeIn) < 0 || pipe(_pipeOut) < 0)
		return (false);

	fcntl(_pipeIn[1], F_SETFL, O_NONBLOCK);
	fcntl(_pipeOut[0], F_SETFL, O_NONBLOCK);

	pid_t	pid = fork();
	if (pid < 0)
		return (false);
	if (pid == 0) {
		close(_pipeIn[1]);
		close(_pipeOut[0]);

		std::string	scriptDir = _scriptPath.substr(0, _scriptPath.find_last_of('/'));
		if (!scriptDir.empty() && chdir(scriptDir.c_str()) != 0) {
			freePipes(_pipeIn, _pipeOut);
			exit(1);
		}
		if (dup2(_pipeIn[0], STDIN_FILENO) < 0) {
			freePipes(_pipeIn, _pipeOut);
			exit(false);
		}
		close(_pipeIn[0]);
		if (dup2(_pipeOut[1], STDOUT_FILENO) < 0) {
			freePipes(_pipeIn, _pipeOut);
			exit(false);
		}
		dup2(_pipeOut[1], STDERR_FILENO);
		close(_pipeOut[1]);
		executeScript(env);
	}
	close(_pipeIn[0]);
	close(_pipeOut[1]);
	_startTime = time(NULL);
	_readComplete = false;
	return (true);
}

int	CGI::getStatusCode() const {
	return (this->_statusCode);
}

std::vector<char>	CGI::getOutput() const {
	return (this->_output);
}

std::string	CGI::getBody() const {
	return (this->_body);
}

std::map<std::string, std::string>	CGI::getHeaders() const {
	return (this->_cgiHeaders);
}

void	CGI::appendOutput(const char* buffer, size_t size) {
	_output.insert(_output.end(), buffer, buffer + size);
}

pid_t	CGI::getPid() const {
	return (_pid);
}

void	CGI::finalizeCGI(int status) {
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0)
			_statusCode = 502;
	}
	else if (WIFSIGNALED(status)) {
		_statusCode = 502;
	}
	parseOutput();
}

int	CGI::getPipeOut() const {
	return (_pipeOut[0]);
}

int	CGI::getPipeIn() const {
	return (_pipeIn[1]);
}

size_t	CGI::getWrittenBytes() const {
	return (_writtenBytes);
}

void	CGI::addWrittenBytes(size_t bytes) {
	_writtenBytes += bytes;
}

bool	CGI::executeAsync(const Request &req) {
	this->_interpreter = findInterpreter();
	if (this->_interpreter.empty())
		return (false);

	if (_server.cgi.find(this->_interpreter) == _server.cgi.end())
		return (false);
	
	std::string	interpreterPath = _server.cgi[_interpreter];
	if (access(interpreterPath.c_str(), X_OK) != 0)
		return (false);
	
	int	scriptFd = open(_scriptPath.c_str(), O_RDONLY);
	if (scriptFd < 0)
		return (false);
	close(scriptFd);

	char	**cgiEnv = setupEnv(req);
	
	if (pipe(_pipeIn) < 0) {
		freeEnv(cgiEnv);
		return (false);
	}
	if (pipe(_pipeOut) < 0) {
		close(_pipeIn[0]);
		close(_pipeIn[1]);
		freeEnv(cgiEnv);
		return (false);
	}
	fcntl(_pipeIn[1], F_SETFL, O_NONBLOCK);
	fcntl(_pipeOut[0], F_SETFL, O_NONBLOCK);

	_pid = fork();
	if (_pid < 0) {
		close(_pipeIn[0]);
		close(_pipeIn[1]);
		close(_pipeOut[0]);
		close(_pipeOut[1]);
		freeEnv(cgiEnv);
		return (false);
	}

	if (_pid == 0) {
		close(_pipeIn[1]);
		close(_pipeOut[0]);

		std::string	scriptDir = _scriptPath.substr(0, _scriptPath.find_last_of('/'));
		if (!scriptDir.empty() && chdir(scriptDir.c_str()) != 0) {
			close(_pipeIn[0]);
			close(_pipeOut[1]);
			exit(1);
		}

		if (dup2(_pipeIn[0], STDIN_FILENO) < 0) {
			close(_pipeIn[0]);
			close(_pipeOut[1]);
			exit(1);
		}
		close(_pipeIn[0]);

		if (dup2(_pipeOut[1], STDOUT_FILENO) < 0) {
			close(_pipeOut[1]);
			exit(1);
		}
		dup2(_pipeOut[1], STDERR_FILENO);
		close(_pipeOut[1]);

		executeScript(cgiEnv);

		exit(127);
	}
	close(_pipeIn[0]);
	close(_pipeOut[1]);
	_startTime = time(NULL);
	_readComplete = false;

	freeEnv(cgiEnv);
	return (true);
}