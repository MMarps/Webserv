/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 19:34:30 by mmarpaul          #+#    #+#             */
/*   Updated: 2026/02/16 14:50:13 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger&	Logger::instance() {
	static Logger inst;
	return (inst);
}

////////////////////////////////////////////

Logger::Logger(): _ofs(), _active() {}

Logger::~Logger() {
	_close();
}

////////////////////////////////////////////

void	Logger::init(const std::vector<ServerConfig> servers) {
	for (size_t si = 0; si < servers.size(); si++) {
		std::string filePath = servers[si].log;
		instance()._initLogger(filePath, si);
	}
}

////////////////////////////////////////////

void	Logger::log(const std::string& msg) {
	instance()._generateLogForAll(msg);
}

void	Logger::log(const std::string& msg, int srvIdx) {
	if (srvIdx >= 0)
		instance()._generateLog(msg, static_cast<size_t>(srvIdx));
}

void	Logger::info(const std::string& msg) {
	instance()._generateLogForAll("[INFO] " + msg);
}

void	Logger::info(const std::string& msg, int srvIdx) {
	if (srvIdx >= 0)
		instance()._generateLog("[INFO] " + msg, static_cast<size_t>(srvIdx));
}

void	Logger::error(const std::string& msg) {
	instance()._generateLogForAll("[ERROR] " + msg);
}

void	Logger::error(const std::string& msg, int srvIdx) {
	if (srvIdx >= 0)
		instance()._generateLog("[ERROR] " + msg, static_cast<size_t>(srvIdx));
}

////////////////////////////////////////////

bool	Logger::_initLogger(const std::string& filePath, size_t srvIdx) {
	if (!_isDirForFile(filePath)) {
		std::cerr << "Error: Logger: unable to create directory for " << filePath << std::endl;
		_active[srvIdx] = false;
		return (false);
	}

	if (access(filePath.c_str(), F_OK) == 0) {
		if (access(filePath.c_str(), W_OK) != 0) {
			std::cerr << "Error: Logger: file exist but not writable " << filePath << std::endl;
			_active[srvIdx] = false;
			return (false);
		}
	}

	if (_ofs.find(srvIdx) == _ofs.end() || _ofs[srvIdx] == NULL)
		_ofs[srvIdx] = new std::ofstream();
	
	_ofs[srvIdx]->open(filePath.c_str(), std::ios::out | std::ios::app);
	if (!_ofs[srvIdx]->is_open()) {
		std::cerr << "Error: Logger: failed to open file " << filePath << std::endl;
		_active[srvIdx] = false;
		return (false);
	}

	_active[srvIdx] = true;
	return (true);
}

bool	Logger::_isDirForFile(const std::string& filePath) {
	size_t pos = filePath.rfind('/');
	if (pos == filePath.npos)
		return (true);
	return (_createDir(filePath.substr(0, pos)));
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

void	Logger::_generateLog(const std::string& msg, size_t srvIdx) {
	std::stringstream	ss;

	ss << "Server[" << srvIdx << "] " << _makeTimestamp() << " - " << msg;

	std::string log = ss.str();

	std::cout << log << std::endl;

	if (_active[srvIdx] && _ofs[srvIdx] && _ofs[srvIdx]->is_open())
        (*_ofs[srvIdx]) << log << std::endl;
}

void	Logger::_generateLogForAll(const std::string& msg) {
	std::map<size_t, std::ofstream*>::iterator	it;
	std::stringstream							ss;
	std::string	ts = _makeTimestamp();

	size_t idx = 0;
	for (it = _ofs.begin(); it != _ofs.end(); it++) {
		std::ofstream* ofs_ptr = it->second;
		if (ofs_ptr && ofs_ptr->is_open()) {
			ss << "Server[" << idx << "] " << ts << " - " << msg;
			std::string log = ss.str();
			*ofs_ptr << log << std::endl;
			ss.str("");
		}
		idx++;
	}

	std::cout << "Servers   " << ts << " - " << msg << std::endl;
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
	std::map<size_t, std::ofstream*>::iterator it;
	for (it = _ofs.begin(); it != _ofs.end(); it++) {
		std::ofstream* ofs_ptr = it->second;
		if (ofs_ptr) {
			if (ofs_ptr->is_open()) {
				ofs_ptr->flush();
				ofs_ptr->close();
			}
			delete ofs_ptr;
		}
		_active[it->first] = false;
	}
	_ofs.clear();
	_active.clear();
}