/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 19:33:12 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/14 16:38:19 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "Webserv.hpp"
# include "Config.hpp"

class	Logger {
public:
	static Logger&	instance();

	static void	init(const std::vector<ServerConfig> servers);

	static void	log(const std::string& msg);
	static void	log(const std::string& msg, int srvIdx);
	static void	info(const std::string& msg);
	static void	info(const std::string& msg, int srvIdx);
	static void	error(const std::string& msg);
	static void	error(const std::string& msg, int srvIdx);

private:
	Logger();
	~Logger();

	std::map<size_t, std::ofstream*>	_ofs;
	std::map<size_t, bool>				_active;

	bool		_initLogger(const std::string& filePath, size_t srvIdx);
	bool		_isDirForFile(const std::string& filePath);
	bool		_createDir(const std::string& dir);

	void		_generateLog(const std::string& msg, size_t srvIdx);
	void		_generateLogForAll(const std::string& msg);
	std::string	_makeTimestamp() const;

	void		_close();
};

#endif