/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arotondo <arotondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 17:45:47 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/03/16 12:20:59 by arotondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//////* Libraries *//////

# include "color.h"

# include <algorithm> 
# include <iterator>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/epoll.h>
# include <sys/time.h>
# include <ctime>
# include <sys/wait.h>
# include <signal.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <fcntl.h>
# include <dirent.h>
# include <unistd.h>
# include <errno.h>
# include <csignal>
# include <cstdio>
# include <cstdlib>
# include <cstring>

# include <iostream>
# include <sstream>
# include <fstream>

# include <string>
# include <vector>
# include <map>

# include <stdexcept>

# include "Request.hpp"
# include "Response.hpp"
# include "color.h"

#endif