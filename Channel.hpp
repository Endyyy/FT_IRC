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
# include <climits>

class Channel
{
	private:
		std::string	_name;
		std::string	_topic;
		std::string	_password;
		int			_limit;
		std::vector<User*>	_reg_users;
		std::vector<User*>	_reg_ope;
		Channel();
		Channel(Channel const& source);
		Channel& operator=(Channel const& source);

	public:
		Channel(const std::string& name, User *user);
		~Channel();

		std::string	get_password() const;
		void		set_password(std::string password);

		std::string	get_topic() const;
		void		set_topic(std::string topic);
		void		clear_topic();

		bool	hasUser(User* user) const;
		bool	getUserPrivilege(User *user) const;
		void	addUser(User *user);
		void	addOpe(User *user);
		void	removeUser(User *user);
		void	sendMessage(const std::string& message, type_sock sender_socket);

		void	setLimit(int limit);
		int		getLimit() const;

};

#endif
