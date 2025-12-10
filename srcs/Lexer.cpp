/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 18:34:52 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/11 00:49:21 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"
#include <fstream>
#include <sstream>

Token::Token()
	: type(T_EOF),
	  content(),
	  l(1),
	  c(1) {}

Lexer::Lexer(const char* path): _path(path), _f(), _t(), _l(1), _c(1), i(0) {
	std::ifstream	file(path, std::ios::binary);
	if (!file.is_open())
		throw LexerError("Cannot open " + _path);
	std::ostringstream	ss;
	ss << file.rdbuf();
	_f = ss.str();
	file.close();
	if (_f.empty())
		throw LexerError(_path + " is empty");

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
			newToken.type = T_IDENT;
			newToken.content = _f.substr(start, end - start);
			newToken.l = _l;
			newToken.c = _c - newToken.content.size();
			_t.push_back(newToken);
		}
		else if (isSep(_f[i])) {
			newToken.type = T_SYMBOL;
			newToken.content = _f.substr(i, 1);
			newToken.l = _l;
			newToken.c = _c;
			_t.push_back(newToken);
			i++;
			_c++;
		}
		SkipWhiteSpaceAndComment();
	}
}

Lexer::~Lexer() {}

////////////////////////////////////////////

const std::list<Token>&	Lexer::getTokens() const {
	return (_t);
}

static const char* tokenTypeToString(tokenType t) {
	switch (t) {
		case T_EOF:    return "T_EOF";
		case T_SYMBOL: return "T_SYMBOL";
		case T_IDENT:  return "T_IDENT";
		case T_STR:    return "T_STR";
		case T_NUM:    return "T_NUM";
		default:       return "UNKNOWN";
	}
}

void Lexer::printTokens() {
	size_t idx = 0;
	for (std::list<Token>::const_iterator it = _t.begin(); it != _t.end(); ++it, ++idx) {
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
	if (std::isspace(c) || c == '{' || c == '}' || c == ';')
		return (true);
	return (false);
}