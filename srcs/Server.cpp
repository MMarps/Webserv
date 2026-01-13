/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:18:11 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/13 19:06:10 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::string& confFileName) {
	Lexer	ts(confFileName);
	Parser	p(ts);

	// ts.printTokens();
	_conf = p.parseConfig();

	_epollFd = epoll_create(1);
	if (_epollFd < 0)
		throw ServerError("Epoll create failed");
}

Server::~Server() {
	if (_epollFd != -1)
		close(_epollFd);
}

/////////////////////////////////////

void	Server::_setupServerSockets() {
	for (size_t si = 0; si < _conf.servers.size(); si++) {
		std::vector<Listen>::iterator it;
		for (it = _conf.servers[si].listens.begin(); it != _conf.servers[si].listens.end(); it++) {
			int fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd < 0)
				throw ServerError("Socket: Failed to create socket");

			int opt = 1;
			setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

			_setNonBlocking(fd);

			struct sockaddr_in	addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_port = htons(it->port);

			if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
				std::ostringstream msg;
				msg << "Failed to bind, " << it->host << ":" << it->port;
				close(fd);
				throw ServerError(msg.str());
			}

			if (listen(fd, SOMAXCONN) < 0) {
				close(fd);
				throw ServerError("Failed to listen");
			}

			_addToEpoll(fd);
			_serveurSockets[fd] = si;
			std::cout << "Server listening on port " << it->port << " (epoll)" << std::endl;
		}
	}
}

void	Server::_setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		throw ServerError("fcntl: Failed to get flags");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw ServerError("fcntl: failed to set non blocking socket");
}

void	Server::_addToEpoll(int fd) {
	struct epoll_event	event;

	std::memset(&event, 0, sizeof(event));

	event.events = EPOLLIN;
	event.data.fd = fd;
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) < 0)
		throw ServerError("Epoll ctl failed");
	
}

/////////////////////////////////////

const Config&	Server::getConfig() const {
	return (_conf);
}