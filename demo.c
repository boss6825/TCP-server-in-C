#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h> //for multi threading in order to broadcast messages to all clients

#define max_clients 10
#define buffer_size 1024

// isme we hold the connected clients socket and an id
typedef struct {
    struct sock_addr_in addr; //might not be used , this is only for logging, comes from <netinet.h> ,stores address (IP, port)
    int uid;
    int sock_fd; //when server accepts connection, accept() returns a int called socket file descriptor. number (sockfd) represents a unique communication endpoint between the server and that particular client. It’s how the operating system identifies which client you’re talking to.
} client_sol;  // typedefined struct - can be called just using clie\nt_sol
 
client_sol *clients[max_clients]; //arr of ptrs to connected clientsu 
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; // protects clients[]
// client mutexx is a variable that can be used to get lock on array

//helper functions to add client in array , remove from arr upon disconnecting 

//add client to array
void add_client(client_sol *cl){
    pthread_mutex_lock(&clients_mutex); //attain mutex before modification(locked)
    for(int i = 0; i < max_clients; i++){
        if(!clients[i]){ 
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

//remove client usinf uid
void remove_client(int uid){
    pthread_mutex_lock(&clients_mutex);
    for(int i =0; i<max_clients; i++){
        if(clients[i]){
            if(clients[i]->uid == uid){
                clients[i] == NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *message,int sender_uid){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < max_clients; i++){
        if(clients[i]){
            if(clients[i]->uid != sender_uid){
                //write is a posix syscall, writes from message to file descriptor upto length given by strlen
                write(clients[i]->sock_fd, message, strlen(message));
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
}

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
