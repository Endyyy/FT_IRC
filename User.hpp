#ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <stdexcept>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <unistd.h>
# include <vector>
# include <map>
# include <algorithm>
# include <arpa/inet.h>
# include <sstream>

class User
{
    private:
        std::string const   _nickname;
        std::string         _username;
        int                 _user_socket;
        // std::map<std::string, bool> _channel_privileges;

        User();
        User(User const& source);
        User& operator=(User const& source);

    public:
        User(std::string const nickname);
        ~User();

        void    setUsername(std::string username);
        void    setUser_socket(int user_socket);

        std::string getNickname() const;
        std::string getUsername() const;
        int         getUser_socket() const;
};

#endif