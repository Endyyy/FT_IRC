#ifndef CHANNEL_HPP
# define CHANNEL_HPP

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
        std::string                 name;
        std::string                 topic;
        bool                        topic_state;
        bool                        invite_state;
        bool                        password_state;
        std::string                 password;
        std::vector<std::string>    operators;
        std::vector<std::string>    users;
    public:
        Channel(const std::string& name) : name(name);
        ~Channel();
        void addUser(const User& user);
        void removeUser(const User& user);
        int getUserPrivilege(const User& user) const;
};

#endif