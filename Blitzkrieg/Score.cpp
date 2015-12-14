#include "Score.h"

Score::Score()
{
	mg = 0;
	eg = 0;
}

Score::Score(int x)
{
	mg = x;
	eg = x;
}

Score Score::operator+(Score const& s)
{
	Score r;
	r.mg = mg + s.mg;
	r.eg = eg + s.eg;
	return r;
}

Score Score::operator-(Score const& s)
{
	Score r;
	r.mg = mg - s.mg;
	r.eg = eg - s.eg;
	return r;
}

Score Score::operator*(Score const& s)
{
	Score r;
	r.mg = mg * s.mg;
	r.eg = eg * s.eg;
	return r;
}

Score Score::operator/(Score const& s)
{
	Score r;
	r.mg = mg / s.mg;
	r.eg = eg / s.eg;
	return r;
}

Score Score::operator+(int s)
{
	Score r;
	r.mg = mg + s;
	r.eg = eg + s;
	return r;
}

Score Score::operator-(int s)
{
	Score r;
	r.mg = mg - s;
	r.eg = eg - s;
	return r;
}

Score Score::operator*(int s)
{
	Score r;
	r.mg = mg * s;
	r.eg = eg * s;
	return r;
}

Score Score::operator/(int s)
{
	Score r;
	r.mg = mg / s;
	r.eg = eg / s;
	return r;
}
