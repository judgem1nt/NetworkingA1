#include <SDL.h>
#include <SDL_net.h>
#include <SDL_pixels.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "networking.h"


using namespace std;

const float WINDOW_WIDTH = 800;
const float WINDOW_HEIGHT = 600;
const int PLAYER_SIZE = 50;
const int INVADER_SIZE = 30;
const int BULLET_SIZE = 10;
const int INVADER_SPEED = 2;
const int INVADER_MOVE_DOWN = 10;


std::string playerScore;

std::string username;
char input[6];

int totalscore = 0;

int score;
int totalInvaders = 0;

bool gameOver = false;
bool show_score = true;
bool finished_data_upload = false;

std::vector<std::string> scoreLines;


// Get the starting time
Uint32 startTime = SDL_GetTicks();
Uint32 currentTime;
Uint32 elapsedTime;

struct Entity {
    float x, y, width, height;
};


void drawRect(SDL_Renderer* renderer, const Entity& entity, SDL_Color color) {
    SDL_Rect rect = {(int)entity.x, (int)entity.y, (int)entity.width, (int)entity.height};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

bool checkCollision(const Entity& a, const Entity& b) {
    return !(a.x + a.width < b.x || a.x > b.x + b.width || a.y + a.height < b.y || a.y > b.y + b.height);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }
    TCPsocket client = init("127.0.0.1", 8080);

    SDL_Window* window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font
    TTF_Font *font = TTF_OpenFont("Pixelon.ttf", 24);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return -1;
    }

    SDL_Event event;
    bool running = true;
    playerScore = "0000";
    score = 0;

     // Set text color and create surface and texture
    SDL_Color textColor = {255, 255, 255, 255};  // White color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "SCORE", textColor);
    SDL_Surface *lineSurface = TTF_RenderText_Solid(font, "-----", textColor);
    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, playerScore.c_str(), textColor);
    SDL_Surface *timeSurface = TTF_RenderText_Solid(font, "placeholder", textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Texture *lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);
    SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(lineSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_FreeSurface(timeSurface);

    SDL_Surface* wonSurface = nullptr;
    SDL_Texture* wonTexture = nullptr;
    SDL_Rect wonRect = {WINDOW_WIDTH / 2 - 100, 
                        WINDOW_HEIGHT / 2 - 25, 200, 50}; // Centered position

    SDL_Rect finalScoreRect = {WINDOW_WIDTH / 2 - 100, 
                              WINDOW_HEIGHT / 2 + 25,200, 50}; // Centered position
    SDL_Rect finalScoreSheetRect = {WINDOW_WIDTH / 2 - 100, 
                                   WINDOW_HEIGHT / 2 + 100,200, 100}; // Centered position
    // Create the "YOU WON" surface and texture
    wonSurface = TTF_RenderText_Solid(font, "YOU WON", textColor);
    wonTexture = SDL_CreateTextureFromSurface(renderer, wonSurface);
    SDL_FreeSurface(wonSurface);
    
    // Define where the text should appear
    SDL_Rect textRect = {static_cast<int>(WINDOW_WIDTH * 0.70), 50, 200, 50};  // Position and size
    SDL_Rect timeRect = {static_cast<int>(WINDOW_WIDTH * 0.20), 50, 200, 50};  // Position and size
    SDL_Rect lineRect = {static_cast<int>(WINDOW_WIDTH * 0.70), 80, 200, 50};  // Position and size
    SDL_Rect scoreRect = {static_cast<int>(WINDOW_WIDTH * 0.70), 110, 200, 50};  // Position and size


    Entity player = {WINDOW_WIDTH / 2 - PLAYER_SIZE / 2, WINDOW_HEIGHT - 60, PLAYER_SIZE, PLAYER_SIZE};
    std::vector<Entity> invaders;
    std::vector<Entity> bullets;

    bool moveDown = false;
    int moveDirection = 1; // 1 for right, -1 for left

    

    // Initialize invaders
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
       
            Entity invader;
            invader.x = i * (INVADER_SIZE + 10) +50;
            invader.y = j * (INVADER_SIZE + 10) +50;
            invader.width = INVADER_SIZE;
            invader.height = INVADER_SIZE;

            invaders.push_back(invader);

            //invaders.push_back({i * (INVADER_SIZE + 10) + 50, j * (INVADER_SIZE + 10) + 50, INVADER_SIZE, INVADER_SIZE});
            totalInvaders++;
        }
    }

    while (running) {
        if(!gameOver){
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_LEFT) {
                        player.x -= 10;
                    } else if (event.key.keysym.sym == SDLK_RIGHT) {
                        player.x += 10;
                    } else if (event.key.keysym.sym == SDLK_SPACE) {
                        bullets.push_back({player.x + PLAYER_SIZE / 2 - BULLET_SIZE / 2, player.y - BULLET_SIZE, BULLET_SIZE, BULLET_SIZE});
                    }
                }
            } 
        }
        //While game is running

        // Update bullets
        for (auto& bullet : bullets) {
            bullet.y -= 10;
        }

        // Move invaders
        bool edgeReached = false;
        for (const auto& invader : invaders) {
            if (invader.x <= 0 || invader.x + INVADER_SIZE >= WINDOW_WIDTH) {
                edgeReached = true;
                break;
            }
        }

        if (edgeReached) {
            moveDirection *= -1; // Change direction
            moveDown = true; // Move down
        }

        for (auto& invader : invaders) {
            invader.x += INVADER_SPEED * moveDirection;
        }

        if (moveDown) {
            for (auto& invader : invaders) {
                invader.y += INVADER_MOVE_DOWN;
            }
            moveDown = false; // Reset move down flag
        }


        // Check collisions
        // Remove bullets that hit invaders
        for (auto it = bullets.begin(); it != bullets.end(); ) {
            bool hit = false;
            for (auto invIt = invaders.begin(); invIt != invaders.end(); ) {
                if (checkCollision(*it, *invIt)) {
                    invIt = invaders.erase(invIt); // Remove invader
                    totalInvaders--;
                    hit = true;
                    score += 1000;

                    // Destroy old score texture
                    SDL_DestroyTexture(scoreTexture);

                    // Create new score texture
                    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), textColor);
                    scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                    SDL_FreeSurface(scoreSurface);

                    //playerScore = std::to_string(score);
                } else {
                    ++invIt;
                }
            }
            if (hit) {
                it = bullets.erase(it); // Remove bullet
            } else {
                ++it;
            }
        }
        
        if(totalInvaders == 0)
            gameOver = true;

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw player
        drawRect(renderer, player, {0, 255, 0, 255});

        // Draw invaders
        for (const auto& invader : invaders) {
            drawRect(renderer, invader, {255, 0, 0, 255});
        }

        // Draw bullets
        for (const auto& bullet : bullets) {
            drawRect(renderer, bullet, {255, 255, 0, 255});
        }

        // Render text
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_RenderCopy(renderer, lineTexture, NULL, &lineRect);
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
         
        if (!gameOver){

            // Calculate elapsed time
            currentTime = SDL_GetTicks();
            elapsedTime = currentTime - startTime;
            //DEBUG
            // Print the elapsed time in seconds
            // printf("Running time: %u ms\n", elapsedTime);
            // printf("Total Number of Invaders: %d\n", totalInvaders);
        }
        else {
            // Draw black rectangle
            drawRect(renderer, {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, {0, 0, 0, 255});
            if(SDL_GetTicks() - currentTime > 10000)
                show_score = false;

            if(show_score)
            {
                // Render the "YOU WON" texture
                SDL_RenderCopy(renderer, wonTexture, NULL, &wonRect);
                
                    // Destroy old score texture
                    SDL_DestroyTexture(scoreTexture);

                    // Create new score texture
                    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, std::to_string(score - elapsedTime).c_str(), textColor);
                    scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                    SDL_FreeSurface(scoreSurface);

                SDL_RenderCopy(renderer, scoreTexture, 
                    NULL, &finalScoreRect);

                //SDL_Delay(3000); // Show for 3 seconds            
            }

            if(!show_score)
            {

                // Load a new font with a smaller size (e.g., 18)
                TTF_Font *scoreSheetFont = TTF_OpenFont("Pixelon.ttf", 10);  // Adjust the path and size as needed
                if (!scoreSheetFont) {
                    printf("Failed to load smaller font! TTF_Error: %s\n", TTF_GetError());
                    return -1;
                }
                  // Load a font for the header text
                TTF_Font *headerFont = TTF_OpenFont("Pixelon.ttf", 24);  // A slightly larger font for the header
                if (!headerFont) {
                    printf("Failed to load header font! TTF_Error: %s\n", TTF_GetError());
                    return -1;
                }

                // Prepare the heading text
                const char* headingText = "Scoreboard";  // Text for the heading
                SDL_Color headingColor = {255, 255, 255};  // White color for heading
                SDL_Surface* headingSurface = TTF_RenderText_Solid(headerFont, headingText, headingColor);
                if (!headingSurface) {
                    printf("Failed to create heading surface! TTF_Error: %s\n", TTF_GetError());
                    return -1;
                }

                // Create a texture from the heading surface
                SDL_Texture* headingTexture = SDL_CreateTextureFromSurface(renderer, headingSurface);
                SDL_FreeSurface(headingSurface);

                // Get the dimensions of the heading texture to center it
                int headingWidth, headingHeight;
                SDL_QueryTexture(headingTexture, NULL, NULL, &headingWidth, &headingHeight);

                // Calculate the x position to center the heading
                int windowWidth = 800;  // Replace with your actual window width
                int headingX = (windowWidth - headingWidth) / 2;  // Centered horizontally
                int headingY = 50;  // Y position for the heading

                // Render the heading
                SDL_Rect headingRect = {headingX, headingY, headingWidth, headingHeight};
                SDL_RenderCopy(renderer, headingTexture, NULL, &headingRect);

                // Set initial offsets for the first score line
                int xOffset = 50;    // Initial horizontal position
                int yOffset = headingY + headingHeight + 20;  // Position below the heading

                int maxNamesPerColumn = 15;  // Number of names per column before shifting
                int namesPrinted = 0;        // Track how many names have been printed

                // Loop through each line in scoreLines and render them
                for (const auto& line : scoreLines) {
                    // Render each line as a new surface
                    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, line.c_str(), textColor);
                    if (!scoreSurface) {
                        printf("Failed to create score surface! TTF_Error: %s\n", TTF_GetError());
                        continue;
                    }

                    // Create a texture from the surface
                    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                    SDL_FreeSurface(scoreSurface);

                    // Define the rectangle for positioning each score line
                    SDL_Rect scoreRect = {xOffset, yOffset, 200, 30};  // Adjust width/height as needed
                    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

                    // Destroy the texture after rendering
                    SDL_DestroyTexture(scoreTexture);

                    // Move yOffset down for the next line
                    yOffset += 40;  // Vertical spacing between lines

                    // Increase the printed names count
                    namesPrinted++;

                    // Check if we've printed 10 names in this column
                    if (namesPrinted >= maxNamesPerColumn) {
                        // Move to the next column
                        xOffset += 225;  // Shift right for the next column
                        yOffset = 80;   // Reset yOffset to the top

                        // Reset namesPrinted for the next column
                        namesPrinted = 0;
                    }
                }

                // Display the screen after rendering all score lines
                SDL_RenderPresent(renderer);

                totalscore = score - elapsedTime;
                

                printf("YOU WON!!!\n");

                cout << "Enter a username up to 5 letters: "; // get username after game because game score runs off runtime length
                cin.get(input, 6);
                username = input;

                cout << "" << username << " " << totalscore << "\n";

                networkLoop(client,username,totalscore);

                cleanup(client);

                SDL_Delay(1000);  // Show the score for 1 second

                running = false;

            }
        }
            
        
        // Destroy old score texture
        // // Destroy old time texture if it exists
        if (timeTexture) {
            SDL_DestroyTexture(timeTexture);
        }

        // Create new score texture
        SDL_Surface *timeSurface = TTF_RenderText_Solid(font,std::to_string(elapsedTime/100).c_str(), textColor);
        timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
        SDL_FreeSurface(timeSurface);

        // Now render the time texture
        SDL_RenderCopy(renderer, timeTexture, NULL, &timeRect);


        // Present
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Approximately 60 FPS
    }

    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(lineTexture);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}


