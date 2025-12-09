/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 18:34:52 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/09 18:39:30 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

Token::Token()
	: type(T_EOF),
	  content(),
	  l(1),
	  c(1),
	  next(nullptr),
	  prev(nullptr) {}

Lexer::Lexer(const char* path): _t(nullptr) {
	
}