#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "tools.hpp"
# include "User.hpp"
# include <iostream>
# include <stdexcept>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <unistd.h>
# include <vector>
# include <algorithm>
# include <arpa/inet.h>
# include <cctype>

class Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string	_password;
		bool		_topic_state;
		bool		_invite_state;
		bool		_password_state;

		std::map<std::string, User&>	_reg_moderators;
		std::map<std::string, User&>	_reg_users;

		Channel();
		Channel(Channel const& source);
		Channel& operator=(Channel const& source);

	public:
		Channel(const std::string& name);
		~Channel();

		void	addUser(const User& user, int moderator);
		void	removeUser(const User& user);
		int		getUserPrivilege(const User& user) const;
};

#endif