/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/10 19:42:41 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

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
#include "Config.hpp"
#include "Request.hpp"
#include "Webserv.hpp"

struct ServerConfig;
class Request;

class Response
{
private:
	Request &_req;
	std::vector<char> _content;
	std::string _contentLength;
	std::string _response;
	std::map<int, std::string> _statutMessage;
	std::map<std::string, std::string> _contentType;

public:
	Response(Request &req);
	~Response();

	// generate response
	void makeLocation();
	void getCodePage();
	// old
	std::string getRep() const;
	void getText();
	void getDoc();
	void checkDoc();
	void getDefaultResponse();
	void getFullResponse();
	void getResponseCode();
	void makeRedirect();
	std::vector<std::string> getLstDir();
	void generateAutoindex();

	// refacor
	void makeRep(ServerConfig &server);
	void generateHeader();
	void generateBody();
	void checkFile(bool save);
	std::string intToString(int n);
	void makeError();
	std::string getResponse() const;
	std::vector<char> getContent() const;

};

std::ostream	&operator<<(std::ostream &o, Response const &response);

#endif