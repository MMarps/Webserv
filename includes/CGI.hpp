#ifndef CGI_HPP
# define CGI_HPP

# include "Webserv.hpp"
# include "Request.hpp"
# include "Config.hpp"

class	Request;
struct	ServerConfig;

class CGI {
	private:
		Request				&_req;
		ServerConfig		&_server;
		std::string			_scriptPath;
		std::string			_interpreter;
		std::vector<char>	_output;
		std::string			_body;
		std::map<std::string, std::string> _cgiHeaders;
		int					_statusCode;
		int					_timeout;
		size_t				_writtenBytes;
		bool				_outputOverflow;

		void				addHTTPHeaders(std::vector<std::string> &env);
		void				parseOutput();
		bool				processScript(char **env);
		void				executeScript(char **env);
		bool				validateExecutionContext();
		std::string			findInterpreter();
		std::string			integerToString(size_t value);
		void				freePipes(int *fdIn, int *fdOut);
		void				freeEnv(char **env);
		void				addEnv(std::vector<std::string> &env, const std::string &key, const std::string &value);
		char				**vectorToEnv(const std::vector<std::string> &env);
		char				**setupEnv(const Request &req);

		pid_t				_pid;
		int					_pipeIn[2];
		int					_pipeOut[2];
		time_t				_startTime;

	public:
		CGI(Request &req, ServerConfig &server);
		~CGI();
		bool				executeAsync(const Request &req);
		int					getStatusCode() const;
		std::string			getBody() const;
		std::map<std::string, std::string>	getHeaders() const;
		bool				isCGI(const Request &req, const ServerConfig &server);

		pid_t				getPid() const;
		void				finalizeCGI(int status);
		int					getPipeOut() const;
		int					getPipeIn() const;
		size_t				getWrittenBytes() const;
		void				addWrittenBytes(size_t bytes);
		bool				isTimedOut() const;
		bool				appendOutput(const char* buffer, size_t size);

		enum {
			MAX_CGI_OUTPUT_BYTES = 8 * 1024 * 1024
		};
};

# endif