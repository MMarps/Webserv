/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/26 16:53:09 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Request.hpp"

Request::Request() : _isComplete(false), _code(0), _bodySize(0), _isChunked(false), _serverPort(0) {}

Request::~Request() {}

void	Request::parse(ServerConfig &server, std::string &buffer, int code) {
	this->_code = code;
	if (this->_code != 0)
		return ;
	makeRequest(server, buffer);
	checkRequest();
}

size_t		hexToDecimal(const std::string &hex) {
	size_t	value = 0;

	for (size_t i = 0; i < hex.size(); i++) {
		char	c = hex[i];
		value *= 16;
		if (c >= '0' && c <= '9')
			value += c - '0';
		else if (c >= 'A' && c <= 'F')
			value += c - 'A' + 10;
		else if (c >= 'a' && c <= 'f')
			value += c - 'a' + 10;
		else
			break;
	}
	return (value);
}

bool	Request::parseChunkedBody(const std::string &newData) {
	_rawBuffer += newData;
	size_t	pos = 0;
	
	while (pos < _rawBuffer.size()) {
		size_t	lineEnd = _rawBuffer.find("\r\n", pos); // chercher
		if (lineEnd == std::string::npos)
			return (false); // donnees incompletes -> attendre
		std::string	chunkSizeStr = _rawBuffer.substr(pos, lineEnd - pos);
		size_t		chunkSize = hexToDecimal(chunkSizeStr);
		if (chunkSize == static_cast<size_t>(-1)) {
			_code = 400;
			return (false);
		}
		if (chunkSize == 0) { // check si chunk final
			size_t	finalEnd = _rawBuffer.find("\r\n", lineEnd + 2);
			if (finalEnd == std::string::npos)
				return (false); // si \r\n final pas trouve -> attendre
			// dechunking done
			_bodySize = _body.size();
			_isComplete = true;
			_rawBuffer.clear();
			return (true);
		}
		// ÉTAPE 4 : Vérifier qu'on a assez de données pour ce chunk
		size_t	dataStart = lineEnd + 2; // Position après "\r\n"
		size_t	dataEnd = dataStart + chunkSize;
		size_t	nextChunkStart = dataEnd + 2; // Après les données + "\r\n"

		if (nextChunkStart > _rawBuffer.size())
			return (false); // pas assez de donnees -> attendre
		std::string	chunkData = _rawBuffer.substr(dataStart, chunkSize);

		_body += chunkData;
		pos = nextChunkStart;
	}
	if (pos > 0)
		_rawBuffer = _rawBuffer.substr(pos);
	
	return (false); // pas fini -> attendre + de donnees
}

void	Request::makeRequest(ServerConfig &server, std::string &buffer)
{
	std::istringstream	request(buffer.c_str());
	std::string			line;
	std::string			bodyBuffer;
	bool				headerParsed = false;
	
	while (getline(request, line)) {
		if (!headerParsed) {
			std::istringstream	cut(line);
			std::string			res;
			getline(cut, res, ' ');
			if (res == "GET" || res == "POST" || res == "DELETE" || res == "HEAD")
				parseMethode(server, line);
			else
				parseAttribut(line);
			if (line == "\r" || line.empty() || strcmp(line.c_str(), "\r\n") == 0) {
				headerParsed = true;
			}
		}
		else {
			// Accumuler les lignes du body
			if (!bodyBuffer.empty())
				bodyBuffer += "\n";
			bodyBuffer += line;
		}
	}
	
	// Traiter le body après avoir lu tout le buffer
	if (headerParsed && !bodyBuffer.empty()) {
		if (_isChunked) {
			// Pour les requêtes chunked, parser les chunks
			parseChunkedBody(bodyBuffer);
		}
		else {
			// Pour les requêtes normales, stocker directement
			this->_body += bodyBuffer;
			this->_bodySize = this->_body.size();
			
			// Vérifier si le body est complet
			std::map<std::string, std::string>::iterator	it = _httpHeaders.find("Content-Length");
			if (it != _httpHeaders.end()) {
				size_t expectedSize = atoi(_httpHeaders["Content-Length"].c_str());
				if (this->_bodySize >= expectedSize)
					_isComplete = true;
			}
		}
	}
}

void	Request::checkRequest() {
	if (this->_methode.empty() || this->_path.empty() || this->_version.empty()
		|| this->_host.empty())
		this->_code = 400;
	if (this->_code == 0)
		this->_code = 200;
}

void	Request::parseMethode(ServerConfig &server, std::string &line) {
	std::istringstream cut(line);
	std::string res;
	std::vector<std::string> parsedLine;
	while (getline(cut, res, ' '))
		parsedLine.push_back(res);
	if (parsedLine.size() != 3)
	{
		this->_code = 400;
		return ;
	}
	this->setMethode(parsedLine[0]);
	this->setAndCheckPath(server, parsedLine[1]);
	this->setVersion(parsedLine[2]);
}

void	Request::parseAttribut(std::string &line) {
	std::istringstream	cut(line);
	std::string			res;

	getline(cut, res, ' ');
	std::string headerName = res;
	if (!headerName.empty() && headerName[headerName.size() - 1] == ':') // take off ':'
		headerName = headerName.substr(0, headerName.size() - 1);
	if (res == "Host:") {
		getline(cut, res, ' ');
		this->_host = res;
		this->_httpHeaders["Host"] = res;
	}
	else if (res == "Content-Type:") {
		getline(cut, res);
		if (!res.empty() && res[0] == ' ')
			res = res.substr(1);
		if (!res.empty() && res[res.size() - 1] == '\r')
			res = res.substr(0, res.size() - 1);
		this->_contentType = res;
		this->_httpHeaders["Content-Type"] = res;
	}
	else if (res == "Content-Length:") {
		getline(cut, res, ' ');
		std::stringstream ss(res);
		ss >> this->_bodySize;
		this->_httpHeaders["Content-Length"] = res;
	}
	else if (!headerName.empty()) { // Pour tous les autres headers, les stocker dans _httpHeaders
		std::string	headerValue;
		getline(cut, headerValue);
		// Nettoyer les espaces et \r
		if (!headerValue.empty() && headerValue[0] == ' ')
			headerValue = headerValue.substr(1);
		if (!headerValue.empty() && headerValue[headerValue.size() - 1] == '\r')
			headerValue = headerValue.substr(0, headerValue.size() - 1);
		if (headerName == "Transfer-Encoding") {
			std::string	valueLower;
			for (size_t i = 0; i < headerValue.length(); i++)
				valueLower += tolower(headerValue[i]);
			if (valueLower.find("chunked") != std::string::npos)
				_isChunked = true;
			this->_httpHeaders["Transfer-Encoding"] = headerValue;
		}
		else if (!headerValue.empty())
			this->_httpHeaders[headerName] = headerValue;
	}
}

size_t	haveVariable(std::string &path) {
	size_t		findVar;

	std::string	cutPath;
	findVar = path.find('?');
	return (findVar);
}

std::string	Request::getPathVariable(std::string &path) {
	std::string	cutPath;
	if (haveVariable(path) == std::string::npos)
		return (path);
	cutPath = path.substr(0, haveVariable(path));
	return (cutPath);
}

void	Request::getVariable(std::string &path) {
	std::string variableQuery;
	if (haveVariable(path) == std::string::npos)
		return ;
	variableQuery = path.substr(haveVariable(path) + 1);
	// while (variableQuery[0] == '?')
	// {
	// 	findVar++;
	// 	variableQuery = path.substr(findVar + 1);
	// }
	this->_queryString = variableQuery;
	std::stringstream varLine(variableQuery);
	std::string buff;
	// while (getline(varLine, buff, '?')) {
	while (getline(varLine, buff, '&')) {
		std::stringstream varCut(buff);
		std::string nameBuff;
		std::string valueBuff;
		getline(varCut, nameBuff, '=');
		getline(varCut, valueBuff);
		this->_varLst[nameBuff] = valueBuff;
	}
}

int	Request::getPathType(ServerConfig &server) {
	struct stat	st;

	if (stat(this->_completPath.c_str(), &st) == -1)
		return (-1);
	if (S_ISREG(st.st_mode))
		return (FILE_PATH);
	if (S_ISDIR(st.st_mode)) {
		std::vector<LocationConfig>::iterator it;
		for (it = server.locations.begin(); it < server.locations.end(); it++) {
			if (server.root + it->path == this->_completPath)
				return (SERVER_LOCATION_NO_SLASH);
			else if (server.root + it->path == this->_completPath + '/')
				return (SERVER_LOCATION_WI_SLASH);
		}
		if (this->_path[this->_path.size() - 1] == '/')
			return (DIR_WITH_SLASH);
		return (DIR_NO_SLASH);
	}
	return (0);
}

void	Request::verifFile()
{
	struct stat	st;

	if (stat(this->_completPath.c_str(), &st) == -1) {
		if (errno == ENOENT || errno == ENOTDIR)
			this->_code = 404;
		else if (errno == EACCES || errno == ELOOP)
			this->_code = 403;
		else
			this->_code = 500;
		return ;
	}
}

void	Request::getIndex(ServerConfig &server) {
	for (std::vector<std::string>::iterator it = server.index.begin(); it < server.index.end(); it++) {
		this->_completPath += *it;
		verifFile();
		if (this->_code == 0)
			return ;
	}
}

void	Request::getFilePath(ServerConfig &server, int searchIndex) {
	if ((this->_completPath == server.root + "/" && server.index.size() > 0)
		|| searchIndex) {
		getIndex(server);
		return ;
	}
	verifFile();
}

void	Request::getServerLocationPath(ServerConfig &server) {
	DIR				*folder;
	struct dirent	*readFolder;

	for (std::vector<LocationConfig>::iterator it = server.locations.begin(); it < server.locations.end(); it++) {
		if (it->path == this->_path) {
			if (this->_path[this->_path.size() - 1] == '/')
				this->_path[this->_path.size() - 1] = '\0';
			if (it->autoindex) {
				/*case auto index a gerer, generation d'une page auto*/
				std::string dirPath = server.root + it->path;
				folder = opendir(dirPath.c_str());
				if (!folder) {
					std::cout << BRED << "NOP" << NC << std::endl;
					this->_code = 404;
				}
				else {
					readFolder = readdir(folder);
					while (readFolder) {
						std::cout << GREEN << readFolder->d_name << NC << std::endl;
						readFolder = readdir(folder);
					}
				}
				this->_path = "/autoindexon.html";
				this->_completPath = server.root + "/autoindexon.html";
			}
			else {
				this->_path = "/servelocation.html";
				this->_completPath = server.root + "/servelocation.html";
			}
		}
	}
}

void	Request::setAndCheckPath(ServerConfig &server, std::string &path) {
	int	fileType;

	getVariable(path);
	this->_path = getPathVariable(path);
	this->_completPath = server.root + getPathVariable(path);
	fileType = getPathType(server);
	switch (fileType)
	{
	case -1:
		break ;
	case FILE_PATH:
		getFilePath(server, 0);
		break ;
	case DIR_WITH_SLASH:
		getFilePath(server, 1);
		break ;
	case DIR_NO_SLASH:
		this->_path = path + "/";
		this->_code = 301;
		break ;
	case SERVER_LOCATION_NO_SLASH:
		getServerLocationPath(server);
		// this->_code = 301;
		break ;
	case SERVER_LOCATION_WI_SLASH:
		getServerLocationPath(server);
		// this->_code = 301;
	}
}

std::ostream	&operator<<(std::ostream &o, Request const &request)
{
	o << BGREEN << "mode : " << request.getMethode() << std::endl << "path : " << request.getPath() << std::endl << "version : " << request.getVersion() << std::endl << "host : " << request.getHost() << std::endl << NC;
	return (o);
}

std::string	Request::getMethode() const {
	return (this->_methode);
}

std::string	Request::getPath() const {
	return (this->_path);
}

std::string	Request::getCompletPath() const {
	return (this->_completPath);
}

std::string	Request::getVersion() const {
	return (this->_version);
}

std::string	Request::getHeader() const {
	return (this->_header);
}

std::string	Request::getHost() const {
	return (this->_host);
}

bool	Request::getIsComplete(void) const {
	return (this->_isComplete);
}

int	Request::getCode(void) const {
	return (this->_code);
}

void	Request::setMethode(std::string &methode) {
	if (methode == "GET" || methode == "POST" || methode == "DELETE"
		|| methode == "HEAD")
		this->_methode = methode;
	else
		this->_code = 405;
}

void	Request::setPath(std::string &path) {
	this->_path = path;
}

void	Request::setVersion(std::string &version) {
	if (version != "HTTP/1.1\r")
		this->_code = 400;
	else
		this->_version = version;
}

void	Request::setErrorCode(int errorCode) {
	this->_code = errorCode;
}

std::string	Request::getQueryString() const {
	return (this->_queryString);
}

std::string	Request::getBody() const {
	return (this->_body);
}

std::string	Request::getContentType() const {
	return (this->_contentType);
}

size_t	Request::getBodySize() const {
	return (this->_bodySize);
}

const std::map<std::string, std::string>	&Request::getVarLst() const {
	return (this->_varLst);
}

const std::map<std::string, std::string>	&Request::getHttpHeaders() const {
	return (this->_httpHeaders);
}

std::string	Request::getRemoteAddr() const {
	return (this->_remoteAddr);
}

int	Request::getServerPort() const {
	return (this->_serverPort);
}

void	Request::setRemoteAddr(const std::string &addr) {
	this->_remoteAddr = addr;
}

void	Request::setServerPort(int port) {
	this->_serverPort = port;
}