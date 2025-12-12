/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:28 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/08 13:39:28 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Webserv.hpp"

class Response
{
private:
	const std::string _version;
	const std::string _statutCode;
	const std::string _statutMssage;
	const std::string _contentType;
	const std::string _contentLength;
	const std::string _content;

public:
	Response(/* args */);
	~Response();
};

Response::Response(/* args */)
{
}

Response::~Response()
{
}


#endif