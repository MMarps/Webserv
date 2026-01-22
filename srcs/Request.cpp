/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/22 17:21:31 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Request.hpp"

Request::Request() : _isComplete(false), _errorCode(0)
{
}

Request::~Request()
{
}

void Request::parse(ServerConfig server, std::string buffer, int errorCode)
{
	this->setErrorCode(errorCode);
	if (this->_errorCode != 0)
	{
		return ;
	}
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
	if (this->_errorCode == 0 && (!this->_methode.empty() && !this->_path.empty() && !this->_host.empty()))
		this->_errorCode = 200;
	else
		this->_errorCode = 400;
}

void Request::parseMethode(ServerConfig server, std::string line)
{
	std::istringstream cut(line);
	std::string res;
	std::vector<std::string> parsedLine;
	while (getline(cut, res, ' '))
	{
		parsedLine.push_back(res);
	}
	if (parsedLine.size() != 3)
	{
		this->_errorCode = 400;
		return ;
	}
	this->setMethode(parsedLine[0]);
	this->setPath(server, parsedLine[1]);
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

int Request::getErrorCode(void) const
{
	return (this->_errorCode);
}

void Request::setMethode(std::string methode)
{
	if (methode == "GET" || methode == "POST" || methode == "DELETE"
		|| methode == "HEAD")
		this->_methode = methode;
	else
		this->_errorCode = 400;
}

void Request::setPath(std::string path)
{
	this->_path = path;
}

std::string Request::cutPathVariable(std::string path)
{
	size_t	findVar;

	std::string cutPath;
	std::string cutVar;
	findVar = path.find('?');
	if (findVar == std::string::npos)
		return (path);
	cutPath = path.substr(0, findVar);
	this->_var = path.substr(findVar + 1);
	while (this->_var[0] == '?')
	{
		findVar++;
		this->_var = path.substr(findVar + 1);
	}
	std::cout << this->_var << std::endl;
	return (cutPath);
}

int Request::getPathType(std::string cpPath)
{
	struct stat	st;

	if (stat(cpPath.c_str(), &st) == -1)
		return (-1);
	if (S_ISREG(st.st_mode))
		return (FILE_PATH);
	if (S_ISDIR(st.st_mode))
	{
		if (cpPath[cpPath.size() - 1] == '/')
		{
			return (DIR_WITH_SLASH);
		}
		return (DIR_NO_SLASH);
	}
	return (0);
}

void Request::getfilePath(ServerConfig server, std::string cpPath, int mod)
{
	if ((cpPath == server.root + "/" && server.index.size() > 0) || mod)
	{
		std::cout << BGREEN << cpPath << std::endl;
		for (std::vector<std::string>::iterator it = server.index.begin(); it < server.index.end(); it++)
		{
			cpPath += *it;
			if (access(cpPath.c_str(), F_OK | R_OK) != -1)
			{
				this->_path = cpPath;
				return ;
			}
		}
	}
	if (access(cpPath.c_str(), F_OK) == -1)
	{
		this->_errorCode = 404;
		return ;
	}
	else if (access(cpPath.c_str(), R_OK) == -1)
	{
		this->_errorCode = 403;
		return ;
	}
	else
		this->_path = cpPath;
}



void Request::setPath(ServerConfig server, std::string path)
{
	int	fileType;

	std::string cpPath;
	std::cout << BRED << path << NC << std::endl;
	cpPath = server.root + cutPathVariable(path);
	fileType = getPathType(cpPath);
	switch (fileType)
	{
		case -1:
			this->_errorCode = 404;
			break ;
		case FILE_PATH:
			getfilePath(server, cpPath, 0);
			break ;
		case DIR_WITH_SLASH:
			getfilePath(server, cpPath, 1);
			break ;
		case DIR_NO_SLASH:
			this->setPath(path + "/");
			this->_errorCode = 301;
			break ;
	}
}

void Request::setVersion(std::string version)
{
	if (version != "HTTP/1.1\r")
		this->_errorCode = 400;
	else
		this->_version = version;
}

void Request::setHeader(std::string header)
{
	(void)header;
}

void Request::setHost(std::string host)
{
	(void)host;
}

void Request::setIsComplete(bool isComplet)
{
	(void)isComplet;
}

void Request::setErrorCode(int errorCode)
{
	this->_errorCode = errorCode;
}

std::ostream &operator<<(std::ostream &o, Request const &request)
{
	o << BGREEN << "mode : " << request.getMethode() << std::endl << "path : " << request.getPath() << std::endl << "version : " << request.getVersion() << std::endl << "host : " << request.getHost() << std::endl << NC;
	return (o);
}