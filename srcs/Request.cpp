/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/26 14:51:36 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Request.hpp"

Request::Request() : _isComplete(false), _code(0)
{
}

Request::~Request()
{
}

void Request::parse(ServerConfig server, std::string buffer, int code )
{
	this->_code = code;
	if (this->_code != 0)
		return ;
	makeRequest(server, buffer);
	checkRequest();
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
			parseMethode(server, line);
		else
			parseAttribut(line);
		if (strcmp(line.c_str(), "\r\n") == 0)
			break ;
	}
}

void Request::checkRequest()
{
	if (this->_methode.empty() || this->_path.empty() || this->_version.empty()
		|| this->_host.empty())
		this->_code = 400;
	if (this->_code == 0)
		this->_code = 200;
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
		return ;
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

size_t	haveVariable(std::string path)
{
	size_t	findVar;

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
		return ;
	variableQuery = path.substr(haveVariable(path) + 1);
	// while (variableQuery[0] == '?')
	// {
	// 	findVar++;
	// 	variableQuery = path.substr(findVar + 1);
	// }
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

int Request::getPathType(ServerConfig server, std::string cpPath)
{
	struct stat	st;

	if (stat(cpPath.c_str(), &st) == -1)
		return (-1);
	if (S_ISREG(st.st_mode))
		return (FILE_PATH);
	if (S_ISDIR(st.st_mode))
	{
		std::vector<LocationConfig>::iterator it;
		for (it = server.locations.begin(); it < server.locations.end(); it++)
		{
			if (server.root + it->path == cpPath)
			{
				return (SERVER_LOCATION);
			}
			// if (server.root + it->path == cpPath)
			// 	return (SERVER_LOCATION_NO_SLASH);
			// // else if (server.root + it->path == cpPath + '/')
			// // 	return (SERVER_LOCATION_WI_SLASH);
		}
		if (cpPath[cpPath.size() - 1] == '/')
		{
			return (DIR_WITH_SLASH);
		}
		return (DIR_NO_SLASH);
	}
	return (0);
}

void Request::verifFile(std::string cpPath)
{
	struct stat	st;

	if (stat(cpPath.c_str(), &st) == -1)
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

void Request::getIndex(ServerConfig server, std::string cpPath)
{
	std::cout << BGREEN << cpPath << std::endl;
	for (std::vector<std::string>::iterator it = server.index.begin(); it < server.index.end(); it++)
	{
		cpPath += *it;
		if (access(cpPath.c_str(), F_OK | R_OK) != -1)
		{
			this->_path = cpPath;
			verifFile(cpPath);
			return ;
		}
	}
}

void Request::getfilePath(ServerConfig server, std::string cpPath,
	int searchIndex)
{
	if ((cpPath == server.root + "/" && server.index.size() > 0) || searchIndex)
	{
		getIndex(server, cpPath);
		return ;
	}
	this->_path = cpPath;
	verifFile(cpPath);
}

void Request::getServerLocationPath(ServerConfig server, std::string path)
{
	DIR				*folder;
	struct dirent	*readFolder;

	for (std::vector<LocationConfig>::iterator it = server.locations.begin(); it < server.locations.end(); it++)
	{
		if (it->path == path || it->path == path + '/')
		{
			// if (it->path[it->path.size() - 1] != '/')
			// {
			// 	it->path.append("/");
			// }
			std::string dirPath = server.root + it->path;
			folder = opendir(dirPath.c_str());
			if (!folder)
			{
				std::cout << BRED << "NOP" << NC << std::endl;
				this->_code = 404;
			}
			else
			{
				readFolder = readdir(folder);
				while (readFolder)
				{
					std::cout << GREEN << readFolder->d_name << NC << std::endl;
					readFolder = readdir(folder);
				}
			}
			if (it->autoindex)
			{
				/*case auto index a gerer, generation d'une page auto*/
				this->_path = "/autoindexon.html";
			}
			else
			{
				this->_path = "/servelocation.html";
			}
		}
	}
}

void Request::setAndCheckPath(ServerConfig server, std::string path)
{
	int	fileType;

	std::string cpPath;
	// std::cout << BRED << path << NC << std::endl;
	getVariable(path);
	cpPath = server.root + getPathVariable(path);
	fileType = getPathType(server, cpPath);
	switch (fileType)
	{
	case -1:
		break ;
	case FILE_PATH:
		getfilePath(server, cpPath, 0);
		break ;
	case DIR_WITH_SLASH:
		getfilePath(server, cpPath, 1);
		break ;
	case DIR_NO_SLASH:
		this->setPath(path + "/");
		this->_code = 301;
		break ;
	case SERVER_LOCATION:
		getServerLocationPath(server, path);
		// case SERVER_LOCATION_NO_SLASH:
		// 	getServerLocationPath(server, path);
		// 	this->_code = 301;
		// case SERVER_LOCATION_WI_SLASH:
		// 	getServerLocationPath(server, path);
		// 	this->_code = 301;
	}
}

std::ostream &operator<<(std::ostream &o, Request const &request)
{
	o << BGREEN << "mode : " << request.getMethode() << std::endl << "path : " << request.getPath() << std::endl << "version : " << request.getVersion() << std::endl << "host : " << request.getHost() << std::endl << NC;
	return (o);
}

std::string Request::getMethode() const
{
	return (this->_methode);
}

std::string Request::getPath() const
{
	return (this->_path);
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
	if (methode == "GET" || methode == "POST" || methode == "DELETE"
		|| methode == "HEAD")
		this->_methode = methode;
	else
		this->_code = 405;
}

void Request::setPath(std::string path)
{
	this->_path = path;
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