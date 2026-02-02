/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/02 16:47:32 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Request.hpp"

Request::Request() : _location(NULL), _isLocation(false), _isComplete(false), _makeAutoindex(false), _code(0)
{
}

Request::~Request()
{
}

void Request::parse(ServerConfig server, std::string buffer, int code)
{
	std::cout << "debut parsing request" << std::endl;

	this->_code = code;
	if (this->_code != 0)
		return;
	this->_root = server.root;
	makeRequest(server, buffer);
	checkRequest();
	std::cout << "fin parsing request" << std::endl;
}

void Request::makeRequest(ServerConfig server, std::string buffer)
{
	std::istringstream request(buffer.c_str());
	std::string line;
	while (getline(request, line))
	{
		std::istringstream cut(line);
		std::string res;
		getline(cut, res, ' ');
		if (res == "GET" || res == "POST" || res == "DELETE" || res == "HEAD")
		{
			parseMethode(server, line);
		}
		else
		{
			parseAttribut(line);
		}
		if (strcmp(line.c_str(), "\r\n") == 0)
			break;
	}
}

void Request::parseMethode(ServerConfig server, std::string line)
{
	std::istringstream cut(line);
	std::string res;
	std::vector<std::string> parsedLine;
	while (getline(cut, res, ' '))
		parsedLine.push_back(res);
	if (parsedLine.size() != 3)
	{
		this->_code = 400;
		return;
	}
	this->setMethode(parsedLine[0]);
	this->setAndCheckPath(server, parsedLine[1]);
	this->setVersion(parsedLine[2]);
}

void Request::parseAttribut(std::string line)
{
	std::istringstream cut(line);
	std::string res;
	getline(cut, res, ' ');
	if (res == "Host:")
	{
		getline(cut, res, ' ');
		this->_host = res;
	}
}

void Request::checkRequest()
{
	if (this->_methode.empty() || this->_path.empty() || this->_version.empty() || this->_host.empty())
		this->_code = 400;
	if (this->_code == 0)
		this->_code = 200;
}

void Request::setAndCheckPath(ServerConfig server, std::string path)
{
	int fileType;

	getVariable(path);
	this->_path = getPathVariable(path);
	this->_completPath = this->_root + getPathVariable(path);
	fileType = getPathType(server);
	switch (fileType)
	{
	case -1:
		break;
	case FILE_PATH:
		std::cout << "cest un file" << std::endl;
		getfilePath(server, 0);
		break;
	case DIR_WITH_SLASH:
		std::cout << "cest un dir /" << std::endl;
		getfilePath(server, 1);
		break;
	case DIR_NO_SLASH:
		std::cout << "cest un dir" << std::endl;
		this->_path = path + "/";
		this->_code = 301;
		break;
	case SERVER_LOCATION_NO_SLASH:
		std::cout << "cest une location" << std::endl;
		getServerLocationPath(server);
		_isLocation = true;
		// this->_code = 301;
		break;
	case SERVER_LOCATION_WI_SLASH:
		std::cout << "cest une location" << std::endl;
		getServerLocationPath(server);
		_isLocation = true;
		// this->_code = 301;
	}
}

void Request::getServerLocationPath(const ServerConfig &server)
{
	if (!this->_path.empty() && this->_path[this->_path.size() - 1] == '/')
		this->_path.erase(this->_path.size() - 1);
	std::vector<LocationConfig>::const_iterator it = server.locations.begin();
	for (; it != server.locations.end(); ++it)
	{
		if (it->path == this->_path)
		{
			this->_completPath = this->_root + it->path;
			this->_path = it->path;
			verifFile();
			this->_isLocation = true;
			this->_location = &(*it);
			makeLocationRules(server);
			return;
		}
	}
}

void Request::makeLocationRules(const ServerConfig &server)
{
	bool allowMethod = false;
	if (this->_location != NULL)
	{
		if (this->_location->has_return)
		{
			this->_code = this->_location->return_code;
			this->_path = this->_location->return_url;
			std::cout << "return" << std::endl;

			return;
		}
		else if (!this->_location->methods.empty())
		{
			std::vector<std::string>::const_iterator it = this->_location->methods.begin();
			for (; it != this->_location->methods.begin(); it++)
			{
				if (*it == this->_methode)
				{
					allowMethod = true;
				}
			}
			if (!allowMethod)
			{
				this->_code = 405;
				std::cout << "methode pas allowed" << std::endl;
				return;
			}
		}
		// if (this->_location->client_max_body_size)
		// {
		// 	/* code */
		// }
		if (!this->_location->root.empty())
		{
			this->_root = this->_location->root;
			this->_completPath = this->_root + this->_path;
		}
		// if (check CGI)
		// {

		// }
		// if (check UPLOAD)
		// {
		// }
		getfilePath(server, 0);
		int type = getPathType(server);
		if (type == FILE_PATH || this->_code != 200 || this->_code != 0)
		{
			std::cout << "ca passe file ou erreur" << std::endl;
			return;
		}
		if (this->_location->autoindex)
		{
			this->_makeAutoindex = true;
		}
		std::cout << "ca passe au bout" << std::endl;
	}
}

int Request::getPathType(ServerConfig server)
{
	struct stat st;

	if (stat(this->_completPath.c_str(), &st) == -1)
	{
		verifFile();
		return (-1);
	}
	if (S_ISREG(st.st_mode))
		return (FILE_PATH);
	if (S_ISDIR(st.st_mode))
	{
		std::vector<LocationConfig>::iterator it;
		for (it = server.locations.begin(); it < server.locations.end(); it++)
		{
			if (this->_root + it->path == this->_completPath)
				return (SERVER_LOCATION_NO_SLASH);
			else if (this->_root + it->path + '/' == this->_completPath)
				return (SERVER_LOCATION_WI_SLASH);
		}
		if (this->_path[this->_path.size() - 1] == '/')
			return (DIR_WITH_SLASH);
		return (DIR_NO_SLASH);
	}
	return (0);
}

size_t Request::haveVariable(std::string path)
{
	size_t findVar;

	std::string cutPath;
	findVar = path.find('?');
	return (findVar);
}

std::string Request::getPathVariable(std::string path)
{
	std::string cutPath;
	if (haveVariable(path) == std::string::npos)
		return (path);
	cutPath = path.substr(0, haveVariable(path));
	return (cutPath);
}

void Request::getVariable(std::string path)
{
	std::string variableQuery;
	if (haveVariable(path) == std::string::npos)
		return;
	variableQuery = path.substr(haveVariable(path) + 1);
	std::stringstream varLine(variableQuery);
	std::string buff;
	while (getline(varLine, buff, '?'))
	{
		std::stringstream varCut(buff);
		std::string nameBuff;
		std::string valueBuff;
		getline(varCut, nameBuff, '=');
		getline(varCut, valueBuff);
		this->_varLst[nameBuff] = valueBuff;
	}
}

void Request::getfilePath(ServerConfig server, int searchIndex)
{
	if ((this->_completPath == this->_root + "/" && server.index.size() > 0) || searchIndex)
	{
		getIndex(server);
		return;
	}
	verifFile();
}

void Request::getIndex(ServerConfig server)
{
	for (std::vector<std::string>::iterator it = server.index.begin(); it < server.index.end(); it++)
	{
		this->_completPath += *it;
		verifFile();
		if (this->_code == 0)
			return;
	}
}

void Request::verifFile()
{
	struct stat st;
	if (stat(this->_completPath.c_str(), &st) == -1)
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

std::string Request::getMethode() const
{
	return (this->_methode);
}

std::string Request::getPath() const
{
	return (this->_path);
}

std::string Request::getCompletPath() const
{
	return (this->_completPath);
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

const LocationConfig *Request::getLocation() const
{
	return (this->_location);
}

bool Request::getIsLocation() const
{
	return (this->_isLocation);
}

bool Request::getIsComplete(void) const
{
	return (this->_isComplete);
}

int Request::getCode(void) const
{
	return (this->_code);
}

void Request::setMethode(std::string methode)
{
	if (methode == "GET" || methode == "POST" || methode == "DELETE" || methode == "HEAD")
		this->_methode = methode;
	else
		this->_code = 405;
}

void Request::setPath(std::string path)
{
	this->_path = path;
}

void Request::setCompletPath(std::string completPath)
{
	this->_completPath = completPath;
}

void Request::setVersion(std::string version)
{
	if (version != "HTTP/1.1\r")
		this->_code = 400;
	else
		this->_version = version;
}

void Request::setErrorCode(int errorCode)
{
	this->_code = errorCode;
}

std::ostream &operator<<(std::ostream &o, Request const &request)
{
	o << BGREEN << "mode : " << request.getMethode() << std::endl
	  << "path : " << request.getPath() << std::endl
	  << "version : " << request.getVersion() << std::endl
	  << "host : " << request.getHost() << std::endl
	  << NC;
	return (o);
}