/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:11:24 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/16 18:48:13 by mmarpaul         ###   ########.fr       */
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

# define BUFFER_SIZE 4096
# define MAX_EVENTS 1024

class	Config;

class	ServerError : public std::exception {
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

class	Server {
public:
	Server(const std::string& confFileName);
	~Server();

	const Config&	getConfig() const;

	void			run();

private:
	Config					_conf;

	int						_epollFd;
	struct epoll_event		_events[MAX_EVENTS];

	std::map<int, int>		_serveurSockets;

	std::map<int, Client*>	_clients;

	void				_setupServerSockets();
	void				_setNonBlocking(int fd);
	void				_addToEpoll(int fd, uint32_t events);

	void				_closeConnection(int fd);
	void				_addNewClient(int serverFd);
	void				_handleClientData(int clientFd);
};

#endif