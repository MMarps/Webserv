/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:18:11 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/27 17:19:38 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::string &confFileName)
{
	Lexer ts(confFileName);
	Parser p(ts);
	// ts.printTokens();
	_conf = p.parseConfig();
	_epollFd = epoll_create(1);
	if (_epollFd < 0)
		throw ServerError("Epoll create failed");

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
}

Server::~Server()
{
	if (_epollFd != -1)
		close(_epollFd);
}

/////////////////////////////////////

void	signal_handler(int sig) {
	(void)sig;
	g_terminate = 1;
}

/////////////////////////////////////

void Server::_setupServerSockets()
{
	struct addrinfo								hints;
	struct addrinfo								*res;
	int											status;
	int											fd;
	int											opt;
	std::map<std::pair<std::string, int>, int>	bound;

	for (size_t si = 0; si < _conf.servers.size(); si++)
	{
		std::vector<Listen>::iterator it;
		for (it = _conf.servers[si].listens.begin(); it != _conf.servers[si].listens.end(); it++)
		{
			std::pair<std::string, int>	key(it->host, it->port);
			if (bound.count(key)) {
				_serveurSockets[bound[key]].push_back(si);
				std::cout << "Server " << "[" << si << "] listening on port "
						  << it->port << " (fd=" << fd << ")" << std::endl;
				continue ;
			}
			std::memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;
			std::stringstream ss;
			ss << it->port;
			std::string strPort = ss.str();
			status = getaddrinfo(it->host.c_str(), strPort.c_str(), &hints,
					&res);
			if (status != 0) {
				freeaddrinfo(res);
				_closeSocketFds();
				throw ServerError(gai_strerror(status));
			}
			fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd < 0) {
				freeaddrinfo(res);
				_closeSocketFds();
				throw ServerError("Socket: Failed to create socket");
			}
			opt = 1;
			setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

			if (_setNonBlocking(fd) != 0) {
				freeaddrinfo(res);
				close(fd);
				continue ;
			}

			if (bind(fd, res->ai_addr, res->ai_addrlen) < 0) {
				int save_errno = errno;
				freeaddrinfo(res);
				close(fd);
				if (save_errno == EADDRINUSE) {
					std::pair<std::string, int>	wildcardKey("*", it->port);
					if (bound.count(wildcardKey)) {
						_serveurSockets[bound[wildcardKey]].push_back(si);
						std::cout << "Server " << "[" << si << "] listening on port "
					  			  << it->port << " (fd=" << fd << ")" << std::endl;
						continue ;
					}
				}
				std::ostringstream msg;
				msg << "Failed to bind, " << it->host << ":" << it->port
					<< " errno=" << save_errno << " " << strerror(save_errno);
				_closeSocketFds();
				throw ServerError(msg.str());
			}
			freeaddrinfo(res);
			if (listen(fd, SOMAXCONN) < 0) {
				close(fd);
				_closeSocketFds();
				throw ServerError("Failed to listen");
			}
			_addToEpoll(fd, EPOLLIN);
			_serveurSockets[fd].push_back(si);
			std::cout << "Server " << "[" << si << "] listening on port "
					  << it->port << " (fd=" << fd << ")" << std::endl;
			bound[key] = fd;
		}
	}
}

int Server::_setNonBlocking(int fd)
{
	int	flags;

	flags = fcntl(fd, F_GETFL);
	if (flags == -1) {
		perror("fcntl");
		return (1);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl");
		return (2);
	}
	return (0);
}

int Server::_addToEpoll(int fd, uint32_t events)
{
	struct epoll_event	event;

	std::memset(&event, 0, sizeof(event));
	event.events = events;
	event.data.fd = fd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) < 0) {
		perror("epoll_ctl");
		return (1);
	}
	return (0);
}

int Server::_modEpoll(int fd, uint32_t newEvents)
{
	struct epoll_event	event;

	std::memset(&event, 0, sizeof(event));
	event.events = newEvents;
	event.data.fd = fd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &event) < 0) {
		perror("epoll_ctl");
		return (1);
	}
	return (0);
}

/////////////////////////////////////

void Server::run()
{
	int			nfds;
	int			currentFd;
	uint32_t	currentEvent;

	_setupServerSockets();
	std::cout << BGREEN << "\nServer Ready\n" << NC << std::endl; 
	while (true)
	{
		nfds = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		if (nfds < 0) {
			if (errno == EINTR && g_terminate) {
				std::cout << " Signal received, shutdown asked" << std::endl;
				_closeAllClients();
				_closeSocketFds();
				break ;
			}
			else {
				perror("epoll_wait");
				continue ;
			}
		}
		for (int i = 0; i < nfds; i++)
		{
			currentFd = _events[i].data.fd;
			currentEvent = _events[i].events;
			if (currentEvent & (EPOLLHUP | EPOLLERR))
			{
				_closeConnection(currentFd);
				continue ;
			}
			if (_serveurSockets.find(currentFd) != _serveurSockets.end())
				_addNewClient(currentFd);
			else if (currentEvent & EPOLLIN)
				_handleClientData(currentFd);
			else if (currentEvent & EPOLLOUT)
				_sendResponse(currentFd);
		}
	}
	if (g_terminate)
		std::cout << BGREEN << "Shutdown complete" << NC << std::endl;
}

void Server::_closeConnection(int fd)
{
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
		perror("epoll_ctl");

	close(fd);
	delete _clients[fd];

	if (_clients.count(fd))
		_clients.erase(fd);

	// Nettoyer les métadonnées réseau
	if (_clientMetadata.count(fd))
		_clientMetadata.erase(fd);

	std::cout << "Connection closed: " << fd << std::endl;
}

void Server::_addNewClient(int serverFd)
{
	struct sockaddr_in	clientAddr;
	socklen_t			addrLen;
	int					clientFd;

	addrLen = sizeof(clientAddr);
	clientFd = accept(serverFd, (sockaddr *)&clientAddr, &addrLen);
	if (clientFd < 0) {
		std::cerr << "Error accepting client: " << strerror(errno) << std::endl;
		return ;
	}

	std::string	remoteAddr = inet_ntoa(clientAddr.sin_addr);
	int			serverPort = _conf.servers[_serveurSockets[serverFd][0]].listens[0].port;

	_setNonBlocking(clientFd);
	_addToEpoll(clientFd, EPOLLIN);
	_clients[clientFd] = new Client(clientFd, _serveurSockets[serverFd][0], remoteAddr, serverPort);
	_clientMetadata[clientFd] = std::make_pair(remoteAddr, serverPort); // stocker la map pour la passer a request dans parseResponse

	std::cout << "New connection: " << clientFd <<" from " << remoteAddr << std::endl;
}

void	Server::_handleClientData(int clientFd) {
	char	buf[BUFFER_SIZE];
	Client	*client;
	ssize_t	nbytes;

	client = _clients[clientFd];
	nbytes = recv(clientFd, buf, BUFFER_SIZE - 1, 0);
	if (nbytes <= 0)
	{
		_closeConnection(clientFd);
		return ;
	}
	buf[nbytes] = '\0';
	client->getBuffer().append(buf, nbytes);
	if (client->getBuffer().find("\r\n\r\n") != std::string::npos)
	{
		client->isRequestFinished = true;
		std::cout << "Request received completely." << std::endl;
		_parseResponse(client);
		_modEpoll(clientFd, EPOLLOUT);
	}
}

void	Server::_parseResponse(Client *c) {
	Request	req;

	int	clientFd = c->getFd();
	if (_clientMetadata.find(clientFd) != _clientMetadata.end()) { // transmettre les metadata reseau a Request
		req.setRemoteAddr(_clientMetadata[clientFd].first);
		req.setServerPort(_clientMetadata[clientFd].second);
	}

	req.parse(_conf.servers[c->getServerIdx()], c->getBuffer(), 0);
	Response response(req);
	response.makeRep(this->_conf.servers[c->getServerIdx()]);
	c->getResponse().append(response.getRep());

	std::cout << BRED << req << NC << std::endl;
	std::cout  << BBLUE << response << NC << std::endl;

	const std::vector<char> &content = response.getContent();
	if (!content.empty()) {
		c->getResponse().append(content.data(), content.size());
	}
}

void Server::_sendResponse(int clientFd)
{
	Client	*client;
	ssize_t	sent;

	client = _clients[clientFd];
	std::string &resp = client->getResponse();
	sent = send(client->getFd(), resp.c_str(), resp.size(), 0);
	if (sent == -1)
	{
		perror("send");
		_closeConnection(clientFd);
		return ;
	}
	if (static_cast<size_t>(sent) >= resp.size())
	{
		std::cout << "Response sent fully." << std::endl;
		client->getBuffer().clear();
		client->getResponse().clear();
		client->isRequestFinished = false;
		_modEpoll(clientFd, EPOLLIN);
	}
	else
	{
		resp = resp.substr(sent);
	}
}

/////////////////////////////////////

void	Server::_closeSocketFds() {
	std::map<int, std::vector<int> >::const_iterator it;

	if (_serveurSockets.empty())
		return ;
	for (it = _serveurSockets.begin(); it != _serveurSockets.end(); ++it)
	{
		int fd = it->first;
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
	}
	_serveurSockets.clear();
	std::cout << "All sockets closed" << std::endl;
}

void	Server::_closeAllClients() {
	std::map<int, Client*>::const_iterator it;

	if (_clients.empty())
		return ;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		int fd = it->first;
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		delete it->second;
	}
	_clients.clear();
	std::cout << "All clients disconnected" << std::endl;
}

/////////////////////////////////////

const Config &Server::getConfig() const
{
	return (_conf);
}