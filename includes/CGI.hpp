#ifndef CGI_HPP
# define CGI_HPP

# include "Webserv.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Config.hpp"
# include "Client.hpp"

class	Request;
class	Client;
struct	ServerConfig;

class CGI {
	private:
		Request				&_req; // reference vers la Requete
		ServerConfig		&_server; // reference vers la Config-Serveur
		// Client				&_client; // reference vers le Client
		std::string			_scriptPath; // path du script a exec
		std::string			_interpreter; // path de l interpreter (.php, .py...)
		std::vector<char>	_output; // output du script
		std::string			_body; // body de la sortie (apres headers)
		std::map<std::string, std::string> _cgiHeaders; // header retourne
		int					_statusCode; // statut de l'output du script
		int					_timeout; // timeout en secondes
		size_t				_writtenBytes;

		void				addHTTPHeaders(std::vector<std::string> &env);
		void				parseOutput();
		bool				waitProcess(pid_t pid);
		bool				processScript(char **env);
		void				executeScript(char **env);
		void				parentProcess(int *fdIn, int *fdOut);
		// UtilsCGI.hpp
		std::string			findInterpreter();
		std::string			integerToString(size_t value);
		void				freePipes(int *fdIn, int *fdOut);
		void				freeEnv(char **env);
		void				addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value);
		char				**vectorToEnv(std::vector<std::string> &env);
		char				**setupEnv(const Request &req);

		pid_t				_pid;
		int					_pipeIn[2];
		int					_pipeOut[2];
		time_t				_startTime;
		bool				_readComplete;

	public:
		CGI(Request &req, ServerConfig &server);
		~CGI();
		bool				execute(const Request &req);
		bool				executeAsync(const Request &req);
		int					getStatusCode() const;
		std::vector<char>	getOutput() const;
		std::string			getBody() const;
		std::map<std::string, std::string>	getHeaders() const; // recupere les headers CGI
		bool				isCGI(const Request &req, const ServerConfig &server);
	
		// Methodes pour epoll
		void				appendOutput(const char* buffer, size_t size);
		pid_t				getPid() const;
		void				finalizeCGI(int status);
		int					getPipeOut() const;
		int					getPipeIn() const;
		size_t				getWrittenBytes() const;
		void				addWrittenBytes(size_t bytes);
};

# endif