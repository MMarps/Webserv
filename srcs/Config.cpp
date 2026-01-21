/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 17:36:59 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/21 21:33:56 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Listen::Listen(): host(""), port(0) {}

Listen::Listen(const std::string& newHost, const int& newPort)
	: host(newHost), port(newPort) {}

bool	Listen::operator==(const Listen& other) {
	if (this->host == "*"
		&& (other.host == "0.0.0.0" || other.host == "localhost")
		&& this->port == other.port)
		return (true);
	else if (this->host == "0.0.0.0"
		&& (other.host == "*" || other.host == "localhost")
		&& this->port == other.port)
		return (true);
	else if (this->host == "localhost"
		&& (other.host == "*" || other.host == "0.0.0.0")
		&& this->port == other.port)
		return (true);
	return (this->host == other.host && this->port == other.port);
}

///////////////////////////////////

LocationConfig::LocationConfig()
	: path("/"),
	  root(),
	  index(),
	  methods(),
	  autoindex(false),
	  upload_store(),
	  cgi(),
	  client_max_body_size(0),
	  has_client_max_body_size(false),
	  has_return(false),
	  return_code(0),
	  return_url() {}

ServerConfig::ServerConfig()
	: listens(),
	  server_names(),
	  root(),
	  index(),
	  error_pages(),
	  client_max_body_size(0),
	  has_client_max_body_size(false),
	  locations() {}
	  