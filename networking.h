#pragma once

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



TCPsocket init(const char* host, Uint16 port);

void cleanup(TCPsocket& server);

void networkLoop(TCPsocket& client, std::string username_, int totalscore_);
