/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:11:24 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/01 18:18:27 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"

template <typename T>
class	Server {
public:
	Server(const std::string& confFileName);
	Server(const Server& other);
	Server&	operator=(const Server& other);
	~Server();


private:
	int							_port;
	std::string					_root;
	std::vector<std::string>	_index;
	std::string					_serverName;

	std::map<std::string, T>	_map;

};

#endif