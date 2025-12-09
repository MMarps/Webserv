/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 17:36:59 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/09 17:49:11 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Listen::Listen(): host(""), port(0) {}

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
	  