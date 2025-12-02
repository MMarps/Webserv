/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarpaul <mmarpaul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/02 16:52:34 by mmarpaul          #+#    #+#             */
/*   Updated: 2025/12/02 18:17:46 by mmarpaul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Webserv.hpp"

class	ConfigParser {
public:
	ConfigParser(const std::string& path);

	class	ErrorException : public std::exception {
	public:
		ErrorException(const std::string& msg) throw() {
			_msg = "Error: ConfigParser: " + msg;
		}
		virtual const char*	what() const throw() {
			return (_msg.c_str());
		}
	private:
		std::string	_msg;
	};

	bool	parse();

private:
	std::string	_path;
};

#endif