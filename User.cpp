#include "User.hpp"

// attributes:
// std::string const    _nickname;
// std::string          _username;
// int                  _user_socket;

////////////////////////////////////////////////////////////////////////////////
//  Forbidden :
User::User() {}
User::User(User const& source) { (void)source; }
User& User::operator=(User const& source) { (void)source; return (*this); }
////////////////////////////////////////////////////////////////////////////////

User::User(std::string const nickname) : _nickname(nickname)
{
    _username = "";
    _user_socket = 0;
    std::cout << "New User created. NICK set as " << nickname << std::endl;
}

User::~User()
{  
    std::cout << _nickname << " has been destroyed" << std::endl;
}

void    User::setUsername(std::string username)
{
    _username = username;
}

void    User::setUser_socket(int user_socket)
{
    _user_socket = user_socket;
}

std::string User::getNickname() const
{
    return (_nickname);
}

std::string User::getUsername() const
{
    return (_username);
}

int         User::getUser_socket() const
{
    return (_user_socket);
}
