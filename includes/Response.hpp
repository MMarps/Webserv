/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/19 17:39:38 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserv.hpp"

class Request;

class Response
{
  private:
	std::string _version;
	std::string _contentType;
	std::string _contentLength;
	std::string _content;
	std::string _response;
	std::map<int, std::string> _statutMessage;

  public:
	Response();
	~Response();

	std::string getRep() const;

	void getDoc(std::string docPath);
	void makeRep(Request request);
};

std::ostream &operator<<(std::ostream &o, Response const &response);

#endif