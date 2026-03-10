/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 17:47:25 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/03/10 15:12:46 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Lexer.hpp"
#include "ConfigPrint.hpp"
#include "Server.hpp"

int	main(int ac, char **av) {
	if (ac != 1 && ac != 2) {
		std::cout << "Using: >./webserv || >./webserv <config file>"
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
		s.run();
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
	}
	return (0);
}