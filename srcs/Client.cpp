/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:17:46 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/02 17:54:35 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clientFd, int serverIdx) : isRequestFinished(false),
	_fd(clientFd), _serverIdx(serverIdx)
{
}

Client::~Client()
{
}

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
