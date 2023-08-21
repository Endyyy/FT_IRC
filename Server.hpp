#ifndef SERVER_HPP
# define SERVER_HPP

# include "tools.hpp"
# include "Channel.hpp"
# include "User.hpp"
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
# include "User.hpp"

# define MAX_CLIENTS 100
# define BUFFER_SIZE 1024

class Server
{
	private:
		int const			_port;
		std::string const	_serverPassword;
		type_sock const		_serverSocket;
		sockaddr_in			_address;
		fd_set				_readfds;
		type_sock			_topSocket;

		std::map<type_sock, User*>	_clients;

		Server();
		Server(Server const& source);
		Server&	operator=(Server const& source);

	public:
		Server(int port, std::string serverPassword);
		~Server();

		void	bind_socket_to_address();
		void	start_listening();
		void	run();

		void		reset_fd_set();
		void		waiting_for_activity();
		bool		check_server_activity();
		type_sock	add_user_to_server();
		std::string	read_from_user(int userSocket, char const* sentence);
		void		set_address();


		void	cmdPass(std::string arg, int sd);
		void	cmdNick(std::string arg, int sd);
		void	cmdUser(std::string arg, int sd);
		void	cmdKick(std::string arg, int sd);
		void	cmdInvite(std::string arg, int sd);
		void	cmdTopic(std::string arg, int sd);
		void	cmdMode(std::string arg, int sd);
		void	cmdJoin(std::string arg, int sd);
		void	cmdPrivMsg(std::string arg, int sd);
		void	cmdQuit();
		void	checkCommand(int sd, char *buffer);

		//ERROR_MSGS

		class ERR_INVALIDSOCKET :		public std::exception { virtual const char* what() const throw(); };
		class ERR_BINDFAILURE :			public std::exception { virtual const char* what() const throw(); };
		class ERR_LISTENINGFAILURE :	public std::exception { virtual const char* what() const throw(); };
		class ERR_SELECTFAILURE :		public std::exception { virtual const char* what() const throw(); };
		class ERR_ACCEPTFAILURE :		public std::exception { virtual const char* what() const throw(); };

		// "Error reading password from client."class ERR_CANNOTREADPWD :		public std::exception { virtual const char* what() const throw(); };
		// "Client authentication failed. Connection rejected."class ERR_WRONGPASSWORD :		public std::exception { virtual const char* what() const throw(); };
		// "Error reading nickname from client."class ERR_CANNOTREADNICKNAME :	public std::exception { virtual const char* what() const throw(); };
		// "Error reading username from client."class ERR_CANNOTREADUSERNAME :	public std::exception { virtual const char* what() const throw(); };
		// class ERR_CLIENTDISCONNECTED :		public std::exception { virtual const char* what() const throw(); };
		// class ERR_ :		public std::exception { virtual const char* what() const throw(); };


		class ERR_ALREADYREGISTRED :	public std::exception { virtual const char* what() const throw(); };
		class ERR_BADCHANMASK :			public std::exception { virtual const char* what() const throw(); };
		class ERR_CANNOTSENDTOCHAN :	public std::exception { virtual const char* what() const throw(); };
		class ERR_CHANOPRIVSNEEDED :	public std::exception { virtual const char* what() const throw(); };
		class ERR_ERRONEUSNICKNAME :	public std::exception { virtual const char* what() const throw(); };
		class ERR_KEYSET :				public std::exception { virtual const char* what() const throw(); };
		class ERR_NEEDMOREPARAMS :		public std::exception { virtual const char* what() const throw(); };
		class ERR_NICKCOLLISION :		public std::exception { virtual const char* what() const throw(); };
		class ERR_NICKNAMEINUSE :		public std::exception { virtual const char* what() const throw(); };
		class ERR_NOLOGIN :				public std::exception { virtual const char* what() const throw(); };
		class ERR_NONICKNAMEGIVEN :		public std::exception { virtual const char* what() const throw(); };
		class ERR_NORECIPIENT :			public std::exception { virtual const char* what() const throw(); };
		class ERR_NOSUCHCHANNEL :		public std::exception { virtual const char* what() const throw(); };
		class ERR_NOSUCHNICK :			public std::exception { virtual const char* what() const throw(); };
		class ERR_NOTEXTTOSEND :		public std::exception { virtual const char* what() const throw(); };
		class ERR_NOTONCHANNEL :		public std::exception { virtual const char* what() const throw(); };
		class ERR_NOTOPLEVEL :			public std::exception { virtual const char* what() const throw(); };
		class ERR_TOOMANYTARGET :		public std::exception { virtual const char* what() const throw(); };
		class ERR_UMODEUNKNOWNFLAG :	public std::exception { virtual const char* what() const throw(); };
		class ERR_UNKNOWNMODE :			public std::exception { virtual const char* what() const throw(); };
		class ERR_USERONCHANNEL :		public std::exception { virtual const char* what() const throw(); };
		class ERR_USERSDONTMATCH :		public std::exception { virtual const char* what() const throw(); };
		class ERR_WILDTOPLEVEL :		public std::exception { virtual const char* what() const throw(); };

		class RPL_AWAY :				public std::exception { virtual const char* what() const throw(); };
		class RPL_BANLIST :				public std::exception { virtual const char* what() const throw(); };
		class RPL_CHANNELMODEIS :		public std::exception { virtual const char* what() const throw(); };
		class RPL_ENDOFBANLIST :		public std::exception { virtual const char* what() const throw(); };
		class RPL_INVITING :			public std::exception { virtual const char* what() const throw(); };
		class RPL_NOTOPIC :				public std::exception { virtual const char* what() const throw(); };
		class RPL_SUMMONING :			public std::exception { virtual const char* what() const throw(); };
		class RPL_TOPIC :				public std::exception { virtual const char* what() const throw(); };
		class RPL_UMODEIS :				public std::exception { virtual const char* what() const throw(); };
};

#endif