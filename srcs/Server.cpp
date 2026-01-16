/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:18:11 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/16 20:29:52 by mmarpaul         ###   ########.fr       */
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
			struct addrinfo hints;
			struct addrinfo *res;

			std::memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;

			std::stringstream ss;
			ss << it->port;
			std::string strPort = ss.str();

			int status = getaddrinfo(it->host.c_str(), strPort.c_str(), &hints, &res);
			if (status != 0) {
				freeaddrinfo(res);
				throw ServerError(gai_strerror(status));
			}

			int fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd < 0) {
				freeaddrinfo(res);
				throw ServerError("Socket: Failed to create socket");
			}

			int opt = 1;
			setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

			_setNonBlocking(fd);

			if (bind(fd, res->ai_addr, res->ai_addrlen) < 0) {
				freeaddrinfo(res);
				std::ostringstream msg;
				msg << "Failed to bind, " << it->host << ":" << it->port;
				close(fd);
				throw ServerError(msg.str());
			}

			freeaddrinfo(res);

			if (listen(fd, SOMAXCONN) < 0) {
				close(fd);
				throw ServerError("Failed to listen");
			}

			_addToEpoll(fd, EPOLLIN);
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

void	Server::_addToEpoll(int fd, uint32_t events) {
	struct epoll_event	event;

	std::memset(&event, 0, sizeof(event));

	event.events = events;
	event.data.fd = fd;
	
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) < 0)
		throw ServerError("Epoll ctl failed");
	
}

/////////////////////////////////////

void	Server::run() {
	_setupServerSockets();

	while (true) {
		int nfds = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		if (nfds < 0)
			throw ServerError("Epoll wait failed");

		for (int i = 0; i < nfds; i++) {
			int currentFd = _events[i].data.fd;
			uint32_t currentEvent = _events[i].events;

			if (currentEvent & (EPOLLHUP | EPOLLERR)) {
				_closeConnection(currentFd);
				continue ;
			}
			if (_serveurSockets.find(currentFd) != _serveurSockets.end())
				_addNewClient(currentFd);
			else if (currentEvent & EPOLLIN)
				_handleClientData(currentFd);
		}
	}
}

void	Server::_closeConnection(int fd) {
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	if (_clients.count(fd)) {
		delete _clients[fd];
		_clients.erase(fd);
	}
	std::cout << "Connection closed: " << fd << std::endl;
}

void	Server::_addNewClient(int serverFd) {
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);

	int clientFd = accept(serverFd, (sockaddr *)&clientAddr, &addrLen);
	if (clientFd < 0) {
		std::cerr << "Error accepting client" << std::endl;
		return ;
	}
	_setNonBlocking(clientFd);
	_addToEpoll(clientFd, EPOLLIN | EPOLLOUT);
	_clients[clientFd] = new Client(clientFd, _serveurSockets[serverFd]);
	std::cout << "New connection: " << clientFd << std::endl;
}

void	Server::_handleClientData(int clientFd) {
	char buf[BUFFER_SIZE];
	Client*	client = _clients[clientFd];
	ssize_t nbytes = recv(clientFd, buf, BUFFER_SIZE - 1, 0);
	if (nbytes <= 0)
		_closeConnection(clientFd);
	else {
		buf[nbytes] = '\0';
		client->getBuffer().append(buf, nbytes);
		if (client->getBuffer().find("\r\n\r\n") != std::string::npos) {
			client->isRequestFinished = true;
			std::cout << "Request received from " << clientFd << std::endl;
		}
	}
	// if (client->isRequestFinished == true) {
	// 	Request 	req;
	// 	req.parse(_conf.servers[_serveurSockets[clientFd]], client->getBuffer());
	// 	Response	response(req);
	// 	response.makeRep();

	// 	std::cout << "=== REQUEST ===" << std::endl;
	// 	std::cout << client->getBuffer() << std::endl;
	// 	std::cout << "=== RESPONSE ===" << std::endl;
	// 	std::cout << response.getRep() << std::endl;

	// 	if (send(clientFd, response.getRep().c_str(), response.getRep().size(), 0) == -1
	// 		|| send(clientFd, response.getContent().data(), response.getContent().size(), 0) == -1)
	// 	{
	// 		perror("send");
	// 	}
	// 	client->isRequestFinished = false;
	// }
}

/////////////////////////////////////

const Config&	Server::getConfig() const {
	return (_conf);
}