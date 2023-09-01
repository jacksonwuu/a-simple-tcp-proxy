#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main() {
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Failed to create socket" << endl;
        return 1;
    }

    // Set up server address and port
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);  // Replace with your desired port
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        cerr << "Invalid address/Address not supported" << endl;
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Connection failed" << endl;
        return 1;
    }

    // Send data to the server
    const char* message = "Hello, server!";
    if (send(clientSocket, message, strlen(message), 0) < 0) {
        cerr << "Failed to send data" << endl;
        return 1;
    }

    // Receive data from the server
    char buffer[1024] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead < 0) {
        cerr << "Failed to receive data" << endl;
        return 1;
    }

    // Print the received data
    cout << "Server response: " << buffer << endl;

    // Close the socket
    close(clientSocket);

    return 0;
}