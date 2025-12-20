/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/20 18:35:49 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserv.hpp"

class	Request;

class Response
{
  private:
	Request _req;

	std::string _contentPath;
	std::string _contentExtention;
	std::string _content;
	std::string _contentLength;
	std::string _response;
	std::map<int, std::string> _statutMessage;
	std::map<std::string, std::string> _contentType;

  public:
	Response(Request req);
	~Response();

	std::string getRep() const;

	void getText();
	void getDoc();
	void makeRep();
	void getContentExtention();
	void getDefaultResponse();
	void getFullResponse();
	void getResponseCode();
};

std::ostream &operator<<(std::ostream &o, Response const &response);

#endif