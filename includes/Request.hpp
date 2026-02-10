/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:18 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/10 19:41:27 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Config.hpp"

struct ServerConfig;
struct LocationConfig;

enum PathType
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
	std::string	_methode;
	std::string	_path;
	std::string	_completPath;
	std::string	_version;
	std::string	_header;
	std::string	_host;
	bool		_isComplete;
	bool		_isChunked;
	int			_code;
	std::map<std::string, std::string>	_varLst;
	std::map<std::string, std::string>	_httpHeaders;
	// bool _autoindex;

	// CGI part
	std::string	_queryString; // partie apres le '?' dans l'URL
	std::string	_body; // contenu du body pour la requete POST
	size_t		_bodySize; // taille du body
	std::string	_contentType; // contentType de la requete

public:
	Request();
	~Request();

	/* parsing request*/
	bool		parseChunkedBody(std::string &line);
	void		parse(ServerConfig &server, std::string &buffer, int code);
	void		makeRequest(ServerConfig &server, std::string &buffer);
	void		parseMethode(ServerConfig &server, std::string &line);
	void		parseAttribut(std::string &line);

	/*check*/
	void		checkRequest();

	/*getter*/
	std::string	getMethode() const;
	std::string	getPath() const;
	std::string	getCompletPath() const;
	std::string	getVersion() const;
	std::string	getHeader() const;
	std::string	getHost() const;
	bool		getIsComplete() const;
	int			getCode() const;

	/* get && verif path type*/
	void		setAndCheckPath(ServerConfig &server, std::string &path);
	int			getPathType(ServerConfig &server);

	/*check path variable query*/
	std::string	getPathVariable(std::string &path);
	void		getVariable(std::string &path);

	/* get && verif path file*/
	void		getFilePath(ServerConfig &server, int searchIndex);
	void		getIndex(ServerConfig &server);
	void		verifFile();

	/*classic setter*/
	void		setMethode(std::string &Methode);
	void		setPath(std::string &path);
	void		setVersion(std::string &version);
	void		setErrorCode(int errorCode);

	// CGI part
	std::string	getQueryString() const;
	std::string	getBody() const;
	std::string	getContentType() const;
	size_t		getBodySize() const;
	const std::map<std::string, std::string>	&getVarLst() const;
	const std::map<std::string, std::string>	&getHttpHeaders() const;

	// old
	void		getServerLocationPath(ServerConfig &server);
};

std::ostream	&operator<<(std::ostream &o, Request const &request);

#endif