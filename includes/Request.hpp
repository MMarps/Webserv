/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:18 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/07 17:52:27 by jle-doua         ###   ########.fr       */
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

	/* parsing request*/
	void parse(ServerConfig server, std::string header, int code);
	void makeRequest(ServerConfig server, std::string buffer);
	void parseMethode(ServerConfig server, std::string line);
	void parseAttribut(std::string line);

	/*check*/
	void checkRequest();

	/* get && verif path type*/
	void setAndCheckPath(ServerConfig server, std::string path);
	void getServerLocationPath(const ServerConfig &server);
	void setContentExtention();

	/*chec path variable query*/
	std::string getPathVariable(std::string path);
	void getVariable(std::string path);

	/* get && verif path file*/
	void getfilePath(ServerConfig server, int searchIndex);
	void getIndex(ServerConfig server);

	/*attribut getter*/
	std::string getMethode() const;
	std::string getPath() const;
	std::string getCompletPath() const;
	std::string getVersion() const;
	std::string getHeader() const;
	std::string getHost() const;
	std::string getContentExtention() const;
	LocationConfig getLocation() const;
	bool getIsLocation() const;
	bool getIsComplete() const;
	bool getMakeAutoindex();
	int getCode() const;

	/*attribut setter*/
	void setMethode(std::string Methode);
	void setPath(std::string path);
	void setCompletPath(std::string completPath);
	void setVersion(std::string version);
	void setErrorCode(int errorCode);

	void makeLocationRules(const ServerConfig &server);

	void getServerLocationConfig(const ServerConfig &server, std::string path);

	void getRuleForResponse(const ServerConfig &server, std::string completPath);
	void setLocationConfig(LocationConfig location);
	std::string getFileName() const;
	std::string getFileExtention() const;

	// new refact
	void preparePath(ServerConfig server);
	void cutVariableToPath();
	void splitVarQuery(std::string variableQuery);
	void cutPath();
	void makeAllPathRules(ServerConfig server);
	void copyLocationRules(ServerConfig server, std::string folder, std::string piecePath);
	void verifFile(std::string path);
	void makeExtentionAndNameFile(std::string file);

	int checkPathType(ServerConfig server, std::string piecePath);
	size_t haveVariable();

	// getter
	std::map<std::string, std::string> getVarLst() const;
};

std::ostream &operator<<(std::ostream &o, Request const &request);

#endif