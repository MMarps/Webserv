#include "CGI.hpp"

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
	addEnv(_envCgi, "SERVER_PORT", integerToString(req.getServerPort()));
	addEnv(_envCgi, "REMOTE_ADDR", req.getRemoteAddr());

	addHTTPHeaders(_envCgi);
	return (vectorToEnv(_envCgi));
}

std::string	CGI::integerToString(size_t value) {
	std::ostringstream	oss;
	oss << value;
	return (oss.str());
}

std::string	CGI::findInterpreter() {
	std::string	extension;
	size_t		dotPos = _scriptPath.find_last_of('.');
	if (dotPos == std::string::npos)
		return ("");
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

void	CGI::addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value) {
	env.push_back(key + "=" + value);
}