#include <iostream>
#include <stdexcept>
#include "Server.hpp"
#include "User.hpp"
#include "tools.hpp"

void run_server(int port, std::string servPass)
{
    Server myServ;
    const int MAX_CLIENTS = 5;
    const int BUFFER_SIZE = 1024;
    int serverSocket, maxSocket, activity, newSocket;
    sockaddr_in address;
    fd_set readfds;

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket." << std::endl;
        return ;
    }

    // Prepare the sockaddr_in structure
    address.sin_family = AF_INET; //Set adress for IPV4
    address.sin_addr.s_addr = INADDR_ANY; //Set the IP address to bind the socket to
    address.sin_port = htons(port); // Use our desired port number

    // Bind the socket to a specific address and port
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed." << std::endl;
        return ;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) < 0)
    {
        std::cerr << "Listen failed." << std::endl;
        return ;
    }

    std::cout << "Server started. Waiting for connections..." << std::endl;

    std::vector<int> clientSockets(MAX_CLIENTS, 0);

    while (true)
    {
        FD_ZERO(&readfds);

        // Add the server socket to the set
        FD_SET(serverSocket, &readfds);
        maxSocket = serverSocket;

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
        if (FD_ISSET(serverSocket, &readfds))
        {
            if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&address)) < 0)
            {
                std::cerr << "Accept error." << std::endl;
                return ;
            }

            std::cout << "New connection, socket fd: " << newSocket << ", IP: " << inet_ntoa(address.sin_addr) << ", Port: " << ntohs(address.sin_port) << std::endl;

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
            if (clientPassword == servPass) {
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
                    myServ.checkCommand(sd, buffer);
                }
            }
        }
    }
}

int main(int argc, char** argv)
{    
    try
    {
        if (argc == 3 && check_port(argv[1]) && check_password(argv[2]))
            run_server(atoi(argv[1]), argv[2]);
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