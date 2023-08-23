#ifndef USER_HPP
# define USER_HPP

# include "tools.hpp"
# include <iostream>
# include <stdexcept>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <unistd.h>
# include <vector>
# include <map>
# include <algorithm>
# include <arpa/inet.h>
# include <sstream>

class User
{
	private:
		int const	_userSocket;
		std::string	_nickname;
		std::string	_username;
		int			_userState;

		User();
		User(User const& source);
		User& operator=(User const& source);

	public:
		User(type_sock const socket);
		~User();

		void	set_username(std::string username);
		void	set_nickname(std::string username);
		void	set_userState(int state);

		std::string get_nickname() const;
		std::string get_username() const;
		type_sock	get_userSocket() const;
		int			get_userState() const;
};

#endif
