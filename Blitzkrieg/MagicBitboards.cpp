#include "MagicBitboards.h"

Bitset RookAttacks[64][4096];
Bitset BishopAttacks[64][512];

Magic BishopMagicTable[64];
Magic RookMagicTable[64];

const Bitset RookMagic[64] = {
	0x8001d026804000ULL,
	0x40004020001000ULL,
	0xa00108008204200ULL,
	0x2080100008020480ULL,
	0x200020010080420ULL,
	0x200020008044190ULL,
	0x400040208210090ULL,
	0x10801ac100002180ULL,
	0x42002041020080ULL,
	0x1400040201002ULL,
	0x82002040820810ULL,
	0x4421001000082100ULL,
	0xc01001004080102ULL,
	0x4002000402000910ULL,
	0x2004841040200ULL,
	0x3005000040810012ULL,
	0x120410020800104ULL,
	0x10044000200248ULL,
	0x30120022804200ULL,
	0x2100090021011000ULL,
	0x124008004080080ULL,
	0x600c080110402004ULL,
	0x40008212210ULL,
	0xa06200004c0089ULL,
	0x480004140002000ULL,
	0xc10081002c4000ULL,
	0x1001100200041ULL,
	0x4000100100200900ULL,
	0x22002200040810ULL,
	0x1000400801042090ULL,
	0x440040101000200ULL,
	0x40082001c4405ULL,
	0x2920401020800080ULL,
	0x410002002400048ULL,
	0x28208042001201ULL,
	0x10004400400800ULL,
	0x880100501000801ULL,
	0x84000824012050ULL,
	0x5000411002200ULL,
	0x14a302000144ULL,
	0x410200820022ULL,
	0x830500020004004ULL,
	0x2445408112060020ULL,
	0x2008100100210009ULL,
	0x4021001008010004ULL,
	0x1142001020040400ULL,
	0x2100802040001ULL,
	0x800004400820001ULL,
	0x1440008000204080ULL,
	0x401002200804200ULL,
	0xa2448122001200ULL,
	0x10100100082100ULL,
	0x200a000c0600ULL,
	0x4004002010040ULL,
	0x20801500400ULL,
	0x42084004d1200ULL,
	0x8012004080210aULL,
	0x440008210482101ULL,
	0x41422001080400aULL,
	0x201002004100109ULL,
	0x4200a01044082aULL,
	0x13000400020801ULL,
	0x200102102081084ULL,
	0x2800240020410082ULL,
};

const Bitset BishopMagic[64] = {
	0x2040020409102108ULL,
	0x42080218120c2008ULL,
	0x90010061004001ULL,
	0x13804ad02010400ULL,
	0x4042000100100ULL,
	0x2582020220a02004ULL,
	0x2000480210110008ULL,
	0xb01140608040400ULL,
	0x14111d010510040ULL,
	0x80208022420ULL,
	0x6014484801022201ULL,
	0x824082042410010ULL,
	0xc00045040000100ULL,
	0x508220202401ULL,
	0x10040082113040d8ULL,
	0xa010042022020ULL,
	0xa0000802102201ULL,
	0x5464100188090bULL,
	0x4010180204820008ULL,
	0x3a94002804624a04ULL,
	0x1000811400002ULL,
	0x802000241101100ULL,
	0x480110c404020200ULL,
	0x2038d229c0440ULL,
	0x2020054049102404ULL,
	0x124044402280800ULL,
	0x10c404020e080058ULL,
	0x404004010200ULL,
	0x4001001041004000ULL,
	0x2002022101002ULL,
	0x108018002025502ULL,
	0x2400420140420200ULL,
	0x4a04200900041080ULL,
	0x101280200481000ULL,
	0x809044101080800ULL,
	0x802008020120200ULL,
	0x4200200002080ULL,
	0x401210a00010801ULL,
	0x1016040140050861ULL,
	0x2001110200002200ULL,
	0x1012060223088ULL,
	0x1011009004001003ULL,
	0x5082804040800ULL,
	0x1000004200880800ULL,
	0x43012100400ULL,
	0x4012042010100ULL,
	0x10010204310080ULL,
	0x2022052000d00ULL,
	0x4008208220502ULL,
	0x80100484404002cULL,
	0x2080a8420880021ULL,
	0x140020108480001ULL,
	0x834404005010504ULL,
	0x8042900a022106ULL,
	0x50802080a0120ULL,
	0x420428061c4005ULL,
	0x2002008201100304ULL,
	0x1201212028a2800ULL,
	0x280010042080400ULL,
	0x44110c09086800ULL,
	0x440402810020220ULL,
	0x80064002182240ULL,
	0x2000403024810440ULL,
	0x4c0020421043300ULL,
};

Bitset random_uint64() {
	Bitset u1, u2, u3, u4;
	u1 = (Bitset)(rand()) & 0xFFFF; u2 = (Bitset)(rand()) & 0xFFFF;
	u3 = (Bitset)(rand()) & 0xFFFF; u4 = (Bitset)(rand()) & 0xFFFF;
	return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

Bitset random_uint64_fewbits() {
	return random_uint64() & random_uint64() & random_uint64();
}

int count_1s(Bitset b) {
	int r;
	for (r = 0; b; r++, b &= b - 1);
	return r;
}

const int BitTable[64] = {
	63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
	51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
	26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
	58, 20, 37, 17, 36, 8
};

int pop_1st_bit(Bitset *bb) {
	Bitset b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return BitTable[(fold * 0x783a9b23) >> 26];
}

Bitset index_to_uint64(int index, int bits, Bitset m) {
	int i, j;
	Bitset result = 0ULL;
	for (i = 0; i < bits; i++) {
		j = pop_1st_bit(&m);
		if (index & (1 << i)) result |= (1ULL << j);
	}
	return result;
}

Bitset generateRookMask(int sq) {
	Bitset result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1; r <= 6; r++) result |= (1ULL << (fl + r * 8));
	for (r = rk - 1; r >= 1; r--) result |= (1ULL << (fl + r * 8));
	for (f = fl + 1; f <= 6; f++) result |= (1ULL << (f + rk * 8));
	for (f = fl - 1; f >= 1; f--) result |= (1ULL << (f + rk * 8));
	return result;
}

Bitset generateBishopMask(int sq) {
	Bitset result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++) result |= (1ULL << (f + r * 8));
	for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--) result |= (1ULL << (f + r * 8));
	for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++) result |= (1ULL << (f + r * 8));
	for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--) result |= (1ULL << (f + r * 8));
	return result;
}

Bitset generateRookAttacks(int sq, Bitset block) {
	Bitset result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1; r <= 7; r++) {
		result |= (1ULL << (fl + r * 8));
		if (block & (1ULL << (fl + r * 8))) break;
	}
	for (r = rk - 1; r >= 0; r--) {
		result |= (1ULL << (fl + r * 8));
		if (block & (1ULL << (fl + r * 8))) break;
	}
	for (f = fl + 1; f <= 7; f++) {
		result |= (1ULL << (f + rk * 8));
		if (block & (1ULL << (f + rk * 8))) break;
	}
	for (f = fl - 1; f >= 0; f--) {
		result |= (1ULL << (f + rk * 8));
		if (block & (1ULL << (f + rk * 8))) break;
	}
	return result;
}

Bitset generateBishopAttacks(int sq, Bitset block) {
	Bitset result = 0ULL;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
		result |= (1ULL << (f + r * 8));
		if (block & (1ULL << (f + r * 8))) break;
	}
	return result;
}

int transform(Bitset b, Bitset magic, int bits) {
	return (int)((b * magic) >> (64 - bits));
}

Bitset find_magic(int sq, int m, int bishop) {
	Bitset mask, b[4096], a[4096], used[4096], magic;
	int i, j, k, n, fail;

	mask = bishop ? generateBishopMask(sq) : generateRookMask(sq);
	n = count_1s(mask);

	for (i = 0; i < (1 << n); i++) {
		b[i] = index_to_uint64(i, n, mask);
		a[i] = bishop ? generateBishopAttacks(sq, b[i]) : generateRookAttacks(sq, b[i]);
	}
	for (k = 0; k < 100000000; k++) {
		magic = random_uint64_fewbits();
		if (count_1s((mask * magic) & 0xFF00000000000000ULL) < 6) continue;
		for (i = 0; i < 4096; i++) used[i] = 0ULL;
		for (i = 0, fail = 0; !fail && i < (1 << n); i++) {
			j = transform(b[i], magic, m);
			if (used[j] == 0ULL) used[j] = a[i];
			else if (used[j] != a[i]) fail = 1;
		}
		if (!fail) return magic;
	}
	printf("***Failed***\n");
	return 0ULL;
}

int RookBits[64] = {
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12
};

int BishopBits[64] = {
	6, 5, 5, 5, 5, 5, 5, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 7, 7, 7, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 5, 5, 5, 5, 5, 5, 6
};

void attacksinit()
{
	Bitset Slides[6][32];
	//Sliding Moves init
	for (int i = 0;i < 6;i++)
	{
		for (int j = 0;j < 32;j++)
		{
			int jr = j&(getPos2Bit(i) - 1);
			int jl = (j >> i) << (i + 1);
			int jext = (jr | jl) << 1;
			jext &= 0xff;
			Slides[i][j] = SlidingMoves[i + 1][jext];
		}
	}
	Bitset SlidesEnd[2][64];
	for (int j = 0;j < 64;j++)
	{
		int jext = (j << 1);
		SlidesEnd[0][j] = SlidingMoves[7][jext];
		SlidesEnd[1][j] = SlidingMoves[0][jext];
	}
	for (int i = 0;i < 64;i++)
	{
		//Rook
		for (int j = 0;j < 4096;j++)
		{
			RookAttacks[i][j] = 0;
			int rankocc = j >> 5;
			if (getFile(i) == 0 || getFile(i) == 7) rankocc = j >> 6;
			//Rank Moves
			if (getFile(i) == 0)
			{
				int rankocc = j >> 6;
				RookAttacks[i][j] |= SlidesEnd[0][rankocc] << (8 * (i / 8));
			}
			else if (getFile(i) == 7)
			{
				int rankocc = j >> 6;
				RookAttacks[i][j] |= SlidesEnd[1][rankocc] << (8 * (i / 8));
			}
			else
			{
				int rankocc = j >> 5;
				RookAttacks[i][j] |= Slides[(i % 8) - 1][rankocc] << (8 * (i / 8));
			}

			//File Moves
			if (getRank(i) == 0)
			{
				int fileocc = j & 0x3f;
				Bitset b = SlidesEnd[0][fileocc];
				Bitset p = 0x0;
				for (int k = 0;k<8;k++)
				{
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[turn270[k]] * s;
				}
				RookAttacks[i][j] |= (p >> (getFile(i)));
			}
			else if (getRank(i) == 7)
			{
				int fileocc = j & 0x3f;
				Bitset b = SlidesEnd[0][fileocc];
				Bitset p = 0x0;
				for (int k = 0;k<8;k++)
				{
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[turn270[k]] * s;
				}
				RookAttacks[i][j] |= (p >> (getFile(i)));
			}
			else
			{
				int fileocc = j & 0x1f;
				Bitset b = Slides[getRank(i) - 1][fileocc];
				Bitset p = 0x0;
				for (int k = 0;k<8;k++)
				{
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[turn270[k]] * s;
				}
				RookAttacks[i][j] |= (p >> (getFile(i)));
			}
		}

		//Bishop
		for (int j = 0;j < 512;j++)
		{
			int occ1 = j >> BishopBits[i];
			int occ2 = j&(getPos2Bit(BishopBits[i]) - 1);

			//Bishop A1H8 Moves
			int flag = 0;
			Bitset b;
			if ((getFile(i) == 0 || getFile(i) == 7) && (getRank(i) == 0 || getRank(i) == 7))
			{
				if (getFile(i) == 0)
					b = SlidesEnd[0][occ1];
				else
					b = SlidesEnd[1][occ1];
			}
			else if (turn135[i] >= 32)
			{
				b = Slides[7 - getRank(i) - 1][occ1];
				flag = 1;
			}
			else
				b = Slides[7 - getFile(i) - 1][occ1];
			Bitset p = 0x0;
			int m = i;
			if (getFile(m) != 0 && getRank(m) != 0)
			{
				while (true)
				{
					m = m - 7;
					if (m<0 || m>63)
					{
						break;
					}
					int k = 0;
					if (flag == 1)
						k = 7 - getRank(m);
					else
						k = 7 - getFile(m);
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[m] * s;
					if (getFile(m) == 0 || getRank(m) == 0)
					{
						break;
					}
				}
			}
			m = i;
			if (getFile(m) != 7 && getRank(m) != 7)
			{
				while (true)
				{
					m = m + 7;
					if (m<0 || m>63)
					{
						break;
					}
					int k = 0;
					if (flag == 1)
						k = 7 - getRank(m);
					else
						k = 7 - getFile(m);
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[m] * s;
					if (getFile(m) == 7 || getRank(m) == 7)
					{
						break;
					}
				}
			}
			BishopAttacks[i][j] = p;

			//Bishop A8H1 Moves
			flag = 0;
			if ((getFile(i) == 0 || getFile(i) == 7) && (getRank(i) == 0 || getRank(i) == 7))
			{
				if (getFile(i) == 0)
					b = SlidesEnd[0][occ2];
				else
					b = SlidesEnd[1][occ2];
			}
			else if (turn45[i] >= 32)
			{
				b = Slides[7 - getFile(i) - 1][occ2];
				flag = 1;
			}
			else
				b = Slides[getRank(i) - 1][occ2];
			p = 0x0;
			m = i;
			if (getFile(m) != 7 && getRank(m) != 0)
			{
				while (true)
				{
					m = m - 9;
					if (m<0 || m>63)
					{
						break;
					}
					int k = 0;
					if (flag == 1)
						k = 7 - getFile(m);
					else
						k = getRank(m);
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[m] * s;
					if (getFile(m) == 7 || getRank(m) == 0)
					{
						break;
					}
				}
			}
			m = i;
			if (getFile(m) != 0 && getRank(m) != 7)
			{
				while (true)
				{
					m = m + 9;
					if (m<0 || m>63)
					{
						break;
					}
					int k = 0;
					if (flag == 1)
						k = 7 - getFile(m);
					else
						k = getRank(m);
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[m] * s;
					if (getFile(m) == 0 || getRank(m) == 7)
					{
						break;
					}
				}
			}
			BishopAttacks[i][j] |= p;
		}
	}
}

void magicinit()
{
	for (int i = 0;i < 64;i++)
	{
		RookMagicTable[i].magic = RookMagic[i];
		RookMagicTable[i].mask = generateRookMask(i);
		RookMagicTable[i].shift = 64 - RookBits[i];

		BishopMagicTable[i].magic = RookMagic[i];
		BishopMagicTable[i].mask = generateRookMask(i);
		BishopMagicTable[i].shift = 64 - BishopBits[i];
	}
}

void generateMagics()
{
	int square;

	fstream file("magic.txt", ios::out);
	if (!file.is_open())
	{
		cout << "ERROR opening file" << endl;
		return;
	}

	file << "const uint64 RookMagic[64] = {\n";
	//printf("const uint64 RMagic[64] = {\n");
	for (square = 0; square < 64; square++)
		//printf("  0x%llxULL,\n", find_magic(square, RookBits[square], 0));
		file << "  0x" << hex << find_magic(square, RookBits[square], 0) << "ULL,\n";
	//printf("};\n\n");
	file << "};\n\n";
	
	//printf("const uint64 BMagic[64] = {\n");
	file << "const uint64 BishopMagic[64] = {\n";
	for (square = 0; square < 64; square++)
		//printf("  0x%llxULL,\n", find_magic(square, BishopBits[square], 1));
		file << "  0x" << hex << find_magic(square, BishopBits[square], 1) << "ULL,\n";
	//printf("};\n\n");
	file << "};\n\n";

	file.close();
}