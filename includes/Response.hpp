/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/18 14:34:56 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Webserv.hpp"

class Response
{
private:
	std::string _version;
	std::string _statutCode;
	std::string _statutMessage;
	std::string _contentType;
	std::string _contentLength;
	std::string _content;
	std::string _response;

public:
	Response(std::string docPath);
	~Response();
	void getDoc(std::string docPath);
	std::string getRep() const;
};

std::ostream &operator<<(std::ostream &o, Response const &response);

#endif