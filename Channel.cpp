#include "Channel.hpp"

///////////////////////////////////////////////////////////////////////////////
//  Forbidden :
Channel::Channel() {}
Channel::Channel(Channel const& source) { (void)source; }
Channel& Channel::operator=(Channel const& source) { (void)source; return (*this); }
///////////////////////////////////////////////////////////////////////////////

Channel::Channel(const std::string& name, User *user) :
_name(name), _limit(INT_MAX), _inviteState(false)
{
	addOpe(user);
	std::cout << "New channel \"" << _name << "\" is created" << std::endl;
}

Channel::~Channel()
{
	std::cout << "Channel \"" << _name << "\" is destroyed" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// Getters

std::string Channel::get_password() const
{
	return (_password);
}

std::string Channel::get_topic() const
{
	return (_topic);
}

int	Channel::getLimit() const
{
	return (_limit);
}

bool Channel::get_inviteState() const
{
	return (_inviteState);
}

///////////////////////////////////////////////////////////////////////////////
// Setters

void	Channel::set_password(std::string password)
{
	_password = password;
}

void	Channel::set_topic(std::string topic)
{
	_topic = topic;
}

void	Channel::setLimit(int limit)
{
	_limit = limit;
}

void Channel::set_inviteState()
{
	_inviteState = true;
}

///////////////////////////////////////////////////////////////////////////////
// Checkers

bool Channel::getUserPrivilege(User *user) const
{
	for (size_t i = 0; i < _reg_ope.size(); ++i)
	{
		if (_reg_ope[i] == user)
			return true;
	}
	return false;
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

///////////////////////////////////////////////////////////////////////////////
// Methods

void Channel::addUser(User *user)
{
	_reg_users.push_back(user);
}

void Channel::addOpe(User *user)
{
	_reg_ope.push_back(user);
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

///////////////////////////////////////////////////////////////////////////////
// Cleaners

void	Channel::clear_topic()
{
	_topic.clear();
}

void Channel::removeUser(User *user)
{
	for (size_t i = 0; i < _reg_users.size(); ++i)
	{
		if (_reg_users[i] == user)
		   _reg_users.erase(_reg_users.begin() + i);
	}
	for (size_t i = 0; i < _reg_ope.size(); ++i)
	{
		if (_reg_ope[i] == user)
		   _reg_ope.erase(_reg_ope.begin() + i);
	}
}
