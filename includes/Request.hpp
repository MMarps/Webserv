/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:18 by jle-doua          #+#    #+#             */
/*   Updated: 2026/01/26 16:29:25 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Config.hpp"
# include "Webserv.hpp"

struct ServerConfig;

enum	PathType
{
	FILE_PATH,
	DIR_WITH_SLASH,
	DIR_NO_SLASH,
	SERVER_LOCATION,
	SERVER_LOCATION_NO_SLASH,
	SERVER_LOCATION_WI_SLASH
};

class Request
{
  private:
	std::string _methode;
	std::string _path;
	std::string _completPath;
	std::string _version;
	std::string _header;
	std::string _host;
	std::map<std::string, std::string> _varLst;
	bool _autoindex;
	bool _isComplete;
	int _code;

  public:
	Request();
	~Request();

	// refactor

	/* parsing request*/
	void parse(ServerConfig server, std::string buffer, int code);
	void makeRequest(ServerConfig server, std::string buffer);
	void parseMethode(ServerConfig server, std::string line);
	void parseAttribut(std::string line);

	/*check*/
	void checkRequest();

	/*getter*/
	std::string getMethode() const;
	std::string getPath() const;
	std::string getCompletPath() const;
	std::string getVersion() const;
	std::string getHeader() const;
	std::string getHost() const;
	bool getIsComplete() const;
	int getCode() const;

	/* get && verif path type*/
	void setAndCheckPath(ServerConfig server, std::string path);
	int getPathType(ServerConfig seerver);

	/*chec path variable query*/
	std::string getPathVariable(std::string path);
	void getVariable(std::string path);

	/* get && verif path file*/
	void getfilePath(ServerConfig server, int searchIndex);
	void getIndex(ServerConfig server);
	void verifFile();

	/*classic setter*/
	void setMethode(std::string Methode);
	void setPath(std::string path);
	void setVersion(std::string version);
	void setErrorCode(int errorCode);

	// old

	void getServerLocationPath(ServerConfig server);
};

std::ostream &operator<<(std::ostream &o, Request const &request);

#endif