/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 18:34:52 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/15 14:10:07 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"
#include <fstream>
#include <sstream>

Token::Token()
	: type(T_UNKNOWN),
	  content(),
	  l(1),
	  c(1) {}

Lexer::Lexer(const std::string& path)
	: _path(path), _f(), _tok(), _l(1), _c(1), i(0) {
	std::ifstream	file(path.c_str(), std::ios::binary);
	if (!file.is_open())
		throw ParserError("Cannot open " + _path);
	std::ostringstream	ss;
	ss << file.rdbuf();
	_f = ss.str();
	file.close();
	if (_f.empty())
		throw ParserError(_path + " is empty");

	size_t	start = 0;
	size_t	end = 0;
	while (i < _f.size()) {
		Token	newToken;
		SkipWhiteSpaceAndComment();
		if (i >= _f.size())
			break ;
		if (!isSep(_f[i])) {
			start = i;
			while (i < _f.size() && !isSep(_f[i])) {
				_c++;
				i++;
			}
			end = i;
			newToken.content = _f.substr(start, end - start);
			newToken.type = findIdentifier(newToken.content);
			newToken.l = _l;
			newToken.c = _c - newToken.content.size();
			_tok.push_back(newToken);
		}
		else if (isSep(_f[i])) {
			newToken.content = _f.substr(i, 1);
			newToken.type = findIdentifier(newToken.content);
			newToken.l = _l;
			newToken.c = _c;
			_tok.push_back(newToken);
			i++;
			_c++;
		}
		SkipWhiteSpaceAndComment();
	}
	if (_tok.empty())
		throw ParserError(_path + " is empty");
	Token	lastToken;
	lastToken.type = T_EOF;
	lastToken.content = "";
	lastToken.l = 0;
	lastToken.c = 0;
	_tok.push_back(lastToken);

	it = _tok.begin();
}

Lexer::~Lexer() {}

////////////////////////////////////////////

const std::list<Token>&	Lexer::getTokens() const {
	return (_tok);
}

static const char* tokenTypeToString(tokenType t) {
	switch (t) {
		case T_EOF:			return "T_EOF";
		case T_SYMBOL:		return "T_SYMBOL";
		case T_IDENT:		return "T_IDENT";
		case T_LBRACE:		return "T_LBRACE";
		case T_RBRACE:		return "T_RBRACE";
		case T_SEMICOLON:	return "T_SEMICOLON";
		case T_STR:			return "T_STR";
		case T_NUM:			return "T_NUM";
		default:   			return "T_UNKNOWN";
	}
}

void Lexer::printTokens() {
	size_t idx = 0;
	for (std::list<Token>::const_iterator it = _tok.begin(); it != _tok.end(); ++it, ++idx) {
		const Token& tok = *it;
		std::cout << idx << ": "
					<< tokenTypeToString(tok.type)
					<< " | content: \"" << tok.content << "\""
					<< " | line: " << tok.l << " col: " << tok.c
					<< '\n';
	}
}

////////////////////////////////////////////

void	Lexer::SkipWhiteSpaceAndComment() {
	while (i < _f.size() && _f[i] == '#') {
		while (i < _f.size()) {
			if (_f[i] == '\n') {
				_l++;
				_c = 1;
				i++;
				break ;
			}
			_c++;
			i++;
		}
	}
	while (i < _f.size() && std::isspace(_f[i])) {
		if (_f[i] == '\n') {
			_l++;
			_c = 1;
		}
		else if (_f[i] == '\t')
			_c += 4;
		else
			_c++;
		i++;
	}
}

bool	Lexer::isSep(const char& c) const {
	if (std::isspace(static_cast<unsigned char>(c)) || c == '{' || c == '}' || c == ';')
		return (true);
	return (false);
}

bool	isNumber(const std::string& str) {
	if (str.empty())
		return (false);
	for (size_t j = 0; j < str.size(); j++) {
		if (!std::isdigit(static_cast<unsigned char>(str[j])))
			return (false);
	}
	return (true);
}

tokenType	Lexer::findIdentifier(const std::string& str) const {
	if (str == "server"
		|| str == "listen"
		|| str == "server_name"
		|| str == "root"
		|| str == "index"
		|| str == "client_max_body_size"
		|| str == "error_page"
		|| str == "location"
		|| str == "methods"
		|| str == "autoindex"
		|| str == "upload_store"
		|| str == "cgi"
		|| str == "return")
		return (T_IDENT);
	else if (str == "{")
		return (T_LBRACE);
	else if (str == "}")
		return (T_RBRACE);
	else if (str == ";")
		return (T_SEMICOLON);
	else if (isNumber(str))
		return (T_NUM);
	else
		return (T_STR);
}

////////////   Token Stream   //////////////

const Token&	Lexer::peek() const {
	if (it == _tok.end())
		return (*(_tok.end()));
	return (*it);
}

const Token&	Lexer::peekLast() const {
	return (*_last);
}

const Token&	Lexer::next() {
	if (it == _tok.end())
		return (*(_tok.end()));
	const Token& t = *it;
	_last = it;
	++it;
	return (t);
}

bool	Lexer::eof() const {
	return (it == _tok.end());
}

void	Lexer::restore(Iter newIt) {
	it = newIt;
}