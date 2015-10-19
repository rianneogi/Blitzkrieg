#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include "Engine.h"
#include "Board.h"

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
    void makeMove();
	void unmakeMove();
	void think();
	void thinkdepth();
    void getMoves();
    void movecount();
    void help();
    void exit();
    void info();
	void UCI();
	void Winboard();
};

string getStringToken(string str, char delimiter, int token);

#endif // INTERFACE_H_INCLUDED
