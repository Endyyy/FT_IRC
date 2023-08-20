#include "Server.hpp"

////////////////////////////////////////////////////////////////////////////////
//  Forbidden :
Server::Server() : _port(), _serverPassword(), _serverSocket() {}
Server::Server(Server const& source) : _port(), _serverPassword(), _serverSocket() { (void)source; }
Server& Server::operator=(Server const& source) { (void)source; return (*this); }
////////////////////////////////////////////////////////////////////////////////

Server::Server(int port, std::string serverPassword) :
_port(port), _serverPassword(serverPassword), _serverSocket(socket(AF_INET, SOCK_STREAM, 0))
{
    if (_serverSocket == -1)
        throw (ERR_INVALIDSOCKET());
    set_address();
    std::cout << "Server created" << std::endl;
}

Server::~Server()
{
    std::cout << "Server destroyed" << std::endl;
}

void    Server::set_address()
{
    //Set adress for IPV4
    _address.sin_family = AF_INET;
    // Configure socket to bind to any available IP address on the system
    _address.sin_addr.s_addr = INADDR_ANY;
    // Use our desired port number
    _address.sin_port = htons(_port);
}

int     Server::get_serverSocket() const
{
    return (_serverSocket);
}

void    Server::bind_socket_to_address()
{
    if (bind(_serverSocket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
        throw (ERR_BINDFAILURE());
}

void    Server::start_listening()
{
    if (listen(_serverSocket, 3) < 0)
        throw (ERR_LISTENINGFAILURE());
    std::cout << "Server started. Waiting for connections..." << std::endl;
}

void    Server::run()
{
    std::vector<int> clientSockets(MAX_CLIENTS, 0);

    while (true)
    {
        FD_ZERO(&readfds);

        // Add the server socket to the set
        FD_SET(get_serverSocket(), &readfds);
        maxSocket = get_serverSocket();

        // Add child sockets to the set
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            int sd = clientSockets[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > maxSocket)
                maxSocket = sd;
        }

        // Wait for activity on any of the sockets
        activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            std::cerr << "Select error." << std::endl;
            return ;
        }

        // New incoming connection
        if (FD_ISSET(get_serverSocket(), &readfds))
        {
            socklen_t addrLength = sizeof(_address);
            if ((newSocket = accept(get_serverSocket(), (struct sockaddr *)&_address, &addrLength)) < 0)
            {
                std::cerr << "Accept error." << std::endl;
                return ;
            }

            std::cout << "New connection, socket fd: " << newSocket << ", IP: " << inet_ntoa(_address.sin_addr) << ", Port: " << ntohs(_address.sin_port) << std::endl;

            // Ask the client for the password
            std::string clientPassword;
            std::string clientUsername;
            std::string clientNickname;
            char passwordBuffer[BUFFER_SIZE] = {0};
            char usernameBuffer[BUFFER_SIZE] = {0};
            char nicknameBuffer[BUFFER_SIZE] = {0};
            send(newSocket, "PASS <password>\n", strlen("PASS <password>\n"), 0);
            int valread = read(newSocket, passwordBuffer, BUFFER_SIZE);
            if (valread > 0)
            {
                clientPassword = passwordBuffer;
                clientPassword.erase(std::remove(clientPassword.begin(), clientPassword.end(), '\n'), clientPassword.end()); // Remove newline character
            } else
            {
                // Error reading password
                std::cerr << "Error reading password from client." << std::endl;
                close(newSocket);
                continue;
            }
            // Check if the entered password matches the server's password
            if (clientPassword == _serverPassword)
            {
                // Password match, accept the connection
                std::cout << "Client authenticated. Connection accepted." << std::endl;
                // Add new socket to array of client sockets
                for (int i = 0; i < MAX_CLIENTS; ++i)
                {
                    if (clientSockets[i] == 0)
                    {
                        clientSockets[i] = newSocket;
                        break;
                    }
                }
                send(newSocket, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
                valread = read(newSocket, nicknameBuffer, BUFFER_SIZE);
                if (valread > 0)
                {
                    clientNickname = nicknameBuffer;
                    clientNickname.erase(std::remove(clientNickname.begin(), clientNickname.end(), '\n'), clientNickname.end()); // Remove newline character
                }
                else
                {
                    // Error reading nickname
                    std::cerr << "Error reading nickname from client." << std::endl;
                    close(newSocket);
                    continue;
                }
                send(newSocket, "USER <username>\n", strlen("USER <username>\n"), 0);
                valread = read(newSocket, usernameBuffer, BUFFER_SIZE);
                if (valread > 0)
                {
                    clientUsername = usernameBuffer;
                    clientUsername.erase(std::remove(clientUsername.begin(), clientUsername.end(), '\n'), clientUsername.end()); // Remove newline character
                }
                else
                {
                    // Error reading username
                    std::cerr << "Error reading username from client." << std::endl;
                    close(newSocket);
                    continue;
                }
            }
            else
            {
                // Password doesn't match, reject the connection
                std::cout << "Client authentication failed. Connection rejected." << std::endl;
                close(newSocket);
                return ;
            }

            // Add new socket to array of client sockets
            for (int i = 0; i < MAX_CLIENTS; ++i)
            {
                if (clientSockets[i] == 0)
                {
                    clientSockets[i] = newSocket;
                    break;
                }
            }
        }

        // Handle data from clients
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            int sd = clientSockets[i];
            if (FD_ISSET(sd, &readfds))
            {
                char buffer[BUFFER_SIZE] = {0};
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0)
                {
                    // Client disconnected
                    std::cout << "Client disconnected, socket fd: " << sd << std::endl;
                    close(sd);
                    clientSockets[i] = 0;
                }
                else if (valread == -1)
                {
                    // Error or abrupt disconnection
                    std::cerr << "Error reading data from client, socket fd: " << sd << std::endl;
                    close(sd);
                    clientSockets[i] = 0;
                }
                else {
                    // Handle data from the client
                    checkCommand(sd, buffer);
                }
            }
        }
    }
}

void Server::cmdPass(std::string arg, int sd)
{
    std::stringstream stream(arg);
    std::string cmd_name;
    std::string passwd;
    std::string end;

    if (stream)
    {
        stream >> cmd_name;
    }
    if (stream)
    {
        stream >> passwd;
        if (passwd[0] == 0)
        {
            send(sd, "PASS <password>\n", strlen("PASS <password>\n"), 0);
            return ;
        }
    }
    if (stream)
    {
        stream >> end;
        if (end[0])
        {
            send(sd, "PASS <password>\n", strlen("PASS <password>\n"), 0);
        }
    }
}

void Server::cmdNick(std::string arg, int sd)
{
    std::stringstream stream(arg);
    std::string cmd_name;
    std::string nick_name;
    std::string end;

    if (stream)
    {
        stream >> cmd_name;
    }
    if (stream)
    {
        stream >> nick_name;
        if (nick_name[0] == 0)
        {
            send(sd, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
            return ;
        }
    }
    if (stream)
    {
        stream >> end;
        if (end[0])
        {
            send(sd, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
        }
    }
}

void Server::cmdUser(std::string arg, int sd)
{
    std::stringstream stream(arg);
    std::string cmd_name;
    std::string user_name;
    std::string end;

    if (stream)
    {
        stream >> cmd_name;
    }
    if (stream)
    {
        stream >> user_name;
        if (user_name[0] != ':')
        {
            send(sd, "USER :<username>\n", strlen("USER :<username>\n"), 0);
            return ;
        }
    }
    if (stream)
    {
        stream >> end;
        if (end[0])
        {
            send(sd, "USER :<username>\n", strlen("USER :<username>\n"), 0);
        }
    }
}

void Server::cmdKick(std::string arg, int sd)
{
    (void)arg;
    (void)sd;
}

void Server::cmdInvite(std::string arg, int sd)
{
    (void)arg;
    (void)sd;
}

void Server::cmdTopic(std::string arg, int sd)
{
    (void)arg;
    (void)sd;
}

void Server::cmdMode(std::string arg, int sd)
{
    (void)arg;
    (void)sd;
}

void Server::cmdJoin(std::string arg, int sd)
{
    (void)arg;
    (void)sd;
}

void Server::cmdPrivMsg(std::string arg, int sd)
{
    (void)arg;
    (void)sd;
}

void Server::cmdQuit()
{

}

void Server::checkCommand(int sd, char *buffer)
{
    std::string arg = buffer;
    if (arg.compare(0, 4, "JOIN") == 0)
        cmdJoin(arg, sd);
    if (arg.compare(0, 7, "PRIVMSG") == 0)
        cmdPrivMsg(arg, sd);
    if (arg.compare(0, 6, "INVITE") == 0)
        cmdInvite(arg, sd);
    if (arg.compare(0, 4, "KICK") == 0)
        cmdKick(arg, sd);
    if (arg.compare(0, 4, "MODE") == 0)
        cmdMode(arg, sd);
    if (arg.compare(0, 5, "TOPIC") == 0)
        cmdTopic(arg, sd);
    if (arg.compare(0, 4, "NICK") == 0)
        cmdNick(arg, sd);
    if (arg.compare(0, 4, "PASS") == 0)
        cmdPass(arg, sd);
    if (arg.compare(0, 4, "USER") == 0)
        cmdUser(arg, sd);
    if (arg.compare(0, 4, "Quit") == 0)
        cmdQuit();
    std::cout << "Received data from client, socket fd: " << sd << ", Data: " << buffer << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  ERROR_MSGS
const char *Server::ERR_INVALIDSOCKET::what() const throw()     { return "Error creating socket"; }

const char *Server::ERR_BINDFAILURE::what() const throw()       { return "Failed to bind socket to address"; }

const char *Server::ERR_LISTENINGFAILURE::what() const throw()  { return "Socket failed to start listening"; }



const char *Server::ERR_ALREADYREGISTRED::what() const throw()  { return ("Already registered"); }

const char *Server::ERR_CANNOTSENDTOCHAN::what() const throw()  { return ("<channel name> :Cannot send to channel"); }

const char *Server::ERR_CHANOPRIVSNEEDED::what() const throw()  { return ("You're not channel operator"); }

const char *Server::ERR_ERRONEUSNICKNAME::what() const throw()  { return ("Erroneous nickname"); }

const char *Server::ERR_KEYSET::what() const throw()            { return ("<channel> :Channel key already set"); }

const char *Server::ERR_NEEDMOREPARAMS::what() const throw()    { return ("Need more params"); }

const char *Server::ERR_NICKNAMEINUSE::what() const throw()     { return ("Nickname is already in use"); }

const char *Server::ERR_NOLOGIN::what() const throw()           { return ("<user> :User not logged in"); }

const char *Server::ERR_NONICKNAMEGIVEN::what() const throw()   { return ("No nickname given"); }

const char *Server::ERR_NORECIPIENT::what() const throw()       { return ("No recipient given (<command>)"); }

const char *Server::ERR_NOSUCHCHANNEL::what() const throw()     { return ("No such channel"); }

const char *Server::ERR_NOSUCHNICK::what() const throw()        { return ("No such nick/channel"); }

const char *Server::ERR_NOTEXTTOSEND::what() const throw()      { return (":No text to send"); }

const char *Server::ERR_NOTONCHANNEL::what() const throw()      { return ("You're not on that channel"); }

const char *Server::ERR_NOTOPLEVEL::what() const throw()        { return ("<mask> :No toplevel domain specified"); }

const char *Server::ERR_TOOMANYTARGET::what() const throw()     { return ("<target> :Duplicate recipients. No message delivered"); }

const char *Server::ERR_USERONCHANNEL::what() const throw()     { return ("<user> <channel> :is already on channel"); }

const char *Server::ERR_UNKNOWNMODE::what() const throw()       { return ("<char> :is unknown mode char to me"); }

const char *Server::ERR_USERSDONTMATCH::what() const throw()    { return (":Cant change mode for other users"); }

const char *Server::ERR_UMODEUNKNOWNFLAG::what() const throw()  { return (":Unknown MODE flag"); }

const char *Server::ERR_WILDTOPLEVEL::what() const throw()      { return ("<mask> :Wildcard in toplevel domain"); }

//-----------------

const char *Server::RPL_AWAY::what() const throw()              { return ("<nick> :<away message>"); }

const char *Server::RPL_BANLIST::what() const throw()           { return ("<channel> <banid>"); }

const char *Server::RPL_CHANNELMODEIS::what() const throw()     { return ("<channel> <mode> <mode params>"); }

const char *Server::RPL_ENDOFBANLIST::what() const throw()      { return ("<channel> :End of channel ban list"); }

const char *Server::RPL_INVITING::what() const throw()          { return ("<channel> <nick>"); }

const char *Server::RPL_NOTOPIC::what() const throw()           { return ("<channel> :No topic is set"); }

const char *Server::RPL_SUMMONING::what() const throw()         { return ("<user> :Summoning user to IRC"); }

const char *Server::RPL_TOPIC::what() const throw()             { return ("<channel> :<topic>"); }

const char *Server::RPL_UMODEIS::what() const throw()           { return ("<user mode string>"); }

////////////////////////////////////////////////////////////////////////////////
