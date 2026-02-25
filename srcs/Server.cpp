/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:18:11 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/25 16:59:48 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::string &confFileName)
	: _conf(),
	  _serverSockets(),
	  _serverPorts(),
	  _clients(),
	  _clientMetadata() {

	Lexer ts(confFileName);
	Parser p(ts);
	ts.printTokens();
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
	struct addrinfo		hints;
	struct addrinfo		*res;
	int					status;
	int					fd;
	int					opt;
	std::ostringstream 	oss;

	for (size_t si = 0; si < _conf.servers.size(); si++) {
		std::vector<Listen>::iterator it;
		for (it = _conf.servers[si].listens.begin(); it != _conf.servers[si].listens.end(); it++) {
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
			_serverSockets[fd] = si;
			_serverPorts[fd] = it->port;

			oss << "Listening on port " << it->port << " (fd=" << fd << ")";
			Logger::info(oss.str(), si);
			oss.str("");
		}
	}
}

int	Server::_setNonBlocking(int fd) {
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

int	Server::_addToEpoll(int fd, uint32_t events) {
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

int	Server::_modEpoll(int fd, uint32_t newEvents) {
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

void	Server::run() {
	int			nfds;
	int			currentFd;
	uint32_t	currentEvent;

	_setupServerSockets();
	Logger::log("Server Ready");
	while (true) {
		nfds = epoll_wait(_epollFd, _events, MAX_EVENTS, -1);
		if (nfds < 0) {
			if (errno == EINTR && g_terminate) {
				std::cout << std::endl;
				Logger::info("Signal received, shutdown asked");
				_closeAllClients();
				_closeSocketFds();
				break ;
			}
			else {
				perror("epoll_wait");
				continue ;
			}
		}
		for (int i = 0; i < nfds; i++) {
			currentFd = _events[i].data.fd;
			currentEvent = _events[i].events;
			if (currentEvent & (EPOLLHUP | EPOLLERR)) {
				_closeConnection(currentFd);
				continue ;
			}
			if (_serverSockets.find(currentFd) != _serverSockets.end())
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
	int					srvIdx;
	std::stringstream	oss;
	Client*				client = _clients[fd];

	srvIdx = client->getServerIdx();
	oss << "Connection closed: " << client->getAllInfos();

	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
		perror("epoll_ctl");
	close(fd);
	delete client;

	if (_clients.count(fd))
		_clients.erase(fd);

	if (_clientMetadata.count(fd)) // Nettoyer les metadonnees reseau
		_clientMetadata.erase(fd);

	Logger::info(oss.str(), srvIdx);
}

void	Server::_addNewClient(int serverFd) {
	struct sockaddr_in	clientAddr;
	socklen_t			addrLen;
	int					clientFd;
	std::stringstream	oss;

	memset(&clientAddr, 0, sizeof(clientAddr));
	addrLen = sizeof(clientAddr);
	clientFd = accept(serverFd, (sockaddr *)&clientAddr, &addrLen);
	if (clientFd < 0) {
		oss << "Accepting client: " << strerror(errno) << ", on port: " << _serverPorts[serverFd];
		Logger::error(oss.str(), serverFd);
		return ;
	}

	std::string	remoteAddr = _getClientAddr(clientAddr);
	int			serverPort = _serverPorts[serverFd];

	_setNonBlocking(clientFd);
	_addToEpoll(clientFd, EPOLLIN);
	_clients[clientFd] = new Client(clientFd, _serverSockets[serverFd], remoteAddr, serverPort);
	_clientMetadata[clientFd] = std::make_pair(remoteAddr, serverPort); // stocker la map pour la passer a request dans parseResponse
	
	oss << "New connection: " << _clients[clientFd]->getAllInfos();
	Logger::info(oss.str(), _serverSockets[serverFd]);
}

void	Server::_handleClientData(int clientFd) {
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

			if (client->expectedBodySize > static_cast<size_t>(maxSize)) {
				_parseResponse(client, 413);
				_modEpoll(clientFd, EPOLLOUT);
				return ;
			}

			_setUploadStream(client);

			size_t headerFullSize = pos + 4;
			if (client->getHeader().size() > headerFullSize) {
				size_t extraSize = client->getHeader().size() - headerFullSize;
				const char* bodyData = client->getHeader().data() + headerFullSize;

				if (client->isUpload && client->uploadStream.is_open())
					client->uploadStream.write(bodyData, extraSize);
				else
					client->appendBody(bodyData, extraSize);

				client->getHeader().resize(headerFullSize);
				client->actualBodySize += extraSize;
			}
		}
	}
	else {
		client->actualBodySize += nbytes;
		if (client->actualBodySize > client->expectedBodySize) {
			_parseResponse(client, 413);
			_modEpoll(clientFd, EPOLLOUT);
			return ;
		}
		if (client->isUpload && client->uploadStream.is_open())
			client->uploadStream.write(buf, nbytes);
		else
			client->appendBody(buf, nbytes);
	}
	if (client->isHeaderFinished && (client->actualBodySize >= client->expectedBodySize)) {
        if (client->isUpload && client->uploadStream.is_open()) {
            client->uploadStream.close();
            
            if (client->isMultipart) {
                _processMultipart(client);
                client->isMultipart = false;
            } 

            client->isUpload = false;
            client->uploadFileName = "";
            
            Logger::info("Upload completed successfully.", client->getServerIdx());
            _parseResponse(client, 201);
        }
        else {
            Logger::info("Request received completely.", client->getServerIdx());
            _parseResponse(client, 200);
        }
        
        client->isRequestFinished = true;
        _modEpoll(clientFd, EPOLLOUT);
    }
}

void	Server::_parseResponse(Client *c, int errCode) {
	Request	req;
	int		clientFd = c->getFd();

	if (_clientMetadata.find(clientFd) != _clientMetadata.end()) { // transmettre les metadata reseau a Request
		req.setRemoteAddr(_clientMetadata[clientFd].first);
		req.setServerPort(_clientMetadata[clientFd].second);
	}

	req.parse(_conf.servers[c->getServerIdx()], c->getHeader(), errCode);
	Response	response(req);
	response.makeRep(this->_conf.servers[c->getServerIdx()]);
	c->getResponse().append(response.getResponse());
	const std::vector<char>	&content = response.getContent();

	if (!content.empty())
		c->getResponse().append(content.data(), content.size());
}

void	Server::_sendResponse(int clientFd) {
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
		// std::cout << "Response sent fully." << std::endl;
		Logger::info("Response sent fully.", client->getServerIdx());
		client->getHeader().clear();
		client->getResponse().clear();
		client->getBody().clear();
		client->isHeaderFinished = false;
		client->isRequestFinished = false;
		_modEpoll(clientFd, EPOLLIN);
	}
	else
		resp = resp.substr(sent);
}

/////////////////////////////////////

long	Server::_extractContentLen(const std::string& header) {
	std::string	search = "Content-Length: ";
	size_t		pos = header.find(search);

	if (pos == header.npos)
		return (0);
	size_t		start = pos + search.length();
	size_t		end = header.find("\r\n", start);

	if (end == header.npos)
		return (0);	
	std::string	val = header.substr(start, end - start);

	return (std::atol(val.c_str()));
}

long	Server::_getLocationMaxBodySize(Client* client) {
	std::string	&buf = client->getHeader();
	size_t		firstLineEnd = buf.find("\r\n");
	if (firstLineEnd == std::string::npos)
		return (_conf.servers[client->getServerIdx()].client_max_body_size);

	std::string	firstLine = buf.substr(0, firstLineEnd);
	std::string	method, uri, version;
	std::stringstream	ss(firstLine);
	ss >> method >> uri >> version;

	const LocationConfig* loc = _findBestLocation(uri, client->getServerIdx());
	if (loc)
		return (loc->client_max_body_size);
	else
		return (_conf.servers[client->getServerIdx()].client_max_body_size);
}

const LocationConfig	*Server::_findBestLocation(const std::string& uri, int serverIdx) {
	const ServerConfig		&conf = _conf.servers[serverIdx];
	const LocationConfig	*bestMatch = NULL;
	size_t					longestMatch = 0;

	for (size_t i = 0; i < conf.locations.size(); ++i) {
		const std::string	&locPath = conf.locations[i].path;
		if (uri.compare(0, locPath.length(), locPath) == 0) {
			if (locPath.length() >= longestMatch) {
				longestMatch = locPath.length();
				bestMatch = &conf.locations[i];
			}
		}
	}
	return (bestMatch);
}

std::string	Server::_getClientAddr(const struct sockaddr_in& clientAddr) {
	unsigned int ip = ntohl(clientAddr.sin_addr.s_addr);

	std::stringstream	oss;
	oss << ((ip >> 24) & 0xFF) << '.'
		<< ((ip >> 16) & 0xFF) << '.'
		<< ((ip >> 8) & 0xFF) << '.'
		<< (ip & 0xFF);

	return (oss.str());
}

/////////////////////////////////////

void	Server::_setUploadStream(Client* client) {
	std::string &header = client->getHeader();

	size_t ctPos = header.find("Content-Type: ");
	if (ctPos != std::string::npos) {
		size_t ctEnd = header.find("\r\n", ctPos);
		std::string ctLine = header.substr(ctPos, ctEnd - ctPos);

		if (ctLine.find("multipart/form-data") != std::string::npos) {
			size_t bPos = ctLine.find("boundary=");
			if (bPos != std::string::npos) {
				client->isMultipart = true;
				client->boundary = "--" + ctLine.substr(bPos + 9);
				client->boundaryEnd = client->boundary + "--";
			}
		}
	}

	size_t firstLineEnd = header.find("\r\n");
	if (firstLineEnd == std::string::npos) return;

	std::string firstLine = header.substr(0, firstLineEnd);
	std::string method, uri, version;
	std::stringstream ss(firstLine);
	ss >> method >> uri >> version;

	if (method != "POST") return;

	const LocationConfig* loc = _findBestLocation(uri, client->getServerIdx());
	if (loc && !loc->upload_store.empty()) {
		client->isUpload = true;

		std::string fullpath = loc->upload_store;
		if (fullpath[fullpath.length() - 1] != '/')
			fullpath += '/';

		if (client->isMultipart) {
			std::stringstream oss;
			oss << fullpath << ".tmp_upload_" << client->getFd();
			client->uploadFileName = oss.str();
		}
		else {
			std::string filename = "default_upload";
			size_t lastSlash = uri.rfind('/');
			if (lastSlash != std::string::npos && lastSlash + 1 < uri.length())
				filename = uri.substr(lastSlash + 1);
			client->uploadFileName = fullpath + filename;
		}

		client->uploadStream.open(client->uploadFileName.c_str(), std::ios::out | std::ios::binary);
		
		if (!client->uploadStream.is_open()) {
			Logger::error("Failed to open upload stream: " + client->uploadFileName, client->getServerIdx());
			client->isUpload = false;
		} else {
			Logger::info("Started streaming upload to: " + client->uploadFileName, client->getServerIdx());
		}
	}
}

void	Server::_processMultipart(Client* client) {
	std::ifstream src(client->uploadFileName.c_str(), std::ios::binary);
	if (!src.is_open())
		return ;

	std::string content((std::istreambuf_iterator<char>(src)), std::istreambuf_iterator<char>());
	src.close();

	size_t namePos = content.find("filename=\"");
	std::string finalName = "uploaded_file";
	if (namePos != std::string::npos) {
		size_t nameEnd = content.find("\"", namePos + 10);
		finalName = content.substr(namePos + 10, nameEnd - (namePos + 10));
	}

	size_t dataStart = content.find("\r\n\r\n", content.find(client->boundary)) + 4;

	size_t dataEnd = content.find(client->boundary, dataStart);
	if (dataEnd == std::string::npos) {
		Logger::error("Multipart: Boundary final non trouvé", client->getServerIdx());
		return ;
	}
	dataEnd -= 2;

	std::string finalPath = _getUploadPath(client) + "/" + finalName;
	std::ofstream dest(finalPath.c_str(), std::ios::binary);
	dest.write(content.data() + dataStart, dataEnd - dataStart);
	dest.close();

	std::remove(client->uploadFileName.c_str());
	Logger::info("Multipart processed: " + finalName, client->getServerIdx());
}

std::string	Server::_getUploadPath(Client* client) {
    std::string header = client->getHeader();
    size_t firstLineEnd = header.find("\r\n");
    if (firstLineEnd == std::string::npos)
		return (".");

    std::string firstLine = header.substr(0, firstLineEnd);
    std::string method, uri, version;
    std::stringstream ss(firstLine);
    ss >> method >> uri >> version;

    const LocationConfig* loc = _findBestLocation(uri, client->getServerIdx());
    if (loc && !loc->upload_store.empty()) {
        return (loc->upload_store);
    }
    return (".");
}

/////////////////////////////////////

void	Server::_closeSocketFds() {
	std::map<int, int>::const_iterator it;

	if (_serverSockets.empty())
		return ;
	for (it = _serverSockets.begin(); it != _serverSockets.end(); ++it) {
		int fd = it->first;
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
	}
	_serverSockets.clear();
	Logger::info("All sockets closed");
}

void	Server::_closeAllClients() {
	if (_clients.empty())
		return ;
	std::map<int, Client*>::const_iterator	it = _clients.begin();
	for (; it != _clients.end(); it++) {
		int	fd = it->first;
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		delete it->second;
	}
	_clients.clear();
	Logger::info("All clients disconnected");
}

/////////////////////////////////////

const Config &Server::getConfig() const {
	return (_conf);
}