#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include "Engine.h"
#include "Board.h"

#include <fstream>

extern const int ENGINEVERSION;
extern string ENGINENAME;
extern string ENGINEAUTHOR;

class Interface
{
    public:
    Board board;
    Engine e1;
    //Engine e2;

    Interface();
    ~Interface();
    void start();
    void display(int flip);
    void makeMove(string s);
	void unmakeMove();
	void think();
    void getMoves();
    void movecount();
    void help();
    void exit();
    void info();
	void UCI();
};

#endif // INTERFACE_H_INCLUDED
