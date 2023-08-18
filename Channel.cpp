#include "Channel.hpp"

    Channel::Channel(const std::string& name) : name(name)
    {

    }

    Channel::~Channel()
    {

    }

    void Channel::addUser(const User& user, int operator)
    {
        users[user.getUsername()] = operator;
    }

    void Channel::removeUser(const User& user)
    {
        users.erase(user.getUsername());
    }

    int Channel::getUserPrivilege(const User& user) const
    {
        std::map<std::string, int>::const_iterator it = users.find(user.getUsername());
        if (it != users.end())
        {
            return it->second;
        }
        return -1;
    }