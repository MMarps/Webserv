/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 02:32:29 by jle-doua          #+#    #+#             */
/*   Updated: 2026/02/11 15:50:10 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request &req) : _req(req)
{
	_statutMessage.insert(std::make_pair(200, "OK"));
	_statutMessage.insert(std::make_pair(201, "Created"));
	_statutMessage.insert(std::make_pair(301, "Moved Permanently"));
	_statutMessage.insert(std::make_pair(302, "Found"));
	_statutMessage.insert(std::make_pair(400, "Bad Request"));
	_statutMessage.insert(std::make_pair(403, "Forbidden"));
	_statutMessage.insert(std::make_pair(404, "Not Found"));
	_statutMessage.insert(std::make_pair(405, "Method Not Allowed"));
	_statutMessage.insert(std::make_pair(409, "Conflict"));
	_statutMessage.insert(std::make_pair(413, "Payload Too Large"));
	_statutMessage.insert(std::make_pair(500, "Internal Server Error"));
	_contentType.insert(std::make_pair(".html", "text/html"));
	_contentType.insert(std::make_pair(".css", "text/css"));
	_contentType.insert(std::make_pair(".js", "text/javascript"));
	_contentType.insert(std::make_pair(".png", "image/png"));
	_contentType.insert(std::make_pair(".jpg", "image/jpeg"));
	_contentType.insert(std::make_pair(".jpeg", "image/jpeg"));
	_contentType.insert(std::make_pair(".gif", "image/gif"));
	_contentType.insert(std::make_pair(".ico", "image/x-icon"));
	_contentType.insert(std::make_pair(".mp4", "video/mp4"));
	_contentType.insert(std::make_pair(".mp3", "audio/mpeg"));
	_contentType.insert(std::make_pair(".mp3", "audio/mpeg"));
	_contentType.insert(std::make_pair("nodotdetected", "text/plain"));

	/*telecharge automatiquement la ressource !!!*/
	// _contentType.insert(std::make_pair("...", "application/octet-stream"));
}

Response::~Response()
{
}

void Response::makeRep(ServerConfig &server)
{
	(void)server;
	generateBody();
	generateHeader();
}

void Response::generateHeader()
{
	this->_response = "HTTP/1.1 " + intToString(this->_req.getCode()) + " " + this->_statutMessage[this->_req.getCode()] + "\n";
	if (this->_req.getCode() == 301)
	{
		return;
	}
	this->_response += "Content-Type: " + this->_contentType[this->_req.getFileExtention()] + "\n";
	this->_response += "Content-length: " + this->_contentLength + "\n";
	this->_response += "\n\n";
}

void Response::generateBody()
{

	if (!this->_req.getFileName().empty())
	{
		if (this->_req.getMethode() == "HEAD")
			checkFile(false);
		else
			checkFile(true);
		return;
	}
	else if (this->_req.getMakeAutoindex())
	{
		generateAutoindex();
	}
	// else
	// 	makeError();
}

void Response::checkFile(bool save)
{
	std::ifstream file(this->_req.getCompletPath().c_str(), std::ios::binary);
	std::istreambuf_iterator<char> first(file);
	std::istreambuf_iterator<char> last;

	std::vector<char> buffer(first, last);
	this->_contentLength = intToString(buffer.size());
	std::cout << BRED << _contentLength << NC << std::endl;
	if (save)
		this->_content.swap(buffer);
}

std::string Response::intToString(int n)
{
	std::stringstream ss;
	ss << n;
	return (ss.str());
}

void Response::generateAutoindex()
{
	std::string htmlpage;
	std::vector<std::string> lstFiles;
	htmlpage = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body>";
	lstFiles = getLstDir();
	for (long unsigned int i = 0; i < lstFiles.size(); i++)
	{
		htmlpage += "<a href=\"" + this->_req.getPath() + "/" + lstFiles[i] + "\">" + lstFiles[i] + "</a></br>";
	}
	htmlpage += "</body></html>";
	std::vector<char> tmp(htmlpage.begin(), htmlpage.end());
	this->_content.swap(tmp);
	std::stringstream ss;
	ss << htmlpage.size();
	this->_contentLength = ss.str();
	std::cout << BBLUE << this->_contentLength << NC << std::endl;
}

std::vector<std::string> Response::getLstDir()
{
	DIR *folder;
	struct dirent *readFolder;
	std::vector<std::string> lstFiles;

	folder = opendir(this->_req.getCompletPath().c_str());
	readFolder = readdir(folder); // probleme ici
	std::cout << BBLUE << "ca passe 2" << std::endl;
	while (readFolder)
	{
		std::cout << "a" << std::endl;
		if (strcmp(readFolder->d_name, ".") != 0 && strcmp(readFolder->d_name, "..") != 0)
			lstFiles.push_back(readFolder->d_name);
		readFolder = readdir(folder);
	}
	std::sort(lstFiles.begin(), lstFiles.end());
	return (lstFiles);
}

// void Response::makeRep(ServerConfig server)
// {<< response.getContent().data() << NC << std::endl;
// 	(void) server; //a virer si pas nescessaire
// 	std::cout << "debut parsing response" << std::endl;
// 	getDefaultResponse();
// 	if (this->_req.getCode() == 200)
// 	{
// 		if (this->_req.getIsLocation())
// 			makeLocation();
// 		else
// 		{
// 			getFullResponse();
// 		}
// 	}
// 	else if (this->_req.getCode() == 301)
// 		makeRedirect();
// 	// else
// 	// 	getCodePage();
// 	std::cout << "fin parsing response" << std::endl;
// }

// void Response::getDefaultResponse()
// {
// 	this->_response = "HTTP/1.1 ";
// 	getResponseCode();
// }

// void Response::getResponseCode()
// {
// 	std::stringstream errorCode;
// 	errorCode << this->_req.getCode();
// 	this->_response += errorCode.str() + " " + this->_statutMessage[this->_req.getCode()];
// }

// void Response::makeLocation()
// {
// 	if (this->_req.getMakeAutoindex())
// 	{
// 		generateAutoindex();
// 		this->_response += "\nContent-Type: " + this->_contentType[".html"];
// 		this->_response += "\nContent-Length: " + this->_contentLength;
// 		this->_response += "\n\n";
// 	}
// }

// // void Response::getCodePage()
// // {
// // 	if (this.re)
// // 	{
// // 		this->_req.setPath(server.error_pages[this->_req.getCode()]);
// // 		this->_req.setCompletPath(this->_req.getPath());
// // 		getFullResponse();
// // 	}
// // 	else
// // 	{
// // 		this->_response += "\nContent-Length: 0";
// // 		this->_response += "\n\n";
// // 	}
// // }

// void Response::getDoc()
// {
// 	std::ifstream file(this->_req.getCompletPath().c_str(), std::ios::binary);
// 	// if (!file.is_open())
// 	// {
// 	// 	this->_req.setErrorCode(404);
// 	// 	return;
// 	// }

// 	std::istreambuf_iterator<char> it(file);
// 	std::istreambuf_iterator<char> end;
// 	std::vector<char> buffer(it, end);
// 	std::stringstream ss;

// 	ss << buffer.size();
// 	this->_contentLength = ss.str();
// 	this->_content.swap(buffer);
// }

// void Response::checkDoc()
// {
// 	std::ifstream file(this->_req.getCompletPath().c_str(), std::ios::binary);
// 	// if (!file.is_open())
// 	// {
// 	// 	this->_req.setErrorCode(404);
// 	// 	return;
// 	// }
// 	std::istreambuf_iterator<char> it(file);
// 	std::istreambuf_iterator<char> end;
// 	std::vector<char> buffer(it, end);
// 	std::stringstream ss;
// 	ss << buffer.size();
// 	this->_contentLength = ss.str();
// }

// void Response::makeRedirect()
// {
// 	this->_response += "\nLocation: " + this->_req.getPath();
// 	this->_response += "\nConnection: close";
// 	this->_response += "\nContent-Length: 0";
// 	this->_response += "\n\n";
// }

// void Response::getFullResponse()
// {
// 	if (this->_req.getMethode() == "HEAD")
// 		checkDoc();
// 	else
// 		getDoc();
// 	this->_response += "\nContent-Type: " + this->_contentType[this->_req.getFileExtention()];
// 	this->_response += "\nContent-Length: " + this->_contentLength;
// 	this->_response += "\n\n";
// }

// std::string Response::getRep() const
// {
// 	return (this->_response);
// }

std::string Response::getResponse() const
{
	return (this->_response);
}

std::vector<char> Response::getContent() const
{
	return (this->_content);
}

std::ostream &operator<<(std::ostream &o, Response const &response)
{
	o << BYELLOW << response.getResponse() << std::endl;

	return (o);
}
