/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:55:59 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/11 00:09:07 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

# include "Webserv.hpp"
# include <list>

enum	tokenType {
	T_EOF,
	T_IDENT,
	T_STR,
	T_NUM,
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
	Lexer(const char *path);
	~Lexer();

	const std::list<Token>&	getTokens() const;
	void					printTokens();
	
	class	LexerError : public std::exception {
	public:
		LexerError(const std::string& msg) throw() {
			_msg = "Error: Lexer: " + msg;
		}
		virtual ~LexerError() throw() {}
		virtual const char*	what() const throw() {
			return (_msg.c_str());
		}
	private:
		std::string	_msg;
	};

private:
	std::string			_path;
	std::string			_f;
	std::list<Token>	_t;
	size_t				_l;
	size_t				_c;
	size_t				i;

	void	SkipWhiteSpaceAndComment();
	bool	isSep(const char& c) const;
};


#endif