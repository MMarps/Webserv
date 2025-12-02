/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 17:47:25 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/02 17:17:51 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

int	main(int ac, char **av)
{
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	int clientFd;
	socklen_t addrSize;
    struct sockaddr_in myAddr, otherAddr;
	int opt = 1;

	if (serverFd == -1)
	{
		std::cout << "pb serverfd" << std::endl;
		return (1);
	}
		
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cout << "pb setsock" << std::endl;
		return (1);
	}	

	memset(&myAddr, 0, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // écoute toutes les interfaces
    myAddr.sin_port = htons(8080);   

	if (bind(serverFd, (struct sockaddr *) &myAddr,sizeof(myAddr)) == -1)
	{
		std::cout << "pb bind : " << errno << std::endl;
		return (1);
	}	
	if (listen(serverFd, SOMAXCONN) == -1)
	{
		
		std::cout << "pb listen" << std::endl;
		return (1);
	}	
	while (1)
	{
		addrSize = sizeof(otherAddr);
		clientFd = accept(serverFd, (struct sockaddr *)&otherAddr, &addrSize);
		if (clientFd == -1)
			return (1);
		std::cout << "nice" << std::endl;
	}
	
	
	
	return (0);
}