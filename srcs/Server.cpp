/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:18:11 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/14 16:34:59 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

Server::Server(const std::string &confFileName) {
	Lexer ts(confFileName);
	Parser p(ts);
	// ts.printTokens();
	_conf = p.parseConfig();
	_epollFd = epoll_create(1);
	if (_epollFd < 0)
		throw ServerError("Epoll create failed");

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	Logger::init(_conf.servers);
	Logger::log("Logger initialised");
}

Server::~Server() {
	if (_epollFd != -1)
		close(_epollFd);
}

/////////////////////////////////////

void	signal_handler(int sig) {
	(void)sig;
	g_terminate = 1;
}

/////////////////////////////////////

void Server::_setupServerSockets() {
	struct addrinfo								hints;
	struct addrinfo								*res;
	int											status;
	int											fd;
	int											opt;
	std::map<std::pair<std::string, int>, int>	bound;
	std::ostringstream 							oss;

	for (size_t si = 0; si < _conf.servers.size(); si++) {
		std::vector<Listen>::iterator it;
		for (it = _conf.servers[si].listens.begin(); it != _conf.servers[si].listens.end(); it++) {
			std::pair<std::string, int>	key(it->host, it->port);
			if (bound.count(key)) {
				_serveurSockets[bound[key]].push_back(si);
				oss << "Listening on port " << it->port << " (fd=" << fd << ")";
				Logger::info(oss.str(), si);
				oss.str("");
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
						oss << "Listening on port " << it->port << " (fd=" << fd << ")";
						Logger::info(oss.str(), si);
						oss.str("");
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

			oss << "Listening on port " << it->port << " (fd=" << fd << ")";
			Logger::info(oss.str(), si);
			oss.str("");

			bound[key] = fd;
		}
	}
}

int Server::_setNonBlocking(int fd) {
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

int Server::_addToEpoll(int fd, uint32_t events) {
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

int Server::_modEpoll(int fd, uint32_t newEvents) {
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

void Server::run() {
	int					nfds;
	int					currentFd;
	uint32_t			currentEvent;

	_setupServerSockets();
	Logger::log("Server Ready");
	while (true) {
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
			if (currentEvent & (EPOLLHUP | EPOLLERR)) {
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
	if (g_terminate) {
		Logger::log("Shutdown complete\n");
	}
}

void Server::_closeConnection(int fd) {
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
		perror("epoll_ctl");

	close(fd);
	delete _clients[fd];

	if (_clients.count(fd))
		_clients.erase(fd);

	std::cout << "Connection closed: " << fd << std::endl;
}

void Server::_addNewClient(int serverFd) {
	struct sockaddr_in	clientAddr;
	socklen_t			addrLen;
	int					clientFd;

	addrLen = sizeof(clientAddr);
	clientFd = accept(serverFd, (sockaddr *)&clientAddr, &addrLen);
	if (clientFd < 0) {
		std::cerr << "Error accepting client: " << strerror(errno) << std::endl;
		return ;
	}
	_setNonBlocking(clientFd);
	_addToEpoll(clientFd, EPOLLIN);
	_clients[clientFd] = new Client(clientFd, _serveurSockets[serverFd][0]);
	std::cout << "New connection: " << clientFd << std::endl;
}

void Server::_handleClientData(int clientFd) {
	char	buf[BUFFER_SIZE];
	Client	*client;
	ssize_t	nbytes;

	client = _clients[clientFd];
	nbytes = recv(clientFd, buf, BUFFER_SIZE - 1, 0);
	if (nbytes <= 0) {
		_closeConnection(clientFd);
		return ;
	}
	buf[nbytes] = '\0';
	if (!client->isHeaderFinished) {
		client->getHeader().append(buf, nbytes);
		size_t pos = client->getHeader().find("\r\n\r\n");
		if (pos != std::string::npos) {
			client->isHeaderFinished = true;

			client->expectedBodySize = _extractContentLen(client->getHeader());
			long maxSize = _getLocationMaxBodySize(client);

			std::cout << BRED << "expectedBodySize = " << client->expectedBodySize << std::endl
					  << "maxSize = " << maxSize << NC << std::endl;

			if (client->expectedBodySize > static_cast<size_t>(maxSize)) {
				_parseResponse(client, 413);
				_modEpoll(clientFd, EPOLLOUT);
				return ;
			}

			size_t headerFullSize = pos + 4;
			if (client->getHeader().size() > headerFullSize) {
				size_t extraSize = client->getHeader().size() - headerFullSize;
				client->appendBody(client->getHeader().data() + headerFullSize, extraSize);
				client->getHeader().resize(headerFullSize);
				client->actualBodySize += extraSize;
			}
		}
	}
	else {
		client->actualBodySize += nbytes;
		client->appendBody(buf, nbytes);
		if (client->actualBodySize > client->expectedBodySize) {
			_parseResponse(client, 413);
			_modEpoll(clientFd, EPOLLOUT);
			return ;
		}
	}
	if (client->isHeaderFinished) {
		if (client->getBody().size() >= client->expectedBodySize) {
			std::cout << "Request received completely." << std::endl;
			client->isRequestFinished = true;
            _parseResponse(client, 200);
            _modEpoll(clientFd, EPOLLOUT);
		}
	}
}

void Server::_parseResponse(Client *c, int errCode) {
	Request	req;

	req.parse(_conf.servers[c->getServerIdx()], c->getHeader(), errCode);
	Response response(req);
	response.makeRep();
	c->getResponse().append(response.getResponse());

	std::cout  << BBLUE << response << NC << std::endl;

	const std::vector<char> &content = response.getContent();
	if (!content.empty()) {
		c->getResponse().append(content.data(), content.size());
	}
}

void Server::_sendResponse(int clientFd) {
	Client	*client;
	ssize_t	sent;

	client = _clients[clientFd];
	std::string &resp = client->getResponse();
	sent = send(client->getFd(), resp.c_str(), resp.size(), 0);
	if (sent == -1) {
		perror("send");
		_closeConnection(clientFd);
		return ;
	}
	if (static_cast<size_t>(sent) >= resp.size()) {
		std::cout << "Response sent fully." << std::endl;
		client->getHeader().clear();
		client->getResponse().clear();
		client->getBody().clear();
		client->isHeaderFinished = false;
		client->isRequestFinished = false;
		_modEpoll(clientFd, EPOLLIN);
	}
	else {
		resp = resp.substr(sent);
	}
}

/////////////////////////////////////

long	Server::_extractContentLen(const std::string& header) {
	std::string search = "Content-Length: ";
	size_t pos = header.find(search);

	if (pos == header.npos)
		return (0);

	size_t start = pos + search.length();
	size_t end = header.find("\r\n", start);

	if (end == header.npos)
		return (0);
	
	std::string val = header.substr(start, end - start);

	return (std::atol(val.c_str()));
}

long	Server::_getLocationMaxBodySize(Client* client) {
	std::string &buf = client->getHeader();
	size_t firstLineEnd = buf.find("\r\n");
	if (firstLineEnd == std::string::npos)
		return (_conf.servers[client->getServerIdx()].client_max_body_size);

	std::string firstLine = buf.substr(0, firstLineEnd);
	std::stringstream ss(firstLine);
	std::string method, uri, version;
	ss >> method >> uri >> version;

	const LocationConfig* loc = _findBestLocation(uri, client->getServerIdx());
	if (loc)
		return loc->client_max_body_size;
	else
		return (_conf.servers[client->getServerIdx()].client_max_body_size);
	}

const LocationConfig*	Server::_findBestLocation(const std::string& uri, int serverIdx) {
	const ServerConfig& conf = _conf.servers[serverIdx];
	const LocationConfig* bestMatch = NULL;
	size_t longestMatch = 0;

	for (size_t i = 0; i < conf.locations.size(); ++i) {
		const std::string& locPath = conf.locations[i].path;
		if (uri.compare(0, locPath.length(), locPath) == 0) {
			if (locPath.length() >= longestMatch) {
				longestMatch = locPath.length();
				bestMatch = &conf.locations[i];
			}
		}
	}
	return bestMatch;
}

/////////////////////////////////////

void	Server::_closeSocketFds() {
	std::map<int, std::vector<int> >::const_iterator it;

	if (_serveurSockets.empty())
		return ;
	for (it = _serveurSockets.begin(); it != _serveurSockets.end(); ++it) {
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

const Config &Server::getConfig() const {
	return (_conf);
}