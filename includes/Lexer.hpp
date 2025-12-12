/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:55:59 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/12 18:21:36 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

# include "Webserv.hpp"
# include <list>

class	ParserError : public std::exception {
public:
	ParserError(const std::string& msg) throw() {
		_msg = "Error: Parsing: " + msg;
	}
	virtual ~ParserError() throw() {}
	virtual const char*	what() const throw() {
		return (_msg.c_str());
	}
private:
	std::string	_msg;
};

enum	tokenType {
	T_EOF,
	T_UNKNOWN,
	T_IDENT,
	T_STR,
	T_NUM,
	T_LBRACE,
	T_RBRACE,
	T_SEMICOLON,
	T_SYMBOL
};

struct	Token {
	tokenType		type;
	std::string		content;
	size_t			l;
	size_t			c;
	Token();
};

class	Lexer {
public:
	Lexer(const std::string& path);
	~Lexer();

	const std::list<Token>&	getTokens() const;
	void					printTokens();

	/// Token Stream
	typedef std::list<Token>::const_iterator	Iter;

	const Token&	peek() const;
	const Token&	next();
	bool			eof() const;

	void			restore(Iter newIt);

private:
	std::string			_path;
	std::string			_f;
	std::list<Token>	_tok;
	size_t				_l;
	size_t				_c;

	size_t				i;
	Iter				it;

	void		SkipWhiteSpaceAndComment();
	bool		isSep(const char& c) const;
	tokenType	findIdentifier(const std::string& str) const;
};

bool	isNumber(const std::string& str);

#endif