/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/19 17:38:54 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"


Response::Response()
{
    this->_statutMessage[200] = "OK";
    this->_statutMessage[301] = "Moved Permanently";
    this->_statutMessage[302] = "Found";
    this->_statutMessage[400] = "Bad Request";
    this->_statutMessage[403] = "Forbidden";
    this->_statutMessage[404] = "Not Found";
    this->_statutMessage[405] = "Method Not Allowed";
    this->_statutMessage[413] = "Payload Too Large";
    this->_statutMessage[500] = "Internal Server Error";
    
}

Response::~Response()
{
}



std::string Response::getRep() const
{
    return (this->_response);
}

void Response::getDoc(std::string docPath)
{
    std::string globalPath = docPath;
    std::ifstream file(globalPath.c_str());
    std::stringstream ss;
    std::string line;
    while (getline(file, line))
        this->_content += line;
    ss << this->_content.length();
    this->_contentLength = ss.str();
    
}

void Response::makeRep(Request request)
{
    std::stringstream errorCode;
    errorCode << request.getErrorCode();

    this->_version = "HTTP/1.1";
    this->_contentType = "text/html";
    getDoc(request.getPath());
    this->_response = this->_version + " " + errorCode.str() + " " + this->_statutMessage[request.getErrorCode()] + "\nContent-Type: " + this->_contentType 
        + "\nContent-Length: " + this->_contentLength + "\n\n" + this->_content;
}

std::ostream &operator<<(std::ostream &o, Response const &response)
{
    o << BYELLOW << response.getRep() << NC << std::endl;
    return (o);
}
