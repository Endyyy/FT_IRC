#ifndef IRC_HPP
#define IRC_HPP
#include <iostream>
#include <cstdlib>

class Server
{
    public:

        Server();
        ~Server();
        void    Kick();
        void    Invite();
        void    Topic();
        void    Mode();
        void    checkCommand(int sd, char *buffer);
        // class ERR_NEEDMOREPARAMS : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_ALREADYREGISTRED : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_NONICKNAMEGIVEN : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_ERRONEUSNICKNAME : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_NICKNAMEINUSE : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_NICKCOLLISION : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_BADCHANMASK : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_NOTONCHANNEL : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_NOSUCHCHANNEL : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_NOSUCHNICK : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_CHANOPRIVSNEEDED : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_USERONCHANNEL : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_INVITING : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_AWAY : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_TOPIC : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_NOTOPIC : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_UNKNOWNMODE : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_USERSDONTMATCH : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_UMODEUNKNOWNFLAG : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class ERR_KEYSET : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_UMODEIS : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_BANLIST : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_CHANNELMODEIS : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
        // class RPL_ENDOFBANLIST : public std::exception
        // {
        //     virtual const char* what() const throw();
        // };
};

#endif