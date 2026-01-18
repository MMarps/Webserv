/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/15 14:47:55 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request &req) : _req(req)
{
	_statutMessage.insert(std::make_pair(200, "OK"));
	_statutMessage.insert(std::make_pair(301, "Moved Permanently"));
	_statutMessage.insert(std::make_pair(302, "Found"));
	_statutMessage.insert(std::make_pair(400, "Bad Request"));
	_statutMessage.insert(std::make_pair(403, "Forbidden"));
	_statutMessage.insert(std::make_pair(404, "Not Found"));
	_statutMessage.insert(std::make_pair(405, "Method Not Allowed"));
	_statutMessage.insert(std::make_pair(500, "Internal Server Error"));
	_contentType.insert(std::make_pair("html", "text/html"));
	_contentType.insert(std::make_pair("css", "text/css"));
	_contentType.insert(std::make_pair("js", "text/javascript"));
	_contentType.insert(std::make_pair("png", "image/png"));
	_contentType.insert(std::make_pair("jpg", "image/jpeg"));
	_contentType.insert(std::make_pair("jpeg", "image/jpeg"));
	_contentType.insert(std::make_pair("gif", "image/gif"));
	_contentType.insert(std::make_pair("ico", "image/x-icon"));
	_contentType.insert(std::make_pair("mp4", "video/mp4"));
	_contentType.insert(std::make_pair("mp3", "audio/mpeg"));
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
		std::cout << "fait chiez ca passe : " << this->_req.getPath() << std::endl;
		this->_req.setErrorCode(404);
		return ;
	}
	std::istreambuf_iterator<char> it(file);
	std::istreambuf_iterator<char> end;
	std::vector<char> buffer(it, end);
	std::stringstream ss;
	ss << buffer.size();
	std::cout << GREEN << buffer.size() << NC << std::endl;
	this->_contentLength = ss.str();
	this->_content.swap(buffer);
}

std::vector<char> Response::getContent()
{
	return (this->_content);
}

void Response::makeRep(ServerConfig server)
{
	getDefaultResponse();
	if (this->_req.getErrorCode() == 200)
	{
		getContentExtention();
		getFullResponse();
	}
	else
	{
		if (!server.error_pages[this->_req.getErrorCode()].empty())
		{
			this->_req.setPath(server.error_pages[this->_req.getErrorCode()]);
			
			getContentExtention();
			getFullResponse();
		}
		
	}
}

void Response::getContentExtention()
{
	std::stringstream path(this->_req.getPath());
	std::string get;
	getline(path, get, '.');
	getline(path, get);
	this->_contentExtention = get;
}

void Response::getDefaultResponse()
{
	this->_response = "HTTP/1.1 ";
	getResponseCode();
}

void Response::getFullResponse()
{
	getDoc();
	this->_response += "\nContent-Type: "
		+ this->_contentType[this->_contentExtention];
	this->_response += "\nContent-Length: " + this->_contentLength;
	this->_response += "\n\n";
}

void Response::getResponseCode()
{
	std::stringstream errorCode;
	errorCode << this->_req.getErrorCode();
	this->_response += errorCode.str() + " "
		+ this->_statutMessage[this->_req.getErrorCode()];
}

std::ostream &operator<<(std::ostream &o, Response const &response)
{
	o << BYELLOW << response.getRep() << NC << std::endl;
	return (o);
}
