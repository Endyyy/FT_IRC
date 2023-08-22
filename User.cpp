#include "User.hpp"

// attributes:
// std::string _nickname;
// std::string _username;
// int const   _userSocket;

////////////////////////////////////////////////////////////////////////////////
//  Forbidden :
User::User() : _userSocket(0) {}
User::User(User const& source) : _userSocket(0) { (void)source; }
User& User::operator=(User const& source) { (void)source; return (*this); }
////////////////////////////////////////////////////////////////////////////////

User::User(int const socket) : _userSocket(socket)
{
	std::cout << "New User created. User socket is " << _userSocket << std::endl;
}

User::~User()
{
	std::cout << _nickname << " has been destroyed" << std::endl;
}

void	User::set_username(std::string username)
{
	_username = username;
}

void	User::set_nickname(std::string nickname)
{
	_nickname = nickname;
}

std::string User::get_nickname() const
{
	return (_nickname);
}

std::string User::get_username() const
{
	return (_username);
}

type_sock	User::get_userSocket() const
{
	return (_userSocket);
}
