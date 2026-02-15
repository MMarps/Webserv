/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arotondo <arotondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:14:53 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/15 17:22:46 by arotondo         ###   ########.fr       */
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
	std::string&		getHeader();
	std::string&		getResponse();
	std::string			getRemoteAddr() const;
	int					getServerPort() const;

	std::vector<char>&	getBody();
	size_t				getBodySize() const;
	void				setBody(std::vector<char> body);
	void				appendBody(const char* buf, size_t size);

	bool				isHeaderFinished;
	bool				isRequestFinished;

	size_t				expectedBodySize;
	size_t				actualBodySize;

private:
	int					_fd;
	int					_serverIdx;
	std::string			_header;
	std::string			_response;
	std::vector<char>	_body;
};

#endif