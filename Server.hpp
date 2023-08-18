#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <stdexcept>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <unistd.h>
# include <vector>
# include <algorithm>
# include <arpa/inet.h>
# include <sstream>

class Server
{
    public:

        Server();
        ~Server();
        void    cmdPass(std::string arg, int sd);
        void    cmdNick(std::string arg, int sd);
        void    cmdUser(std::string arg, int sd);
        void    cmdKick(std::string arg, int sd);
        void    cmdInvite(std::string arg, int sd);
        void    cmdTopic(std::string arg, int sd);
        void    cmdMode(std::string arg, int sd);
        void    cmdJoin(std::string arg, int sd);
        void    cmdPrivMsg(std::string arg, int sd);
        void    cmdQuit();
        void    checkCommand(int sd, char *buffer);

        //ERROR_MSGS

        class ERR_NEEDMOREPARAMS : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_ALREADYREGISTRED : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NONICKNAMEGIVEN : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_ERRONEUSNICKNAME : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NICKNAMEINUSE : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NICKCOLLISION : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_BADCHANMASK : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NOTONCHANNEL : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NOSUCHCHANNEL : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NOSUCHNICK : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_CHANOPRIVSNEEDED : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_USERONCHANNEL : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_INVITING : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_AWAY : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_TOPIC : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_NOTOPIC : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_UNKNOWNMODE : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_USERSDONTMATCH : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_UMODEUNKNOWNFLAG : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_KEYSET : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_UMODEIS : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_BANLIST : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_CHANNELMODEIS : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_ENDOFBANLIST : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NORECIPIENT : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_CANNOTSENDTOCHAN : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_WILDTOPLEVEL : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NOTEXTTOSEND : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NOTOPLEVEL : public std::exception
        {
            virtual const char* what() const throw();
        };
        class RPL_SUMMONING : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_NOLOGIN : public std::exception
        {
            virtual const char* what() const throw();
        };
        class ERR_TOOMANYTARGET : public std::exception
        {
            virtual const char* what() const throw();
        };

};

#endif