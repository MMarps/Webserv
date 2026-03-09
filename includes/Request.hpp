/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:18 by jle-doua          #+#    #+#             */
/*   Updated: 2026/03/06 17:39:59 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Config.hpp"

struct ServerConfig;
struct LocationConfig;
class Client;

enum PathType
{
	NOTHING,
	FILE_PATH,
	DIR_WITH_SLASH,
	DIR_NO_SLASH,
	SERVER_LOCATION,
};

class Request
{
private:
	std::string _methode;
	std::string _root;
	std::string _path;
	std::string _completPath;
	std::string _fileName;
	std::string _fileExtention;
	std::string _version;
	std::string _header;
	std::string _host;
	std::string _cgiPath;
	std::string _newPath;
	LocationConfig *_location;
	bool _isLocation;
	bool _isPost;
	bool _isComplete;
	bool _makeAutoindex;
	bool _isRedirection;
	bool _urlIsMessage;
	bool _isCgi;
	int _code;
	std::vector<std::string> _index;
	std::vector<std::string> _cutPath;
	std::map<std::string, std::string> _varLst;
	std::map<std::string, std::string> _httpHeaders;
	std::string _queryString;
	std::string _body;
	size_t _bodySize;
	std::string _contentType;
	bool _isChunked;
	std::string _rawBuffer;
	std::string _remoteAddr;
	int _serverPort;

public:
	Request();
	~Request();

	void parse(ServerConfig &server, Client *client, int code);
	void makeRequest(ServerConfig &server, Client *c);
	void parseMethode(ServerConfig &server, std::string &line);
	void parseAttribut(std::string &line);

	void prepareReq(ServerConfig &server);
	void finalLogger(Client *c);
	void checkErrorPage(ServerConfig &server);
	void cutVariableToPath();
	size_t haveVariable();
	void splitVarQuery(std::string &variableQuery);
	void cutPath();
	void makeAllPathRules(ServerConfig &server);
	int checkPathType(ServerConfig &server, bool slash, std::string &piecePath);
	void verifFile(std::string path);
	void copyLocationRules(ServerConfig &server, std::string &folder);
	void makeExtentionAndNameFile(std::string file);
	void formatPath();
	void makeLocationRules();
	void checkAllowMethods();
	void searchIndex();
	void checkIsCgi(ServerConfig &server);
	void getErrorPage(ServerConfig &server);
	void checkRequest();
	void setCode(int code);
	std::string getMethode() const;
	std::string getRoot() const;
	std::string getPath() const;
	std::string getCompletPath() const;
	std::string getFileName() const;
	std::string getFileExtension() const;
	std::string getVersion() const;
	std::string getHeader() const;
	std::string getHost() const;
	std::string getNewPath() const;
	std::string getCgiPath() const;
	std::vector<std::string> getIndex() const;
	std::vector<std::string> getCutPath() const;
	std::map<std::string, std::string> getVarLst() const;
	LocationConfig *getLocation() const;
	bool getIsLocation() const;
	bool getIsPost() const;
	bool getIsComplete() const;
	bool getIsRedirection() const;
	bool getMakeAutoindex() const;
	bool getUrlIsMesssage() const;
	bool getIsCgi() const;
	int getCode() const;
	void accessFolder(std::string newCompletPath);
	bool parseChunkedBody(const std::string &newData);
	std::string getQueryString() const;
	std::string getBody() const;
	std::string getContentType() const;
	size_t getBodySize() const;
	const std::map<std::string, std::string> &getHttpHeaders() const;
	std::string getRemoteAddr() const;
	int getServerPort() const;
	void setRemoteAddr(const std::string &addr);
	void setServerPort(int port);
};

std::ostream &operator<<(std::ostream &o, Request const &request);
size_t hexToDecimal(const std::string &hex);

#endif