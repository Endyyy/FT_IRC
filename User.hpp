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
        int const   _userSocket;
        std::string _nickname;
        std::string _username;

        User();
        User(User const& source);
        User& operator=(User const& source);

    public:
        User(int const socket);
        ~User();

        void    set_username(std::string username);
        void    set_nickname(std::string username);
        void    set_userSocket(int user_socket);

        std::string getNickname() const;
        std::string getUsername() const;
        int         getUser_socket() const;
};

#endif