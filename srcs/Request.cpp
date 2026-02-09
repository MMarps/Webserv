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

Request::Request() : _isComplete(false), _isChunked(false), _code(0), _bodySize(0) {}

Request::~Request() {}

void	Request::parse(ServerConfig &server, std::string &buffer, int code)
{
	this->_code = code;
	if (this->_code != 0)
		return ;
	makeRequest(server, buffer);
	checkRequest();
}

bool hexToDecimal(const std::string &hex, size_t &result)
{
    std::istringstream iss(hex);
    iss >> std::hex >> result;
    return !iss.fail();
}

bool	Request::parseChunkedBody(std::string &line) {
	static std::string	chunkedBody;
	size_t				pos;

	chunkedBody += line;
	pos = chunkedBody.find("\r\n");
	if (pos == std::string::npos)
		return (false);
	else {
		std::string	strChunkSize;
		for (size_t i = 0; i < pos; i++)
			strChunkSize += line[i];
		size_t	chunkSize;
		if (!hexToDecimal(strChunkSize, chunkSize)) {
			_code = 400;
			return (false);
		}
		// if (!chunkSize)
	}
	if (chunkedBody.find("0\r\n\r\n") != std::string::npos) {

	}
}

void	Request::makeRequest(ServerConfig &server, std::string &buffer)
{
	std::istringstream	request(buffer.c_str());
	std::string			line;
	bool				headerFlag = false;
	
	while (getline(request, line)) {
		if (!headerFlag) {
			std::istringstream	cut(line);
			std::string			res;
			getline(cut, res, ' ');
			if (res == "GET" || res == "POST" || res == "DELETE" || res == "HEAD")
				parseMethode(server, line);
			else
				parseAttribut(line);
			if (line == "\r" || line.empty() || strcmp(line.c_str(), "\r\n") == 0)
				headerFlag = true;
		}
		else {
			if (!_isChunked && parseChunkedBody(line))
				break ;
			if (!this->_body.empty())
				this->_body += "\n";
			this->_body += line;
		}
	}
	this->_bodySize = this->_body.size();
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
		if (!headerValue.empty())
			this->_httpHeaders[headerName] = headerValue;
		if (headerName == "Transfer-Encoding") {
			if (headerValue == "chunked")
				_isChunked = true;
		}
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

int	Request::getPathType(ServerConfig &server)
{
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

	if (stat(this->_completPath.c_str(), &st) == -1)
	{
		if (errno == ENOENT || errno == ENOTDIR)
			this->_code = 404;
		else if (errno == EACCES || errno == ELOOP)
			this->_code = 403;
		else
			this->_code = 500;
		return ;
	}
}

void	Request::getIndex(ServerConfig &server)
{
	for (std::vector<std::string>::iterator it = server.index.begin(); it < server.index.end(); it++)
	{
		this->_completPath += *it;
		verifFile();
		if (this->_code == 0)
			return ;
	}
}

void	Request::getFilePath(ServerConfig &server, int searchIndex)
{
	if ((this->_completPath == server.root + "/" && server.index.size() > 0)
		|| searchIndex)
	{
		getIndex(server);
		return ;
	}
	verifFile();
}

void	Request::getServerLocationPath(ServerConfig &server)
{
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

void	Request::setAndCheckPath(ServerConfig &server, std::string &path)
{
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