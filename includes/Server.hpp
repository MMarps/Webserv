/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:11:24 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/11 16:37:17 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"
# include "Config.hpp"
# include "Lexer.hpp"

class	Server {
public:
	Server(const std::string& confFileName);
	~Server();
	

	class	ErrorException : public std::exception {
	public:
		ErrorException(const std::string& msg) throw() {
			_msg = "Error: Server: " + msg;
		}
		virtual ~ErrorException() throw() {}
		virtual const char*	what() const throw() {
			return (_msg.c_str());
		}
	private:
		std::string	_msg;
	};

	// const Lexer&	getTok() const;

private:
	// Lexer	*_tok;
	// Config	*_conf;

};

#endif