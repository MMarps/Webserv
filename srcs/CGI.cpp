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

// void	CGI::importEnv(std::vector<std::string> &env, const std::string &key) {
// 	const char	*val = std::getenv(key.c_str());
// 	if (val)
// 		env.push_back(key + "=" + val);
// }

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

	char	**envRet = vectorToEnv(_envCgi);
	return (envRet);
}

bool	CGI::executeScript(char **env) {

}

bool	CGI::execute(const Request &req) {
	this->_interpreter = findInterpreter();
	char	**cgiEnv = setupEnv(req);
	if (executeScript(cgiEnv)) {
		parseOutput();
		// freeEnv(cgiEnv);
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

std::string	integerToString(size_t val) {
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