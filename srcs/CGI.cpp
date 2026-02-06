#include "CGI.hpp"

CGI::CGI(Request &req, ServerConfig &server) 
	: _req(req), _server(server), _statusCode(200), _timeout(30) {
	_scriptPath = _req.getCompletPath();
}

CGI::~CGI() {}

std::string	CGI::findInterpreter() {
	std::string	extension;
	size_t		dotPos = _scriptPath.find_last_of('.');
	if (dotPos == std::string::npos)
		return (NULL);
	extension = _scriptPath.substr(dotPos);
	return (extension);
}

bool	CGI::isCGI() {
	std::string	extension = findInterpreter();
	if (extension.empty())
		return (false);
	if (_server.cgi.find(extension) != _server.cgi.end())
		return (true);
	for (size_t i = 0; i < _server.locations.size(); i++) {
		if (_server.locations[i].cgi.find(extension) != _server.locations[i].cgi.end())
			return (true);
	}
	return (false);
}

void	CGI::addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value) {
	env.push_back(key + "=" + value);
}

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
			char c = headerName[i];
			if (c == '-')
				envName += "_";
			else
				envName += std::toupper(c);
		}
		addEnv(env, envName, headerValue);
	}
}

char	**CGI::setupEnv(const Request &req) {
	std::vector<std::string>	_envCgi;

	addEnv(_envCgi, "GATEWAY_INTERFACE", "CGI/1.1");
	addEnv(_envCgi, "SERVER_PROTOCOL", "HTTP/1.1");
	addEnv(_envCgi, "REQUEST_METHOD", req.getMethode());
	addEnv(_envCgi, "REQUEST_URI", req.getPath());
	addEnv(_envCgi, "SCRIPT_FILENAME", _scriptPath);
	addEnv(_envCgi, "SCRIPT_NAME", req.getPath());
	addEnv(_envCgi, "QUERY_STRING", req.getQueryString());
	addEnv(_envCgi, "CONTENT_TYPE", req.getContentType());
	addEnv(_envCgi, "CONTENT_LENGTH", integerToString(req.getBodySize()));
	addEnv(_envCgi, "SERVER_NAME", req.getHost());
	// addEnv(_envCgi, "SERVER_PORT", req.);
	// addEnv(_envCgi, "REMOTE_ADDR", req.);

	addHTTPHeaders(_envCgi);
	return (vectorToEnv(_envCgi));
}

void	CGI::parseOutput() {
	std::string	output(_output.begin(), _output.end());
	size_t		separatorPos = output.find("\r\n\r\n");
	size_t		separatorSize = 4;

	if (separatorPos == std::string::npos) { // si pas trouve de separateur, chercher \n\n
		separatorPos = output.find("\n\n");
		separatorSize = 2;
	}	
	if (separatorPos == std::string::npos) { // si pas trouve, pas de header et tout dans le body
		_body = output;
		return ;
	}

	std::string headersSection = output.substr(0, separatorPos);
	size_t		bodyStart = separatorPos + separatorSize;
	if (bodyStart < output.size())
		_body = output.substr(bodyStart);

	std::istringstream	headerStream(headersSection);
	std::string			line;

	while (std::getline(headerStream, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r') // Enlever le \r à la fin si présent
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

void	CGI::freePipes(int *fdIn, int *fdOut) {
	if (fdIn) {
		close(fdIn[0]);
		close(fdIn[1]);
	}
	if (fdOut) {
		close(fdOut[0]);
		close(fdOut[1]);
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
	exit(1);
}

void	CGI::parentProcess(int *fdIn, int *fdOut) {
	close(fdIn[0]);
	close(fdOut[1]);
	if (_req.getMethode() == "POST") { // Si POST : écrire le body dans le pipe d'entrée
		std::string body = _req.getBody();
		write(fdIn[1], body.c_str(), body.size());
	}
	close(fdIn[1]);

	char	buffer[4096];
	ssize_t	bytesRead;
	while ((bytesRead = read(fdOut[0], buffer, sizeof(buffer))) > 0) {
		_output.insert(_output.end(), buffer, buffer + bytesRead);
	}
	close(fdOut[0]);
}

bool	CGI::waitProcess(pid_t pid) {
	time_t	startTime = time(NULL);
	int		status;
	bool	isTimeout = false;

	while (true) {
		pid_t	ret = waitpid(pid, &status, WNOHANG);
		if (ret == pid)
			break ; // process termine
		else if (ret < 0)
			return (false); // error
		time_t	elapsedTime = time(NULL) - startTime;
		if (elapsedTime >= _timeout) {
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
			isTimeout = true;
			break ;
		}
		usleep(100000); // attendre pour pas consommer trop de CPU
	}
	if (isTimeout) {
		_statusCode = 504;
		return (false);
	}
	if (WIFEXITED(status) && !WEXITSTATUS(status)) // script bien exec, pas de timeout
		return (true);
	return (false);
}

bool	CGI::processScript(char **env) {
	int		pipeIn[2];
	int		pipeOut[2];

	if (pipe(pipeIn) < 0)
		return (false);
	if (pipe(pipeOut) < 0)
		return (false);
	pid_t	pid = fork();
	if (pid < 0)
		return (false);
	if (pid == 0) {
		close(pipeIn[1]);
		close(pipeOut[0]);

		std::string	scriptDir = _scriptPath.substr(0, _scriptPath.find_last_of('/')); // on change de dir pour aller ou se trouve le script
		if (!scriptDir.empty() && chdir(scriptDir.c_str()) != 0) {
			freePipes(pipeIn, pipeOut);
			exit(1);
		}
		if (dup2(pipeIn[0], STDIN_FILENO) < 0) {
			freePipes(pipeIn, pipeOut);
			exit(false);
		}
		close(pipeIn[0]);
		if (dup2(pipeOut[1], STDOUT_FILENO) < 0) {
			freePipes(pipeIn, pipeOut);
			exit(false);
		}
		dup2(pipeOut[1], STDERR_FILENO);
		close(pipeOut[1]);
		executeScript(env);
	}
	parentProcess(pipeIn, pipeOut);
	return (waitProcess(pid));
}

bool	CGI::execute(const Request &req) {
	this->_interpreter = findInterpreter();
	if (this->_interpreter.empty())
		return (false);
	if (open(_scriptPath.c_str(), O_RDONLY, O_EXCL) < 0)
		return (false);

	char	**cgiEnv = setupEnv(req);
	if (processScript(cgiEnv)) {
		freeEnv(cgiEnv);
		parseOutput();
		return (true);
	}
	freeEnv(cgiEnv);
	return (false);
}

void	CGI::freeEnv(char **env) {
	for (int i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
}

char	**CGI::vectorToEnv(std::vector<std::string> &env) {
	int		vecLen = env.size();
	char	**envRet = new char*[vecLen + 1];
	int		i = 0;
	
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++) {
		size_t	len = it->length() + 1;
		envRet[i] = new char[len];
		std::strcpy(envRet[i], it->c_str());
		i++;
	}
	envRet[vecLen] = NULL;
	return (envRet);
}

std::string	CGI::integerToString(size_t val) {
	std::ostringstream	oss;
	oss << val;
	return (oss.str());
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