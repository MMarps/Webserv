#ifndef CGI_HPP
# define CGI_HPP

# include "Webserv.hpp"
# include "Request.hpp"
# include "Config.hpp"

class Request;
struct ServerConfig;

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

		void		setupEnv(char **envp);
		void		addMetaVariables(std::vector<std::string> &env, const std::string &key, const std::string &value); // ajoute une variable
		void		addHTTPHeaders(std::vector<std::string> &env);
		void		freeEnvp(char **envp);
		void		parseOutput();
		char		**vectorToEnvp(const std::vector<std::string> &env);
		bool		executeScript(char **envp);
		std::string	findInterpreter();

	public:
		CGI(Request &req, ServerConfig &server);
		~CGI();
		bool				isCGI();
		bool				execute(); 
		int					getStatusCode();
		std::vector<char>	getOutput();
		std::map<std::string, std::string>	getHeaders(); // recupere les headers CGI
};

# endif