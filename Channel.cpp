#include "Channel.hpp"

// attributes:
// std::string					_name;
// std::string					_topic;
// bool							_topic_state;
// bool							_invite_state;
// bool							_password_state;
// std::string					 _password;
// std::map<std::string, User&>	_reg_moderators;
// std::map<std::string, User&>	_reg_users;

////////////////////////////////////////////////////////////////////////////////
//  Forbidden :
Channel::Channel() {}
Channel::Channel(Channel const& source) { (void)source; }
Channel& Channel::operator=(Channel const& source) { (void)source; return (*this); }
////////////////////////////////////////////////////////////////////////////////

Channel::Channel(const std::string& name, User *user) :
_name(name), _invite_state(false)
{
	addOpe(user);
	std::cout << "New channel \"" << _name << "\" is created" << std::endl;
}

Channel::~Channel()
{
	std::cout << "Channel \"" << _name << "\" is destroyed" << std::endl;
}

void Channel::addUser(User *user)
{
	_reg_users.push_back(user);
}

void Channel::addOpe(User *user)
{
	_reg_ope.push_back(user);
}

bool Channel::hasUser(User* user) const
{
    for (size_t i = 0; i < _reg_users.size(); ++i)
    {
        if (_reg_users[i] == user)
            return true;
    }
    for (size_t i = 0; i < _reg_ope.size(); ++i)
    {
        if (_reg_ope[i] == user)
            return true;
    }
    return false;
}

void Channel::sendMessage(const std::string& message, type_sock sender_socket)
{
	for (size_t i = 0; i < _reg_users.size(); ++i)
	{
		User* user = _reg_users[i];
		if (user->get_userSocket() != sender_socket)
			send(user->get_userSocket(), message.c_str(), message.size(), 0);
	}
    for (size_t i = 0; i < _reg_ope.size(); ++i)
	{
		User* user = _reg_ope[i];
		if (user->get_userSocket() != sender_socket)
			send(user->get_userSocket(), message.c_str(), message.size(), 0);
	}
}

std::string Channel::get_password() const
{
	return (_password);
}

// void Channel::removeUser(const User& user)
// {
//	 (void)user;//delete
//	 // _reg_users.erase(user.getUsername());
// }

// int Channel::getUserPrivilege(const User& user) const
// {
//	 (void)user;//delete
//	 std::map<std::string, User&>::const_iterator it = _reg_users.find(user.get_username());
//	 if (it != _reg_users.end())
//	 {
//		 return it->second.get_userSocket();
//	 }
//	 return -1;
// }