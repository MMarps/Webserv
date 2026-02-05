/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 19:33:12 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/06 00:10:28 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "Webserv.hpp"

class	Logger {
public:
	static Logger&	instance();

	static bool	init(const std::string& filePath);

	static void	log(const std::string& msg);
	static void	info(const std::string& msg);
	static void	error(const std::string& msg);

private:
	Logger();
	~Logger();

	std::string		_filePath;
	std::ofstream	_ofs;
	bool			_active;

	bool		_initLogger(const std::string& filePath);
	bool		_isDirForFile();
	bool		_createDir(const std::string& dir);

	void		_generateLog(const std::string& msg);
	std::string	_makeTimestamp() const;

	void		_close();
};

#endif