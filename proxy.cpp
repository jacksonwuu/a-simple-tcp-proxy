#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#ifdef __linux__
#include <pthread.h>
#include <cstring>
#include <algorithm>
#endif

const int BUFFER_SIZE = 4096;

void handleConnection(int clientSocket, const std::string& serverAddress, int serverPort, const sockaddr_in& clientAddress)
{
    std::cout << "Client connected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;

    // Create a connection to target server
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error creating server socket." << std::endl;

        close(clientSocket);
        return;
    }

    sockaddr_in serverSocketAddress;
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(serverPort);

    // Add address to serverSocketAddress.sin_addr
    if (inet_pton(AF_INET, serverAddress.c_str(), &(serverSocketAddress.sin_addr)) <= 0) {
        std::cerr << "Invalid server address." << std::endl;

        close(serverSocket);
        close(clientSocket);
        return;
    }

    if (connect(serverSocket, reinterpret_cast<sockaddr*>(&serverSocketAddress), sizeof(serverSocketAddress)) < 0) {
        std::cerr << "Error connecting to server." << std::endl;

        close(serverSocket);
        close(clientSocket);
        return;
    }

    // Core logic of proxy
    // Read the data from clientSocket and write the same data to serverSocket
    char buffer[BUFFER_SIZE];
    int bytesRead;
    while ((bytesRead = read(clientSocket, buffer, BUFFER_SIZE)) > 0) {
        write(serverSocket, buffer, bytesRead);
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(serverSocket);
    close(clientSocket);

    std::cout << "Client disconnected: " << inet_ntoa(clientAddress.sin_addr) << std::endl;
};

void startProxy(int proxyPort, const std::string& serverAddress, int serverPort)
{
    /*
    Start listening on proxyPort.
    */
    int proxySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxyPort < 0) {
        std::cerr << "Error creating proxy socket." << std::endl;
        return;
    }

    sockaddr_in proxyAddress;
    proxyAddress.sin_family = AF_INET;
    proxyAddress.sin_addr.s_addr = INADDR_ANY;
    proxyAddress.sin_port = htons(proxyPort);

    if (bind(proxySocket, reinterpret_cast<sockaddr*>(&proxyAddress), sizeof(proxyAddress)) < 0) {
        std::cerr << "Error binding proxy socket." << std::endl;
        return;
    }

    // The second parameter make this listen sock can hold at most 100 connections at the same time.
    if (listen(proxySocket, 100) < 0) {
        std::cerr << "Error listening on proxy socket." << std::endl;
        return;
    }

    std::cout << "Proxy listening on port " << proxyPort << std::endl;

    /*
    Accept connection and handle it.
    */

    std::vector<std::thread> threads;

    while (true) {
        sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        int clientSocket = accept(proxySocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
        if (clientSocket < 0) {
            std::cerr << "Error accepting client connection." << std::endl;
            continue;
        }

        std::thread clientThread(handleConnection, clientSocket, serverAddress, serverPort, clientAddress);
        threads.push_back(std::move(clientThread));

        // Remove finished threads from the vector
        threads.erase(std::remove_if(threads.begin(), threads.end(), [](const std::thread& t) {
            // Check if thread is joinable (i.e., finished)
            return !t.joinable();
        }),
            threads.end());

        std::cout << "There are " << threads.size() << "threads." << std::endl;
    }

    // Wait for all the client threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    close(proxySocket);
};

int main()
{
    // Start a server listen on port 12345
    // and proxy every tcp packet to localhost 8080
    int proxyPort = 12345;
    int serverPort = 8080;
    std::string serverAddress = "127.0.0.1";

    startProxy(proxyPort, serverAddress, serverPort);

    return 0;
};