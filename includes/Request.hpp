/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:18 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/15 14:40:13 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Webserv.hpp"
# include "Config.hpp"

struct ServerConfig;

class Request
{
  private:
	std::string _methode;
	std::string _path;
	std::string _version;
	std::string _header;
	std::string _host;
	bool _isComplete;
	int _errorCode;

  public:

	Request();
	~Request();

	void parse(ServerConfig server, std::string buffer);
	void parseMethode(ServerConfig server, std::string line);
	void parseAttribut(std::string line);

	std::string getMethode() const;
	std::string getPath() const;
	std::string getVersion() const;
	std::string getHeader() const;
	std::string getHost() const;
	bool getIsComplete() const;
	int getErrorCode() const;

	void setMethode(std::string Methode);
	void setPath(ServerConfig server , std::string path);
	void setPath(std::string path);
	void setVersion(std::string version);
	void setHeader(std::string header);
	void setHost(std::string host);
	void setIsComplete(bool isComplet);
	void setErrorCode(int errorCode);
};

std::ostream &operator<<(std::ostream &o, Request const &request);

#endif