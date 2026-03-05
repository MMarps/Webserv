/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/03/05 15:23:42 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Request.hpp"
#include "Logger.hpp"
#include "Client.hpp"

Request::Request() : _location(NULL), _isLocation(false), _isPost(false), _isComplete(false),
					 _makeAutoindex(false), _isRedirection(false), _urlIsMessage(false), _isCgi(false), _code(200), _bodySize(0),
					 _isChunked(false), _serverPort(0)
{
}
Request::~Request() {}

void Request::parse(ServerConfig &server, Client *client, int code)
{
	this->_code = code;
	if (this->_code != 200)
		return;
	this->_root = server.root;
	this->_index = server.index;
	makeRequest(server, client->getHeader());
	// std::cout << client->getHeader() << std::endl << std::endl;
	std::cout << *this << std::endl;
	if ( this->_code == 301 || (this->_location && this->_location->has_return))
		return;
	checkRequest();
	finalLogger();
}

void Request::finalLogger()
{
	if (this->_isLocation)
	{
		Logger::info("request " + this->_methode + " location " + this->_path + " parsed");
		return;
	}
	Logger::info("request " + this->_methode + " ressource " + this->_path + " parsed");
}

// bool	Request::parseChunkedBody(const std::string &newData) {
// 	_rawBuffer += newData;
// 	size_t	pos = 0;

// 	while (pos < _rawBuffer.size()) {
// 		size_t	lineEnd = _rawBuffer.find("\r\n", pos); // chercher
// 		if (lineEnd == std::string::npos)
// 			return (false); // donnees incompletes -> attendre
// 		std::string	chunkSizeStr = _rawBuffer.substr(pos, lineEnd - pos);
// 		ssize_t		chunkSize = hexToDecimal(chunkSizeStr);
// 		if (chunkSize == static_cast<ssize_t>(-1)) {
// 			_code = 400;
// 			return (false);
// 		}
// 		if (chunkSize == 0) { // check si chunk final
// 			size_t	finalEnd = _rawBuffer.find("\r\n", lineEnd + 2);
// 			if (finalEnd == std::string::npos)
// 				return (false); // si \r\n final pas trouve -> attendre
// 			// dechunking done
// 			_bodySize = _body.size();
// 			_isComplete = true;
// 			_rawBuffer.clear();
// 			return (true);
// 		}
// 		// verifier qu on a assez de donnees pour ce chunk
// 		size_t	dataStart = lineEnd + 2; // Position après "\r\n"
// 		size_t	dataEnd = dataStart + chunkSize;
// 		size_t	nextChunkStart = dataEnd + 2; // apres les données + "\r\n"

// 		if (nextChunkStart > _rawBuffer.size())
// 			return (false); // pas assez de donnees -> attendre
// 		std::string	chunkData = _rawBuffer.substr(dataStart, chunkSize);

// 		_body += chunkData;
// 		pos = nextChunkStart;
// 	}
// 	if (pos > 0)
// 		_rawBuffer = _rawBuffer.substr(pos);

// 	return (false); // pas fini -> attendre + de donnees
// }

void Request::makeRequest(ServerConfig &server, std::string &buffer)
{
	std::istringstream request(buffer.c_str());
	std::string line;
	std::string bodyBuffer;
	bool headerParsed = false;
	while (getline(request, line))
	{
		if (!headerParsed)
		{
			std::istringstream cut(line);
			std::string res;
			cut >> res;
			if (res == "GET" || res == "POST" || res == "DELETE" || res == "HEAD")
				parseMethode(server, line);
			else
				parseAttribut(line);
			if (line == "\r" || line.empty() || strcmp(line.c_str(), "\r\n") == 0)
				headerParsed = true;
		}
		else
		{
			// accumuler les lignes du body
			if (!bodyBuffer.empty())
				bodyBuffer += "\n";
			bodyBuffer += line;
		}
	}

	if (headerParsed && !bodyBuffer.empty())
	{
		// if (_isChunked)
		// parseChunkedBody(bodyBuffer); // pour les requetes chunked, parser les chunks
		// else {
		// pour les requetes normales, stocker directement
		this->_body += bodyBuffer;
		this->_bodySize = this->_body.size();

		std::map<std::string, std::string>::iterator it = _httpHeaders.find("Content-Length");
		if (it != _httpHeaders.end())
		{ // verifier si le body est complet
			size_t expectedSize = atoi(_httpHeaders["Content-Length"].c_str());
			if (this->_bodySize >= expectedSize)
				_isComplete = true;
		}
		// }
	}
}

void Request::parseMethode(ServerConfig &server, std::string &line)
{
	std::stringstream ss(line);
	std::string path;

	ss >> this->_methode >> this->_path >> this->_version;

	prepareReq(server);
}

void Request::prepareReq(ServerConfig &server)
{
	int pathType;
	cutVariableToPath();
	copyLocationRules(server, this->_path);
	if (!this->_isLocation)
	{
		if (this->_path != "/")
			pathType = checkPathType(server, false, this->_path);
		else
			pathType = checkPathType(server, true, this->_path);
	}
	else
	{
		pathType = SERVER_LOCATION;
		makeLocationRules();
		
		if (this->_location && this->_location->has_return)
			return;
	}
	if (pathType == DIR_NO_SLASH || (pathType == SERVER_LOCATION && this->_path[this->_path.size() - 1] != '/'))
	{
		std::cout << "ca passe " << this->_path << std::endl;
		this->_newPath = this->_path + "/";
		this->_code = 301;
		this->_isRedirection = true;
		return;
	}
	cutPath();
	makeAllPathRules(server);
	if (this->_location && this->_location->has_return)
	{
		std::cout << BRED << "ca passe " << this->_code << NC << std::endl;
		checkErrorPage(server);
		return;
	}
	searchIndex();

	if (this->_code == 200 && this->_fileName.empty() && this->_location && this->_location->autoindex)
	{
		this->_makeAutoindex = true;
		this->_fileExtention = ".html";
		this->_completPath = this->_root + this->_path;
		return;
	}
	this->_completPath = this->_root + this->_path;
	checkIsCgi(server);
	if (this->_code == 200 && this->_fileName.empty())
		this->_code = 404;
	checkErrorPage(server);
}

void Request::checkErrorPage(ServerConfig &server)
{
	if (this->_code != 200 && !server.error_pages.empty() && !server.error_pages[this->_code].empty())
	{
		this->_completPath = server.error_pages[this->_code];
		makeExtentionAndNameFile(this->_completPath);
		return;
	}
}

void Request::cutVariableToPath()
{
	std::string variableQuery;
	if (haveVariable() == std::string::npos)
		return;
	variableQuery = this->_path.substr(haveVariable() + 1);
	this->_path = this->_path.substr(0, haveVariable());
	splitVarQuery(variableQuery);
}

size_t Request::haveVariable()
{
	size_t findVar;
	std::string cutPath;

	findVar = this->_path.find('?');
	return (findVar);
}

void Request::splitVarQuery(std::string &variableQuery)
{
	std::stringstream varLine(variableQuery);
	std::string buff;
	while (getline(varLine, buff, '&'))
	{
		std::stringstream varCut(buff);
		std::string nameBuff;
		std::string valueBuff;
		getline(varCut, nameBuff, '=');
		getline(varCut, valueBuff);
		this->_varLst[nameBuff] = valueBuff;
	}
}

void Request::cutPath()
{
	size_t findPos;
	size_t lastFindPos;
	std::string res;
	std::string path = this->_path;
	std::istringstream cut(path);
	if (path.size() != 1)
	{
		findPos = path.find('/', 1);
		lastFindPos = 0;
		while (findPos != std::string::npos)
		{
			res = path.substr(lastFindPos, findPos);
			this->_cutPath.push_back(res);
			path = path.substr(findPos);
			findPos = path.find('/', 1);
		}
	}
	this->_cutPath.push_back(path);
}

void Request::makeAllPathRules(ServerConfig &server)
{
	int pathType;
	std::string newPath;
	std::string newCompletPath;
	std::vector<std::string>::iterator it = this->_cutPath.begin();
	for (; it != this->_cutPath.end(); it++)
	{
		if (this->_code == 301)
			return;
		newPath += *it;
		newCompletPath = this->_root + newPath;
		if (*it == "/")
			pathType = checkPathType(server, true, newPath);
		else
			pathType = checkPathType(server, false, newPath);
		switch (pathType)
		{
		case -1:
			verifFile(newCompletPath);
			return;
		case DIR_WITH_SLASH:
			accessFolder(newCompletPath);
			break;
		case DIR_NO_SLASH:
			break;
		case SERVER_LOCATION:
			copyLocationRules(server, newPath);
			makeLocationRules();
			std::cout << BBLUE << "code " << _code << NC << std::endl;

			accessFolder(newCompletPath);
			break;
		case FILE_PATH:
			makeExtentionAndNameFile(*it);
			if (access(newCompletPath.c_str(), R_OK) != 0)
			{
				this->_code = 403;
				break;
			}
			break;
		}
	}
}

void Request::accessFolder(std::string newCompletPath)
{
	if (this->_path != "/" && access(newCompletPath.c_str(), X_OK) != 0)
	{
		this->_code = 403;
	}
}

int Request::checkPathType(ServerConfig &server, bool slash, std::string &piecePath)
{
	struct stat st;

	if (slash)
		return (NOTHING);
	std::string cPath = this->_root + piecePath;

	if (stat(cPath.c_str(), &st) == -1)
		return (-1);
	if (S_ISREG(st.st_mode))
		return (FILE_PATH);
	if (S_ISDIR(st.st_mode))
	{
		std::vector<LocationConfig>::iterator it = server.locations.begin();
		for (; it < server.locations.end(); it++)
		{
			if (this->_root + it->path == this->_root + piecePath)
				return (SERVER_LOCATION);
		}
		if (this->_path[this->_path.size() - 1] == '/')
			return (DIR_WITH_SLASH);
		return (DIR_NO_SLASH);
	}
	return (0);
}

void Request::verifFile(std::string path)
{
	struct stat st;
	if (stat(path.c_str(), &st) == -1)
	{
		if (errno == ENOENT || errno == ENOTDIR)
			this->_code = 404;
		else if (errno == EACCES || errno == ELOOP)
			this->_code = 403;
		else
			this->_code = 500;
		return;
	}
}

void Request::copyLocationRules(ServerConfig &server, std::string &folder)
{
	std::string formatPath;
	formatPath = folder;
	if (!formatPath.empty() && formatPath[formatPath.size() - 1] == '/')
		formatPath.erase(formatPath.size() - 1);
	for (size_t i = 0; i < server.locations.size(); ++i)
	{
		if (server.locations[i].path == formatPath)
		{
			this->_isLocation = true;
			this->_location = &server.locations[i];
			return;
		}
	}
}

void Request::makeExtentionAndNameFile(std::string file)
{
	size_t dotPos;

	dotPos = file.rfind('.');
	if (dotPos == std::string::npos)
	{
		this->_fileExtention = "nodotdetected";
		this->_fileName = file;
	}
	else
	{
		this->_fileExtention = file.substr(dotPos);
		this->_fileName = file.substr(0, dotPos);
	}
}

void Request::formatPath()
{
	if (this->_fileName.empty() && this->_path[this->_path.size() - 1] != '/')
		this->_path += '/';
}

void Request::makeLocationRules()
{
	if (this->_location && this->_location->has_return)
	{
		this->_code = this->_location->return_code;
		std::cout << BBLUE << _code << std::endl;
		if (!this->_location->return_url.empty())
		{
			this->_newPath = this->_location->return_url;
			if (this->_code == 200)
			{
				this->_urlIsMessage = true;
				this->_fileExtention = "nodotdetected";
			}
			else
				this->_isRedirection = true;
		}
		return;
	}
	checkAllowMethods();
	if (_code != 200)
		return;
	if (this->_location && !this->_location->root.empty())
		this->_root = this->_location->root;
	if (this->_location && !this->_location->index.empty())
		this->_index = this->_location->index;
}

void Request::checkAllowMethods()
{
	if (this->_location && !this->_location->methods.empty())
	{
		for (size_t i = 0; i < this->_location->methods.size(); i++)
		{
			if (this->_location->methods[i] == this->_methode)
				return;
		}
		this->_code = 405;
	}
}

void Request::searchIndex()
{
	std::string path;
	if (this->_fileName.empty() && this->_fileExtention.empty())
	{
		for (std::vector<std::string>::iterator index = this->_index.begin(); index < this->_index.end(); index++)
		{
			path = this->_root + this->_path + *index;
			verifFile(path);
			if (this->_code == 200)
			{
				this->_path += *index;
				makeExtentionAndNameFile(*index);
				return;
			}
		}
		this->_code = 200;
	}
}

void Request::checkIsCgi(ServerConfig &server)
{
	if (this->_fileName.empty() || this->_fileExtention.empty())
		return;
	std::map<std::string, std::string> mapCgi;
	if (this->_location && this->_isLocation)
		mapCgi = this->_location->cgi;
	else
		mapCgi = server.cgi;

	std::map<std::string, std::string>::iterator cgi = mapCgi.begin();
	for (; cgi != mapCgi.end(); cgi++)
	{
		if (cgi->first == this->_fileExtention)
		{
			this->_isCgi = true;
			this->_cgiPath = cgi->second;
			return;
		}
	}
}

void Request::parseAttribut(std::string &line)
{
	std::istringstream cut(line);
	std::string res;

	getline(cut, res, ' ');
	std::string headerName = res;
	if (!headerName.empty() && headerName[headerName.size() - 1] == ':') // take off ':'
		headerName = headerName.substr(0, headerName.size() - 1);
	if (res == "Host:")
	{
		getline(cut, res, ' ');
		this->_host = res;
		this->_httpHeaders["Host"] = res;
	}
	else if (res == "Content-Type:")
	{
		getline(cut, res);
		if (!res.empty() && res[0] == ' ')
			res = res.substr(1);
		if (!res.empty() && res[res.size() - 1] == '\r')
			res = res.substr(0, res.size() - 1);
		this->_contentType = res;
		this->_httpHeaders["Content-Type"] = res;
	}
	else if (res == "Content-Length:")
	{
		getline(cut, res, ' ');
		std::stringstream ss(res);
		ss >> this->_bodySize;
		this->_httpHeaders["Content-Length"] = res;
	}
	else if (!headerName.empty())
	{ // Pour tous les autres headers, les stocker dans _httpHeaders
		std::string headerValue;
		getline(cut, headerValue);
		// Nettoyer les espaces et \r
		if (!headerValue.empty() && headerValue[0] == ' ')
			headerValue = headerValue.substr(1);
		if (!headerValue.empty() && headerValue[headerValue.size() - 1] == '\r')
			headerValue = headerValue.substr(0, headerValue.size() - 1);
		if (headerName == "Transfer-Encoding")
		{
			std::string valueLower;
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

void Request::checkRequest()
{
	if (this->_code != 200)
		return;
	if (this->_methode.empty() || this->_path.empty() || this->_version.empty() || this->_host.empty())
		this->_code = 400;
	verifFile(this->_completPath);
}

std::string Request::getMethode() const
{
	return (this->_methode);
}

std::string Request::getRoot() const
{
	return (this->_root);
}

std::string Request::getPath() const
{
	return (this->_path);
}

std::string Request::getCompletPath() const
{
	return (this->_completPath);
}

std::string Request::getFileName() const
{
	return (this->_fileName);
}

std::string Request::getFileExtension() const
{
	return (this->_fileExtention);
}

std::string Request::getVersion() const
{
	return (this->_version);
}

std::string Request::getHeader() const
{
	return (this->_header);
}

std::string Request::getHost() const
{
	return (this->_host);
}

std::string Request::getNewPath() const
{
	return (this->_newPath);
}

bool Request::getUrlIsMesssage() const
{
	return (this->_urlIsMessage);
}

std::string Request::getCgiPath() const
{
	return (this->_cgiPath);
}

std::vector<std::string> Request::getIndex() const
{
	return (this->_index);
}

std::vector<std::string> Request::getCutPath() const
{
	return (this->_cutPath);
}

std::map<std::string, std::string> Request::getVarLst() const
{
	return (this->_varLst);
}

LocationConfig *Request::getLocation() const
{
	return (this->_location);
}

bool Request::getIsLocation() const
{
	return (this->_isLocation);
}

bool Request::getIsPost() const
{
	return (this->_isPost);
}

bool Request::getIsComplete() const
{
	return (this->_isComplete);
}

bool Request::getIsRedirection() const
{
	return (this->_isRedirection);
}

bool Request::getMakeAutoindex() const
{
	return (this->_makeAutoindex);
}

bool Request::getIsCgi() const
{
	return (this->_isCgi);
}

int Request::getCode() const
{
	return (this->_code);
}

void Request::setCode(int code)
{
	this->_code = code;
}

std::string Request::getQueryString() const
{
	return (this->_queryString);
}

std::string Request::getBody() const
{
	return (this->_body);
}

std::string Request::getContentType() const
{
	return (this->_contentType);
}

size_t Request::getBodySize() const
{
	return (this->_bodySize);
}

const std::map<std::string, std::string> &Request::getHttpHeaders() const
{
	return (this->_httpHeaders);
}

std::string Request::getRemoteAddr() const
{
	return (this->_remoteAddr);
}

int Request::getServerPort() const
{
	return (this->_serverPort);
}

void Request::setRemoteAddr(const std::string &addr)
{
	this->_remoteAddr = addr;
}

void Request::setServerPort(int port)
{
	this->_serverPort = port;
}

std::ostream &operator<<(std::ostream &o, Request const &request)
{
	o << BGREEN << std::endl;
	o << "////////// REQUEST //////////" << std::endl;
	o << "code      : " << request.getCode() << std::endl;
	o << "mode      : " << request.getMethode() << std::endl;
	o << "path      : " << request.getPath() << std::endl;
	o << "version   : " << request.getVersion() << std::endl;
	o << "host      : " << request.getHost() << std::endl;
	o << "filename  : " << request.getFileName() << std::endl;
	o << "file ext  : " << request.getFileExtension() << std::endl;
	o << "comp path : " << request.getCompletPath() << std::endl;
	if (request.getIsCgi())
		o << "cgi path  : " << request.getCgiPath() << std::endl;

	// o << request.getIsLocation() <<std::endl;
	if (!request.getVarLst().empty())
	{
		o << "var       : " << std::endl;
		std::map<std::string, std::string> var = request.getVarLst();
		std::map<std::string, std::string>::const_iterator it = var.begin();
		for (; it != var.end(); it++)
			o << "	" << it->first << "=" << it->second << std::endl;
	}
	if (request.getIsLocation() && request.getLocation())
	{
		o << "location  :" << std::endl;
		o << "	path      : " << request.getLocation()->path << std::endl;
		if (!request.getLocation()->root.empty())
			o << "	root      : " << request.getLocation()->root << std::endl;
		else
			o << "	root      : empty" << std::endl;

		if (!request.getLocation()->index.empty())
		{
			o << "	index      : ";
			std::vector<std::string> index = request.getLocation()->index;
			std::vector<std::string>::iterator it = index.begin();
			for (; it != index.end(); it++)
				o << *it << " ";
			o << std::endl;
		}
		if (request.getLocation()->autoindex)
			o << "	autoindex : true" << std::endl;
		else
			o << "	autoindex : false" << std::endl;
		if (!request.getLocation()->cgi.empty())
		{
			o << "	cgi       : ";
			std::map<std::string, std::string> cgi = request.getLocation()->cgi;
			std::map<std::string, std::string>::iterator it = cgi.begin();
			for (; it != cgi.end(); it++)
				o << it->first << " ";
			o << std::endl;
		}
	}
	o << "/////////////////////////////" << std::endl;
	o << NC;
	return (o);
}

size_t hexToDecimal(const std::string &hex)
{
	size_t value = 0;

	for (size_t i = 0; i < hex.size(); i++)
	{
		char c = hex[i];
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