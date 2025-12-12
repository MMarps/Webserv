/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:52:31 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/12 18:37:27 by mmarps           ###   ########.fr       */
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
			throwError("Expected 'server' at top level");
	}
	return (cfg);
}

ServerConfig	Parser::parseServer() {
	ServerConfig	srv;

	if (_ts.eof() || _ts.peek().type != T_LBRACE)
		throwError("Expected '{' after 'server' identifier");
	_ts.next();
	while (!_ts.eof() && _ts.peek().type != T_RBRACE) {
		const Token& t = _ts.peek();
		if (t.type == T_IDENT && t.content == "location") {
			srv.locations.push_back(parseLocation());
		}
		else if (t.type == T_IDENT) {
			parseDirective(srv);
		}
		// else if (t.type == T_EOF)
		// 	break ;
		// else if (_ts.eof())
		// 	break ;
		else
			throwError("Unexpected token inside 'server' block");
	}
	if (_ts.eof())
		throwError("Unterminated 'server' block");
	_ts.next();
	return (srv);
}

LocationConfig	Parser::parseLocation() {
	LocationConfig	loc;

	_ts.next();
	if (_ts.eof())
		throwError("Expected location path after 'location'");
	const Token& p = _ts.peek();
	if (p.type != T_IDENT && p.type != T_STR && p.type != T_NUM)
		throwError("Invalid location path");
	loc.path = p.content;
	_ts.next();
	if (_ts.eof() || _ts.peek().type != T_LBRACE)
		throwError("Expected '{' after location path");
	_ts.next();
	while (!_ts.eof() && _ts.peek().type != T_RBRACE) {
		if (_ts.peek().type == T_IDENT)
			parseDirective(loc);
		else
			throwError("Unexpected token inside 'location' block");
	}
	if (_ts.eof())
		throwError("Unterminated 'location' block");
	_ts.next();
	return (loc);
}

void	Parser::parseDirective(ServerConfig& srv) {
	const Token&				t = _ts.next();
	std::string					name = t.content;
	std::vector<std::string>	args = collectArgs();

	if (name == "listen")
		srv.listens.push_back(parseListen(args));
	else if (name == "server_name")
		for (size_t i = 0; i < args.size(); i++)
			srv.server_names.push_back(args[i]);
	else if (name == "root") {
		if (args.size() != 1)
			throwError("One argument expected for 'root'");
		srv.root = args[0];
	}
	else if (name == "index")
		for (size_t i = 0; i < args.size(); i++)
			srv.index.push_back(args[i]);
	else if (name == "error_page")
		parseErrorPage(srv, args);
	else if (name == "client_max_body_size") {
		if (args.size() != 1)
			throwError("One argument expected for 'client_max_body_size'");
		srv.client_max_body_size = parseSize(args[0]);
		srv.has_client_max_body_size = true;
	}
	else
		throwError("Directive unknown");
}

void	Parser::parseDirective(LocationConfig& loc) {
	std::string					name = _ts.next().content;
	std::vector<std::string>	args = collectArgs();

	if (name == "root") {
		if (args.size() != 1)
			throwError("One argument expected for 'root'");
		loc.root = args[0];
	}
	else if (name == "index")
		for (size_t i = 0; i < args.size(); i++)
			loc.index.push_back(args[i]);
	else if (name == "autoindex") {
		if (args.size() != 1)
			throwError("One argument expected for 'autoindex'");
		if (args[0] == "on")
			loc.autoindex = true;
		else
			loc.autoindex = false;
	}
	else if (name == "cgi") {
		parseCgi(loc, args);
	}
	else if (name == "client_max_body_size") {
		if (args.size() != 1)
			throwError("One argument expected for 'client_max_body_size'");
		loc.client_max_body_size = parseSize(args[0]);
		loc.has_client_max_body_size = true;
	}
	else if (name == "methods")
		loc.methods = parseMethods(args);
	else if (name == "upload_store") {
		if (args.size() != 1)
			throwError("One argument expected for 'upload_store'");
		loc.upload_store = args[0];
	}
	else if (name == "return")
		parseReturn(loc, args);
	else
		throwError("Directive unknown1");
}

Listen			Parser::parseListen(std::vector<std::string>& args) {
	Listen l;

	if (args.size() == 0)
		throwError("Expected argument after directive 'listen'");
	else if (args.size() > 1)
		throwError("Too many arguments after directive 'listen'");

	std::string s = args[0];
	size_t pos = s.find(':');
	if (pos == std::string::npos) {
		if (!isNumber(s))
			throwError("Invalid port in 'listen'");
		int p = std::atoi(s.c_str());
		if (p <= 0 || p > 65535)
			throwError("Port out of range, must be between [0, 65535]");
		l.port = p;
		l.host = "*";
	}
	else {
		l.host = s.substr(0, pos);
		std::string sub = s.substr(pos + 1);
		if (!isNumber(sub))
			throwError("Invalid port in 'listen'");
		int p = std::atoi(sub.c_str());
		if (p <= 0 || p > 65535)
			throwError("Port out of range, must be between [0, 65535]");
		l.port = p;
	}
	return (l);
}

void	Parser::parseErrorPage(ServerConfig& srv, const std::vector<std::string>& args) {
	if (args.size() < 2 || (args.size() % 2 != 0))
		throwError("'error_page' expect code and path");
	for (size_t i = 0; i < args.size(); i += 2) {
		const std::string &code = args[i];
		const std::string &path = args[i+1];
		if (!isNumber(code))
			throwError("Invalid error code");
		int c = atoi(code.c_str());
		srv.error_pages[c] = path;
	}
}

size_t	Parser::parseSize(const std::string& s) {
	std::string	num = s;
	char		suffix = 0;

	if (!isdigit(static_cast<unsigned char>(num[num.size() - 1]))) {
		suffix = num[num.size() - 1];
		num = num.substr(0, num.size() - 1);
	}
	if (!isNumber(num))
		throwError("Invalid number");
	size_t val = static_cast<size_t>(std::atol(num.c_str()));
	if (suffix == 'K' || suffix == 'k')
		val *= 1024;
	else if (suffix == 'M' || suffix == 'm')
		val *= 1024 * 1024;
	else if (suffix != 0)
		throwError("Unknown suffix");
	return (val);
}

std::vector<std::string>	Parser::parseMethods(const std::vector<std::string>& args) {
	std::vector<std::string>	res;

	if (args.size() == 0)
		throwError("Missing arguments for 'methods'");
	for (size_t i = 0; i < args.size(); i++) {
		std::string	m = args[i];
		if (m != "GET" && m != "POST" && m != "DELETE")
			throwError("Unsupported method '" + m + "'");
		res.push_back(m);
	}
	return (res);
}

void	Parser::parseCgi(LocationConfig& loc, const std::vector<std::string>& args) {
	if (args.size() % 2 != 0)
		throwError("extension and executable expected by 'cgi'");
	for (size_t i = 0; i < args.size(); i += 2) {
		const std::string &ext = args[i];
		const std::string &exec = args[i+1];
		loc.cgi[ext] = exec;
	}
}

void Parser::parseReturn(LocationConfig &loc, const std::vector<std::string> &args) {
	if (args.size() != 2)
		throwError("'return' expects status and target");
	const std::string &s = args[0];
	if (!isNumber(s))
		throwError("Invalid return status");
	int st = atoi(s.c_str());
	loc.return_code = st;
	loc.return_url = args[1];
}

/////////////////////////////////////

void Parser::throwError(const std::string &msg) const {
	const Token &t = _ts.peek();
	std::ostringstream oss;
	if (t.l > 0)
		oss << "ligne " << t.l << " " << "col " << t.c << ": " << msg;
	else
		oss << msg;
	throw ParserError(oss.str());
}

std::vector<std::string>	Parser::collectArgs() {
	std::vector<std::string>	args;

	while (!_ts.eof() && _ts.peek().type != T_SEMICOLON) {
		const Token& t = _ts.peek();
		if (t.type == T_IDENT || t.type == T_STR || t.type == T_NUM) {
			args.push_back(t.content);
			_ts.next();
		}
		else
			throwError("Unexpected token inside directive");
	}
	if (_ts.eof())
		throwError("Unterminated directive block");
	_ts.next();
	return (args);
}

/////////////////////////////////////

