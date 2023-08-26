#include "Server.hpp"

bool Server::_active = true;

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
	erase_all_channels();
	erase_all_users();
	close(_serverSocket);
	std::cout << "All users are deleted" << std::endl;
	std::cout << "Server destroyed" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

void	Server::ctrlC_behaviour(int signal)
{
	(void)signal;
	_active = false;
	std::cout << "CTRL C SUCCESS" << std::endl;
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
	std::cout << "before select" << std::endl;
	int activity = select(_topSocket + 1, &_readfds, NULL, NULL, NULL);
	std::cout << "after select" << std::endl;
	if (_active && activity < 0)
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

void	Server::erase_one_channel(std::string channel_name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(channel_name);
	if (it != _channels.end())
	{
		delete it->second;
		it->second = NULL;
		_channels.erase(it);
	}
}

void	Server::erase_all_users()
{
	for (std::map<type_sock, User*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		_death_note.push_back(it->first);

	erase_death_note();
}

void	Server::erase_all_channels()
{
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
		_closing_list.push_back(it->first);

	erase_closing_list();
}

void	Server::erase_death_note()
{
	for (std::vector<type_sock>::iterator it = _death_note.begin(); it != _death_note.end(); it = _death_note.begin())
	{
		erase_one_user(*it);
		std::cout << "user erased from map" << std::endl;
		_death_note.erase(it);
		std::cout << "user erased from vector" << std::endl;
	}
}

void	Server::erase_closing_list()
{
	for (std::vector<std::string>::iterator it = _closing_list.begin(); it != _closing_list.end(); it = _closing_list.begin())
	{
		erase_one_channel(*it);
		std::cout << "channel erased from map" << std::endl;
		_closing_list.erase(it);
		std::cout << "channel erased from vector" << std::endl;
	}
}

bool	Server::recv_from_user(type_sock userSocket)
{
	char		buffer[BUFFER_SIZE] = {0};
	ssize_t bytes = recv(userSocket, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	std::cout << "input received. bytes = " << bytes << std::endl;

	if (bytes < 0)
	{
		std::cout << "except Problem with recv, bytes == -1" << std::endl;
		throw (std::runtime_error("recv error"));
	}
	if (bytes == 0)
	{
		std::cout << "except disconnection detected, bytes == 0" << std::endl;
		throw (std::runtime_error("client disconnected"));
	}
	if (bytes > 0)
	{
		std::cout << "normal behaviour, bytes > 0" << std::endl;
		_clients[userSocket]->add_to_inputStack(buffer);
		if (_clients[userSocket]->get_inputStack()[_clients[userSocket]->get_inputStack().size() - 1] == '\n')/////////condition la plus appropriee ?
		{
			_clients[userSocket]->set_inputStack(_clients[userSocket]->get_inputStack().erase(_clients[userSocket]->get_inputStack().size() - 1));
			std::cout << "input cleaned" << std::endl;
			return (true);
		}
		else
			send(userSocket, "^D", strlen("^D"), 0);
	}
	return (false);
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

////////////////////////////////////////////////////////////////////////////////

void	Server::run()
{
	type_sock socket = 0;

	signal(SIGINT, ctrlC_behaviour);

	while (_active)
	{
		std::cout << std::endl << "while" << std::endl;

		reset_fd_set();
		std::cout << "fd_set reseted" << std::endl;

		waiting_for_activity();
		std::cout << "waiting is over" << std::endl;

		// Server part

		if (_active && check_activity(_serverSocket))
		{
			std::cout << "connection attempt detected on server" << std::endl;

			socket = get_incoming_socket();
			std::cout << "socket identified" << std::endl;

			if (_active && _clients.find(socket) == _clients.end())
			{
				std::cout << "user not find in map" << std::endl;

				add_new_user(socket);
				std::cout << "new user added" << std::endl;

				if (_active && _clients[socket]->get_userState() == 0)
					send(socket, "PASS <password>\n", strlen("PASS <password>\n"), 0);
			}
		}

		// Client part

		for (std::map<type_sock, User*>::iterator it = _clients.begin(); _active && it != _clients.end(); it++)
		{
			std::cout << "for" << std::endl;
			socket = it->first;
			if (_active && check_activity(it->second->get_userSocket()))
			{
				std::cout << "client activity detected" << std::endl;
				try
				{
					if (_active)
					{
						// std::string	input = recv_from_user(socket);
						if (recv_from_user(socket))
						{
							std::string	input = _clients[socket]->get_inputStack();
							std::cout << "input correctly recieved" << std::endl;

							checkCommand(input, socket);
							std::cout << "command checked" << std::endl;
							_clients[socket]->reset_inputStack();
						}
					}
				}
				catch(std::exception const& e)
				{
					if (_active)
					{
						std::string str = e.what();
						if (str == "client disconnected")
							std::cout << "Client disconnected. Info : " << get_clientDatas(socket) << std::endl;
						else if (str == "recv error")
							std::cout << "Lost connection with client. Info : " << get_clientDatas(socket) << std::endl;
						_death_note.push_back(socket);
					}
				}
			}
		}

		if (_active)
			erase_death_note();
	}
}

////////////////////////////////////////////////////////////////////////////////

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
	int						lvl = _clients[client_socket]->get_userState();
	std::stringstream		stream(input);
	std::string				cmd;

	if (lvl < 3)
		ask_for_login_credentials(input, client_socket, lvl);
	else
	{
		if (stream >> cmd)
		{
			if (cmd == "JOIN")
				cmdJoin(input, client_socket);
			else if (cmd == "PRIVMSG")
				cmdPrivMsg(input, client_socket);
			else if (cmd == "PART")
				cmdPart(input, client_socket);
			else if (cmd == "INVITE")
				cmdInvite(input, client_socket);
			else if (cmd == "KICK")
				cmdKick(input, client_socket);
			else if (cmd == "MODE")
				cmdMode(input, client_socket);
			else if (cmd == "TOPIC")
				cmdTopic(input, client_socket);
			else if (cmd == "NICK")
				cmdNick(input, client_socket);
			else if (cmd == "QUIT" && input.size() == 4)///////////////////voir si on permet QUIT aussi pour les lvl < 3
				erase_one_user(client_socket);///////////////pas encore testee
			else
				send(client_socket, "Commands available : JOIN, PRIVMSG, INVITE, KICK, MODE, TOPIC, NICK, PART, QUIT.\n",\
				strlen("Commands available : JOIN, PRIVMSG, INVITE, KICK, MODE, TOPIC, NICK, PART, QUIT.\n"), 0);
		}
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

	// if (!(stream >> cmd) || cmd != "PASS" || !(stream >> passwd))///////test de syntaxe plus courte
	// {
	// 	std::cout << "cmd || passwd" << std::endl;
	// 	return (false);
	// }
	// if (stream)
	// {
	// 	std::cout << "stream" << std::endl;
	// 	stream >> end;
	// 	if (end[0])//////////////////////////////////voir pour end.size() > 0 pour rester en mode c++
	// 		return (false);
	// }

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
		if (!end.empty())
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
	std::string			nickname;
	std::string			end;

	std::cout << "cmdNick" << std::endl;

	// syntax test
	if (!(stream >> cmd) || cmd != "NICK")
		return (false);
	if (!(stream >> nickname))
	{
		return (false);
	}
	if (stream)
	{
		stream >> end;
		if (!end.empty())
			return (false);
	}

	// availability test
	for (std::map<int, User*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		User* user = it->second;
		if (user->get_nickname() == nickname && nickname[0] != '\n')
		{
			send(client_socket, "Nickname already taken.\n", strlen("Nickname already taken.\n"), 0);
			return (false);
		}
	}

	// forbiden character test
	for (int i = 0; nickname[i]; i++)
	{
		if (nickname[i] < 32 || nickname[i] == ':' || nickname[i] == ';' || nickname[i] == '!' || nickname[i] == ',')
		{
			send(client_socket, "Erroneous nickname.\n", strlen("Erroneous nickname.\n"), 0);
			return (false);
		}
	}
	_clients[client_socket]->set_nickname(nickname);
	return (true);
}

bool Server::cmdUser(std::string arg, int client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			username;
	std::string			end;

	std::cout << "cmdUser" << std::endl;

	// syntax test
	if (!(stream >> cmd) || cmd != "USER") {
		return (false);
	}
	if (!(stream >> username)) {
		return (false);
	}
	if (stream)
	{
		stream >> end;
		if (!end.empty() || username[0] != ':' || username.size() == 1)
			return (false);
	}

	// forbidden character test
	for (int i = 1; username[i]; i++)
	{
		if (username[i] < 32 || username[i] == ':' || username[i] == ';' || username[i] == '!' || username[i] == ',')
		{
			send(client_socket, "Erroneous username.\n", strlen("Erroneous username.\n"), 0);
			return (false);
		}
	}
	_clients[client_socket]->set_username(username);
	return (true);
}

type_sock	Server::findSocketFromNickname(std::string target)
{
	type_sock targetSocket = -1;
	for (std::map<type_sock, User*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
    	User* user = it->second;
		if (user->get_nickname() == target)
		{
			targetSocket = user->get_userSocket();
			break ;
		}
	}
	return (targetSocket);
}

void Server::cmdKick(std::string arg, type_sock client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			channel_name;
	std::string			banned_user;
	std::string			ban_msg;

	if (!(stream >> cmd) || cmd != "KICK")
		return ;
	if (!(stream >> channel_name))
	{
		send(client_socket, "KICK <#channel_name> <nickname> [<comment>]\n", strlen("KICK <#channel_name> <nickname> [<comment>]\n"), 0);
		return ;
	}
	if (channel_name[0] != '#' || channel_name.size() == 1)
	{
		send(client_socket, "KICK <#channel_name> <nickname> [<comment>]\n", strlen("KICK <#channel_name> <nickname> [<comment>]\n"), 0);
		return ;
	}
	if (!(stream >> banned_user))
	{
		send(client_socket, "KICK <#channel_name> <nickname> [<comment>]\n", strlen("KICK <#channel_name> <nickname> [<comment>]\n"), 0);
		return ;
	}
	if (stream)
	{
		stream >> std::ws;
		std::getline(stream, ban_msg);
	}
	if (!(_channels[channel_name]->getUserPrivilege(_clients[client_socket])))
	{
		send(client_socket, "You don't have the right to use this command !\n", \
		strlen("You don't have the right to use this command !\n"), 0);
		return ;
	}
	if (_channels.find(channel_name) == _channels.end())
	{
		send(client_socket, "This channel does not exist !\n", strlen("This channel does not exist !\n"), 0);
		return ;
	}
	type_sock targetSocket = findSocketFromNickname(banned_user);
	if (targetSocket == -1)
	{
		send(client_socket, "Target does not exist !\n", strlen("Target does not exist !\n"), 0);
		return ;
	}
	if (!(_channels[channel_name]->hasUser(_clients[targetSocket])))
	{
		send(client_socket, "The target is not registered in the channel !\n", strlen("The target is not registered in the channel !\n"), 0);
		return ;
	}
	if (banned_user == _clients[client_socket]->get_nickname())
	{
		send(client_socket, "Use PART <#channel_name> to leave a channel , don't KICK yourself bro\n", strlen("Use PART <#channel_name> to leave a channel , don't KICK yourself bro\n"), 0);
		return ;
	}
	_channels[channel_name]->removeUser(_clients[targetSocket]);
	if (!ban_msg.empty())
	{
		std::string ban_message_plus = channel_name + " :You were kicked by " + _clients[client_socket]->get_nickname() + " (" + ban_msg + ")\n";
		send(targetSocket, ban_message_plus.c_str(), ban_message_plus.size(), 0);
		std::string ban_chan_message_plus = channel_name + " :" + _clients[targetSocket]->get_nickname() + " was kicked by " + _clients[client_socket]->get_nickname() + " (" + ban_msg + ")\n";
    	_channels[channel_name]->sendMessage(ban_chan_message_plus, client_socket);
	}
	else
	{
		std::string ban_message = channel_name + " :You were kicked by " + _clients[client_socket]->get_nickname() + "\n";
		send(targetSocket, ban_message.c_str(), ban_message.size(), 0);
		std::string ban_chan_message = channel_name + " :" + _clients[targetSocket]->get_nickname() + " was kicked by " + _clients[client_socket]->get_nickname() + "\n";
    	_channels[channel_name]->sendMessage(ban_chan_message, client_socket);
	}
}

void Server::cmdInvite(std::string arg, type_sock client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			channel_name;
	std::string			invited_user;

	if (!(stream >> cmd) || cmd != "INVITE")
		return ;
	if (!(stream >> channel_name))
	{
		send(client_socket, "INVITE <#channel_name> <nickname>\n", strlen("INVITE <#channel_name> <nickname>\n"), 0);
		return ;
	}
	if (channel_name[0] != '#' || channel_name.size() == 1)
	{
		send(client_socket, "INVITE <#channel_name> <nickname>\n", strlen("INVITE <#channel_name> <nickname>\n"), 0);
		return ;
	}
	if (!(stream >> invited_user))
	{
		send(client_socket, "INVITE <#channel_name> <nickname>\n", strlen("INVITE <#channel_name> <nickname>\n"), 0);
		return ;
	}
	if (_channels.find(channel_name) == _channels.end())
	{
		send(client_socket, "This channel does not exist !\n", strlen("This channel does not exist !\n"), 0);
		return ;
	}
	type_sock targetSocket = findSocketFromNickname(invited_user);
	if (targetSocket == -1)
	{
		send(client_socket, "Target does not exist !\n", strlen("Target does not exist !\n"), 0);
		return ;
	}
	if (_channels[channel_name]->get_inviteState())
	{
		if (!(_channels[channel_name]->getUserPrivilege(_clients[client_socket])))
		{
			send(client_socket, "You don't have the right to use this command !\n", \
			strlen("You don't have the right to use this command !\n"), 0);
			return ;
		}
	}
	_channels[channel_name]->addUser(_clients[targetSocket]);
    std::string invite_message = "You were invited by " + _clients[client_socket]->get_nickname() + " on " + channel_name + "\n";
    send(targetSocket ,invite_message.c_str(), invite_message.size(), 0);
}

void Server::cmdPart(std::string arg, type_sock client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			channel_name;
	std::string			end;

	if (!(stream >> cmd) || cmd != "PART")
		return ;
	if (!(stream >> channel_name))
	{
		send(client_socket, "PART <#channel_name>\n", strlen("PART <#channel_name>\n"), 0);
		return ;
	}
	if (channel_name[0] != '#' || channel_name.size() == 1)
	{
		send(client_socket, "PART <#channel_name>\n", strlen("PART <#channel_name>\n"), 0);
		return ;
	}
	if (stream)
	{
		stream >> end;
		if (!end.empty())
			return ;
	}
	if (_channels.find(channel_name) == _channels.end())
	{
		send(client_socket, "This channel does not exist !\n", strlen("This channel does not exist !\n"), 0);
		return ;
	}
	if (!(_channels[channel_name]->hasUser(_clients[client_socket])))
	{
		send(client_socket, "You're not even registered in the channel !\n", strlen("You're not even registered in the channel !\n"), 0);
		return ;
	}
	_channels[channel_name]->removeUser(_clients[client_socket]); /////////////Voir pour le remove channel
	std::string leaving_message = ":" + _clients[client_socket]->get_nickname() + " has left " + channel_name + "\n";
    _channels[channel_name]->sendMessage(leaving_message, client_socket);


}

void Server::cmdTopic(std::string arg, type_sock client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			channel_name;
	std::string			topic;

	if (!(stream >> cmd) || cmd != "TOPIC") 	//Check nom de la commande
		return ;
	if (!(stream >> channel_name))
	{
		send(client_socket, "TOPIC <#channel_name> :<topic>\n", strlen("TOPIC <#channel_name> :<topic>\n"), 0);
		return ;
	}
	if (channel_name[0] != '#' || channel_name.size() == 1)
	{
		send(client_socket, "TOPIC <#channel_name> :<topic>\n", strlen("TOPIC <#channel_name> :<topic>\n"), 0);
		return ;
	}
	if (stream)												//Recupere potentiel 3eme argument
	{
		stream >> std::ws;
		std::getline(stream, topic);
	}
	if (_channels.find(channel_name) == _channels.end())
	{
		send(client_socket, "This channel does not exist !\n", strlen("This channel does not exist !\n"), 0);
		return ;
	}
	if (!(_channels[channel_name]->hasUser(_clients[client_socket])))
	{
		send(client_socket, "You're not registered in the channel !\n", strlen("You're not registered in the channel !\n"), 0);
		return ;
	}
	if (topic.empty())
	{
		if (_channels[channel_name]->get_topic().empty())
		{
			send(client_socket, "There is no active topic on this channel !\n", strlen("There is no active topic on this channel !\n"), 0);
			return ;
		}
		std::string topic_message = "TOPIC " + channel_name + " " + _channels[channel_name]->get_topic() + "\n";
		send(client_socket, topic_message.c_str(), topic_message.size(), 0);
	}
	else
	{
		if (topic[0] != ':')
		{
			send(client_socket, "TOPIC <#channel_name> :<topic>\n", strlen("TOPIC <#channel_name> :<topic>\n"), 0);
			return ;
		}
		if (topic.size() == 1 && _channels[channel_name]->getUserPrivilege(_clients[client_socket]))
		{
			_channels[channel_name]->clear_topic();
			return ;
		}
		if (_channels[channel_name]->getUserPrivilege(_clients[client_socket]))
		{
			_channels[channel_name]->set_topic(topic);
			std::string new_topic_message = "TOPIC " + channel_name + " " + _channels[channel_name]->get_topic() + "\n";
    		_channels[channel_name]->sendMessage(new_topic_message, client_socket);
			return ;
		}
		else
			send(client_socket, "You don't have the right to change topic on this channel !\n", \
			strlen("You don't have the right to change topic on this channel !\n"), 0);
	}
}

void Server::cmdMode(std::string arg, type_sock client_socket)
{
	// (void)arg;
	// (void)client_socket;
	// MODE - Changer le mode du channel :
	// — i : Définir/supprimer le canal sur invitation uniquement
	// ft_irc Internet Relay Chat
	// — t : Définir/supprimer les restrictions de la commande TOPIC pour les opé-
	// rateurs de canaux
	// — k : Définir/supprimer la clé du canal (mot de passe)
	// — o : Donner/retirer le privilège de l’opérateur de canal
	// — l : Définir/supprimer la limite d’utilisateurs pour le canal
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			channel_name;
	std::string			limit;
	std::string			user;
	std::string			ban_mask;
	std::string			end;

	if (!(stream >> cmd) || cmd != "MODE") 	//Check nom de la commande
		return ;
	if (!(stream >> channel_name))
	{
		send(client_socket, "MODE <channel> {[+|-]|o|i|t|k|l} [<limit>] [<user>] [<ban mask>]\n", strlen("JMODE <channel> {[+|-]|o|i|t|k|l} [<limit>] [<user>] [<ban mask>]\n"), 0);
		return ;
	}
	if (channel_name[0] != '#' || channel_name.size() == 1) //Check syntaxe du nom du channel
		return ;
	if (stream)												//Recupere potentiel 3eme argument
		stream >> limit;
	// if (stream)
	// {
	// 	stream >> end;
	// 	if (!end.empty())
	// 	{
	// 		send(client_socket, "JOIN <#channel_name> <limit>\n", strlen("JOIN <#channel_name> <limit>\n"), 0);
	// 		return ;
	// 	}
	// }
	// if (_channels.find(channel_name) == _channels.end())  //Check si channel existe
	// {
	// 	if (!limit.empty())								//Si channel n'existe pas et que le client a mis un pass, faux
	// 	{
	// 		send(client_socket, "This channel does not exist !\n", strlen("This channel does not exist !\n"), 0);
	// 		return ;
	// 	}
	// 	_channels[channel_name] = new Channel(channel_name, _clients[client_socket]);  //Sinon cree channel
	// 	send(client_socket, "New channel created !\n", strlen("New channel created !\n"), 0);
	// 	return ;
	// }
	// if (!limit.empty() && _channels[channel_name]->get_password().empty()) //Si le channel n'a pas de pass et que le client en a mis un, faux
	// {
	// 	send(client_socket, "No limit set on the channel yet !\n", strlen("No limit set on the channel yet !\n"), 0);
	// 	return ;
	// }
	// else
	// {
	// 	if (!(_channels[channel_name]->get_password().empty())) //Check si channel password
	// 	{
	// 		if (limit.empty() || limit != _channels[channel_name]->get_password()) //Check pass donne par le client
	// 		{
	// 			send(client_socket, "Wrong channel password !\n", strlen("Wrong channel password !\n"), 0);
	// 			return ;
	// 		}
	// 	}
	// 	if (_channels[channel_name]->hasUser(_clients[client_socket])) //Check si le client est deja sur le channel
	// 	{
    //         send(client_socket, "You are already in the channel !\n", strlen("You are already in the channel !\n"), 0);
    //         return ;
    //     }
	// 	_channels[channel_name]->addUser(_clients[client_socket]); //Sinon add le nouvel user et en averti les autres sur le channel
    // 	std::string user_join_message = ":" + _clients[client_socket]->get_nickname() + " JOIN " + channel_name + "\n";
    // 	_channels[channel_name]->sendMessage(user_join_message, client_socket);
	// }
}

void Server::cmdJoin(std::string arg, type_sock client_socket) //potentiellement passe en void
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			channel_name;
	std::string			key;
	std::string			end;

	if (!(stream >> cmd) || cmd != "JOIN") 	//Check nom de la commande
		return ;
	if (!(stream >> channel_name))
	{
		send(client_socket, "JOIN <#channel_name> <key>\n", strlen("JOIN <#channel_name> <key>\n"), 0);
		return ;
	}
	if (channel_name[0] != '#' || channel_name.size() == 1) //Check syntaxe du nom du channel
		return ;
	if (stream)												//Recupere potentiel 3eme argument
		stream >> key;
	if (stream)
	{
		stream >> end;
		if (!end.empty())
		{
			send(client_socket, "JOIN <#channel_name> <key>\n", strlen("JOIN <#channel_name> <key>\n"), 0);
			return ;
		}
	}
	if (_channels.find(channel_name) == _channels.end())  //Check si channel existe
	{
		if (!key.empty())								//Si channel n'existe pas et que le client a mis un pass, faux
		{
			send(client_socket, "This channel does not exist !\n", strlen("This channel does not exist !\n"), 0);
			return ;
		}
		_channels[channel_name] = new Channel(channel_name, _clients[client_socket]);  //Sinon cree channel
		send(client_socket, "New channel created !\n", strlen("New channel created !\n"), 0);
		return ;
	}
	if (!key.empty() && _channels[channel_name]->get_password().empty()) //Si le channel n'a pas de pass et que le client en a mis un, faux
	{
		send(client_socket, "No key set on the channel yet !\n", strlen("No key set on the channel yet !\n"), 0);
		return ;
	}
	else
	{
		if (!(_channels[channel_name]->get_password().empty())) //Check si channel password
		{
			if (key.empty() || key != _channels[channel_name]->get_password()) //Check pass donne par le client
			{
				send(client_socket, "Wrong channel password !\n", strlen("Wrong channel password !\n"), 0);
				return ;
			}
		}
		if (_channels[channel_name]->hasUser(_clients[client_socket])) //Check si le client est deja sur le channel
		{
            send(client_socket, "You are already in the channel !\n", strlen("You are already in the channel !\n"), 0);
            return ;
        }
		_channels[channel_name]->addUser(_clients[client_socket]); //Sinon add le nouvel user et en averti les autres sur le channel
    	std::string user_join_message = ":" + _clients[client_socket]->get_nickname() + " JOIN " + channel_name + "\n";
    	_channels[channel_name]->sendMessage(user_join_message, client_socket);
	}
}

void Server::cmdPrivMsg(std::string arg, type_sock client_socket)
{
	std::stringstream	stream(arg);
	std::string			cmd;
	std::string			target;
	std::string			message;

	if (!(stream >> cmd) || cmd != "PRIVMSG") 	//Check nom de la commande
		return ;
	if (!(stream >> target))
	{
		send(client_socket, "PRIVMSG <target> :<message>\n", strlen("PRIVMSG <target> :<message>\n"), 0);
		return ;
	}
	if (stream)												//Recupere potentiel 3eme argument
	{
		stream >> std::ws;
		std::getline(stream, message);
	}
	if (message.empty() || message[0] != ':')
	{
		send(client_socket, "PRIVMSG <target> :<message>\n", strlen("PRIVMSG <target> :<message>\n"), 0);
		return ;
	}
	if (target[0] == '#')
	{
		if (target.size() == 1 || _channels.find(target) == _channels.end())
		{
			send(client_socket, "This channel does not exist !\n", strlen("This channel does not exist !\n"), 0);
			return ;
		}
		if (!(_channels[target]->hasUser(_clients[client_socket])))
		{
			send(client_socket, "You're not registered in the channel !\n", strlen("You're not registered in the channel !\n"), 0);
			return ;
		}
		std::string priv_message_chan = target + " <" + _clients[client_socket]->get_nickname() + "> " + message + "\n";
		_channels[target]->sendMessage(priv_message_chan, client_socket);
	}
	else
	{
		std::string priv_message = "<" + _clients[client_socket]->get_nickname() + "> " + message + "\n";
		type_sock targetSocket = findSocketFromNickname(target);
		if (targetSocket == -1)
		{
			send(client_socket, "Target does not exist !\n", strlen("Target does not exist !\n"), 0);
			return ;
		}
		send(targetSocket, priv_message.c_str(), priv_message.size(), 0);
	}

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
