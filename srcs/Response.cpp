/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/26 14:43:36 by jle-doua         ###   ########.fr       */
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
	std::ifstream file(this->_req.getPath().c_str(), std::ios::binary);
	if (!file.is_open())
	{
		this->_req.setErrorCode(404);
		return ;
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
	std::ifstream file(this->_req.getPath().c_str(), std::ios::binary);
	if (!file.is_open())
	{
		this->_req.setErrorCode(404);
		return ;
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

void Response::makeRep(ServerConfig server)
{
	getDefaultResponse();
	if (this->_req.getCode() == 200)
	{
		getContentExtention();
		getFullResponse();
	}
	else if(this->_req.getCode() == 301)
	{
		makeRedirect();
	}
	else
	{
		if (!server.error_pages[this->_req.getCode()].empty()
			&& access(server.error_pages[this->_req.getCode()].c_str(),
				F_OK) != -1)
		{
			this->_req.setPath(server.error_pages[this->_req.getCode()]);
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
	std::stringstream path(this->_req.getPath());
	std::string get;
	this->_contentExtention = this->_req.getPath().substr(this->_req.getPath().rfind('.'));
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
	this->_response += "\nContent-Type: "
		+ this->_contentType[this->_contentExtention];
	this->_response += "\nContent-Length: " + this->_contentLength;
	this->_response += "\n\n";
}

void Response::getResponseCode()
{
	std::stringstream errorCode;
	errorCode << this->_req.getCode();
	this->_response += errorCode.str() + " "
		+ this->_statutMessage[this->_req.getCode()];
}

std::ostream &operator<<(std::ostream &o, Response const &response)
{
	o << BYELLOW << response.getRep() << NC << std::endl;
	return (o);
}
