/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:11:24 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/23 20:27:52 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"
# include "Config.hpp"
# include "Parser.hpp"
# include "Client.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Logger.hpp"

# define BUFFER_SIZE 4096
# define MAX_EVENTS 1024

class	Config;

static volatile sig_atomic_t g_terminate = 0;

void	signal_handler(int sig);

class ServerError : public std::exception {
	public:
		ServerError(const std::string& msg) throw() {
			_msg = "Error: Server: " + msg;
		}
		virtual ~ServerError() throw() {}
		virtual const char*	what() const throw() {
			return (_msg.c_str());
		}
	private:
		std::string	_msg;
};

class Server {
	public:
		Server(const std::string& confFileName);
		~Server();
	
		const Config&	getConfig() const;
	
		void			run();
	
	private:
		Config					_conf;
	
		int						_epollFd;
		struct epoll_event		_events[MAX_EVENTS];
	
		std::map<int, int>		_serverSockets;
		std::map<int, int>		_serverPorts;
	
		std::map<int, Client*>	_clients;
	
		std::map<int, std::pair<std::string, int> >	_clientMetadata; // map de metadata, au format: fd, IP client, port serv
	
		void					_setupServerSockets();
		int						_setNonBlocking(int fd);
		int						_addToEpoll(int fd, uint32_t events);
		int						_modEpoll(int fd, uint32_t newEvents);
	
		void					_closeConnection(int fd);
		void					_addNewClient(int serverFd);
		void					_handleClientData(int clientFd);
		void					_parseResponse(Client* c, int errCode);
		void					_sendResponse(int clientFd);
	
		void					_closeSocketFds();
		void					_closeAllClients();
	
		long					_extractContentLen(const std::string& header);
		long					_getLocationMaxBodySize(Client* client);
		const LocationConfig*	_findBestLocation(const std::string& uri, int serverIdx);
		std::string				_getClientAddr(const struct sockaddr_in& clientAddr);

		void					_setUploadStream(Client* client);
		void					_processMultipart(Client* client);
		std::string				_getUploadPath(Client* client);
};

#endif