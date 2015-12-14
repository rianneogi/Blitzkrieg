#include "Score.h"

Score::Score() : mg(0), eg(0) {}

Score::Score(int x) : mg(x), eg(x) {}

Score::Score(int m, int e) : mg(m), eg(e) {}

Score Score::operator+(Score const& s) const
{
	Score r;
	r.mg = mg + s.mg;
	r.eg = eg + s.eg;
	return r;
}

Score Score::operator-(Score const& s) const
{
	Score r;
	r.mg = mg - s.mg;
	r.eg = eg - s.eg;
	return r;
}

Score Score::operator*(Score const& s) const
{
	Score r;
	r.mg = mg * s.mg;
	r.eg = eg * s.eg;
	return r;
}

Score Score::operator/(Score const& s) const
{
	Score r;
	r.mg = mg / s.mg;
	r.eg = eg / s.eg;
	return r;
}

Score Score::operator+=(Score const& s)
{
	mg += s.mg;
	eg += s.eg;
	return *this;
}

Score Score::operator-=(Score const& s)
{
	mg -= s.mg;
	eg -= s.eg;
	return *this;
}

Score Score::operator*=(Score const& s)
{
	mg *= s.mg;
	eg *= s.eg;
	return *this;
}

Score Score::operator/=(Score const& s)
{
	mg /= s.mg;
	eg /= s.eg;
	return *this;
}

Score Score::operator+(int s) const
{
	Score r;
	r.mg = mg + s;
	r.eg = eg + s;
	return r;
}

Score Score::operator-(int s) const
{
	Score r;
	r.mg = mg - s;
	r.eg = eg - s;
	return r;
}

Score Score::operator*(int s) const
{
	Score r;
	r.mg = mg * s;
	r.eg = eg * s;
	return r;
}

Score Score::operator/(int s) const
{
	Score r;
	r.mg = mg / s;
	r.eg = eg / s;
	return r;
}

Score Score::operator+=(int s)
{
	mg += s;
	eg += s;
	return *this;
}

Score Score::operator-=(int s)
{
	mg -= s;
	eg -= s;
	return *this;
}

Score Score::operator*=(int s)
{
	mg *= s;
	eg *= s;
	return *this;
}

Score Score::operator/=(int s)
{
	mg /= s;
	eg /= s;
	return *this;
}

Score Score::operator+(float s) const
{
	Score r;
	r.mg = mg + s;
	r.eg = eg + s;
	return r;
}

Score Score::operator-(float s) const
{
	Score r;
	r.mg = mg - s;
	r.eg = eg - s;
	return r;
}

Score Score::operator*(float s) const
{
	Score r;
	r.mg = mg * s;
	r.eg = eg * s;
	return r;
}

Score Score::operator/(float s) const
{
	Score r;
	r.mg = mg / s;
	r.eg = eg / s;
	return r;
}

Score Score::operator+=(float s)
{
	mg += s;
	eg += s;
	return *this;
}

Score Score::operator-=(float s)
{
	mg -= s;
	eg -= s;
	return *this;
}

Score Score::operator*=(float s)
{
	mg *= s;
	eg *= s;
	return *this;
}

Score Score::operator/=(float s)
{
	mg /= s;
	eg /= s;
	return *this;
}

//Score Score::operator+(uint64_t s) const
//{
//	Score r;
//	r.mg = mg + s;
//	r.eg = eg + s;
//	return r;
//}
//
//Score Score::operator-(uint64_t s) const
//{
//	Score r;
//	r.mg = mg - s;
//	r.eg = eg - s;
//	return r;
//}
//
//Score Score::operator*(uint64_t s) const
//{
//	Score r;
//	r.mg = mg * s;
//	r.eg = eg * s;
//	return r;
//}
//
//Score Score::operator/(uint64_t s) const
//{
//	Score r;
//	r.mg = mg / s;
//	r.eg = eg / s;
//	return r;
//}
//
//Score Score::operator+=(uint64_t s)
//{
//	mg += s;
//	eg += s;
//	return *this;
//}
//
//Score Score::operator-=(uint64_t s)
//{
//	mg -= s;
//	eg -= s;
//	return *this;
//}
//
//Score Score::operator*=(uint64_t s)
//{
//	mg *= s;
//	eg *= s;
//	return *this;
//}
//
//Score Score::operator/=(uint64_t s)
//{
//	mg /= s;
//	eg /= s;
//	return *this;
//}

Score::operator int() const
{
	return mg;
}
