#include "Channel.hpp"

// attributes:
// std::string                     _name;
// std::string                     _topic;
// bool                            _topic_state;
// bool                            _invite_state;
// bool                            _password_state;
// std::string                     _password;
// std::map<std::string, User&>    _reg_moderators;
// std::map<std::string, User&>    _reg_users;

////////////////////////////////////////////////////////////////////////////////
//  Forbidden :
Channel::Channel() {}
Channel::Channel(Channel const& source) { (void)source; }
Channel& Channel::operator=(Channel const& source) { (void)source; return (*this); }
////////////////////////////////////////////////////////////////////////////////

Channel::Channel(const std::string& name) :
_name(name), _topic_state(false), _invite_state(false), _password_state(false)
{
    (void)_name;//delete
    (void)_topic_state;//delete
    (void)_invite_state;//delete
    (void)_password_state;//delete
    std::cout << "New channel \"" << _name << "\" is created" << std::endl;
}

Channel::~Channel()
{
    std::cout << "Channel \"" << _name << "\" is destroyed" << std::endl;
}

void Channel::addUser(const User& user, int moderator)
{
    (void)user;//delete
    (void)moderator;//delete
    // _reg_users[user.getUsername()] = moderator;
}

void Channel::removeUser(const User& user)
{
    (void)user;//delete
    // _reg_users.erase(user.getUsername());
}

int Channel::getUserPrivilege(const User& user) const
{
    (void)user;//delete
    // std::map<std::string, int>::const_iterator it = _reg_users.find(user.getUsername());
    // if (it != _reg_users.end())
    // {
    //     return it->second;
    // }
    return -1;
}