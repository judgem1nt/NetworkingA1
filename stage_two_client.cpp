#include <SDL.h>
#include <SDL_net.h>
#include <stdio.h>
#include <string.h>

TCPsocket init(const char* host, Uint16 port)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, host, port);

    TCPsocket client = SDLNet_TCP_Open(&ip);

    return client;
}

void cleanup(TCPsocket &server)
{
    SDLNet_TCP_Close(server);
    SDLNet_Quit();
    SDL_Quit();
}


void networkLoop(TCPsocket& client)
{
    //char buffer[1024];
    char buffer[1024];

    bool clientLoop = true;

    while(clientLoop)
    {
        printf("Enter a message to send (or type 'quit' to exit): ");
        fflush(stdout);

        //SEND Data
        //####################################################################
        //####################################################################
            // Read user input
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                break;  // Exit the loop on EOF or error
            }

            // Remove newline character at the end of the input
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }

            SDLNet_TCP_Send(client, buffer, sizeof(buffer));
            
            if (strcmp(buffer, "quit") == 0) {
                break;  // Exit the loop if the user enters 'quit'
            }
        //END SEND Data
        //####################################################################
        //####################################################################

        //Recieve DATA
        //####################################################################
        //####################################################################
        int received = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
        if (received > 0) {

            buffer[received - 1] = '\0';  // Null-terminate the received data
		    if(strcmp(buffer, "exit") == 0)
            {
                printf("SERVER WAnts to close\n");
                break;  // Exit the loop if the user enters 'quit'

            }
            else
                printf("SERVER: %s\n", buffer);
        }
        //####################################################################
        //####################################################################
    }
}

int main(int argc, char* argv[]) {

    TCPsocket client = init("127.0.0.1", 8080);
    networkLoop(client);

    cleanup(client);
return 0;
}
