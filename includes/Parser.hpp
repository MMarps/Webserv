/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arotondo <arotondo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:52:34 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/17 15:49:40 by arotondo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "Webserv.hpp"
# include "Lexer.hpp"
# include "Config.hpp"

class	Lexer;
struct	Listen;
struct	LocationConfig;
struct	ServerConfig;
class	Config;

class Parser {
	public:
		Parser(Lexer& ts);
		Config	parseConfig();
		
	private:
		Lexer&	_ts;
	
		void	throwError(const std::string& msg, bool flg) const;
	
		std::vector<std::string>	collectArgs();
	
		ServerConfig	parseServer();
		LocationConfig	parseLocation();
	
		void			parseDirective(ServerConfig& srv);
		void			parseDirective(LocationConfig& srv);
	
		Listen			parseListen(std::vector<std::string>& args);
		void			parseErrorPage(ServerConfig& srv, const std::vector<std::string>& args);
		size_t			parseSize(const std::string& s);
	
		std::vector<std::string>	parseMethods(const std::vector<std::string>& args);
	
		void			parseCgi(ServerConfig& srv, const std::vector<std::string>& args);
		void			parseCgi(LocationConfig& loc, const std::vector<std::string>& args);
		void 			parseReturn(LocationConfig& loc, const std::vector<std::string>& args);
	
		void			putDefaultValues(Config &cfg);
		void			checkCgi(Config &cfg);
		void			getLogFile(ServerConfig& srv, int srvIdx);
		std::string		findRootDir(const std::string& root);
};

#endif