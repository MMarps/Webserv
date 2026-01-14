/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/14 16:09:40 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(char *buffer)
{
	std::istringstream request(buffer);
	std::string line;
	std::string get;
	getline(request, line);
	std::istringstream cut(line);
	getline(cut, get, '/');
	this->_methode = get;
	getline(cut, get, ' ');
	this->_path = get;
	if (!this->_path.length())
	{
		this->_path = "index.html";
	}
	
	
	getline(cut, get, ' ');
	this->_version = get;
}

Request::~Request()
{
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

std::ostream &operator<<(std::ostream &o, Request const &request)
{
	o << BGREEN << request.getMethode() << request.getPath() <<  request.getVersion() << NC << std::endl;
	return (o);
}