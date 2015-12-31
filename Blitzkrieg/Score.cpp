#include "Score.h"

Scale::Scale() : mg(0), eg(0) {}

Scale::Scale(float x) : mg(x), eg(x) {}

Scale::Scale(float m, float e) : mg(m), eg(e) {}

Scale Scale::operator+(Scale const& s) const
{
	Scale r;
	r.mg = mg + s.mg;
	r.eg = eg + s.eg;
	return r;
}

Scale Scale::operator-(Scale const& s) const
{
	Scale r;
	r.mg = mg - s.mg;
	r.eg = eg - s.eg;
	return r;
}

Scale Scale::operator*(Scale const& s) const
{
	Scale r;
	r.mg = mg * s.mg;
	r.eg = eg * s.eg;
	return r;
}

Scale Scale::operator/(Scale const& s) const
{
	Scale r;
	r.mg = mg / s.mg;
	r.eg = eg / s.eg;
	return r;
}

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

Score Score::operator+(Scale const & s) const
{
	Score r;
	r.mg = mg + s.mg;
	r.eg = eg + s.eg;
	return r;
}

Score Score::operator-(Scale const & s) const
{
	Score r;
	r.mg = mg - s.mg;
	r.eg = eg - s.eg;
	return r;
}

Score Score::operator*(Scale const & s) const
{
	Score r;
	r.mg = mg * s.mg;
	r.eg = eg * s.eg;
	return r;
}

Score Score::operator/(Scale const & s) const
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

Score Score::operator+=(Scale const& s)
{
	mg += s.mg;
	eg += s.eg;
	return *this;
}

Score Score::operator-=(Scale const& s)
{
	mg -= s.mg;
	eg -= s.eg;
	return *this;
}

Score Score::operator*=(Scale const& s)
{
	mg *= s.mg;
	eg *= s.eg;
	return *this;
}

Score Score::operator/=(Scale const& s)
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

Score::operator string() const
{
	return (std::to_string(mg) + ", " + std::to_string(eg));
}

std::ostream& operator<<(std::ostream& os, const Score& s)
{
	os << (std::to_string(s.mg) + ", " + std::to_string(s.eg));
	return os;
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
