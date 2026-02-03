/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:17:46 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/03 19:40:24 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clientFd, int serverIdx)
	: isHeaderFinished(false),
	  isRequestFinished(false),
	  expectedBodySize(0),
	  _fd(clientFd),
	  _serverIdx(serverIdx) {}

Client::~Client() {}

//////////////////////////////////////

int Client::getFd() const {
	return (_fd);
}

int Client::getServerIdx() const {
	return (_serverIdx);
}

std::string &Client::getHeader() {
	return (_header);
}

std::string &Client::getResponse() {
	return (_response);
}

void Client::setBody(std::vector<char> body) {
	this->_body.swap(body);
}

std::vector<char> Client::getBody() {
	return (this->_body);
}

void	Client::appendBody(const char* buf, size_t size) {
	_body.insert(_body.end(), buf, buf + size);
}