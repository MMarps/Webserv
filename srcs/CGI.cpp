#include "CGI.hpp"

CGI::CGI(Request &req, ServerConfig &server) 
	: _req(req), _server(server), _statusCode(200), _timeout(30) {
	_scriptPath = _req.getCompletPath();
}

CGI::~CGI() {}

bool	CGI::isCGI() {
	std::string	extension;
	if (!_scriptPath.empty()) {
		for (size_t i = _scriptPath.find_last_of('.', 0); i < _scriptPath.size(); i++)
			extension += _scriptPath[i];
	}
	for () {

	}
	return (false);
}

bool	CGI::execute() {

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