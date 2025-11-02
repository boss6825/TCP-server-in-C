#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
// comments are not AI generated slope, were written to undertand the flow code and concepts used in this
int main()
{
    // step 1
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); 
        // socket() is a system call that creates a new socket.
        // AF_INET -> Stands for Address Family Internet. This means we are using IPv4.
        // SOCK_STREAM -> This specifies the type of socket. SOCK_STREAM means TCP (reliable, connection-oriented).                                               
        // 0 -> Protocol. 0 means the OS chooses the appropriate protocol (TCP for SOCK_STREAM).                                                                                               
        // socket() returns a file descriptor (integer) which represents the socket   
        
        
    // step 2 bind the socket to an ip address and a port
    struct sockaddr_in address;
    address.sin_family = AF_INET;

       // convert string IP to binary form 
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    address.sin_port = htons(8080); // Port 8080

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));

    // listening to the incoming req
    listen(server_fd, 3); // Max 3 waiting connections
    printf("Server is running and listening on port 8080...\n");

    while (1)
    { // Step 4: Accept an incoming connection (Three-way handshake happens here)
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        if (new_socket < 0)
        {
            perror("Failed to accept connection");
            continue;
        }

        printf("Connection accepted.\n");
        // we got connected, below is data transfer logic
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);         // Read data sent by the client
        printf("Client message: %s\n", buffer); // Print client's message

        // now server will send response to the client
        char *response = "Hello from the server!";
        write(new_socket, response, strlen(response)); // Send data to the client

        // data got transferred, now close connection by a 4 way handshake
        close(new_socket); // Close the client connection
        printf("Connection closed.\n");
    }

    // Close the listening socket (this part will never be reached in the current loop)
    close(server_fd);
    return 0;
}
