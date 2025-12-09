/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:18:11 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/09 18:33:28 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::string& confFileName) {
	
}

Server::Server(const Server& other)
	: _port(other._port),
	  _root(other._root),
	  _index(other._index),
	  _serverName(other._serverName) {}

Server&	Server::operator=(const Server& other) {
	if (this != &other) {
		this->_port = other._port;
		this->_root = other._root;
		this->_index = other._index;
		this->_serverName = other._serverName;
	}
	return (*this);
}

Server::~Server() {}

