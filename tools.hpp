#ifndef TOOLS_HPP
# define TOOLS_HPP

# define PORT_MIN 1024
# define PORT_MAX 65536
# define LEN_PWD_MIN 1
# define LEN_PWD_MAX 32
# define LEN_PORT_MAX 5

# include <iostream>
# include <stdexcept>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <unistd.h>
# include <vector>
# include <algorithm>
# include <arpa/inet.h>

// bool	isInt(char *av);
// int     checkArgs(char **av);

bool    check_port(const char* arg);
bool    check_password(std::string arg);
bool    have_whitespaces(std::string arg);

#endif