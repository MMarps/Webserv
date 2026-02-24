*This project has been created as part of the 42 curriculum by jle-doua, arotondo, mmarpaul.*

# WebServ - A C++ HTTP/1.1 Server

## Description
This project consists of developing a functional HTTP/1.1 web server in **C++ 98**. The goal is to understand the inner workings of the HTTP protocol, non-blocking I/O, and the multiplexing of connections using `epoll()`.

The server is capable of:
* **Handling multiple client connections** simultaneously without hanging.
* **Parsing and responding** to HTTP methods: `GET`, `POST`, and `DELETE`.
* **Serving static websites** and handling file uploads.
* **Executing CGI scripts** (Python, PHP, etc.) to handle dynamic content.
* **Full configuration** via a file similar to Nginx.

## Instructions

### Prerequisites
* A C++ compiler (`c++` or `g++`).
* `Make` build tool.
* A Linux environment (required for `epoll`).

### Installation & Compilation
1. **Clone the repository:**
   ```Bash
	git clone [https://github.com/jle-doua/webserv.git](https://github.com/jle-doua/webserv.git)
	cd webserv

2.	**Compile the project:**
	```Bash

    make

3.	**Running the Server:**

	To start the server, you must provide a configuration file as an argument:
	```Bash

	./webserv configs/default.conf

Once launched, you can access the server by navigating to http://localhost:8080 (or the port defined in your configuration) in your web browser.

###	Technical Features

* I/O Multiplexing: Uses a single process to handle multiple clients efficiently.

* Custom Configuration: Support for multiple server blocks, listen ports, server_names, and custom error_pages.

* CGI Execution: Handles environment variables and pipes to communicate with external scripts.

* Resilience: The server remains stable even under heavy load or invalid requests.

###	Resources
* Documentation & Articles

* Beej's Guide to Network Programming - The gold standard for socket programming.

* RFC 2616 (HTTP/1.1) - The official protocol specification.

* Nginx Configuration Docs - Reference for the configuration file logic.

###	AI Usage Disclosure

* Task: Assistance in debugging complex socket state transitions and generating the initial boilerplate for the configuration file parser.

* Parts of the project: The ConfigParser class logic and the MimeTypes helper class.

* Tool used: ChatGPT / Gemini to clarify specific RFC edge cases regarding header formatting and error code management.