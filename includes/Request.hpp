/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:18 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/14 16:24:36 by jle-doua         ###   ########.fr       */
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
	std::vector<std::string> _index;
	std::vector<std::string> _cutPath;
	std::map<std::string, std::string> _varLst;
	LocationConfig _location;
	bool _isLocation;
	bool _isPost;
	bool _isComplete;
	bool _makeAutoindex;
	bool _isCgi;

	int _code;

public:
	Request();
	~Request();

	void parse(ServerConfig &server, std::string header, int code);
	void makeRequest(ServerConfig &server, std::string buffer);
	void parseMethode(ServerConfig &server, std::string line);
	void parseAttribut(std::string line);
	void prepareReq(ServerConfig &server);
	void cutVariableToPath();
	size_t haveVariable();
	void splitVarQuery(std::string variableQuery);
	void cutPath();
	void makeAllPathRules(ServerConfig &server);
	int checkPathType(ServerConfig &server, std::string piecePath);
	void verifFile(std::string path);
	void copyLocationRules(ServerConfig &server, std::string folder, std::string piecePath);
	void makeExtentionAndNameFile(std::string file);
	void formatPath();
	void makeLocationRules();
	void checkAllowMethods();
	void searchIndex();
	void checkIsCgi(ServerConfig &server);
	void getErrorpage(ServerConfig &server);
	void checkRequest();

	std::string getMethode() const;
	std::string getRoot() const;
	std::string getPath() const;
	std::string getCompletPath() const;
	std::string getFileName() const;
	std::string getFileExtention() const;
	std::string getVersion() const;
	std::string getHeader() const;
	std::string getHost() const;
	std::string getCgiPath() const;
	std::vector<std::string> getIndex() const;
	std::vector<std::string> getCutPath() const;
	std::map<std::string, std::string> getVarLst() const;
	LocationConfig getLocation() const;
	bool getIsLocation() const;
	bool getIsPost() const;
	bool getIsComplete() const;
	bool getMakeAutoindex() const;
	bool getIsCgi() const;
	int getCode() const;
	
};

std::ostream &operator<<(std::ostream &o, Request const &request);

#endif