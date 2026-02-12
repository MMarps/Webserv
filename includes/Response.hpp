/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/12 14:32:17 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Webserv.hpp"

struct ServerConfig;
class Request;

class Response
{
private:
	Request _req;
	std::vector<char> _content;
	std::string _contentLength;
	std::string _response;
	std::map<int, std::string> _statutMessage;
	std::map<std::string, std::string> _contentType;

public:
	Response(Request req);
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

	// refactor
	void makeRep();
	void generateHeader();
	void generateBody();
	void checkFile(bool save);
	std::string intToString(int n);
	void makeError();
	std::string getResponse() const;
	std::vector<char> getContent() const;
	// void makeError();
};

std::ostream &operator<<(std::ostream &o, Response const &response);

#endif