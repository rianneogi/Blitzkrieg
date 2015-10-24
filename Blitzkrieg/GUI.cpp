#include "GUI.h"
#include "SDL_image.h"
#include <string>

//The attributes of the screen
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

const int SQUARE_WIDTH = 64;
const int SQUARE_HEIGHT = 64;

const int MAX_IMAGES = 26;

const int MIN_TIME = 5;
const int MAX_TIME = 60;

string ImageStrings[MAX_IMAGES] = {"Graphics\\w.gif","Graphics\\b.gif",
								   "Graphics\\wpw.gif","Graphics\\wpb.gif",
                                   "Graphics\\wnw.gif","Graphics\\wnb.gif",
                                   "Graphics\\wbw.gif","Graphics\\wbb.gif",
                                   "Graphics\\wrw.gif","Graphics\\wrb.gif",
                                   "Graphics\\wqw.gif","Graphics\\wqb.gif",
                                   "Graphics\\wkw.gif","Graphics\\wkb.gif",
                                   "Graphics\\bpw.gif","Graphics\\bpb.gif",
                                   "Graphics\\bnw.gif","Graphics\\bnb.gif",
                                   "Graphics\\bbw.gif","Graphics\\bbb.gif",
                                   "Graphics\\brw.gif","Graphics\\brb.gif",
                                   "Graphics\\bqw.gif","Graphics\\bqb.gif",
                                   "Graphics\\bkw.gif","Graphics\\bkb.gif",};

SDL_Event event;

//The surfaces that will be used
SDL_Surface* squares[64];
SDL_Surface* background = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* images[MAX_IMAGES];
SDL_Surface* message = NULL;
SDL_Surface* inputscreen = NULL;
SDL_Surface* outputscreen = NULL;
SDL_Surface* thinkbutton = NULL;
SDL_Surface* autoplaybutton = NULL;
SDL_Surface* undobutton = NULL;
SDL_Surface* depthbutton = NULL;
SDL_Surface* flipbutton = NULL;

//The font that's going to be used
TTF_Font* font = NULL;

//The color of the font
SDL_Color textColor = { 255, 255, 255 };


GUI::GUI(void)
{
	inter = Interface();
	selectedsquare = -1;
	text = "White to move!";
	input = "";
	output = "output";
	quit = false;
	play = 0;
	isover = 0;
	autoplay = true;
	time = 15;
}

GUI::~GUI(void)
{
}

int GUI::start()
{
	//i.start();
    if(init()==false)
	{
		return 1;
	}

	if(load_files()==false)
	{
		return 2;
	}

	draw();
    
    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 3;    
    }

	//While the user hasn't quit
    while( quit == false )
    {
		//While there's an event to handle
        handle_events();
		if(quit)
		{
			break;
		}

		draw();
		if(SDL_Flip( screen ) == -1 )
		{
			return 4;    
		}

		do_actions();
    }

	clean_up();

	return 0;
}

bool GUI::handle_events()
{
	while( SDL_PollEvent( &event ) )
    {
		//If a key was pressed
		if( event.type == SDL_KEYDOWN )
		{
			//Keep a copy of the current version of the string
			//std::string temp = input;
        
			//If the string less than maximum size
			if(input.length() <= 30)
			{
				//If the key is a space
				if( event.key.keysym.sym == SDLK_SPACE )
				{
					//Append the character
					input += char(event.key.keysym.sym);
				}
				//If the key is a number
				else if( ( event.key.keysym.sym >= SDLK_0 ) && ( event.key.keysym.sym <= SDLK_9 ))
				{
					//Append the character
					input += char(event.key.keysym.sym);
				}
				//If the key is a letter
				else if( ( event.key.keysym.sym >= SDLK_a ) && ( event.key.keysym.sym <= SDLK_z ) )
				{
					//Append the character
					input += char(event.key.keysym.sym);
				}
			}
			//If backspace was pressed and the string isn't blank
			if( ( event.key.keysym.sym == SDLK_BACKSPACE ) && ( input.length() != 0 ) )
			{
				//Remove a character from the end
				input.erase( input.length() - 1 );
			}
			if(event.key.keysym.sym == SDLK_RETURN)
			{
				InStream << input;
				inter.do_actions(input);
				input = "";
			}
		}

		//If a mouse button was pressed
		if( event.type == SDL_MOUSEBUTTONDOWN )
		{
			//If the left mouse button was pressed
			if( event.button.button == SDL_BUTTON_LEFT )
			{
				//Get the mouse offsets
				int x = event.button.x;
				int y = event.button.y;
        
				//If the mouse is over the squares
				if( ( x >= 0 ) && ( x < SQUARE_WIDTH*8 ) && ( y >= 0 ) && ( y < SQUARE_HEIGHT*8 ) && play==0 && isover==0)
				{
					int target = getSquare(x/SQUARE_WIDTH,7 - y/SQUARE_HEIGHT);
					if(flipboard==true)
					{
						target = getSquare(7 - x/SQUARE_WIDTH,y/SQUARE_HEIGHT);
					}
					if(inter.board.pos.Squares[target] == SQUARE_EMPTY 
						|| getSquare2Color(inter.board.pos.Squares[target])==getOpponent(inter.board.pos.turn))
					{
						if(selectedsquare != -1)
						{
							//make move
							int from = selectedsquare;
							int to = target;
							inter.board.generateMoves();
							for(int i = 0;i<inter.board.moves.size();i++)
							{
								int special = inter.board.moves.at(i).getSpecial();
								if(from == inter.board.moves.at(i).getFrom() && to == inter.board.moves.at(i).getTo()
									&& special != PIECE_BISHOP && special != PIECE_KNIGHT && special != PIECE_ROOK)
								{
									inter.board.makeMove(i);
			                        inter.e1.makeMove(i);
									selectedsquare = -1;
									int status = inter.board.getGameStatus();
									if(status==STATUS_WHITEMATED) //post the game end status
									{
										text = "White is checkmated!";
										isover = 1;
									}
									else if(status==STATUS_BLACKMATED)
									{
										text = "Black is checkmated!";
										isover = 1;
									}
									else if(status==STATUS_STALEMATE)
									{
										text = "Its a stalemate!";
										isover = 1;
									}
									else if(status==STATUS_INSUFFICIENTMAT)
									{
										text = "Its a draw due to insufficient material!";
										isover = 1;
									}
									else
									{
										if(autoplay==true)
										{
											play = 1;
											text = "Computer is thinking...";
										}
									}
								}
							}
						}
					}
					else
					{
						selectedsquare = target;
					}
				}
				//think button
				if((x > SQUARE_WIDTH*8 + 10) && (x < SQUARE_WIDTH*8 + 210) && (y > 100) && (y < 150) && play==0 && isover==0)
				{
					play = 1;
					text = "Computer is thinking...";
				}
				//autoplay button
				if((x > SQUARE_WIDTH*8 + 10) && (x < SQUARE_WIDTH*8 + 210) && (y > 150) && (y < 200))
				{
					autoplay = !autoplay;
				}
				//undo button
				if((x > SQUARE_WIDTH*8 + 10) && (x < SQUARE_WIDTH*8 + 210) && (y > 200) && (y < 250))
				{
					inter.board.unmakeMove();
					inter.e1 = Engine(COLOR_WHITE,inter.board.pos);
					isover = 0;
					if(inter.board.pos.turn==COLOR_WHITE)
					{
						text = "White to move!";
					}
					else
					{
						text = "Black to move!";
					}
				}
				//time button
				if((x > SQUARE_WIDTH*8 + 10) && (x < SQUARE_WIDTH*8 + 210) && (y > 250) && (y < 300))
				{
					if(time >= MAX_TIME)
					{
						time = MIN_TIME;
					}
					else
					{
						time = time+5;
					}
				}
				//flip button
				if((x > SQUARE_WIDTH*8 + 10) && (x < SQUARE_WIDTH*8 + 210) && (y > 300) && (y < 350))
				{
					flipboard = !flipboard;
				}
			}
			//If the right mouse button was pressed
			if( event.button.button == SDL_BUTTON_RIGHT )
			{
				//Get the mouse offsets
				int x = event.button.x;
				int y = event.button.y;
				//time button
				if((x > SQUARE_WIDTH*8 + 10) && (x < SQUARE_WIDTH*8 + 210) && (y > 250) && (y < 300))
				{
					if(time <= MIN_TIME)
					{
						time = MAX_TIME;
					}
					else
					{
						time = time-5;
					}
				}
			}
		}

		//If the user has Xed out the window
        if( event.type == SDL_QUIT )
        {
            //Quit the program
            quit = true;
        }
	}
	return true;
}

void GUI::draw()
{
	//Apply the surface to the screen
    apply_surface( 0, 0, background, screen );
	thinkbutton = TTF_RenderText_Solid(font,"Play",textColor);
	undobutton = TTF_RenderText_Solid(font,"Undo Move",textColor);
	flipbutton = TTF_RenderText_Solid(font,"Flip Board",textColor);
	apply_surface(SQUARE_WIDTH*8 + 20,110,thinkbutton,screen);
	apply_surface(SQUARE_WIDTH*8 + 20,210,undobutton,screen);
	apply_surface(SQUARE_WIDTH*8 + 20,310,flipbutton,screen);

	SDL_FreeSurface(message);
	//SDL_FreeSurface(inputscreen);
	SDL_FreeSurface(outputscreen);
	SDL_FreeSurface(autoplaybutton);
	SDL_FreeSurface(depthbutton);

	message = TTF_RenderText_Solid(font,text.c_str(),textColor);
	//inputscreen = TTF_RenderText_Solid(font,input.c_str(),textColor);
	outputscreen = TTF_RenderText_Solid(font,debugstring.c_str(),textColor);
	if(autoplay==true)
	{
		autoplaybutton = TTF_RenderText_Solid(font,"Autoplay On",textColor);
	}
	else
	{
		autoplaybutton = TTF_RenderText_Solid(font,"Autoplay Off",textColor);
	}
	depthbutton = TTF_RenderText_Solid(font,("Time "+Int2Str(time)).c_str(),textColor);

	if(message!=NULL)
	{
		apply_surface(SQUARE_WIDTH*8 + 10,350,message,screen);
	}
	//apply_surface(SQUARE_WIDTH*8 + 100,500,inputscreen,screen);
	if(outputscreen!=NULL)
	{
		apply_surface(SQUARE_WIDTH*8 + 10,400,outputscreen,screen);
	}
	if(autoplaybutton!=NULL)
	{
		apply_surface(SQUARE_WIDTH*8 + 20,160,autoplaybutton,screen);
	}
	if(depthbutton!=NULL)
	{
		apply_surface(SQUARE_WIDTH*8 + 20,260,depthbutton,screen);
	}
	for(int i = 0;i<64;i++)
	{
		if(flipboard==true)
		{
			apply_surface(SQUARE_WIDTH*(7-getFile(i)),SQUARE_HEIGHT*getRank(i),images[getImageNumber(inter.board.pos.Squares[i],i)],screen);
		}
		else
		{
			apply_surface(SQUARE_WIDTH*getFile(i),SQUARE_HEIGHT*(7-getRank(i)),images[getImageNumber(inter.board.pos.Squares[i],i)],screen);
		}
	}
}

void GUI::do_actions()
{
	if(play==1)
	{
		inter.board.generateMoves();
		Move computermove = inter.e1.IterativeDeepening(time*1000);
		if(!(computermove==CONS_NULLMOVE))
		{
			inter.board.makeMove(computermove);
			inter.e1.makeMove(computermove);
		}
		if(inter.board.pos.turn==COLOR_WHITE)
		{
			text = "White to move!";
		}
		else
		{
			text = "Black to move!";
		}
		play = 0;

		int status = inter.board.getGameStatus();
		if(status==STATUS_WHITEMATED) //post the game end status
		{
			text = "White is checkmated!";
			isover = 1;
		}
		else if(status==STATUS_BLACKMATED)
		{
			text = "Black is checkmated!";
			isover = 1;
		}
		else if(status==STATUS_STALEMATE)
		{
			text = "Its a stalemate!";
			isover = 1;
		}
		else if(status==STATUS_INSUFFICIENTMAT)
		{
			text = "Its a draw due to insufficient material!";
			isover = 1;
		}
	}
}

bool GUI::init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;    
    }

	if(TTF_Init()==-1)
	{
		return false;
	}
    
    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    
    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;    
    }
    
    //Set the window caption
    SDL_WM_SetCaption( "Gamma Chess", NULL );

	//Enable Unicode
    SDL_EnableUNICODE( SDL_ENABLE );  
    
    //If everything initialized fine
    return true;
}

bool GUI::load_files()
{
	background = load_image("Graphics\\background.png");
    font = TTF_OpenFont("Graphics\\lazy.ttf",28);
	if(background == NULL)
	{
		return false;    
	}
	for(unsigned int i = 0;i<MAX_IMAGES;i++)
	{
		//Load the image
		images[i] = load_image(ImageStrings[i]);
    
		//If there was an error in loading the image
		if(images[i] == NULL)
		{
			return false;    
		}
	}
    
    //If everything loaded fine
    return true;    
}

void GUI::clean_up()
{
	SDL_FreeSurface(background);
    //SDL_FreeSurface(message);
	//SDL_FreeSurface(inputscreen);
	SDL_FreeSurface(outputscreen);
	//SDL_FreeSurface(thinkbutton);
	//SDL_FreeSurface(autoplaybutton);
	//SDL_FreeSurface(undobutton);
	//SDL_FreeSurface(depthbutton);
	//SDL_FreeSurface(flipbutton);
	//TTF_CloseFont(font);
	for(unsigned int i = 0;i<64;i++)
	{
		SDL_FreeSurface(images[i]);
	}
    
    //Quit SDL
    SDL_Quit();    
}

SDL_Surface *load_image( std::string filename ) 
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;
    
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;
    
    //Load the image using SDL_image
    loadedImage = IMG_Load( filename.c_str() );
    
    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );
        
        //Free the old image
        SDL_FreeSurface( loadedImage );

		//If the image was optimized just fine
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF );

			//Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
    }
    
    //Return the optimized image
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;
    
    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;

	//Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

unsigned int getImageNumber(int Piece,int Square)
{
	int squarecolor = COLOR_BLACK;
	if((getRank(Square)+getFile(Square))%2==0)
	{
		squarecolor = COLOR_WHITE;
	}
	return (2*Piece + squarecolor);
}
