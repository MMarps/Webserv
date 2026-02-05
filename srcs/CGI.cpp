#include "CGI.hpp"

CGI::CGI(Request &req, ServerConfig &server, EnvCGI	&env) 
	: _req(req), _server(server), _statusCode(200), _timeout(30), _env(env) {
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
		if (headerName == "Content-Type" || headerName == "Content-length")
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
	addEnv(_envCgi, "SERVER_PROTOCOLE", "HTTP/1.1");
	addEnv(_envCgi, "REQUEST_METHOD", req.getMethode());
	addEnv(_envCgi, "REQUEST_URI", req.getQueryString());
	addEnv(_envCgi, "SCRIPT_FILENAME", req.getCompletPath());
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
	std::string interpreterPath = _server.cgi[_interpreter];
	char		*argv[3];

	argv[0] = strdup(interpreterPath.c_str());
	argv[1] = strdup(_scriptPath.c_str());
	argv[2] = NULL;

	execve(argv[0], argv, env);
	
	free(argv[0]);
	free(argv[1]);
	exit(1);
}

void	CGI::parentProcess(int *fdIn, int *fdOut) {
	close(fdIn[0]);
	close(fdOut[1]);
	// Si POST : écrire le body dans le pipe d'entrée
	if (_req.getMethode() == "POST") {
		std::string body = _req.getBody();
		write(fdIn[1], body.c_str(), body.size());
	}
	close(fdIn[1]);

	char buffer[4096];
	ssize_t bytesRead;
	while ((bytesRead = read(fdOut[0], buffer, sizeof(buffer))) > 0) {
		_output.insert(_output.end(), buffer, buffer + bytesRead);
	}
	close(fdOut[0]);
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

	int	status;

	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		return (true);
	return (false);
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
		free(env[i]);
	free(env);
}

char	**CGI::vectorToEnv(std::vector<std::string> &env) {
	int		vecLen = env.size() + 1;
	char	**envRet = (char **)malloc(sizeof(char *) * vecLen);
	int		i = 0;
	for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++) {
		envRet[i] = strdup(it->c_str()); // ALLOC HERE
		if (!envRet[i]) {
			for (int j = 0; j < i; j++)
				free(envRet[j]);
			return (NULL);
		}
		i++;
	}
	envRet[vecLen] = '\0';
	return (envRet);
}

std::string	CGI::integerToString(size_t val) {
	std::ostringstream	oss;
	oss << val;
	return (oss.str());
}

int	CGI::getStatusCode() {
	return (this->_statusCode);
}

std::vector<char>	CGI::getOutput() {
	return (this->_output);
}

std::map<std::string, std::string>	CGI::getHeaders() {
	return (this->_cgiHeaders);
}