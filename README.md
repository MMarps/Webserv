*This project has been created as part of the 42 curriculum by jle-doua, arotondo, mmarpaul.*

# WebServ - C++ HTTP/1.1 Server

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
* A C++ compiler.
* `Make` build tool.
* A Linux environment (required for `epoll`).

### Installation & Compilation
1. **Clone the repository:**
   ```Bash
	git clone https://github.com/MMarps/Webserv.git
	cd webserv

2.	**Compile the project:**
	```Bash

    make

3.	**Running the Server:**

	To start the server, you must provide a configuration file as an argument:
	```Bash

	./webserv configs/default.conf

Once launched, you can access the server by navigating to http://localhost:8080 (or the port defined in your configuration) in your web browser.

### Configuration file

The server expects a configuration file in a format similar to Nginx. General structure:

- `server { ... }` blocks defining a virtual host.
- Main directives (examples):
  - `listen <host:>?<port>;` — listening address and port (e.g. `listen 127.0.0.1:8080;` or `listen 8080;`).
  - `root <path>;` — document root.
  - `index <file1> [file2 ...];` — index files.
  - `error_page <code> <path>;` — custom error page.
  - `client_max_body_size <size>;` — max body size (suffixes `k`, `m`).
  - `cgi <.ext> <executable>;` — map extension to CGI executable.
  - `#` for comments.

- `location <path> { ... }` blocks allow, among others:
  - `methods <GET|POST|DELETE ...>;`
  - `autoindex on|off;`
  - `upload_store <path>;`
  - `client_max_body_size <size>;`
  - `return <code> <url>;`
  - `root`, `index`, `cgi`, etc. (inherited from the `server` block if not specified).

Minimal compatible example:

```bash
server {
    listen 8080;
    root /var/www/example;
    index index.html index.htm;
    error_page 404 /errors/404.html;
    client_max_body_size 2m;
    cgi .php /usr/bin/php-cgi;

    location / {
        autoindex off;
    }

    location /upload {
        methods POST;
        upload_store /var/www/example/uploads;
        client_max_body_size 100m;
    }

    location /old {
        return 301 /new;
    }
}
```


###	Technical Features

* I/O Multiplexing: Uses a single process to handle multiple clients efficiently.

* Custom Configuration: Support for multiple server blocks, listen ports, and custom error_pages.

* CGI Execution: Handles environment variables and pipes to communicate with external scripts.

* Resilience: The server remains stable even under heavy load or invalid requests.

###	Resources
* Documentation & Articles

* https://www.alimnaqvi.com/blog/webserv.

* RFC 2616 (HTTP/1.1) - The official protocol specification.

* Nginx Configuration Docs - Reference for the configuration file logic.

###	AI Usage Disclosure

* Task: Help for architecture and debugging.

* Tool used: ChatGPT / Gemini / VsCode Copilot.