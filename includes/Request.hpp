/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 13:31:18 by jle-doua          #+#    #+#             */
/*   Updated: 2025/12/15 16:03:51 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "Webserv.hpp"

class Request
{
private:
	std::string _methode;
	std::string _path;
	std::string _version;
	

public:
	Request(char *buffer);
	~Request();
	std::string getMethode() const;
	std::string getPath() const;
	std::string getVersion() const;

};

std::ostream &operator<<(std::ostream &o, Request const &request);

#endif