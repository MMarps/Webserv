/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arotondo <arotondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:17:46 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/16 15:55:21 by arotondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clientFd, int serverIdx, const std::string &remoteAddr, int serverPort)
	: isHeaderFinished(false),
	  isRequestFinished(false),
	  expectedBodySize(0),
	  actualBodySize(0),
	  _fd(clientFd),
	  _serverIdx(serverIdx),
	  _remoteAddr(remoteAddr),
	  _serverPort(serverPort) {}

Client::~Client() {}

//////////////////////////////////////

int Client::getFd() const {
	return (_fd);
}

int Client::getServerIdx() const {
	return (_serverIdx);
}

std::string& Client::getHeader() {
	return (_header);
}

std::string& Client::getResponse() {
	return (_response);
}

void Client::setBody(std::vector<char> body) {
	this->_body.swap(body);
}

std::vector<char>& Client::getBody() {
	return (this->_body);
}

size_t	Client::getBodySize() const {
	return (_body.size());
}

void	Client::appendBody(const char* buf, size_t size) {
	_body.insert(_body.end(), buf, buf + size);
}