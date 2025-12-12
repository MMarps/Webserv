/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:52:34 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/11 19:07:20 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Webserv.hpp"
# include "Lexer.hpp"
# include "Config.hpp"

class	Parser {
public:
	Parser(Lexer& ts);
	Config	parseConfig();
	

private:
	Lexer& _ts;

	void	throwError(const std::string& msg) const;
};

#endif