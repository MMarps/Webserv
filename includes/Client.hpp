/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 17:14:53 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/17 19:41:28 by mmarpaul         ###   ########.fr       */
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
		std::string			getAddr() const;
		int					getPort() const;

		std::string			getAllInfos() const;

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
		std::string			_addr;
		int					_port;
		
		std::string			_header;
		std::vector<char>	_body;
		
		std::string			_response;

		/*     ??
		std::string 		_repHeader
		std::vector<char>	_repBody*/
};

#endif