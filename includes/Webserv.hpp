/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2026/01/14 16:18:12 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//////* Libraries *//////


# include "color.h"

# include <iterator>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/epoll.h>    // Linux epoll
# include <sys/time.h>
# include <sys/wait.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <fcntl.h>
# include <dirent.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>

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