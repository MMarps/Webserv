/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:14:53 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/16 18:05:58 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Webserv.hpp"

class	Client {
public:
	Client(int clientFd, int serverIdx);
	~Client();

	int				getFd() const;
	int				getServerIdx() const;
	std::string&	getBuffer();
	std::string&	getResponse();

	bool			isRequestFinished;

private:
	int			_fd;
	int			_serverIdx;
	std::string	_buffer;
	std::string	_response;
};

#endif