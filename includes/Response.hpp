/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2026/03/03 11:19:22 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Webserv.hpp"
#include "CGI.hpp"
#include "Logger.hpp"

struct ServerConfig;
class Request;
class CGI;

class Response
{
private:
	Request &_req;
	std::string _contentPath;
	std::string _contentExtention;
	std::string _response;
	std::string _contentLength;
	std::vector<char> _content;
	std::map<int, std::string> _statutMessage;
	std::map<std::string, std::string> _contentType;

	// CGI
	bool _isCGI;
	std::map<std::string, std::string> _cgiHeaders;

	bool isDirectoryEmpty(const std::string &dirPath);

public:
	Response(Request &req);
	~Response();

	// generate response
	// old
	// std::string			getRep() const;
	// void				getText();
	// void				getDoc();
	// void				checkDoc();
	// void				getDefaultResponse();
	// void				getFullResponse();
	// void				getResponseCode();
	// void				makeRedirect();

	// refactor
	void makeRep(ServerConfig &server);
	void finalLogger();
	void generateHeader();
	void generateBody();
	void checkFile(bool save);
	void handleDelete();
	std::vector<std::string> getLstDir();
	void generateAutoindex();
	std::string intToString(int n);
	void makeError();
	std::string getResponse() const;
	std::vector<char> getContent() const;

	// CGI
	bool isCGIRequest(ServerConfig &server);
	void handleCGI(ServerConfig &server);
	void buildCGIResponse();
};

std::ostream &operator<<(std::ostream &o, Response const &response);

#endif