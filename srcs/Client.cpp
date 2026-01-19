/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:17:46 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/19 01:08:43 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int clientFd, int serverIdx)
	: isRequestFinished(false),
	  _fd(clientFd),
	  _serverIdx(serverIdx) {}

Client::~Client() {}

//////////////////////////////////////

int	Client::getFd() const {
	return (_fd);
}

int	Client::getServerIdx() const {
	return (_serverIdx);
}

std::string&	Client::getBuffer() {
	return (_buffer);
}

std::string&	Client::getResponse() {
	return (_response);
}

//////////////////////////////////////

std::ostream &operator<<(std::ostream &o, Client& c) {
	o << "-------------------------" << std::endl
	  << "Client <" << c.getFd() << ">" << std::endl
	  << "Server Idx -> " << c.getServerIdx() << std::endl
	  << "Buffer:\n" << c.getBuffer().c_str() << std::endl
	  << "Response:\n" << c.getResponse().c_str() << std::endl
	  << "-------------------------" << std::endl;

	return o;
}