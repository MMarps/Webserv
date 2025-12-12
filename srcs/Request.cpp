/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 14:32:12 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/12 15:46:57 by jle-doua         ###   ########.fr       */
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
	getline(cut, get, ' ');
	this->_methode = get;
	getline(cut, get, ' ');
	this->_path = get;
}

Request::~Request()
{
}

std::string Request::getMethode()
{
	return (this->_methode);
}

std::string Request::getPath()
{
	return (this->_path);
}

std::string Request::getVersion()
{
	return (this->_version);
}