#ifndef CGI_HPP
# define CGI_HPP

# include "Webserv.hpp"
# include "Request.hpp"
# include "Config.hpp"

class Request;
struct ServerConfig;

struct EnvCGI {
	const std::string	&method;
	const std::string	&scriptName;
	const std::string	&scriptFilename;
	const std::string	&queryString;
	const std::string	&contentType;
	const std::string	&contentLength;
};

class CGI {
	private:
		Request&			_req; // reference vers la Requete
		ServerConfig&		_server; // reference vers la Config-Serveur
		std::string			_scriptPath; // path du script a exec
		std::string			_interpreter; // path de l interpreter (.php, .py...)
		std::vector<char>	_output; // output du script
		std::string			_body; // body de la sortie (apres headers)
		std::map<std::string, std::string> _cgiHeaders; // header retourne
		int					_statusCode; // statut de l'output du script
		int					_timeout; // timeout en secondes
		EnvCGI				_env;

		void				addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value);
		// void				importEnv(std::vector<std::string> &env, const std::string &key);
		char				**setupEnv(const Request &req);

		void				addHTTPHeaders(std::vector<std::string> &env);
		void				freeEnv(char **env);
		void				parseOutput();
		char				**vectorToEnv(std::vector<std::string> &env);
		bool				executeScript(char **env);
		std::string			findInterpreter();

	public:
		CGI(Request &req, ServerConfig &server, EnvCGI &env);
		~CGI();
		bool				isCGI();
		bool				execute(const Request &req); 
		int					getStatusCode();
		std::vector<char>	getOutput();
		std::map<std::string, std::string>	getHeaders(); // recupere les headers CGI
};

std::string	integerToString(size_t val);

# endif