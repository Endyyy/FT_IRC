#include "Irc.hpp"

Server::Server()
{

}

Server::~Server()
{

}



void Server::Kick()
{

}

void Server::Invite()
{
    
}

void Server::Topic()
{
    
}

void Server::Mode()
{
    
}

void Server::Join()
{
    
}

void Server::checkCommand(int sd, char *buffer)
{
    std::cout << "Received data from client, socket fd: " << sd << ", Data: " << buffer << std::endl;
}