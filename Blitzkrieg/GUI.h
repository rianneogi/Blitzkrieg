#pragma once
#include "Interface.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <string>
class GUI
{
public:
	GUI(void);
	~GUI(void);

	Interface inter;

	int selectedsquare;
	string text;
	string input;
	string output;
	bool quit;
	int isover;
	int play;
	bool autoplay;
	int time;
	bool flipboard;

	int start();
	bool init();
	bool load_files();
	void clean_up();
	void draw();
	bool handle_events();
	void do_actions();
};

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
SDL_Surface* load_image( std::string filename );
unsigned int getImageNumber(int Piece,int Square);

