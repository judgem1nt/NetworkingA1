
#include "networking.h"

using namespace std;



TCPsocket init(const char* host, Uint16 port)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, host, port);

    TCPsocket client = SDLNet_TCP_Open(&ip);

    return client;
}

void cleanup(TCPsocket& server)
{
    SDLNet_TCP_Close(server);
    SDLNet_Quit();
    SDL_Quit();
}


void networkLoop(TCPsocket& client, string username_, int totalscore_)
{
    char buffer[1024];  // Buffer for sending/receiving data
    bool clientLoop = true;

    while (clientLoop)
    {
        printf("Enter a message to send (or type 'quit' to exit, 'print' to get the scoreboard): ");
        fflush(stdout);

        // Read user input
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;  // Exit the loop on EOF or error
        }

        // Remove newline character at the end of the input
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        std::string userInput = buffer;

        // If the user types 'quit', exit the loop
        if (userInput == "quit") {
            break;
        }

        // If the user types 'print', request the scoreboard
        if (userInput == "print") {
            // Send "print" command to the server
            std::string printCommand = "print";
            if (client) {
                SDLNet_TCP_Send(client, printCommand.c_str(), printCommand.length() + 1);
            }

            // Wait for the server to send the scoreboard
            int received = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
            if (received > 0) {
                buffer[received - 1] = '\0';  // Null-terminate the received data
                printf("SCOREBOARD:\n%s\n", buffer);  // Print the received scoreboard
            }
            else {
                printf("Failed to receive scoreboard from the server.\n");
            }

            continue;  // Skip sending user input to the server for this loop iteration
        }

        // Otherwise, send the user's message (along with username and score) to the server
        std::string new_message = username_ + " " + to_string(totalscore_) + ": " + userInput;
        if (client) {
            SDLNet_TCP_Send(client, new_message.c_str(), new_message.length() + 1);
        }

        // Wait for a response from the server
        int received = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
        if (received > 0) {
            buffer[received - 1] = '\0';  // Null-terminate the received data
            if (strcmp(buffer, "exit") == 0) {
                printf("SERVER wants to close the connection.\n");
                break;  // Exit the loop if the server sends "exit"
            }
            else {
                printf("SERVER: %s\n", buffer);  // Print the server's response
            }
        }
    }
}
