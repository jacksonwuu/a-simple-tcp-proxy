#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <vector>

#ifdef __linux__
#include <pthread.h>
#endif


void handleClient(int clientSocket)
{
    char buffer[1024];

    // Receive and print data from the client
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            break;
        }
        std::cout << "Received data from client " << clientSocket << ": " << buffer << std::endl;
    }

    // Close the client socket
    close(clientSocket);
}

int main()
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[1024];

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Set up the server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    // Start listening for incoming connections
    listen(serverSocket, 5);
    std::cout << "Server listening on port 8080" << std::endl;

    std::vector<std::thread> threads;

    // Accept incoming connections and spawn threads to handle clients
    while (true) {
        socklen_t clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        // Spawn a new thread to handle the client
        std::thread clientThread(handleClient, clientSocket);
        threads.push_back(std::move(clientThread));
    }

    // Wait for all the client threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}