/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2026/03/11 16:44:23 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request &req) : _req(req), _isCGI(false)
{
	_statutMessage.insert(std::make_pair(200, "OK"));
	_statutMessage.insert(std::make_pair(201, "Created"));
	_statutMessage.insert(std::make_pair(204, "No Content"));
	_statutMessage.insert(std::make_pair(301, "Moved Permanently"));
	_statutMessage.insert(std::make_pair(302, "Found"));
	_statutMessage.insert(std::make_pair(400, "Bad Request"));
	_statutMessage.insert(std::make_pair(403, "Forbidden"));
	_statutMessage.insert(std::make_pair(404, "Not Found"));
	_statutMessage.insert(std::make_pair(405, "Method Not Allowed"));
	_statutMessage.insert(std::make_pair(409, "Conflict"));
	_statutMessage.insert(std::make_pair(413, "Payload Too Large"));
	_statutMessage.insert(std::make_pair(500, "Internal Server Error"));
	_statutMessage.insert(std::make_pair(502, "Bad Gateway"));
	_statutMessage.insert(std::make_pair(504, "Gateway Timeout"));
	_contentType.insert(std::make_pair(".html", "text/html"));
	_contentType.insert(std::make_pair(".css", "text/css"));
	_contentType.insert(std::make_pair(".js", "text/javascript"));
	_contentType.insert(std::make_pair(".png", "image/png"));
	_contentType.insert(std::make_pair(".jpg", "image/jpeg"));
	_contentType.insert(std::make_pair(".jpeg", "image/jpeg"));
	_contentType.insert(std::make_pair(".gif", "image/gif"));
	_contentType.insert(std::make_pair(".ico", "image/x-icon"));
	_contentType.insert(std::make_pair(".mp4", "video/mp4"));
	_contentType.insert(std::make_pair(".mp3", "audio/mpeg"));
	_contentType.insert(std::make_pair("nodotdetected", "text/plain"));
}

Response::~Response() {}

void Response::makeRep(ServerConfig &server, Client *client)
{
	(void)client;
	if (_req.getMethode() == "DELETE")
	{
		handleDelete();
		generateHeader();
		std::cout << *this << std::endl;

		finalLogger(client->getServerIdx());
		return;
	}
	if (isCGIRequest(server))
	{
		_isCGI = true;
		handleCGI(server);
		if (_req.getCode() == 502)
		{
			generateHeader();
			finalLogger(client->getServerIdx());
			return;
		}
		buildCGIResponse();
		finalLogger(client->getServerIdx());
		return;
	}
	generateBody();
	generateHeader();
	std::cout << *this << std::endl;
	finalLogger(client->getServerIdx());
}

void Response::finalLogger(int serverIdx)
{
	std::string m;

	m = this->_req.getMethode() + " " + this->_req.getPath() + " " + intToString(this->_req.getCode()) + " " + this->_statutMessage[this->_req.getCode()];
	if (this->_req.getCode() != 200 && this->_req.getCode() != 301)
	{
		Logger::error(m, serverIdx);
		return;
	}
	if (this->_req.getIsRedirection())
	{
		Logger::info(m + " location : " + this->_req.getNewPath(), serverIdx);
		return;
	}
	Logger::info(m, serverIdx);
}

void Response::generateHeader()
{
	this->_response = "HTTP/1.1 " + intToString(this->_req.getCode()) + " " + this->_statutMessage[this->_req.getCode()] + "\r\n";
	if (this->_req.getCode() == 204)
	{
		this->_response += "Connection: close\r\n\r\n";
		return;
	}
	if (this->_req.getIsRedirection())
	{
		this->_response += "location: " + this->_req.getNewPath() + "\r\n";
		this->_response += "Content-length: 0\r\n";
		this->_response += "\r\n";
		return;
	}
	if ((this->_isCGI && this->_req.getCode() == 502) || this->_req.getMethode() == "DELETE" || (this->_req.getCode() != 200 && this->_req.getFileName().empty()))
	{
		this->_response += "Content-length: 0\r\n";
		this->_response += "\r\n";
		return;
	}
	this->_response += "Content-Type: " + this->_contentType[this->_req.getFileExtension()] + "\r\n";
	this->_response += "Content-length: " + this->_contentLength + "\r\n";
	this->_response += "\r\n";
}

void Response::generateBody()
{

	if (this->_req.getUrlIsMesssage())
	{
		std::string message = this->_req.getNewPath();
		this->_content.assign(message.begin(), message.end());
		this->_contentLength = intToString(message.size());
		return;
	}
	if (!this->_req.getFileName().empty())
	{
		if (this->_req.getMethode() == "HEAD")
			checkFile(false);
		else
			checkFile(true);
		return;
	}
	else if (this->_req.getMakeAutoindex() && this->_req.getCode() == 200)
		generateAutoindex();
}

void Response::checkFile(bool save)
{
	std::ifstream file(this->_req.getCompletPath().c_str(), std::ios::binary);
	std::istreambuf_iterator<char> first(file);
	std::istreambuf_iterator<char> last;
	std::vector<char> buffer(first, last);
	this->_contentLength = intToString(buffer.size());

	if (save)
		this->_content.swap(buffer);
}

std::string Response::intToString(int n)
{
	std::stringstream ss;

	ss << n;
	return (ss.str());
}

void Response::generateAutoindex()
{
	std::string htmlpage;
	std::vector<std::string> lstFiles;

	htmlpage = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body>";
	lstFiles = getLstDir();
	for (long unsigned int i = 0; i < lstFiles.size(); i++)
		htmlpage += "<a href=\"" + this->_req.getPath() + lstFiles[i] + "\">" + lstFiles[i] + "</a></br>";
	htmlpage += "</body></html>";
	std::vector<char> tmp(htmlpage.begin(), htmlpage.end());
	this->_content.swap(tmp);
	std::stringstream ss;
	ss << htmlpage.size();
	this->_contentLength = ss.str();
}

std::vector<std::string> Response::getLstDir()
{
	DIR *folder;
	struct dirent *readFolder;
	std::vector<std::string> lstFiles;

	folder = opendir(this->_req.getCompletPath().c_str());
	if (!folder)
		this->_req.setCode(500);
	readFolder = readdir(folder);
	while (readFolder)
	{
		if (strcmp(readFolder->d_name, ".") != 0 && strcmp(readFolder->d_name, "..") != 0)
			lstFiles.push_back(readFolder->d_name);
		readFolder = readdir(folder);
	}
	closedir(folder);
	std::sort(lstFiles.begin(), lstFiles.end());
	return (lstFiles);
}

bool Response::isCGIRequest(ServerConfig &server)
{
	if (_req.getCode() != 200)
		return (false);

	CGI tmpCGI(_req, server);
	return (tmpCGI.isCGI(_req, server));
}

void Response::handleCGI(ServerConfig &server)
{
	CGI _cgi(_req, server);

	if (_cgi.isCGI(_req, server) && !_cgi.execute(_req))
	{
		this->_req.setCode(502);
		return;
	}
	int statusCode = _cgi.getStatusCode();
	if (statusCode != 200)
		this->_req.setCode(statusCode);
	_cgiHeaders = _cgi.getHeaders();
	std::string body = _cgi.getBody();
	_content.assign(body.begin(), body.end());
}

void Response::buildCGIResponse()
{
	std::ostringstream statusLine;
	statusLine << "HTTP/1.1 " << _req.getCode() << ' ' << _statutMessage[_req.getCode()] << "\r\n";
	_response = statusLine.str();

	std::map<std::string, std::string>::iterator it = _cgiHeaders.begin();
	while (it != _cgiHeaders.end())
	{
		_response += it->first + ": " + it->second + "\r\n";
		it++;
	}
	if (_cgiHeaders.find("Content-Length") == _cgiHeaders.end())
	{
		std::ostringstream contentLengthStream;
		contentLengthStream << _content.size();
		_response += "Content-Length: " + contentLengthStream.str() + "\r\n";
	}
	_response += "Connection: close\r\n\r\n";
	_response.append(_content.begin(), _content.end());
}

bool Response::isDirectoryEmpty(const std::string &dirPath)
{
	DIR *dir = opendir(dirPath.c_str());
	if (!dir)
		return (false);

	struct dirent *entry;
	int count = 0;

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			closedir(dir);
			return (false);
		}
		count++;
	}
	closedir(dir);
	return (count == 2);
}

void Response::handleDelete()
{
	std::string filePath = _req.getCompletPath();
	struct stat fileStat;
	if (this->_req.getCode() == 405)
		return ;
	if (stat(filePath.c_str(), &fileStat) != 0)
	{
		_req.setCode(404);
		return;
	}
	if (S_ISDIR(fileStat.st_mode))
	{
		if (access(filePath.c_str(), W_OK | X_OK) != 0)
		{
			_req.setCode(403);
			return;
		}
		if (!isDirectoryEmpty(filePath))
		{

			_req.setCode(409);
			return;
		}
		if (rmdir(filePath.c_str()) != 0)
		{
			_req.setCode(500);
			return;
		}
		_req.setCode(204);
		return;
	}
	if (access(filePath.c_str(), W_OK) != 0)
	{
		_req.setCode(403);
		return;
	}
	if (unlink(filePath.c_str()) != 0)
	{
		_req.setCode(500);
		return;
	}
	_req.setCode(204);
}

std::string Response::getResponse() const
{
	return (this->_response);
}

std::vector<char> Response::getContent() const
{
	return (this->_content);
}

std::ostream &operator<<(std::ostream &o, Response const &response)
{
	o << BYELLOW << response.getResponse() << NC << std::endl;
	return (o);
}