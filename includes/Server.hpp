/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:11:24 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/15 13:40:57 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"
# include "Config.hpp"
# include "Parser.hpp"

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

private:
	Config	conf;
};

#endif