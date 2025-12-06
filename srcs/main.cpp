/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 17:47:25 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/06 16:48:14 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	// int clientFd;
	// socklen_t addrSize;
	struct sockaddr_in myAddr;

	if (serverFd == -1)
	{
		std::cout << "pb serverfd" << std::endl;
		return (1);
	}

	// if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	// {
	// 	std::cout << "pb setsock" << std::endl;
	// 	return (1);
	// }
	memset(&myAddr, 0, sizeof(myAddr));
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY); // écoute toutes les interfaces
	myAddr.sin_port = htons(8080);

	if (bind(serverFd, (struct sockaddr *)&myAddr, sizeof(myAddr)) == -1)
	{
		std::cout << "pb bind : " << errno << std::endl;
		return (1);
	}
	if (listen(serverFd, SOMAXCONN) == -1)
	{
		std::cout << "pb listen" << std::endl;
		return (1);
	}

	fcntl(serverFd, F_SETFL, O_NONBLOCK);

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = serverFd;

	int epollFd = epoll_create1(0);
	epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev);
	struct epoll_event events[1];
	while (1)
	{
		int n = epoll_wait(epollFd, events, 1, -1);
		std::cout << "serveur get event" << std::endl;

		for (int i = 0; i < n; i++)
		{
			if (events[i].data.fd == serverFd)
			{
				printf("Un client arrive !\n");

				int clientFd = accept(serverFd, NULL, NULL);
				char buffer[1024];
				int r = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
				buffer[r] = '\0';

				printf("Reçu : %s\n", buffer);
				char msg[198] = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body><h1>bienvenue</h1></body></html>";
    			if (send(clientFd, msg, strlen(msg), 0) == -1)
        		{

					perror("send");
				}	
				
				close(clientFd);
			}
		}
		// char *buff[1024];
		// int size;
		// addrSize = sizeof(otherAddr);
		// clientFd = accept(serverFd, (struct sockaddr *)&otherAddr, &addrSize);
		// if (clientFd == -1)
		// 	return (1);
		// size = recv(clientFd, buff, sizeof(buff), 0);
		// write(1, buff, size);
	}

	return (0);
}