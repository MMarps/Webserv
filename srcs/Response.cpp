/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/14 16:59:49 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request& req) : _req(req)
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
	std::cout << this->_req.getPath() << std::endl;
	std::ifstream file(this->_req.getPath().c_str(), std::ios::binary);
	std::cout << RED << " ca passe 4" << NC << std::endl;
	
	if (!file.is_open())
	{
		this->_req.setErrorCode(404);
		return ;
	}
	std::cout << RED << " ca passe 5 " << this->_req.getPath() << NC << std::endl;
	std::istreambuf_iterator<char> it(file);
	std::cout << RED << " ca passe 1200" << NC << std::endl;

	std::istreambuf_iterator<char> end;
	std::cout << RED << " ca passe 1201" << NC << std::endl;

	std::vector<char> buffer(it, end);
	std::cout << RED << " ca passe 1202" << NC << std::endl;


	std::stringstream ss;
	ss << buffer.size();
	this->_contentLength = ss.str();
	this->_content.swap(buffer);
}

std::vector<char> Response::getContent()
{
	return (this->_content);
}


void Response::makeRep()
{
	getDefaultResponse();
	std::cout << RED << " ca passe 7" << NC << std::endl;
	if (this->_req.getErrorCode() == 200)
	{
		getContentExtention();
	std::cout << RED << " ca passe 8" << NC << std::endl;

		getFullResponse();
	std::cout << RED << " ca passe 9" << NC << std::endl;

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
	std::cout << RED << " ca passe 10" << NC << std::endl;
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
