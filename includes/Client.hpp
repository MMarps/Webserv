/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:14:53 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/21 17:34:50 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Webserv.hpp"

class	Client {
public:
	Client(int clientFd, int serverIdx, const std::string &remoteAddr, int ServerPort);
	~Client();

	int					getFd() const;
	int					getServerIdx() const;
	std::string&		getBuffer();
	std::string&		getResponse();
	bool				isRequestFinished;

	std::vector<char> 	getBody();
	void				setBody(std::vector<char> body);
	std::string			getRemoteAddr() const;
	int					getServerPort() const;

private:
	int			_fd;
	int			_serverIdx;
	std::string	_buffer;
	std::string	_response;
	
	//add
	std::vector<char>	_body;
	std::string			_remoteAddr;
	int					_serverPort;
};

#endif