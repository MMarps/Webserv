/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:52:31 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/12 01:10:33 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(Lexer& ts): _ts(ts) {}

/////////////////////////////////////

Config	Parser::parseConfig() {
	Config	cfg;
	while (!_ts.eof()) {
		const Token &t = _ts.peek();
		if (t.type == T_IDENT && t.content == "server") {
			_ts.next();
			cfg.servers.push_back(parseServer());
		}
		else if (t.type == T_EOF)
			break;
		else
			throwError("Expected \"server\" at top level");
	}
	return (cfg);
}


/////////////////////////////////////

void Parser::throwError(const std::string &msg) const {
	const Token &t = _ts.eof() ? Token() : _ts.peek();
	std::ostringstream oss;
	if (t.l > 0)
		oss << "Parse error at " << t.l << ":" << t.c << " - " << msg;
	else
		oss << "Parse error - " << msg;
	throw ParserError(oss.str());
}

/////////////////////////////////////

