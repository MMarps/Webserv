/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 17:47:25 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/12 18:02:58 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Lexer.hpp"
#include "ConfigPrint.hpp"
#include "Server.hpp"

int	main(int ac, char **av) {

	if (ac != 1 && ac != 2) {
		std::cout << "Using: >./webserv.out || >./webserv.out <config file>"
				  << std::endl;
		return (0);
	}
	try {
		std::string	path;
		if (ac == 1)
			path = "./configs/default.conf";
		else if (ac == 2)
			path = av[1];
		Server	s(path);

		printConfig(s.getConfig(), std::cout);

	}
	catch (const std::exception& err) {
		std::cout << err.what() << std::endl;
	}
	
	return (0);
}