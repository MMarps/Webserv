/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 16:55:59 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/09 18:39:45 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

# include "Webserv.hpp"

enum	tokenType {
	T_EOF,
	T_SYMBOL,
	T_IDENT,
	T_STR,
	T_NUM,
	T_SYMBOL
};

struct	Token {
	tokenType		type;
	std::string		content;
	unsigned int	l;
	unsigned int	c;
	Token*			next;
	Token*			prev;
	Token();
};

class	Lexer {
public:
	Lexer(const char *path);
	~Lexer();

	// Token*	getNext() const;
	// Token*	getPrev() const;

private:
	std::string	_path;
	Token*		_t;

};


#endif