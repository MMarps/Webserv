/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/12 14:30:18 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Request.hpp"

Request::Request() : _isLocation(false), _isPost(false), _isComplete(false),
					 _makeAutoindex(false), _isCgi(false), _code(200)
{
	std::cout << BGREEN << "construct req" << NC << std::endl;
}

Request::~Request()
{
	std::cout << BRED << "destruct req" << NC << std::endl;

}

void Request::parse(ServerConfig server, std::string header, int code)
{
	std::cout << "debut parsing request" << std::endl;
	this->_code = code;
	if (this->_code != 200)
		return;
	this->_root = server.root;
	this->_index = server.index;
	makeRequest(server, header);
	checkRequest();
	std::cout << *this << std::endl;
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
		cut >> res;
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
	std::stringstream ss(line);
	std::string method, uri, version;
	std::string path;

	ss >> this->_methode >> this->_path >> this->_version;
	prepareReq(server);
}

void Request::prepareReq(ServerConfig server)
{
	cutVariableToPath();
	cutPath();
	makeAllPathRules(server);
	formatPath();
	if (this->_isLocation)
		makeLocationRules();
	if (this->_code == 200 && this->_fileName.empty() && this->_location.autoindex)
	{
		this->_makeAutoindex = true;
		this->_fileExtention = ".html";
		this->_completPath = this->_root + this->_path;
		return;
	}
	searchIndex();
	checkIsCgi(server);
	std::cout << "ca passe" << std::endl;
	if (this->_code != 200 && !server.error_pages.empty() && !server.error_pages[this->_code].empty())
	{
		this->_completPath = server.error_pages[this->_code];
		makeExtentionAndNameFile(this->_completPath);
		return;
	}
	this->_completPath = this->_root + this->_path;
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

void Request::splitVarQuery(std::string variableQuery)
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
			lastFindPos = findPos;
			this->_cutPath.push_back(res);
			path = path.substr(findPos);
			findPos = path.find('/', findPos + 1);
		}
	}
	this->_cutPath.push_back(path);
}

void Request::makeAllPathRules(ServerConfig server)
{
	int pathType;
	std::string newPath;
	std::vector<std::string>::iterator it = this->_cutPath.begin();
	for (; it != this->_cutPath.end(); it++)
	{
		newPath += *it;
		pathType = checkPathType(server, newPath);
		switch (pathType)
		{
		case -1:
			std::cout << BRED << *it << " is PROBLEME" << NC << std::endl;
			return;
		case SERVER_LOCATION:
			std::cout << BPURPLE << *it << " is LOCATION" << NC << std::endl;
			copyLocationRules(server, *it, newPath);
			break;
		case FILE_PATH:
			std::cout << BPURPLE << *it << " is FILE" << NC << std::endl;
			makeExtentionAndNameFile(*it);
			break;
		}
	}
}

int Request::checkPathType(ServerConfig server, std::string piecePath)
{
	struct stat st;

	std::string cPath = server.root + piecePath;
	if (stat(cPath.c_str(), &st) == -1)
	{
		verifFile(server.root + piecePath);
		return (-1);
	}
	if (S_ISREG(st.st_mode))
		return (FILE_PATH);
	if (S_ISDIR(st.st_mode))
	{
		std::vector<LocationConfig>::iterator it;
		for (it = server.locations.begin(); it < server.locations.end(); it++)
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

void Request::copyLocationRules(ServerConfig server, std::string folder, std::string piecePath)
{
	if (!folder.empty() && folder[folder.size() - 1] == '/')
		folder.erase(folder.size() - 1);
	std::vector<LocationConfig>::const_iterator it = server.locations.begin();
	for (; it != server.locations.end(); ++it)
	{
		if (it->path == folder)
		{
			verifFile(server.root + piecePath);
			if (this->_code == 200)
			{
				this->_isLocation = true;
				this->_location = *it;
			}
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
	{
		this->_path += '/';
	}
}

void Request::makeLocationRules()
{
	if (this->_location.has_return)
	{
		this->_path = this->_location.return_url;
		this->_code = this->_location.return_code;
		return;
	}
	checkAllowMethods();
	if (_code != 200)
		return;
	if (!this->_location.root.empty())
		this->_root = this->_location.root;
	if (!this->_location.index.empty())
		this->_index = this->_location.index;
}

void Request::checkAllowMethods()
{
	if (!this->_location.methods.empty())
	{
		for (size_t i = 0; i < this->_location.methods.size(); i++)
		{
			if (this->_location.methods[i] == this->_methode)
				return;
		}
		_code = 405;
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
		std::cout << BRED << "no index found" << NC << std::endl;
	}
}

void Request::checkIsCgi(ServerConfig server)
{
	if (this->_fileName.empty() || this->_fileExtention.empty())
		return;
	std::map<std::string, std::string> mapCgi;
	if (this->_isLocation)
		mapCgi = this->_location.cgi;
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

std::string Request::getFileExtention() const
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

LocationConfig Request::getLocation() const
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
	o << "file ext  : " << request.getFileExtention() << std::endl;
	o << "comp path : " << request.getCompletPath() << std::endl;
	if (request.getIsCgi())
	{
		o << "cgi path  : " << request.getCgiPath() << std::endl;
	}

	// o << request.getIsLocation() <<std::endl;
	if (!request.getVarLst().empty())
	{
		o << "var       : " << std::endl;
		std::map<std::string, std::string> var = request.getVarLst();
		std::map<std::string, std::string>::const_iterator it = var.begin();
		for (; it != var.end(); it++)
		{
			o << "	" << it->first << "=" << it->second << std::endl;
		}
	}
	if (request.getIsLocation())
	{
		o << "location  :" << std::endl;
		o << "	path      : " << request.getLocation().path << std::endl;
		if (!request.getLocation().root.empty())
		{
			o << "	root      : " << request.getLocation().root << std::endl;
		}
		else
			o << "	root      : empty" << std::endl;

		if (!request.getLocation().index.empty())
		{
			o << "	index      : ";
			std::vector<std::string> index = request.getLocation().index;
			std::vector<std::string>::iterator it = index.begin();
			for (; it != index.end(); it++)
			{
				o << *it << " ";
			}
			o << std::endl;
		}
		if (request.getLocation().autoindex)
			o << "	autoindex : true" << std::endl;
		else
			o << "	autoindex : false" << std::endl;
		if (!request.getLocation().cgi.empty())
		{
			o << "	cgi       : ";
			std::map<std::string, std::string> cgi = request.getLocation().cgi;
			std::map<std::string, std::string>::iterator it = cgi.begin();
			for (; it != cgi.end(); it++)
			{
				o << it->first << " ";
			}
			o << std::endl;
		}
	}
	o << "/////////////////////////////" << std::endl;
	o << NC;

	return (o);
}
