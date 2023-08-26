#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "tools.hpp"
# include "User.hpp"


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

class Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string	_password;
		int			_limit;
		bool		_inviteState;

		std::vector<User*>	_reg_users;
		std::vector<User*>	_reg_ope;

		Channel();
		Channel(Channel const& source);
		Channel& operator=(Channel const& source);

	public:
		Channel(const std::string& name, User *user);
		~Channel();

		// Getters
		std::string	get_password() const;
		std::string	get_topic() const;
		int			getLimit() const;
		bool		get_inviteState() const;

		// Setters
		void	set_password(std::string password);
		void	set_topic(std::string topic);
		void	setLimit(int limit);
		void	set_inviteState();

		// Checkers
		bool	getUserPrivilege(User *user) const;
		bool	hasUser(User* user) const;

		// Methods
		void	addUser(User *user);
		void	addOpe(User *user);
		void	sendMessage(const std::string& message, type_sock sender_socket);

		// Cleaners
		void	clear_topic();
		void	removeUser(User *user);

};

#endif
