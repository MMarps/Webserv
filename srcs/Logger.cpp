/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmarps <mmarps@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 19:34:30 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/06 00:12:53 by mmarps           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger&	Logger::instance() {
	static Logger inst;
	return (inst);
}

////////////////////////////////////////////

Logger::Logger(): _filePath(""), _ofs(), _active(false) {}

Logger::~Logger() {
	_close();
}

////////////////////////////////////////////

bool	Logger::init(const std::string& filePath) {
	return (instance()._initLogger(filePath));
}

void	Logger::log(const std::string& msg) {
	instance()._generateLog(msg);
}

void	Logger::info(const std::string& msg) {
	instance()._generateLog("[INFO] " + msg);
}

void	Logger::error(const std::string& msg) {
	instance()._generateLog("[ERROR] " + msg);
}

////////////////////////////////////////////

bool	Logger::_initLogger(const std::string& filePath) {
	_filePath = filePath;

	if (!_isDirForFile()) {
		std::cerr << "Error: Logger: unable to create directory for " << _filePath << std::endl;
		_active = false;
		return (false);
	}

	if (access(_filePath.c_str(), F_OK) == 0) {
		if (access(_filePath.c_str(), W_OK) != 0) {
			std::cerr << "Error: Logger: file exist but not writable " << _filePath << std::endl;
			_active = false;
			return (false);
		}
	}

	_ofs.open(_filePath.c_str(), std::ios::out | std::ios::app);
	if (!_ofs.is_open()) {
		std::cerr << "Error: Logger: failed to open file " << _filePath << std::endl;
		_active = false;
		return (false);
	}

	_active = true;
	return (true);
}

bool	Logger::_isDirForFile() {
	size_t pos = _filePath.rfind('/');
	if (pos == _filePath.npos)
		return (true);
	return (_createDir(_filePath.substr(0, pos)));
}

bool	Logger::_createDir(const std::string& dir) {
	if (dir.empty())
		return (false);

	std::string cur;
	size_t start = 0;
	if (dir[0] == '/') {
		cur = "/";
		start = 1;
	}

	while (start <= dir.size()) {
		size_t pos = dir.find('/', start);
		std::string part;
		if (pos == std::string::npos)
			part = dir.substr(start);
		else
			part = dir.substr(start, pos - start);

		if (!part.empty()) {
			if (cur.size() > 1 && cur[cur.size() - 1] != '/')
				cur += "/";
			cur += part;
		}

		struct stat st;
		if (stat(cur.c_str(), &st) != 0) {
			if (mkdir(cur.c_str(), 0755) != 0) {
				if (errno != EEXIST)
					return false;
			}
		}
		else {
			if (!S_ISDIR(st.st_mode))
				return false;
		}

		if (pos == std::string::npos)
			break;
		start = pos + 1;
	}
	return true;
}

////////////////////////////////////////////

void	Logger::_generateLog(const std::string& msg) {
	std::string log = _makeTimestamp()+ " - " + msg;

	std::cout << log << std::endl;

	if (_active && _ofs.is_open())
		_ofs << log << std::endl;
}

std::string	Logger::_makeTimestamp() const {
	char 			buf[64];
	std::time_t		now;
	struct std::tm*	timeInfo;

	now = std::time(NULL);
	timeInfo = std::localtime(&now);

	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeInfo);

	std::string timestamp = "[" + std::string(buf) + "]";
	return (timestamp);
}

void	Logger::_close() {
	if (_ofs.is_open()) {
		_ofs.flush();
		_ofs.close();
	}
	_active = false;
}