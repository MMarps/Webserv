/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 17:47:25 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/01/14 17:06:35 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Lexer.hpp"
#include "ConfigPrint.hpp"
#include "Server.hpp"


int	main(int ac, char **av)
{
	(void)ac;
	(void)av;
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myAddr;

	if (serverFd == -1)
	{
		std::cout << "pb serverfd" << std::endl;
		return (1);
	}

	
	memset(&myAddr, 0, sizeof(myAddr));
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY); // écoute toutes les interfaces
	myAddr.sin_port = htons(8080);

	int opt = 1;
	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
			char buffer[1024];
			if (events[i].data.fd == serverFd)
			{
				
				ServerConfig server;
				server.index.push_back("index.html");
				server.root = "var/www";

				std::cout << "Un client arrive !" << std::endl << std::endl;
				int clientFd = accept(serverFd, NULL, NULL);
				int r = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
				buffer[r] = '\0';
				Request req;
				req.parse(server ,buffer);
				Response response(req);
				response.makeRep();
				std::cout << RED << " ca passe 6" << NC << std::endl;

				std::cout << "-------------------------" << std::endl;
				std::cout << "request : " << std::endl << buffer << std::endl;
				std::cout << "-------------------------" << std::endl;
				std::cout << "reponse : " << std::endl << response.getRep() << std::endl << std::endl;
				std::cout << "-------------------------" << std::endl;
				if (send(clientFd, response.getRep().c_str(), response.getRep().size(), 0) == -1 || send(clientFd, response.getContent().data(), response.getContent().size(), 0) == -1)
				{
					perror("send");
				}
				close(clientFd);
				
			}
			
		}

	}
	epoll_ctl(epollFd, EPOLL_CTL_DEL, serverFd, NULL);
	close(epollFd);
	close(serverFd);
	std::cout << "webserv stopped cleanly" << std::endl;

	return (0);
}