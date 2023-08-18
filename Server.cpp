#include "Server.hpp"

Server::Server()
{
    std::cout << "Server constructor called" << std::endl;
}

Server::~Server()
{
    std::cout << "Server destructor called" << std::endl;
}

void Server::cmdPass(std::string arg)
{
    (void)arg;
}

void Server::cmdNick(std::string arg)
{
    std::stringstream stream(arg);
    std::string cmd_name;
    std::string nick_name;

    if (stream)
    {
        stream >> cmd_name;
    }
    if (stream)
    {
        stream >> nick_name;
        if (nick_name[0] == 0)
        {
            std::cout << "NICK nickname" << std::endl;
        }
    }
}

void Server::cmdUser(std::string arg)
{
    std::stringstream stream(arg);
    std::string cmd_name;
    std::string user_name;

    if (stream)
    {
        stream >> cmd_name;
    }
    if (stream)
    {
        stream >> user_name;
        if (user_name[0] != ':')
        {
            std::cout << "USER :username" << std::endl;
        }
    }
}

void Server::cmdKick(std::string arg)
{
    (void)arg;
}

void Server::cmdInvite(std::string arg)
{
    (void)arg;
}

void Server::cmdTopic(std::string arg)
{
    (void)arg;
}

void Server::cmdMode(std::string arg)
{
    (void)arg;
}

void Server::cmdJoin(std::string arg)
{
    (void)arg;
}

void Server::cmdPrivMsg(std::string arg)
{
    (void)arg;
}

void Server::cmdQuit()
{

}

void Server::checkCommand(int sd, char *buffer)
{
    std::string arg = buffer;
    if (arg.compare(0, 4, "JOIN") == 0)
        cmdJoin(arg);
    if (arg.compare(0, 7, "PRIVMSG") == 0)
        cmdPrivMsg(arg);
    if (arg.compare(0, 6, "INVITE") == 0)
        cmdInvite(arg);
    if (arg.compare(0, 4, "KICK") == 0)
        cmdKick(arg);
    if (arg.compare(0, 4, "MODE") == 0)
        cmdMode(arg);
    if (arg.compare(0, 5, "TOPIC") == 0)
        cmdTopic(arg);
    if (arg.compare(0, 4, "NICK") == 0)
        cmdNick(arg);
    if (arg.compare(0, 4, "PASS") == 0)
        cmdPass(arg);
    if (arg.compare(0, 4, "USER") == 0)
        cmdUser(arg);
    if (arg.compare(0, 4, "Quit") == 0)
        cmdQuit();
    std::cout << "Received data from client, socket fd: " << sd << ", Data: " << buffer << std::endl;
}

//ERROR_MSGS

const char *Server::ERR_NEEDMOREPARAMS::what() const throw()
{
    return ("Need more params");
}
const char *Server::ERR_ALREADYREGISTRED::what() const throw()
{
    return("Already registered");
}
const char *Server::ERR_NONICKNAMEGIVEN::what() const throw()
{
    return("No nickname given");
}
const char *Server::ERR_ERRONEUSNICKNAME::what() const throw()
{
    return("Erroneous nickname");
}
const char *Server::ERR_NICKNAMEINUSE::what() const throw()
{
    return("Nickname is already in use");
}
const char *Server::ERR_NICKCOLLISION::what() const throw()
{
    return("Nickname collision KILL");
}
const char *Server::ERR_NOTONCHANNEL::what() const throw()
{
    return("You're not on that channel");
}
const char *Server::ERR_NOSUCHCHANNEL::what() const throw()
{
    return("No such channel");
}
const char *Server::ERR_NOSUCHNICK::what() const throw()
{
    return("No such nick/channel");
}
const char *Server::ERR_CHANOPRIVSNEEDED::what() const throw()
{
    return("You're not channel operator");
}
const char *Server::ERR_USERONCHANNEL::what() const throw()
{
    return("<user> <channel> :is already on channel");
}
const char *Server::RPL_INVITING::what() const throw()
{
    return("<channel> <nick>");
}
const char *Server::RPL_AWAY::what() const throw()
{
    return("<nick> :<away message>");
}
const char *Server::RPL_TOPIC::what() const throw()
{
    return("<channel> :<topic>");
}
const char *Server::RPL_NOTOPIC::what() const throw()
{
    return("<channel> :No topic is set");
}
const char *Server::ERR_UNKNOWNMODE::what() const throw()
{
    return("<char> :is unknown mode char to me");
}
const char *Server::ERR_USERSDONTMATCH::what() const throw()
{
    return(":Cant change mode for other users");
}
const char *Server::ERR_UMODEUNKNOWNFLAG::what() const throw()
{
    return(":Unknown MODE flag");
}
const char *Server::ERR_KEYSET::what() const throw()
{
    return("<channel> :Channel key already set");
}
const char *Server::RPL_UMODEIS::what() const throw()
{
    return("<user mode string>");
}
const char *Server::RPL_BANLIST::what() const throw()
{
    return("<channel> <banid>");
}
const char *Server::RPL_CHANNELMODEIS::what() const throw()
{
    return("<channel> <mode> <mode params>");
}
const char *Server::RPL_ENDOFBANLIST::what() const throw()
{
    return("<channel> :End of channel ban list");
}
const char *Server::ERR_NORECIPIENT::what() const throw()
{
    return("No recipient given (<command>)");
}
const char *Server::ERR_CANNOTSENDTOCHAN::what() const throw()
{
    return("<channel name> :Cannot send to channel");
}
const char *Server::ERR_WILDTOPLEVEL::what() const throw()
{
    return("<mask> :Wildcard in toplevel domain");
}
const char *Server::ERR_NOTEXTTOSEND::what() const throw()
{
    return(":No text to send");
}
const char *Server::ERR_NOTOPLEVEL::what() const throw()
{
    return("<mask> :No toplevel domain specified");
}
const char *Server::RPL_SUMMONING::what() const throw()
{
    return("<user> :Summoning user to IRC");
}
const char *Server::ERR_NOLOGIN::what() const throw()
{
    return("<user> :User not logged in");
}
const char *Server::ERR_TOOMANYTARGET::what() const throw()
{
    return("<target> :Duplicate recipients. No message delivered");
}