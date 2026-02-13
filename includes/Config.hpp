/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:15:18 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/13 17:37:10 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Webserv.hpp"

struct Listen {
	std::string	host;
	int			port;

	Listen();
	Listen(const std::string& newHost, const int& newPort);
	bool	operator==(const Listen& other);
};

struct LocationConfig {
	std::string							path;
	std::string							root;
	std::vector<std::string>			index;
	std::vector<std::string>			methods;
	bool 								autoindex;
	std::string 						upload_store;
	std::map<std::string, std::string>	cgi;
	size_t								client_max_body_size;
	bool								has_client_max_body_size;

	bool								has_return;
	int									return_code;
	std::string							return_url;

	LocationConfig();
};

struct ServerConfig {
	std::vector<Listen>					listens;
	std::vector<std::string>			server_names;
	std::string							root;
	std::vector<std::string>			index;
	std::map<int, std::string>			error_pages;
	std::map<std::string, std::string>	cgi;
	size_t 								client_max_body_size;
	bool 								has_client_max_body_size;
	std::vector<LocationConfig>			locations;

	std::string							log;

	ServerConfig();
};

class	Config {
public:
	std::vector<ServerConfig>	servers;
};

#endif