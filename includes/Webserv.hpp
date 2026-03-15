/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrea <andrea@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 17:45:47 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/03/14 23:07:48 by andrea           ###   ########.fr       */
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
# if defined(__linux__)
#  include <sys/epoll.h>
# elif defined(__APPLE__)
#  include <sys/event.h>
#  include <sys/time.h>
# endif
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