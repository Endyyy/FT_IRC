#ifndef USER_HPP
# define USER_HPP

# include "tools.hpp"



# include <algorithm>
# include <arpa/inet.h>
# include <cctype>
# include <climits>
# include <csignal>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <iostream>
# include <map>
# include <sstream>
# include <stdexcept>
# include <string>
# include <vector>

class User
{
	private:
		int const	_userSocket;
		std::string	_nickname;
		std::string	_username;
		int			_userState;
		std::string	_inputStack;

		User();
		User(User const& source);
		User& operator=(User const& source);

	public:
		User(type_sock const socket);
		~User();

		// Getters
		std::string get_nickname() const;
		std::string get_username() const;
		type_sock	get_userSocket() const;
		int			get_userState() const;
		std::string	get_inputStack() const;

		// Setters
		void	set_username(std::string username);
		void	set_nickname(std::string username);
		void	set_userState(int state);
		void	set_inputStack(std::string str);

		// Methods
		void	add_to_inputStack(std::string str);

		// Cleaners
		void	reset_inputStack();

};

#endif
