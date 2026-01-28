/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/28 18:11:33 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request &req) : _req(req)
{
	_statutMessage.insert(std::make_pair(200, "OK"));
	_statutMessage.insert(std::make_pair(201, "Created"));
	_statutMessage.insert(std::make_pair(301, "Moved Permanently"));
	_statutMessage.insert(std::make_pair(302, "Found"));
	_statutMessage.insert(std::make_pair(400, "Bad Request"));
	_statutMessage.insert(std::make_pair(403, "Forbidden"));
	_statutMessage.insert(std::make_pair(404, "Not Found"));
	_statutMessage.insert(std::make_pair(405, "Method Not Allowed"));
	_statutMessage.insert(std::make_pair(409, "Conflict"));
	_statutMessage.insert(std::make_pair(500, "Internal Server Error"));
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
}

Response::~Response()
{
}

std::string Response::getRep() const
{
	return (this->_response);
}

void Response::getDoc()
{
	std::ifstream file(this->_req.getCompletPath().c_str(), std::ios::binary);
	if (!file.is_open())
	{
		this->_req.setErrorCode(404);
		return;
	}

	std::istreambuf_iterator<char> it(file);
	std::istreambuf_iterator<char> end;
	std::vector<char> buffer(it, end);
	std::stringstream ss;

	ss << buffer.size();
	this->_contentLength = ss.str();
	this->_content.swap(buffer);
}

void Response::checkDoc()
{
	std::ifstream file(this->_req.getCompletPath().c_str(), std::ios::binary);
	if (!file.is_open())
	{
		this->_req.setErrorCode(404);
		return;
	}
	std::istreambuf_iterator<char> it(file);
	std::istreambuf_iterator<char> end;
	std::vector<char> buffer(it, end);
	std::stringstream ss;
	ss << buffer.size();
	this->_contentLength = ss.str();
}

std::vector<char> Response::getContent()
{
	return (this->_content);
}

void Response::makeRedirect()
{
	this->_response += "\nLocation: " + this->_req.getPath();
	this->_response += "\nConnection: close";
	this->_response += "\nContent-Length: 0";
	this->_response += "\n\n";
}

void Response::generateAutoindex()
{
	DIR *folder;
	struct dirent *readFolder;
	folder = opendir(this->_req.getCompletPath().c_str());
	if (!folder)
	{
		std::cout << BRED << "NOP" << NC << std::endl;
		this->_req.setErrorCode(404);
	}
	else
	{
		std::string htmlpage;
		std::vector<std::string> lstFiles;
		htmlpage = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body>";

		readFolder = readdir(folder);
		while (readFolder)
		{
			if (strcmp(readFolder->d_name, ".") != 0 && strcmp(readFolder->d_name, "..") != 0)
				lstFiles.push_back(readFolder->d_name);
			readFolder = readdir(folder);
		}
		std::sort(lstFiles.begin(), lstFiles.end());

		for (long unsigned int i = 0; i < lstFiles.size(); i++)
		{
			htmlpage += "<a href=\"" + this->_req.getPath()+ "/" + lstFiles[i] + "\">" + lstFiles[i] + "</a></br>";
		}
		htmlpage += "</body></html>";
		std::vector<char> tmp(htmlpage.begin(), htmlpage.end());
		this->_content.swap(tmp);
		std::stringstream ss;

		ss << htmlpage.size();
		this->_contentLength = ss.str();
		std::cout << BBLUE << this->_contentLength << NC << std::endl;
	}
}

void Response::makeRep(ServerConfig server)
{
	getDefaultResponse();
	if (this->_req.getCode() == 200)
	{
		if (this->_req.getIsLocation())
		{
			std::cout << "ca passe" << std::endl;
			std::vector<LocationConfig>::iterator location = server.locations.begin();
			std::cout << location->path << " " << this->_req.getPath() << std::endl;
			for (; location < server.locations.end(); location++)
				if (location->path == this->_req.getPath())
					break;
			if (location == server.locations.end())
			{
				std::cout << " ca passe 2" << std::endl;
				if (!server.error_pages[this->_req.getCode()].empty())
				{

					this->_req.setPath(server.error_pages[this->_req.getCode()]);
					this->_req.setCompletPath(this->_req.getPath());
					getContentExtention();
					getFullResponse();
				}
				else
				{
					this->_response += "\nContent-Length: 0";
					this->_response += "\n\n";
				}
				return;
			}

			generateAutoindex();
			this->_response += "\nContent-Type: " + this->_contentType[".html"];
			this->_response += "\nContent-Length: " + this->_contentLength;
			this->_response += "\n\n";
		}
		else
		{
			getContentExtention();
			getFullResponse();
		}
	}
	else if (this->_req.getCode() == 301)
	{
		makeRedirect();
	}
	else
	{

		if (!server.error_pages[this->_req.getCode()].empty())
		{

			this->_req.setPath(server.error_pages[this->_req.getCode()]);
			this->_req.setCompletPath(this->_req.getPath());
			getContentExtention();
			getFullResponse();
		}
		else
		{
			this->_response += "\nContent-Length: 0";
			this->_response += "\n\n";
		}
	}
}

void Response::getContentExtention()
{
	std::stringstream path(this->_req.getCompletPath());
	std::string get;
	this->_contentExtention = this->_req.getCompletPath().substr(this->_req.getCompletPath().rfind('.'));
}

void Response::getDefaultResponse()
{
	this->_response = "HTTP/1.1 ";
	getResponseCode();
}

void Response::getFullResponse()
{
	if (this->_req.getMethode() == "HEAD")
		checkDoc();
	else
		getDoc();
	this->_response += "\nContent-Type: " + this->_contentType[this->_contentExtention];

	this->_response += "\nContent-Length: " + this->_contentLength;
	this->_response += "\n\n";
}

void Response::getResponseCode()
{
	std::stringstream errorCode;
	errorCode << this->_req.getCode();
	this->_response += errorCode.str() + " " + this->_statutMessage[this->_req.getCode()];
}

std::ostream &operator<<(std::ostream &o, Response const &response)
{
	o << BYELLOW << response.getRep() << NC << std::endl;
	return (o);
}
