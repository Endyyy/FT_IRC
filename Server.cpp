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
	erase_all_users();
	std::cout << "All users are deleted" << std::endl;
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

void	Server::erase_one_user(type_sock userSocket)
{
	std::map<type_sock, User*>::iterator it = _clients.find(userSocket);
	if (it != _clients.end())
	{
		FD_CLR(userSocket, &_readfds);
		close(userSocket);
		delete it->second;
		it->second = NULL;
		_clients.erase(it);
	}
}

void	Server::erase_all_users()
{
	std::vector<type_sock> death_note;
	for (std::map<type_sock, User*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		death_note.push_back(it->first);

	for (std::vector<type_sock>::iterator it = death_note.begin(); it != death_note.end(); it = death_note.begin())
	{
		erase_one_user(*it);
		std::cout << "user erased from map" << std::endl;
		death_note.erase(it);
		std::cout << "user erased from death note" << std::endl;
	}
}

std::string	Server::recv_from_user(type_sock userSocket)
{
	std::string	str;
	char		buffer[BUFFER_SIZE] = {0};
	ssize_t bytes = recv(userSocket, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	// bytes vaut 0 si user ctrl C, bytes vaut 1 si chaine vide

	std::cout << "input received. bytes = " << bytes << std::endl;

	if (bytes > 0)
	{
		std::cout << "bytes > 0" << std::endl;
		str = buffer;
		str = str.erase(str.size() - 1);
		std::cout << "input cleaned" << std::endl;
		return (str);
	}
	else if (bytes == 0)
	{
		std::cout << "except disconnection detected, bytes == 0" << std::endl;
		throw (std::runtime_error("client disconnected"));
	}
	else if (bytes < 0)
	{
		std::cout << "except Problem with recv, bytes == -1" << std::endl;
		throw (std::runtime_error("recv error"));
	}
	std::cout << "empty string is returned" << std::endl;
	return ("");
}

std::string Server::get_clientDatas(type_sock socket)
{
	std::string str = "";
	std::string tmp = _clients[socket]->get_nickname();
	std::ostringstream oss;
	oss << socket;
	if (tmp != "")
		str += "Nickname \'" + tmp + "\'; ";
	tmp = _clients[socket]->get_username();
	if (tmp != "")
		str += "Username \'" + tmp + "\'; ";
	str += "Socket " + oss.str() + ";";
	return (str);
}

void	Server::run()
{
	type_sock socket = 0;
	while (true)
	{
		std::cout << std::endl << "while" << std::endl;

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

				if (_clients[socket]->get_userState() == 0)
					send(socket, "PASS <password>\n", strlen("PASS <password>\n"), 0);
				else if (_clients[socket]->get_userState() == 1)////////////////////////////utile?
					send(socket, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
				else if (_clients[socket]->get_userState() == 2)
					send(socket, "USER :<username>\n", strlen("USER :<username>\n"), 0);
			}
		}

		// Handle input from clients
		std::vector<type_sock> disconnectedClients;

		for (std::map<type_sock, User*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		{
			std::cout << "for" << std::endl;
			socket = it->first;
			if (check_activity(it->second->get_userSocket()))
			{
				std::cout << "activity checked" << std::endl;
				try
				{
					std::string	input = recv_from_user(socket);
					std::cout << "input correctly recieved" << std::endl;

					checkCommand(input, socket);
					std::cout << "command checked" << std::endl;
				}
				catch(std::exception const& e)
				{
					std::string str = e.what();
					if (str == "client disconnected")
						std::cout << "Client disconnected. Info : " << get_clientDatas(socket) << std::endl;
					else if (str == "recv error")
						std::cout << "Lost connection with client. Info : " << get_clientDatas(socket) << std::endl;
					else//////////////////test
						std::cout << "WTF is going on !" << std::endl;
					disconnectedClients.push_back(socket);
					// erase_one_user(socket);
					// std::cout << "user erased" << std::endl;
				}
			}
		}

		// Fonction qui permet de clean les clients deco, ne pas changer la syntaxe ou segfault :D ////////challenge accepted ;)
		for (std::vector<type_sock>::iterator it = disconnectedClients.begin(); it != disconnectedClients.end(); it = disconnectedClients.begin())
		{
			erase_one_user(*it);
			std::cout << "user erased from map" << std::endl;
			disconnectedClients.erase(it);
			std::cout << "user erased from vector" << std::endl;
		}
	}
}

void	Server::ask_for_login_credentials(std::string input, type_sock client_socket, int lvl)
{
	if (lvl == 0)
	{
		std::cout << "user is lvl 0" << std::endl;
		if (cmdPass(input))
		{
			_clients[client_socket]->set_userState(1);
			std::cout << "user has become lvl 1" << std::endl;
			send(client_socket, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
		}
		else
			send(client_socket, "PASS <password>\n", strlen("PASS <password>\n"), 0);
			//ask for password again
	}
	else if (lvl == 1)
	{
		std::cout << "user is lvl 1" << std::endl;

		if (cmdNick(input, client_socket))
		{
			_clients[client_socket]->set_userState(2);
			std::cout << "user has become lvl 2" << std::endl;
			send(client_socket, "USER :<username>\n", strlen("USER :<username>\n"), 0);
		}
		else
			send(client_socket, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
			//ask for another nickname
	}
	else if (lvl == 2)
	{
		std::cout << "user is lvl 2" << std::endl;

		if (cmdUser(input, client_socket))
		{
			_clients[client_socket]->set_userState(3);
			std::cout << "user has become lvl 3" << std::endl;
			send(client_socket, "You are now one of us !\n", strlen("You are now one of us !\n"), 0);
		}
		else
			send(client_socket, "USER :<username>\n", strlen("USER :<username>\n"), 0);
			//ask for a valid username
	}
}

void Server::checkCommand(std::string input, type_sock client_socket)
{
	int	lvl = _clients[client_socket]->get_userState();
	if (lvl < 3)
		ask_for_login_credentials(input, client_socket, lvl);
	else // utiliser un stringstream ici aussi pour comparer les noms de commandes, le compare est moins adapte qu'un std::string test == std::string cmd
	{
		if (input.compare(0, 4, "JOIN") == 0)
			cmdJoin(input, client_socket);
		else if (input.compare(0, 7, "PRIVMSG") == 0)
			cmdPrivMsg(input, client_socket);
		else if (input.compare(0, 6, "INVITE") == 0)
			cmdInvite(input, client_socket);
		else if (input.compare(0, 4, "KICK") == 0)
			cmdKick(input, client_socket);
		else if (input.compare(0, 4, "MODE") == 0)
			cmdMode(input, client_socket);
		else if (input.compare(0, 5, "TOPIC") == 0)
			cmdTopic(input, client_socket);
		else if (input.compare(0, 4, "NICK") == 0)
			cmdNick(input, client_socket);
		else if (input.compare(0, 4, "QUIT") == 0 && input.size() == 5)
			return ;///////////////////////////////////a modifier
		else
			send(client_socket, "Commands available : JOIN, PRIVMSG, INVITE, KICK, MODE, TOPIC, NICK, QUIT.\n",\
			strlen("Commands available : JOIN, PRIVMSG, INVITE, KICK, MODE, TOPIC, NICK, QUIT.\n"), 0);
	}
	std::cout << "Received data from client, socket fd: " << client_socket << ", Data: " << input << std::endl;
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
