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
    std::cout << "show value : port " << _port << "; _serverSocket " << _serverSocket << std::endl;
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

void    Server::reset_fd_set()
{
    // Erase all preexisting socket values
    FD_ZERO(&_readfds);

    // Add the server socket to the set
    FD_SET(_serverSocket, &_readfds);
    _topSocket = _serverSocket;

    // Add clients sockets to the set
    for (std::map<int, User*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->first > 0)
            FD_SET(it->first, &_readfds);
        if (it->first > _topSocket)
            _topSocket = it->first;
    }
}

void    Server::waiting_for_activity()
{
    int activity = select(_topSocket + 1, &_readfds, NULL, NULL, NULL);
    if (activity < 0)
        throw (ERR_SELECTFAILURE());
}

bool    Server::check_server_activity()
{
    return (FD_ISSET(_serverSocket, &_readfds));
}

int    Server::add_user_to_server()
{
    int tmp_socket = accept(_serverSocket, (struct sockaddr *)&_address, NULL);//NULL or socklen_t &addrLength = sizeof(_address);
    if (tmp_socket < 0)
        throw (ERR_ACCEPTFAILURE());
    _clients.insert(std::make_pair(tmp_socket, new User(tmp_socket)));
    std::cout << "New connection, socket fd: " << tmp_socket << ", IP: " << inet_ntoa(_address.sin_addr)
    << ", Port: " << ntohs(_address.sin_port) << std::endl;
    return (tmp_socket);
}

std::string    Server::read_from_user(int userSocket, char const* sentence)
{
    std::string str;
    char buffer[BUFFER_SIZE] = {0};

    // send(userSocket, "PASS <password>\n", strlen("PASS <password>\n"), 0);//previous
    send(userSocket, sentence, strlen(sentence), 0);
    int len = read(userSocket, buffer, BUFFER_SIZE);
    if (len > 0)//////////////check si taille adaptée à tous les cas de figures
    {
        str = buffer;
        str.erase(str.size() - 1);///////voir le meilleure moyen de trim le str
        // str.erase(std::remove(str.begin(), str.end(), '\n'), userPassword.end()); // Remove newline character
        return (str);
    }
    return ("");
}


void    Server::run()
{
    // std::vector<int> _clients(MAX_CLIENTS, 0);

    while (true)
    {
        reset_fd_set();
        waiting_for_activity();

        // New incoming connection
        if (check_server_activity())
        {
            int userSocket = add_user_to_server();
            std::string sample = read_from_user(userSocket, "PASS <password>\n");
            // Check if password exist
            if (sample.empty())
            {
                // Error reading password
                std::cerr << "Error reading password from client." << std::endl;
                close(userSocket);
                continue;////////////////////faut-il supprimer le user de la map dans ce cas ?
            }
            // Check if the entered password matches the server's password
            if (sample != _serverPassword)
            {
                // Password doesn't match, reject the connection
                std::cout << "Client authentication failed. Connection rejected." << std::endl;
                close(userSocket);
                continue ;//////////////pas continue?
                // return ;
            }

            // Password match, accept the connection
            std::cout << "Client authenticated. Connection accepted." << std::endl;

            // Add new socket to array of client sockets
            // for (int i = 0; i < MAX_CLIENTS; ++i)
            // {
            //     if (_clients[i] == 0)
            //     {
            //         _clients[i] = userSocket;
            //         break;
            //     }
            // }
           
            sample = read_from_user(userSocket, "NICK <nickname>\n");
            if (sample.empty())
            {
                // Error reading nickname
                std::cerr << "Error reading nickname from client." << std::endl;
                close(userSocket);
                continue;
            }
            //////////manque check si nickname dispo, pas de doublons possibles?
            //////////manque maj du user pour ajouter nickname?

            sample = read_from_user(userSocket, "USER <username>\n");
            if (sample.empty())
            {
                // Error reading username
                std::cerr << "Error reading username from client." << std::endl;
                close(userSocket);
                continue;
            }
            //////////manque maj du user pour ajouter username?

            
            // Add new socket to array of client sockets
            // for (int i = 0; i < MAX_CLIENTS; ++i)
            // {
            //     if (_clients[i] == 0)
            //     {
            //         _clients[i] = userSocket;
            //         break;
            //     }
            // }
            ///////////////////////deja fait?
        }

        ///////////////////pas encore fait la suite

        // Handle data from clients
        for (std::map<int, User*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            int client_socket = it->first;
            if (FD_ISSET(client_socket, &_readfds))
            {
                char buffer[BUFFER_SIZE] = {0};
                int valread = read(client_socket, buffer, BUFFER_SIZE);
                if (valread == 0)
                {
                    // Client disconnected
                    std::cout << "Client disconnected, socket fd: " << client_socket << std::endl;
                    close(client_socket);
                    // it->first = 0;////////////impossible. modifier par suppr un user
                }
                else if (valread == -1)
                {
                    // Error or abrupt disconnection
                    std::cerr << "Error reading data from client, socket fd: " << client_socket << std::endl;
                    close(client_socket);
                    // it->first = 0;////////////impossible. modifier par suppr un user
                }
                else {
                    // Handle data from the client
                    checkCommand(client_socket, buffer);
                }
            }
        }
    }
}

void Server::cmdPass(std::string arg, int client_socket)
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
            send(client_socket, "PASS <password>\n", strlen("PASS <password>\n"), 0);
            return ;
        }
    }
    if (stream)
    {
        stream >> end;
        if (end[0])
        {
            send(client_socket, "PASS <password>\n", strlen("PASS <password>\n"), 0);
        }
    }
}

void Server::cmdNick(std::string arg, int client_socket)
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
            send(client_socket, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
            return ;
        }
    }
    if (stream)
    {
        stream >> end;
        if (end[0])
        {
            send(client_socket, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
        }
    }
}

void Server::cmdUser(std::string arg, int client_socket)
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
            send(client_socket, "USER :<username>\n", strlen("USER :<username>\n"), 0);
            return ;
        }
    }
    if (stream)
    {
        stream >> end;
        if (end[0])
        {
            send(client_socket, "USER :<username>\n", strlen("USER :<username>\n"), 0);
        }
    }
}

void Server::cmdKick(std::string arg, int client_socket)
{
    (void)arg;
    (void)client_socket;
}

void Server::cmdInvite(std::string arg, int client_socket)
{
    (void)arg;
    (void)client_socket;
}

void Server::cmdTopic(std::string arg, int client_socket)
{
    (void)arg;
    (void)client_socket;
}

void Server::cmdMode(std::string arg, int client_socket)
{
    (void)arg;
    (void)client_socket;
}

void Server::cmdJoin(std::string arg, int client_socket)
{
    (void)arg;
    (void)client_socket;
}

void Server::cmdPrivMsg(std::string arg, int client_socket)
{
    (void)arg;
    (void)client_socket;
}

void Server::cmdQuit()
{

}

void Server::checkCommand(int client_socket, char *buffer)
{
    std::string arg = buffer;
    if (arg.compare(0, 4, "JOIN") == 0)
        cmdJoin(arg, client_socket);
    if (arg.compare(0, 7, "PRIVMSG") == 0)
        cmdPrivMsg(arg, client_socket);
    if (arg.compare(0, 6, "INVITE") == 0)
        cmdInvite(arg, client_socket);
    if (arg.compare(0, 4, "KICK") == 0)
        cmdKick(arg, client_socket);
    if (arg.compare(0, 4, "MODE") == 0)
        cmdMode(arg, client_socket);
    if (arg.compare(0, 5, "TOPIC") == 0)
        cmdTopic(arg, client_socket);
    if (arg.compare(0, 4, "NICK") == 0)
        cmdNick(arg, client_socket);
    if (arg.compare(0, 4, "PASS") == 0)
        cmdPass(arg, client_socket);
    if (arg.compare(0, 4, "USER") == 0)
        cmdUser(arg, client_socket);
    if (arg.compare(0, 4, "Quit") == 0)
        cmdQuit();
    std::cout << "Received data from client, socket fd: " << client_socket << ", Data: " << buffer << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//  ERROR_MSGS
const char *Server::ERR_INVALIDSOCKET::what() const throw()     { return "Error creating socket"; }

const char *Server::ERR_BINDFAILURE::what() const throw()       { return "Failed to bind socket to address"; }

const char *Server::ERR_LISTENINGFAILURE::what() const throw()  { return "Socket failed to start listening"; }

const char *Server::ERR_SELECTFAILURE::what() const throw()       { return "Select error"; }

const char *Server::ERR_ACCEPTFAILURE::what() const throw()       { return "Accept error"; }



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
