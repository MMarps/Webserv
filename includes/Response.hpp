/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/25 16:16:20 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Config.hpp"
# include "Webserv.hpp"
# include "CGI.hpp"

struct	ServerConfig;
class	Request;
class	CGI;

class Response {
	private:
		Request								&_req;
		std::string							_contentPath;
		std::string							_contentExtention;
		std::string							_response;
		std::string							_contentLength;
		std::vector<char>					_content;
		std::map<int, std::string>			_statutMessage;
		std::map<std::string, std::string>	_contentType;
		// CGI
		bool								_isCGI;
		std::map<std::string, std::string>	_cgiHeaders;

	public:
		Response(Request &req);
		~Response();

		std::string	getRep() const;

		void		getText();
		void		getDoc();
		void		checkDoc();
		void		makeRep(ServerConfig server);
		void		getContentExtention();
		void		getDefaultResponse();
		void		getFullResponse();
		void		getResponseCode();
		void		makeRedirect();
		std::vector<char>	getContent();

		// CGI
		bool		isCGIRequest(ServerConfig &server);
		void		handleCGI(ServerConfig &server);
		void		buildCGIResponse();
};

std::ostream	&operator<<(std::ostream &o, Response const &response);

#endif