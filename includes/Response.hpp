/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2026/03/15 15:13:12 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "Webserv.hpp"
#include "Logger.hpp"
#include "Client.hpp"

struct ServerConfig;
class Request;
class Client;

class Response
{
private:
	Request &_req;
	std::string _response;
	std::string _contentLength;
	std::vector<char> _content;
	std::map<int, std::string> _statutMessage;
	std::map<std::string, std::string> _contentType;
	bool isDirectoryEmpty(const std::string &dirPath);
public:
	Response(Request &req);
	~Response();
	void makeRep(Client *client);
	void finalLogger(int serverIdx);
	void generateHeader();
	void generateBody();
	void checkFile(bool save);
	void handleDelete();
	std::vector<std::string> getLstDir();
	void generateAutoindex();
	std::string intToString(int n);
	std::string getResponse() const;
	std::vector<char> getContent() const;
};

std::ostream &operator<<(std::ostream &o, Response const &response);

#endif