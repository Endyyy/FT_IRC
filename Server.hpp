#ifndef SERVER_HPP
# define SERVER_HPP

# include "tools.hpp"
# include "Channel.hpp"
# include "User.hpp"
# include <iostream>
# include <stdexcept>
# include <cstring>
# include <string>
# include <cstdlib>
# include <cstdio>
# include <csignal>
# include <unistd.h>
# include <vector>
# include <map>
# include <algorithm>
# include <arpa/inet.h>
# include <sstream>
# include "User.hpp"
# include "Channel.hpp"

# define MAX_CLIENTS 100
# define BUFFER_SIZE 1024

class Server
{
	private:
		int const			_port;
		static bool			_active;
		std::string const	_serverPassword;
		type_sock const		_serverSocket;
		sockaddr_in			_address;
		fd_set				_readfds;
		type_sock			_topSocket;

		std::map<type_sock, User*>		_clients;
		std::map<std::string, Channel*>	_channels;
		std::vector<type_sock>			_death_note;

		Server();
		Server(Server const& source);
		Server&	operator=(Server const& source);

	public:
		Server(int port, std::string serverPassword);
		~Server();

		void	bind_socket_to_address();
		void	start_listening();
		void	run();

		void			reset_fd_set();
		void			waiting_for_activity();
		bool			check_activity(type_sock socket);
		void			complete_registration();
		type_sock		get_incoming_socket();
		void			add_new_user(type_sock userSocket);
		std::string		recv_from_user(type_sock userSocket);
		std::string 	get_clientDatas(type_sock socket);
		void			erase_one_user(type_sock userSocket);
		void			erase_death_note();
		void			erase_all_users();
		void			set_address();
		void			ask_for_login_credentials(std::string arg, type_sock client_socket, int lvl);
		static void		ctrlC_behaviour(int signal);


		bool	cmdPass(std::string arg);
		bool	cmdNick(std::string arg, type_sock client_socket);
		bool	cmdUser(std::string arg, type_sock client_socket);
		void	cmdJoin(std::string arg, type_sock client_socket);
		void	cmdKick(std::string arg, type_sock client_socket);
		void	cmdInvite(std::string arg, type_sock client_socket);
		void	cmdTopic(std::string arg, type_sock client_socket);
		void	cmdMode(std::string arg, type_sock client_socket);
		void	cmdPrivMsg(std::string arg, type_sock client_socket);
		void	checkCommand(std::string arg, type_sock client_socket);
		int		findSocketFromNickname(std::string target);

		//ERROR_MSGS

		class ERR_INVALIDSOCKET :		public std::exception { virtual const char* what() const throw(); };
		class ERR_BINDFAILURE :			public std::exception { virtual const char* what() const throw(); };
		class ERR_LISTENINGFAILURE :	public std::exception { virtual const char* what() const throw(); };
		class ERR_SELECTFAILURE :		public std::exception { virtual const char* what() const throw(); };
		class ERR_ACCEPTFAILURE :		public std::exception { virtual const char* what() const throw(); };

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
