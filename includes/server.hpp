/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:11:24 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/01 16:18:25 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"

class	Server {
public:
	Server(const std::string& confFile);
	Server(const Server& other);
	~Server();
	Server&	operator=(const Server& other);


private:
	const int						_port;
	const std::string				_root;
	const std::vector<std::string>	_index;
	const std::string				_serverName;

};

#endif