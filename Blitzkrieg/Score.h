#pragma once

struct Score
{
	int mg;
	int eg;

	Score();
	Score(int x);

	Score operator+(Score const& s);
	Score operator-(Score const& s);
	Score operator*(Score const& s);
	Score operator/(Score const& s);

	Score operator+(int s);
	Score operator-(int s);
	Score operator*(int s);
	Score operator/(int s);

	operator int() const;
};
