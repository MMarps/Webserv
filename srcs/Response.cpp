/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/13 03:05:27 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(std::string docPath)
{
    this->_version = "HTTP/1.0";
    this->_contentType = "text/html";
    getDoc(docPath);
    this->_response = this->_version + " " + this->_statutCode + " " 
        + this->_statutMessage + "\nContent-Type: " + this->_contentType 
        + "\nContent-Length: " + this->_contentLength + "\n\n" + this->_content;
}

Response::~Response()
{
}
void Response::getDoc(std::string docPath)
{
    std::string constPath = "test_doc/";
    std::string globalPath = constPath + docPath;
    std::ifstream file(globalPath.c_str());
    if (!file)
    {
        this->_statutCode = "404";
        this->_statutMessage = "not found";
    }
    else
    {
        this->_statutCode = "200";
        this->_statutMessage = "OK";
    }
	std::string line;
	while (getline(file, line))
		this->_content += line;
        std::cout << _content << std::endl;
	// this->_contentLength = std::to_string(this->_content.length());
    // a voir demain
}


std::string Response::getRep()
{
    return (this->_response);
}
