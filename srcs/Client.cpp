/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:17:46 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/21 17:36:16 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clientFd, int serverIdx, const std::string &remoteAddr, int serverPort) 
		: isRequestFinished(false),	_fd(clientFd), _serverIdx(serverIdx),
		_remoteAddr(remoteAddr), _serverPort(serverPort) {}

Client::~Client() {}

//////////////////////////////////////

int Client::getFd() const {
	return (_fd);
}

int Client::getServerIdx() const {
	return (_serverIdx);
}

std::string &Client::getBuffer() {
	return (_buffer);
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

std::string	Client::getRemoteAddr() const {
	return (this->_remoteAddr);
}

int	Client::getServerPort() const {
	return (this->_serverPort);
}