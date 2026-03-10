/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigPrint.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jle-doua <jle-doua@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:44:36 by mmarps            #+#    #+#             */
/*   Updated: 2026/03/10 15:11:59 by jle-doua         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigPrint.hpp"
#include <iomanip>
#include <sstream>

static void	indent(std::ostream &os, int level) {
	for (int i = 0; i < level; ++i) os << "  ";
}

static void	printStringList(std::ostream &os, std::vector<std::string> const &v) {
	for (size_t i = 0; i < v.size(); ++i) {
		if (i) os << ", ";
		os << v[i];
	}
}

static void	printErrorPages(std::ostream &os, std::map<int, std::string> const &m) {
	bool first = true;
	for (std::map<int, std::string>::const_iterator it = m.begin(); it != m.end(); ++it) {
		if (!first) os << ", ";
		os << it->first << "->" << it->second;
		first = false;
	}
}

void	printConfig(Config const &cfg, std::ostream &os) {
	os << "Config: " << cfg.servers.size() << " server(s)\n";
	for (size_t si = 0; si < cfg.servers.size(); ++si) {
		ServerConfig const &srv = cfg.servers[si];
		os << "Server[" << si << "]\n";

		// listens
		indent(os, 1); os << "listens: ";
		if (srv.listens.empty()) os << "(none)";
		for (size_t i = 0; i < srv.listens.size(); ++i) {
			if (i) os << ", ";
			os << srv.listens[i].host << ":" << srv.listens[i].port;
		}
		os << "\n";

		// root
		indent(os, 1); os << "root: " << (srv.root.empty() ? "(none)" : srv.root) << "\n";

		// index
		indent(os, 1); os << "index: ";
		if (srv.index.empty()) os << "(none)"; else printStringList(os, srv.index);
		os << "\n";

		// error pages
		indent(os, 1); os << "error_pages: ";
		if (srv.error_pages.empty()) os << "(none)"; else printErrorPages(os, srv.error_pages);
		os << "\n";

		// cgi
		indent(os, 1); os << "cgi: ";
		if (srv.cgi.empty()) os << "(none)";
		else {
			bool first = true;
			for (std::map<std::string, std::string>::const_iterator it = srv.cgi.begin(); it != srv.cgi.end(); ++it) {
				if (!first) os << ", ";
				os << it->first << "->" << it->second;
				first = false;
			}
		}
		os << "\n";

		// client max body size
		indent(os, 1);
		os << "client_max_body_size: ";
		if (srv.has_client_max_body_size) os << srv.client_max_body_size;
		else os << "(unset)";
		os << "\n";

		// locations
		os << "  locations: " << srv.locations.size() << "\n";
		for (size_t li = 0; li < srv.locations.size(); ++li) {
			LocationConfig const &loc = srv.locations[li];
			indent(os, 2); os << "Location[" << li << "] path=\"" << loc.path << "\"\n";

			indent(os, 3); os << "root: " << (loc.root.empty() ? "(none)" : loc.root) << "\n";

			indent(os, 3); os << "index: ";
			if (loc.index.empty()) os << "(none)"; else printStringList(os, loc.index);
			os << "\n";

			indent(os, 3); os << "methods: ";
			if (loc.methods.empty()) os << "(none)"; else printStringList(os, loc.methods);
			os << "\n";

			indent(os, 3); os << "autoindex: " << (loc.autoindex ? "on" : "off") << "\n";

			indent(os, 3); os << "upload_store: " << (loc.upload_store.empty() ? "(none)" : loc.upload_store) << "\n";

			indent(os, 3); os << "client_max_body_size: ";
			if (loc.has_client_max_body_size) os << loc.client_max_body_size;
			else os << "(inherit/none)";
			os << "\n";

			indent(os, 3); os << "return: ";
			if (loc.has_return) os << loc.return_code << " -> " << loc.return_url;
			else os << "(none)";
			os << "\n";

			// cgi map
			indent(os, 3); os << "cgi: ";
			if (loc.cgi.empty()) os << "(none)";
			else {
				bool first = true;
				for (std::map<std::string, std::string>::const_iterator it = loc.cgi.begin(); it != loc.cgi.end(); ++it) {
					if (!first) os << ", ";
					os << it->first << "->" << it->second;
					first = false;
				}
			}
			os << "\n";
		}

		os << "\n";
	}
	os.flush();
}