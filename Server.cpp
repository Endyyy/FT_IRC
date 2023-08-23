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
	// Channel* default_Channel = new Channel("#general");// d'apres mes infos, un channel ne peut etre créé qu'à la demande d'un user. d'ailleurs, qui en serait le modo?
	// _channels["#general"] = default_Channel;
	set_address();
	// std::cout << "show value : port " << _port << "; _serverSocket " << _serverSocket << std::endl;//test
	std::cout << "Server created" << std::endl;
}

Server::~Server()
{
	std::cout << "Server destroyed" << std::endl;
}

void	Server::set_address()
{
	//Set adress for IPV4
	_address.sin_family = AF_INET;
	// Configure socket to bind to any available IP address on the system
	_address.sin_addr.s_addr = INADDR_ANY;
	// Use our desired port number
	_address.sin_port = htons(_port);
}

void	Server::bind_socket_to_address()
{
	if (bind(_serverSocket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw (ERR_BINDFAILURE());
}

void	Server::start_listening()
{
	if (listen(_serverSocket, 3) < 0)
		throw (ERR_LISTENINGFAILURE());
	std::cout << "Server started. Waiting for connections..." << std::endl;
}

void	Server::reset_fd_set()
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

void	Server::waiting_for_activity()
{
	//gestion des signaux
	int activity = select(_topSocket + 1, &_readfds, NULL, NULL, NULL);
	if (activity < 0)
		throw (ERR_SELECTFAILURE());
}

bool	Server::check_activity(type_sock socket)
{
	return (FD_ISSET(socket, &_readfds));
}

type_sock	Server::get_incoming_socket()
{
	socklen_t addrLength = sizeof(_address);
	type_sock tmp_socket = accept(_serverSocket, (struct sockaddr *)&_address, &addrLength);
	if (tmp_socket < 0)
		throw (ERR_ACCEPTFAILURE());//////////comportement a definir
	return (tmp_socket);
}

void	Server::add_new_user(type_sock userSocket)
{
	_clients.insert(std::make_pair(userSocket, new User(userSocket)));
	std::cout <<
	"New connection, socket fd: " << userSocket <<
	", IP: " << inet_ntoa(_address.sin_addr) <<
	", Port: " << ntohs(_address.sin_port) <<
	std::endl;
}

std::string	Server::recv_from_user(type_sock userSocket)
{
	std::string	str;
	char		buffer[BUFFER_SIZE] = {0};
	ssize_t bytes = recv(userSocket, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	// bytes vaut 0 si user ctrl C, bytes vaut 1 si chaine vide

	std::cout << "input recieved. bytes = " << bytes << std::endl;

	if (bytes > 0)
	{
		std::cout << "bytes > 0" << std::endl;
		str = buffer;
		str = str.erase(str.size() - 1);
		std::cout << "input cleaned" << std::endl;
		return (str);
	}
	if (bytes == 0)
	{
	// Déconnexion (0) : Si recv() renvoie 0, cela signifie généralement que la connexion a été fermée par le côté distant (le client a fermé la connexion).

	}
	std::cout << "empty string is returned" << std::endl;
	if (bytes < 0)/////////// ou egal a 0 ? quid si chaine vide
	{
		std::cout << "Problem with read !" << std::endl;
	}
	return ("");
// 	Erreur de connexion (-1) : Si recv() renvoie -1, cela peut indiquer qu'une erreur générale de réception s'est produite.
// Erreur de non-disponibilité de données (EAGAIN ou EWOULDBLOCK) : En mode non bloquant, recv() peut renvoyer -1 avec l'erreur EAGAIN ou EWOULDBLOCK si aucune donnée n'est immédiatement disponible à lire.
// Erreur de socket (EBADF) : Si le descripteur de socket fourni n'est pas valide, recv() peut renvoyer -1 avec l'erreur EBADF.
// Erreur de mémoire (ENOMEM) : Si le système n'a pas suffisamment de mémoire pour effectuer l'opération de réception, recv() peut renvoyer -1 avec l'erreur ENOMEM.
// Erreur d'interruption système (EINTR) : Si un signal est reçu pendant l'exécution de recv(), elle peut être interrompue et renvoyer -1 avec l'erreur EINTR.
// Erreur de socket non valide (ENOTSOCK) : Si le descripteur fourni n'est pas un socket valide, recv() peut renvoyer -1 avec l'erreur ENOTSOCK.
// Erreur de socket non supporté (EOPNOTSUPP) : Si l'opération demandée n'est pas prise en charge par le socket, recv() peut renvoyer -1 avec l'erreur EOPNOTSUPP.
}

void	Server::run()
{
	type_sock socket = 0;
	while (true)
	{
		std::cout << "while" << std::endl;

		reset_fd_set();
		std::cout << "fd_set reseted" << std::endl;

		//gestion des signaux a implementer

		waiting_for_activity();
		std::cout << "waiting is over" << std::endl;

		// New incoming connection
		if (check_activity(_serverSocket))
		{
			std::cout << "activity checked" << std::endl;

			socket = get_incoming_socket();
			std::cout << "socket identified" << std::endl;

			if (_clients.find(socket) == _clients.end())
			{
				std::cout << "user not find in map" << std::endl;

				add_new_user(socket);
				std::cout << "new user added" << std::endl;

				send(socket, "Enter password. [PASS <password>]\n", strlen("Enter password. [PASS <password>]\n"), 0);
			}
		}

		// Handle input from clients
		std::vector<type_sock> disconnectedClients;

		for (std::map<type_sock, User*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			socket = it->first;
			if (check_activity(it->second->get_userSocket()))
			{
				try
				{
					std::string	input = recv_from_user(socket);
					checkCommand(input, socket);
				}
				catch(std::exception const& e)
				{
					std::cout << e.what() << std::endl;
				}
			}
		}
		// // Fonction qui permet de clean les clients deco, ne pas changer la syntaxe ou segfault :D
		// for (std::map<type_sock, User*>::iterator it = _clients.begin(); it != _clients.end();)
		// {
		// 	type_sock client_socket = it->first;
		// 	if (std::find(disconnectedClients.begin(), disconnectedClients.end(), client_socket) != disconnectedClients.end())
		// 	{
		// 		close(client_socket);
		// 		delete it->second;
		// 		_clients.erase(it);
		// 		it = _clients.begin();
		// 	}
		// 	else
		// 		it++;
		// }
	}
}

type_sock Server::checkCommand(std::string arg, type_sock client_socket)
{
	int userState = _clients[client_socket]->get_userState();
	std::cout << "Check Command. user _state = " << userState << std::endl;
	if (arg.compare(0, 4, "JOIN") == 0)
		cmdJoin(arg, client_socket);
	else if (arg.compare(0, 7, "PRIVMSG") == 0)
		cmdPrivMsg(arg, client_socket);
	else if (arg.compare(0, 6, "INVITE") == 0)
		cmdInvite(arg, client_socket);
	else if (arg.compare(0, 4, "KICK") == 0)
		cmdKick(arg, client_socket);
	else if (arg.compare(0, 4, "MODE") == 0)
		cmdMode(arg, client_socket);
	else if (arg.compare(0, 5, "TOPIC") == 0)
		cmdTopic(arg, client_socket);
	else if (arg.compare(0, 4, "NICK") == 0)
		cmdNick(arg, client_socket);
	else if (arg.compare(0, 4, "QUIT") == 0 && arg.size() == 5)
		return (1);
	else
		send(client_socket, "Commands available : JOIN, PRIVMSG, INVITE, KICK, MODE, TOPIC, NICK, QUIT.\n",\
		 strlen("Commands available : JOIN, PRIVMSG, INVITE, KICK, MODE, TOPIC, NICK, QUIT.\n"), 0);
	std::cout << "Received input from client, socket fd: " << client_socket << ", Input: " << arg << std::endl;
	return (0);
}

bool Server::cmdPass(std::string arg)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			passwd;
	std::string			end;

	std::cout << "cmdPass" << std::endl;
	std::cout << "arg : " << arg << std::endl;

	if (!(stream >> cmd) || cmd != "PASS") {
	std::cout << "cmd" << std::endl;

		return (false);
	}
	if (!(stream >> passwd)) {
	std::cout << "passwd" << std::endl;

		return (false);
	}
	if (stream)
	{
	std::cout << "stream" << std::endl;
		stream >> end;
		if (end[0])
			return (false);
	}
	if (passwd == _serverPassword)
		return (true);
	return (false);
}

bool Server::cmdNick(std::string arg, int client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			nick_name;
	std::string			end;

	std::cout << "cmdNick" << std::endl;

	if (!(stream >> cmd) || cmd != "NICK") {
		return (false);
	}
	if (!(stream >> nick_name)) {
		return (false);
	}
	if (stream)
	{
		stream >> end;
		if (end[0])
			return (false);
	}
	for (std::map<int, User*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		User* user = it->second;
		if (user->get_nickname() == nick_name && nick_name[0] != '\n')
		{
			send(client_socket, "Nickname already taken.\n", strlen("Nickname already taken.\n"), 0);
			return (false);
		}
	}
	for (int i = 0; nick_name[i]; i++)
	{
		if (nick_name[i] < 32 || nick_name[i] == ':' || nick_name[i] == ';' || nick_name[i] == '!' || nick_name[i] == ',')
		{
			send(client_socket, "Erroneous nickname.\n", strlen("Erroneous nickname.\n"), 0);
			return (false);
		}
	}
	User* user = _clients[client_socket];
	user->set_nickname(nick_name);
	return (true);
}

bool Server::cmdUser(std::string arg, int client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			user_name;
	std::string			end;

	std::cout << "cmdUser" << std::endl;

	if (!(stream >> cmd) || cmd != "USER") {
		return (false);
	}
	if (!(stream >> user_name)) {
		return (false);
	}
	if (stream)
	{
		stream >> end;
		if (end[0] || user_name[0] != ':' || user_name.size() == 1)
			return (false);
	}
	for (int i = 1; user_name[i]; i++)
	{
		if (user_name[i] < 32 || user_name[i] == ':' || user_name[i] == ';' || user_name[i] == '!' || user_name[i] == ',')
		{
			send(client_socket, "Erroneous username.\n", strlen("Erroneous username.\n"), 0);
			return (false);
		}
	}
	User* user = _clients[client_socket];
	user->set_username(user_name);
	return (true);
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

bool Server::cmdJoin(std::string arg, int client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			channel_name;
	std::string			key;

	if (!(stream >> cmd) || cmd != "JOIN") {
		return (false);
	}
	if (!(stream >> channel_name)) {
		return (false);
	}
	if (stream)
	{
		stream >> key;
		if (channel_name[0] != '#' || channel_name.size() == 1)
			return (false);
	}
	if (_channels.find(channel_name) == _channels.end())
		_channels[channel_name] = new Channel(channel_name);
	User* user = _clients[client_socket];
	_channels[channel_name]->addUser(user);
	std::string join_message = "JOIN " + channel_name + "\n";
	send(client_socket, join_message.c_str(), join_message.size(), 0);
	std::string user_join_message = ":" + user->get_nickname() + " JOIN " + channel_name + "\n";
	_channels[channel_name]->sendMessage(user_join_message, client_socket);
	return (true);
}

void Server::cmdPrivMsg(std::string arg, int client_socket)
{
	(void)arg;
	(void)client_socket;
}


////////////////////////////////////////////////////////////////////////////////
//  ERROR_MSGS
const char *Server::ERR_INVALIDSOCKET::what() const throw()		{ return "Error creating socket"; }

const char *Server::ERR_BINDFAILURE::what() const throw()		{ return "Failed to bind socket to address"; }

const char *Server::ERR_LISTENINGFAILURE::what() const throw()	{ return "Socket failed to start listening"; }

const char *Server::ERR_SELECTFAILURE::what() const throw()		{ return "Select error"; }

const char *Server::ERR_ACCEPTFAILURE::what() const throw()		{ return "Accept error"; }

//-----------------

const char *Server::ERR_ALREADYREGISTRED::what() const throw()	{ return ("Already registered"); }

const char *Server::ERR_CANNOTSENDTOCHAN::what() const throw()	{ return ("<channel name> :Cannot send to channel"); }

const char *Server::ERR_CHANOPRIVSNEEDED::what() const throw()	{ return ("You're not channel operator"); }

const char *Server::ERR_ERRONEUSNICKNAME::what() const throw()	{ return ("Erroneous nickname"); }

const char *Server::ERR_KEYSET::what() const throw()			{ return ("<channel> :Channel key already set"); }

const char *Server::ERR_NEEDMOREPARAMS::what() const throw()	{ return ("Need more params"); }

const char *Server::ERR_NICKNAMEINUSE::what() const throw()		{ return ("Nickname is already in use"); }

const char *Server::ERR_NOLOGIN::what() const throw()			{ return ("<user> :User not logged in"); }

const char *Server::ERR_NONICKNAMEGIVEN::what() const throw()	{ return ("No nickname given"); }

const char *Server::ERR_NORECIPIENT::what() const throw()		{ return ("No recipient given (<command>)"); }

const char *Server::ERR_NOSUCHCHANNEL::what() const throw()		{ return ("No such channel"); }

const char *Server::ERR_NOSUCHNICK::what() const throw()		{ return ("No such nick/channel"); }

const char *Server::ERR_NOTEXTTOSEND::what() const throw()		{ return (":No text to send"); }

const char *Server::ERR_NOTONCHANNEL::what() const throw()		{ return ("You're not on that channel"); }

const char *Server::ERR_NOTOPLEVEL::what() const throw()		{ return ("<mask> :No toplevel domain specified"); }

const char *Server::ERR_TOOMANYTARGET::what() const throw()		{ return ("<target> :Duplicate recipients. No message delivered"); }

const char *Server::ERR_USERONCHANNEL::what() const throw()		{ return ("<user> <channel> :is already on channel"); }

const char *Server::ERR_UNKNOWNMODE::what() const throw()		{ return ("<char> :is unknown mode char to me"); }

const char *Server::ERR_USERSDONTMATCH::what() const throw()	{ return (":Cant change mode for other users"); }

const char *Server::ERR_UMODEUNKNOWNFLAG::what() const throw()	{ return (":Unknown MODE flag"); }

const char *Server::ERR_WILDTOPLEVEL::what() const throw()		{ return ("<mask> :Wildcard in toplevel domain"); }

//-----------------

const char *Server::RPL_AWAY::what() const throw()				{ return ("<nick> :<away message>"); }

const char *Server::RPL_BANLIST::what() const throw()			{ return ("<channel> <banid>"); }

const char *Server::RPL_CHANNELMODEIS::what() const throw()		{ return ("<channel> <mode> <mode params>"); }

const char *Server::RPL_ENDOFBANLIST::what() const throw()		{ return ("<channel> :End of channel ban list"); }

const char *Server::RPL_INVITING::what() const throw()			{ return ("<channel> <nick>"); }

const char *Server::RPL_NOTOPIC::what() const throw()			{ return ("<channel> :No topic is set"); }

const char *Server::RPL_SUMMONING::what() const throw()			{ return ("<user> :Summoning user to IRC"); }

const char *Server::RPL_TOPIC::what() const throw()				{ return ("<channel> :<topic>"); }

const char *Server::RPL_UMODEIS::what() const throw()			{ return ("<user mode string>"); }

////////////////////////////////////////////////////////////////////////////////
