/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:52:31 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/02 16:47:13 by jle-doua         ###   ########.fr       */
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
			throwError("Expected 'server' at top level", false);
	}
	putDefaultValues(cfg);
	return (cfg);
}

ServerConfig	Parser::parseServer() {
	ServerConfig	srv;

	if (_ts.eof() || _ts.peek().type != T_LBRACE)
		throwError("Expected '{' after 'server' identifier", false);
	_ts.next();
	while (!_ts.eof() && _ts.peek().type != T_RBRACE) {
		const Token& t = _ts.peek();
		if (t.type == T_IDENT && t.content == "location") {
			srv.locations.push_back(parseLocation());
		}
		else if (t.type == T_IDENT) {
			parseDirective(srv);
		}
		// else if (t.type == T_EOF/*  || _ts.eof() */)
		// 	break ;
		else
			throwError("Unexpected token inside 'server' block", false);
	}
	if (_ts.eof())
		throwError("Unterminated 'server' block", false);
	_ts.next();
	return (srv);
}

LocationConfig	Parser::parseLocation() {
	LocationConfig	loc;

	_ts.next();
	if (_ts.eof())
		throwError("Expected location path after 'location'", false);
	const Token& p = _ts.peek();
	if (p.type != T_IDENT && p.type != T_STR && p.type != T_NUM)
		throwError("Invalid location path", false);
	loc.path = p.content;
	_ts.next();
	if (_ts.eof() || _ts.peek().type != T_LBRACE)
		throwError("Expected '{' after location path", false);
	_ts.next();
	while (!_ts.eof() && _ts.peek().type != T_RBRACE) {
		if (_ts.peek().type == T_IDENT)
			parseDirective(loc);
		else
			throwError("Unexpected token inside 'location' block", false);
	}
	if (_ts.eof())
		throwError("Unterminated 'location' block", false);
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
			throwError("One argument expected for 'root'", true);
		srv.root = args[0];
	}
	else if (name == "index")
		for (size_t i = 0; i < args.size(); i++)
			srv.index.push_back(args[i]);
	else if (name == "error_page")
		parseErrorPage(srv, args);
	else if (name == "cgi")
		parseCgi(srv, args);
	else if (name == "client_max_body_size") {
		if (args.size() != 1)
			throwError("One argument expected for 'client_max_body_size'", true);
		srv.client_max_body_size = parseSize(args[0]);
		srv.has_client_max_body_size = true;
	}
	else
		throwError("Directive unknown", true);
}

void	Parser::parseDirective(LocationConfig& loc) {
	std::string					name = _ts.next().content;
	std::vector<std::string>	args = collectArgs();

	if (name == "root") {
		if (args.size() != 1)
			throwError("One argument expected for 'root'", true);
		loc.root = args[0];
	}
	else if (name == "index")
		for (size_t i = 0; i < args.size(); i++)
			loc.index.push_back(args[i]);
	else if (name == "autoindex") {
		if (args.size() != 1)
			throwError("One argument expected for 'autoindex'", true);
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
			throwError("One argument expected for 'client_max_body_size'", true);
		loc.client_max_body_size = parseSize(args[0]);
		loc.has_client_max_body_size = true;
	}
	else if (name == "methods")
		loc.methods = parseMethods(args);
	else if (name == "upload_store") {
		if (args.size() != 1)
			throwError("One argument expected for 'upload_store'", true);
		loc.upload_store = args[0];
	}
	else if (name == "return")
		parseReturn(loc, args);
	else
		throwError("Directive unknown1", true);
}

Listen			Parser::parseListen(std::vector<std::string>& args) {
	Listen l;

	if (args.size() == 0)
		throwError("Expected argument after directive 'listen'", true);
	else if (args.size() > 1)
		throwError("Too many arguments after directive 'listen'", true);

	std::string s = args[0];
	size_t pos = s.find(':');
	if (pos == std::string::npos) {
		if (!isNumber(s))
			throwError("Invalid port in 'listen'", true);
		int p = std::atoi(s.c_str());
		if (p <= 0 || p > 65535)
			throwError("Port out of range, must be between [0, 65535]", true);
		l.port = p;
		l.host = "*";
	}
	else {
		l.host = s.substr(0, pos);
		std::string sub = s.substr(pos + 1);
		if (!isNumber(sub))
			throwError("Invalid port in 'listen'", true);
		int p = std::atoi(sub.c_str());
		if (p <= 0 || p > 65535)
			throwError("Port out of range, must be between [0, 65535]", true);
		l.port = p;
	}
	return (l);
}

void	Parser::parseErrorPage(ServerConfig& srv, const std::vector<std::string>& args) {
	if (args.size() < 2 || (args.size() % 2 != 0))
		throwError("'error_page' expect code and path", true);
	for (size_t i = 0; i < args.size(); i += 2) {
		const std::string &code = args[i];
		const std::string &path = args[i+1];
		if (!isNumber(code))
			throwError("Invalid error code", true);
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
		throwError("Invalid number", true);
	size_t val = static_cast<size_t>(std::atol(num.c_str()));
	if (suffix == 'K' || suffix == 'k')
		val *= 1024;
	else if (suffix == 'M' || suffix == 'm')
		val *= 1024 * 1024;
	else if (suffix != 0)
		throwError("Unknown suffix", true);
	return (val);
}

std::vector<std::string>	Parser::parseMethods(const std::vector<std::string>& args) {
	std::vector<std::string>	res;

	if (args.size() == 0)
		throwError("Missing arguments for 'methods'", true);
	for (size_t i = 0; i < args.size(); i++) {
		std::string	m = args[i];
		if (m != "GET" && m != "POST" && m != "DELETE")
			throwError("Unsupported method '" + m + "'", true);
		res.push_back(m);
	}
	return (res);
}

void	Parser::parseCgi(ServerConfig& srv, const std::vector<std::string>& args) {
	if (args.size() % 2 != 0)
		throwError("extension and executable expected by 'cgi'", true);
	for (size_t i = 0; i < args.size(); i += 2) {
		const std::string &ext = args[i];
		if (ext[0] != '.') {
			std::ostringstream	oss;
			oss << "\'" << ext << "\' " << "is not a valide extension";
			throwError(oss.str(), true);
		}
		const std::string &exec = args[i + 1];
		srv.cgi[ext] = exec;
	}
}

void	Parser::parseCgi(LocationConfig& loc, const std::vector<std::string>& args) {
	if (args.size() % 2 != 0)
		throwError("extension and executable expected by 'cgi'", true);
	for (size_t i = 0; i < args.size(); i += 2) {
		const std::string &ext = args[i];
		// std::cout << BRED << ext[0] << NC << std::endl;
		if (ext[0] != '.') {
			std::ostringstream	oss;
			oss << "\'" << ext << "\' " << "is not a valide extension";
			throwError(oss.str(), true);
		}
		const std::string &exec = args[i + 1];
		loc.cgi[ext] = exec;
	}
}

void Parser::parseReturn(LocationConfig &loc, const std::vector<std::string> &args) {
	if (args.size() != 2)
		throwError("'return' expects status and target", true);
	const std::string &s = args[0];
	if (!isNumber(s))
		throwError("Invalid return status", true);
	int st = atoi(s.c_str());
	loc.return_code = st;
	loc.return_url = args[1];
}

void	Parser::putDefaultValues(Config &cfg) {
	for (size_t si = 0; si < cfg.servers.size(); si++) {
		ServerConfig &srv = cfg.servers[si];
		if (srv.listens.empty()) {
			Listen l;
			l.host = "0.0.0.0";
			l.port = 8080;
			srv.listens.push_back(l);
		}
		if (srv.root.empty())
			srv.root = "var/www";
		if (srv.index.empty())
			srv.index.push_back("index.html");
	}
}

void			Parser::checkCgi(Config &cfg) {
	for (size_t si = 0; si < cfg.servers.size(); si++) {
		ServerConfig& srv = cfg.servers[si];
		std::map<std::string, std::string>::const_iterator	it;
		for (it = srv.cgi.begin(); it != srv.cgi.end(); it++) {
			std::string ext = it->first;
			if (ext[0] != '.') {
				std::ostringstream	oss;
				oss << "\'" << ext << "\' " << "is not a valide extension";
				throw ParserError(oss.str());
			}
		}
	}
}

/////////////////////////////////////

void Parser::throwError(const std::string &msg, bool flg) const {
	const Token &t = flg ? _ts.peekLast() : _ts.peek();
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
			throwError("Unexpected token inside directive", false);
	}
	if (_ts.eof())
		throwError("Unterminated directive block", false);
	_ts.next();
	return (args);
}

/////////////////////////////////////

