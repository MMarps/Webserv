/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:14:53 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/03 19:45:24 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Webserv.hpp"

class	Client {
public:
	Client(int clientFd, int serverIdx);
	~Client();

	int					getFd() const;
	int					getServerIdx() const;
	std::string&		getHeader();
	std::string&		getResponse();

	std::vector<char> 	getBody();
	void				setBody(std::vector<char> body);
	void				appendBody(const char* buf, size_t size);

	bool				isHeaderFinished;
	bool				isRequestFinished;

	size_t				expectedBodySize;

private:
	int					_fd;
	int					_serverIdx;
	std::string			_header;
	std::string			_response;
	std::vector<char>	_body;
};

#endif