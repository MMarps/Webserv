/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/20 17:59:06 by jle-doua         ###   ########.fr       */
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

void Request::parse(ServerConfig server, std::string buffer)
{
	std::istringstream request(buffer.c_str());
	std::string line;
	while (getline(request, line))
	{
		std::istringstream cut(line);
		std::string res;
		getline(cut, res, ' ');
		if (res == "GET" || res == "POST" || res == "DELETE")
			parseMethode(server, line);
		else
			parseAttribut(line);
		if (strcmp(line.c_str(), "\r\n") == 0)
			break ;
	}
	if (this->_errorCode == 0)
	{
		this->_errorCode = 200;
	}
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
	if (methode == "GET" || methode == "POST" || methode == "DELETE")
		this->_methode = methode;
	else
		this->_errorCode = 400;
}

void Request::setPath(std::string path)
{
	this->_path = path;
}

void Request::setPath(ServerConfig server, std::string path)
{
	std::string cppath;
	cppath = server.root + path;
	if (cppath == server.root + "/" && server.index.size() > 0)
	{
		for (std::vector<std::string>::iterator it = server.index.begin(); it < server.index.end(); it++)
		{
			cppath += *it;
			if (access(cppath.c_str(), F_OK | R_OK) != -1)
			{
				this->_path = cppath;
				return ;
			}
		}
	}
	if (access(cppath.c_str(), F_OK) == -1)
	{
		this->_errorCode = 404;
		return ;
	}
	else if (access(cppath.c_str(), R_OK) == -1)
	{
		this->_errorCode = 403;
		return ;
	}
	else
	{
		this->_path = cppath;
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
	(void)errorCode;
}

std::ostream &operator<<(std::ostream &o, Request const &request)
{
	o << BGREEN << "mode : " << request.getMethode() << std::endl << "path : " << request.getPath() << std::endl << "version : " << request.getVersion() << std::endl << "host : " << request.getHost() << std::endl << NC;
	return (o);
}