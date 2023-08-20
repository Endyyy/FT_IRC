#include <iostream>
#include <stdexcept>
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "tools.hpp"

void ircserv(int port, std::string serverPassword)
{
    Server myServ(port, serverPassword);

    // int maxSocket, activity, newSocket;
    // fd_set readfds;

    // Bind the socket to the specific address
    myServ.bind_socket_to_address();

    // Listen for incoming connections
    myServ.start_listening();

    myServ.run();

    // std::vector<int> clientSockets(MAX_CLIENTS, 0);//pas de max de clients

    // while (true)
    // {
    //     FD_ZERO(&readfds);

    //     // Add the server socket to the set
    //     FD_SET(myServ.get_serverSocket(), &readfds);
    //     maxSocket = myServ.get_serverSocket();

    //     // Add child sockets to the set
    //     for (int i = 0; i < MAX_CLIENTS; ++i)
    //     {
    //         int sd = clientSockets[i];
    //         if (sd > 0)
    //             FD_SET(sd, &readfds);
    //         if (sd > maxSocket)
    //             maxSocket = sd;
    //     }

    //     // Wait for activity on any of the sockets
    //     activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
    //     if (activity < 0)
    //     {
    //         std::cerr << "Select error." << std::endl;
    //         return ;
    //     }

    //     // New incoming connection
    //     if (FD_ISSET(myServ.get_serverSocket(), &readfds))
    //     {
    //         socklen_t addrLength = sizeof(address);
    //         if ((newSocket = accept(myServ.get_serverSocket(), (struct sockaddr *)&address, &addrLength)) < 0)
    //         {
    //             std::cerr << "Accept error." << std::endl;
    //             return ;
    //         }

    //         std::cout << "New connection, socket fd: " << newSocket << ", IP: " << inet_ntoa(address.sin_addr) << ", Port: " << ntohs(address.sin_port) << std::endl;

    //         // Ask the client for the password
    //         std::string clientPassword;
    //         std::string clientUsername;
    //         std::string clientNickname;
    //         char passwordBuffer[BUFFER_SIZE] = {0};
    //         char usernameBuffer[BUFFER_SIZE] = {0};
    //         char nicknameBuffer[BUFFER_SIZE] = {0};
    //         send(newSocket, "PASS <password>\n", strlen("PASS <password>\n"), 0);
    //         int valread = read(newSocket, passwordBuffer, BUFFER_SIZE);
    //         if (valread > 0)
    //         {
    //             clientPassword = passwordBuffer;
    //             clientPassword.erase(std::remove(clientPassword.begin(), clientPassword.end(), '\n'), clientPassword.end()); // Remove newline character
    //         } else
    //         {
    //             // Error reading password
    //             std::cerr << "Error reading password from client." << std::endl;
    //             close(newSocket);
    //             continue;
    //         }
    //         // Check if the entered password matches the server's password
    //         if (clientPassword == servPass) {
    //             // Password match, accept the connection
    //             std::cout << "Client authenticated. Connection accepted." << std::endl;
    //             // Add new socket to array of client sockets
    //             for (int i = 0; i < MAX_CLIENTS; ++i)
    //             {
    //                 if (clientSockets[i] == 0)
    //                 {
    //                     clientSockets[i] = newSocket;
    //                     break;
    //                 }
    //             }
    //             send(newSocket, "NICK <nickname>\n", strlen("NICK <nickname>\n"), 0);
    //             valread = read(newSocket, nicknameBuffer, BUFFER_SIZE);
    //             if (valread > 0)
    //             {
    //                 clientNickname = nicknameBuffer;
    //                 clientNickname.erase(std::remove(clientNickname.begin(), clientNickname.end(), '\n'), clientNickname.end()); // Remove newline character
    //             }
    //             else
    //             {
    //                 // Error reading nickname
    //                 std::cerr << "Error reading nickname from client." << std::endl;
    //                 close(newSocket);
    //                 continue;
    //             }
    //             send(newSocket, "USER <username>\n", strlen("USER <username>\n"), 0);
    //             valread = read(newSocket, usernameBuffer, BUFFER_SIZE);
    //             if (valread > 0)
    //             {
    //                 clientUsername = usernameBuffer;
    //                 clientUsername.erase(std::remove(clientUsername.begin(), clientUsername.end(), '\n'), clientUsername.end()); // Remove newline character
    //             }
    //             else
    //             {
    //                 // Error reading username
    //                 std::cerr << "Error reading username from client." << std::endl;
    //                 close(newSocket);
    //                 continue;
    //             }
    //         }
    //         else
    //         {
    //             // Password doesn't match, reject the connection
    //             std::cout << "Client authentication failed. Connection rejected." << std::endl;
    //             close(newSocket);
    //             return ;
    //         }

    //         // Add new socket to array of client sockets
    //         for (int i = 0; i < MAX_CLIENTS; ++i)
    //         {
    //             if (clientSockets[i] == 0)
    //             {
    //                 clientSockets[i] = newSocket;
    //                 break;
    //             }
    //         }
    //     }

    //     // Handle data from clients
    //     for (int i = 0; i < MAX_CLIENTS; ++i)
    //     {
    //         int sd = clientSockets[i];
    //         if (FD_ISSET(sd, &readfds))
    //         {
    //             char buffer[BUFFER_SIZE] = {0};
    //             int valread = read(sd, buffer, BUFFER_SIZE);
    //             if (valread == 0)
    //             {
    //                 // Client disconnected
    //                 std::cout << "Client disconnected, socket fd: " << sd << std::endl;
    //                 close(sd);
    //                 clientSockets[i] = 0;
    //             }
    //             else if (valread == -1)
    //             {
    //                 // Error or abrupt disconnection
    //                 std::cerr << "Error reading data from client, socket fd: " << sd << std::endl;
    //                 close(sd);
    //                 clientSockets[i] = 0;
    //             }
    //             else {
    //                 // Handle data from the client
    //                 myServ.checkCommand(sd, buffer);
    //             }
    //         }
    //     }
    // }
}

int main(int argc, char** argv)
{    
    try
    {
    if (argc == 3 && check_port(argv[1]) && check_password(argv[2]))
        ircserv(atoi(argv[1]), argv[2]);
    else
        throw (std::invalid_argument("Error : usage ./ircserv <PORT> <PASSWORD>"));
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
		return (1);
    }
    return (0);
}