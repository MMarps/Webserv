/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/01 16:18:11 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/15 16:21:03 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const std::string& confFileName) {
	Lexer	ts(confFileName);
	Parser	p(ts);

	// ts.printTokens();
	conf = p.parseConfig();
}

Server::~Server() {}

const Config&	Server::getConfig() const {
	return (conf);
}