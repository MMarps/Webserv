#ifndef CGI_HPP
# define CGI_HPP

# include "Webserv.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Config.hpp"

class	Request;
struct	ServerConfig;

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

		char				**setupEnv(const Request &req);
		void				addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value);

		void				addHTTPHeaders(std::vector<std::string> &env);
		void				freeEnv(char **env);
		void				freePipes(int *fdIn, int *fdOut);
		void				parseOutput();
		char				**vectorToEnv(std::vector<std::string> &env);
		std::string			integerToString(size_t val);
		std::string			findInterpreter();
		bool				waitProcess(pid_t pid);
		bool				processScript(char **env);
		void				executeScript(char **env);
		void				parentProcess(int *fdIn, int *fdOut);
	public:
		CGI(Request &req, ServerConfig &server);
		~CGI();
		bool				isCGI();
		bool				execute(const Request &req);
		int					getStatusCode() const;
		std::vector<char>	getOutput() const;
		std::string			getBody() const;
		std::map<std::string, std::string>	getHeaders() const; // recupere les headers CGI
};

# endif