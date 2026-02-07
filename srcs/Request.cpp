/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/07 18:02:26 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "Request.hpp"

Request::Request() : _isLocation(false), _isComplete(false),
					 _makeAutoindex(false), _code(200)
{
}

Request::~Request()
{
}

void Request::parse(ServerConfig server, std::string header, int code)
{
	std::cout << "debut parsing request" << std::endl;
	this->_code = code;
	if (this->_code != 200)
		return;
	this->_root = server.root;
	makeRequest(server, header);
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
	preparePath(server);
}

void Request::preparePath(ServerConfig server)
{
	(void)server;
	// int	fileType;
	cutVariableToPath();
	cutPath();
	makeAllPathRules(server);
	this->_completPath = server.root + this->_path;
	// getRuleForResponse(server, path);
	this->_code = 404;
	// getVariable(path);
	// this->_path = getPathVariable(path);
	// this->_completPath = this->_root + getPathVariable(path);
	// fileType = getPathType(server);
	// switch (fileType)
	// {
	// case -1:
	// 	break ;
	// case FILE_PATH:
	// 	std::cout << "cest un file" << std::endl;
	// 	getfilePath(server, 0);
	// 	break ;
	// case DIR_WITH_SLASH:
	// 	std::cout << "cest un dir /" << std::endl;
	// 	getfilePath(server, 1);
	// 	break ;
	// case DIR_NO_SLASH:
	// 	std::cout << "cest un dir" << std::endl;
	// 	this->_path = path + "/";
	// 	this->_code = 301;
	// 	break ;
	// case SERVER_LOCATION:
	// 	std::cout << "cest une location" << std::endl;
	// 	getServerLocationPath(server);
	// 	_isLocation = true;
	// 	this->_code = 301;
	// 	break ;
	// }
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
			return ;
		case SERVER_LOCATION:
			std::cout << BPURPLE << *it << " is LOCATION" << NC << std::endl;
			copyLocationRules(server, *it,newPath);
			break;
		case FILE_PATH:
			std::cout << BPURPLE << *it << " is FILE" << NC << std::endl;
			makeExtentionAndNameFile(*it);
			break;
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

void Request::copyLocationRules(ServerConfig server, std::string folder ,std::string piecePath)
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

void Request::cutVariableToPath()
{
	std::string variableQuery;
	if (haveVariable() == std::string::npos)
		return;
	variableQuery = this->_path.substr(haveVariable() + 1);
	this->_path = this->_path.substr(0, haveVariable());
	std::cout << variableQuery << std::endl;
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
	std::cout << path << std::endl;
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

// void Request::getRuleForResponse(const ServerConfig &server, std::string path)
// {
// 	int pathType;
// 	// pour le test
// 	this->_path = "pastrouve";
// 	// pour le test
// 	(void) path;

// 	cutPath();
// 	if (this->_cutPath.empty())
// 		return;
// 	std::vector<std::string>::iterator it = this->_cutPath.begin();
// 	std::string newpath;
// 	for (; it != this->_cutPath.end(); it++)
// 	{
// 		this->_path.append(*it);
// 		this->_completPath = server.root + this->_path;
// 		pathType = getPathType(server, this->_path);
// 		if (pathType == -1)
// 		{
// 			std::cout << BRED << this->_path << " a une erreur" << NC << std::endl;
// 			verifFile(server.root + *it);
// 			return;
// 		}
// 		if (pathType == SERVER_LOCATION)
// 		{
// 			std::cout << BBLUE << this->_path << " est une location" << NC << std::endl;
// 			getServerLocationConfig(server, *it);
// 		}
// 		if (pathType == FILE_PATH)
// 		{
// 			std::cout << BGREEN << this->_path << " est un file" << NC << std::endl;
// 			setContentExtention();
// 		}
// 		if (pathType == DIR_WITH_SLASH || pathType == DIR_NO_SLASH)
// 		{
// 			std::cout << BYELLOW << this->_path << " est un dir" << NC << std::endl;
// 			// getServerLocationConfig(server, *it);
// 		}
// 	}
// }

std::string Request::getFileExtention() const
{
	return (this->_fileExtention);
}

std::string Request::getFileName() const
{
	return (this->_fileName);
}

// void Request::getServerLocationConfig(const ServerConfig &server,
// 									  std::string path)
// {
	
// }

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

// void Request::getServerLocationPath(const ServerConfig &server)
// {
// 	if (!this->_path.empty() && this->_path[this->_path.size() - 1] == '/')
// 		this->_path.erase(this->_path.size() - 1);
// 	std::vector<LocationConfig>::const_iterator it = server.locations.begin();
// 	for (; it != server.locations.end(); ++it)
// 	{
// 		if (it->path == this->_path)
// 		{
// 			this->_completPath = this->_root + it->path;
// 			this->_path = it->path;
// 			verifFile();
// 			// setContentExtention();
// 			this->_isLocation = true;
// 			setLocationConfig(*it);
// 			makeLocationRules(server);
// 			return;
// 		}
// 	}
// }

// void Request::setLocationConfig(LocationConfig location)
// {
// 	this->_location = location;
// }

// void Request::makeLocationRules(const ServerConfig &server)
// {
// 	bool allowMethod;
// 	int type;

// 	allowMethod = false;

// 	if (this->_location.has_return)
// 	{
// 		this->_code = this->_location.return_code;
// 		this->_path = this->_location.return_url;
// 		std::cout << "return" << std::endl;
// 		return;
// 	}
// 	else if (!this->_location.methods.empty())
// 	{
// 		std::vector<std::string>::iterator it = this->_location.methods.begin();
// 		for (; it != this->_location.methods.begin(); it++)
// 		{
// 			if (*it == this->_methode)
// 			{
// 				allowMethod = true;
// 			}
// 		}
// 		if (!allowMethod)
// 		{
// 			this->_code = 405;
// 			std::cout << "methode pas allowed" << std::endl;
// 			return;
// 		}
// 	}
// 	// if (this->_location->client_max_body_size)
// 	// {
// 	// 	/* code */
// 	// }
// 	if (!this->_location.root.empty())
// 	{
// 		this->_root = this->_location.root;
// 		this->_completPath = this->_root + this->_path;
// 	}
// 	type = getPathType(server);
// 	if (!this->_location.cgi.empty())
// 	{
// 	}
// 	// if (check UPLOAD)
// 	// {
// 	// }
// 	getfilePath(server, 0);
// 	if (type == FILE_PATH || this->_code != 200 || this->_code != 0)
// 	{
// 		std::cout << "ca passe file ou erreur" << std::endl;
// 		return;
// 	}
// 	if (this->_location.autoindex)
// 	{
// 		this->_makeAutoindex = true;
// 	}
// 	std::cout << "ca passe au bout" << std::endl;
// }

// int Request::getPathType(ServerConfig server)
// {
// 	struct stat st;

// 	if (stat(this->_completPath.c_str(), &st) == -1)
// 	{
// 		verifFile();
// 		return (-1);
// 	}
// 	if (S_ISREG(st.st_mode))
// 		return (FILE_PATH);
// 	if (S_ISDIR(st.st_mode))
// 	{
// 		std::vector<LocationConfig>::iterator it;
// 		for (it = server.locations.begin(); it < server.locations.end(); it++)
// 		{
// 			if (this->_root + it->path == this->_completPath /*|| this->_root + it->path + '/' == this->_completPath*/)
// 				return (SERVER_LOCATION);
// 		}
// 		if (this->_path[this->_path.size() - 1] == '/')
// 			return (DIR_WITH_SLASH);
// 		return (DIR_NO_SLASH);
// 	}
// 	return (0);
// }

// int Request::getPathType(ServerConfig server, std::string path)
// {
// 	struct stat st;

// 	std::string cPath = server.root + path;
// 	if (stat(cPath.c_str(), &st) == -1)
// 	{
// 		verifFile();
// 		return (-1);
// 	}
// 	if (S_ISREG(st.st_mode))
// 		return (FILE_PATH);
// 	if (S_ISDIR(st.st_mode))
// 	{
// 		std::vector<LocationConfig>::iterator it;
// 		for (it = server.locations.begin(); it < server.locations.end(); it++)
// 		{
// 			if (this->_root + it->path == this->_root + path /*|| this->_root + it->path + '/' == this->_root + path*/)
// 				return (SERVER_LOCATION);
// 		}
// 		if (this->_path[this->_path.size() - 1] == '/')
// 			return (DIR_WITH_SLASH);
// 		return (DIR_NO_SLASH);
// 	}
// 	return (0);
// }





// std::string Request::getPathVariable(std::string path)
// {
// 	std::string cutPath;
// 	if (haveVariable(path) == std::string::npos)
// 		return (path);
// 	cutPath = path.substr(0, haveVariable(path));
// 	return (cutPath);
// }

// void Request::getfilePath(ServerConfig server, int searchIndex)
// {
// 	if ((this->_completPath == this->_root + "/" && server.index.size() > 0) || searchIndex)
// 	{
// 		getIndex(server);
// 		return;
// 	}
// 	verifFile();
// }

// void Request::getIndex(ServerConfig server)
// {
// 	for (std::vector<std::string>::iterator it = server.index.begin(); it < server.index.end(); it++)
// 	{
// 		this->_completPath += *it;
// 		verifFile();
// 		if (this->_code == 0)
// 			return;
// 	}
// }


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

std::string Request::getContentExtention() const
{
	return (this->_fileExtention);
}

LocationConfig Request::getLocation() const
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

std::map<std::string, std::string> Request::getVarLst() const
{
	return (this->_varLst);
}
