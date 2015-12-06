#include "MagicBitboards.h"

Bitset RookAttacks[64][4096];
Bitset BishopAttacks[64][512];

Magic BishopMagicTable[64];
Magic RookMagicTable[64];

const Bitset RookMagic[64] = {
	0x80002010804001ULL,
	0x840002003100940ULL,
	0x2100150040082000ULL,
	0x1100092101100004ULL,
	0x600041042002018ULL,
	0x200100408020001ULL,
	0x80020000800100ULL,
	0x200003204004081ULL,
	0x34a002042010081ULL,
	0x1800400050002002ULL,
	0x10a1002001001040ULL,
	0x1001000200904ULL,
	0x2101000800061100ULL,
	0x26000825120010ULL,
	0x4024000401100802ULL,
	0x5011000040820100ULL,
	0x480004000200040ULL,
	0x4030004020004000ULL,
	0x210410010200100ULL,
	0x30004008040040ULL,
	0x188110004080100ULL,
	0x941010004000802ULL,
	0x21040010020841ULL,
	0x25060000440289ULL,
	0x4080004140002006ULL,
	0x2140500040002000ULL,
	0x20410100200810ULL,
	0x1000090100100420ULL,
	0x1080050100080011ULL,
	0x2401000900040002ULL,
	0x200010400080290ULL,
	0x85408a00040041ULL,
	0x80012003400140ULL,
	0x1010002001404000ULL,
	0x220080141401000ULL,
	0x18081001002100ULL,
	0x9000411000800ULL,
	0x5c000200800480ULL,
	0x2101001431000200ULL,
	0x501038242002411ULL,
	0x1180022000414000ULL,
	0x1410004020004019ULL,
	0x201004020010011ULL,
	0x98010200101000ULL,
	0x801000800130004ULL,
	0x802001004020009ULL,
	0x2002008001004040ULL,
	0x251000d4881000aULL,
	0x1020208042010200ULL,
	0x10200040100a40ULL,
	0x2008146112200ULL,
	0x10102042000a00ULL,
	0x4884004800800480ULL,
	0x10402004100801ULL,
	0x420a280102700400ULL,
	0x830088044210200ULL,
	0x200e708003402b01ULL,
	0x100400120810617ULL,
	0x4088011402202ULL,
	0x100100200409ULL,
	0x2002000410082046ULL,
	0x10e002104881022ULL,
	0x2005500800c10204ULL,
	0x1027004400209aULL,
};

const Bitset BishopMagic[64] = {
	0x10a5204080210c0ULL,
	0x1010648800802010ULL,
	0x3610040848490000ULL,
	0x1022408100096022ULL,
	0x21104020800200ULL,
	0x2080404000001ULL,
	0x102021002080002ULL,
	0x804104210042002ULL,
	0x2008c208120410ULL,
	0x4000082204204201ULL,
	0x404084204102000ULL,
	0x4000040412821490ULL,
	0x1000411040460200ULL,
	0x410402402080ULL,
	0x41160844020800ULL,
	0x2ea020201493800ULL,
	0x405000810300200ULL,
	0x2104808081080ULL,
	0x2144005204240100ULL,
	0x8002220204108ULL,
	0x4002022401210800ULL,
	0x6002001240500440ULL,
	0x4200401208040420ULL,
	0x1003020200860908ULL,
	0x5010014002024cULL,
	0x2011204030042f01ULL,
	0x700240008004404ULL,
	0x8080000202020ULL,
	0x10030001200801ULL,
	0x10002001040100ULL,
	0x404042261140cULL,
	0x42108100c42280ULL,
	0xa02202008100254ULL,
	0x2140208010210c0ULL,
	0x102011000010044ULL,
	0x1820080800220a01ULL,
	0x240082008c0104ULL,
	0x429010102020048ULL,
	0x61b00121101a0081ULL,
	0x8010052010040ULL,
	0x880484040320eaULL,
	0x202280c04801a00ULL,
	0x4101804040800ULL,
	0x140004208010080ULL,
	0x2080052012000300ULL,
	0x104101040401600ULL,
	0xcc4041404120054ULL,
	0x4412441400820020ULL,
	0x201240202400000ULL,
	0x1800240202100011ULL,
	0x40002010a98648cULL,
	0x1004004210540008ULL,
	0x184005010000ULL,
	0x5001102001044260ULL,
	0x20080220c40058ULL,
	0x404041800530040ULL,
	0x400420210010400ULL,
	0x28204064046008ULL,
	0x2000130044022131ULL,
	0x1080000004420214ULL,
	0x704040082200ULL,
	0x80500403a040108ULL,
	0x200404242020200ULL,
	0x40820080a084a10ULL,
};

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

int magictransform(Bitset b, Bitset magic, int bits) {
	return (int)((b * magic) >> (64 - bits));
}

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
			//Bitset j2 = index_to_uint64(j, RookBits[i], generateRookMask(i));
			//j2 = (int)((j2*RookMagic[i]) >> (64-RookBits[i]));
			//RookAttacks[i][j2] = 0;
			int j2 = j;
			int rankocc = j >> 5;
			if (getFile(i) == 0 || getFile(i) == 7) rankocc = j >> 6;
			//Rank Moves
			if (getFile(i) == 0)
			{
				int rankocc = j >> 6;
				RookAttacks[i][j2] |= SlidesEnd[0][rankocc] << (8 * (i / 8));
			}
			else if (getFile(i) == 7)
			{
				int rankocc = j >> 6;
				RookAttacks[i][j2] |= SlidesEnd[1][rankocc] << (8 * (i / 8));
			}
			else
			{
				int rankocc = j >> 5;
				RookAttacks[i][j2] |= Slides[(i % 8) - 1][rankocc] << (8 * (i / 8));
			}

			//File Moves
			if (getRank(i) == 0)
			{
				int fileocc = j & 0x3f;
				Bitset b = SlidesEnd[1][fileocc];
				Bitset p = 0x0;
				for (int k = 0;k<8;k++)
				{
					Bitset s = (b >> k) % 2;
					p |= Pos2Bit[turn270[k]] * s;
				}
				RookAttacks[i][j2] |= (p >> (getFile(i)));
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
				RookAttacks[i][j2] |= (p >> (getFile(i)));
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
				RookAttacks[i][j2] |= (p >> (getFile(i)));
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
		for (i = 0, fail = 0; !fail && i < (1 << n); i++) 
		{
			j = magictransform(b[i], magic, m);
			if (used[j] == 0ULL) used[j] = a[i];
			else if (used[j] != a[i]) fail = 1;
			/*if (popcnt(j) != popcnt(b[i]))
			{
				fail = 1;
			}*/
		}
		if (!fail) return magic;
	}
	printf("***Failed***\n");
	return 0ULL;
}

void magicinit()
{
	for (int i = 0;i < 64;i++)
	{
		RookMagicTable[i].magic = RookMagic[i];
		RookMagicTable[i].mask = generateRookMask(i);
		RookMagicTable[i].shift = 64 - RookBits[i];

		BishopMagicTable[i].magic = BishopMagic[i];
		BishopMagicTable[i].mask = generateBishopMask(i);
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

	file << "const Bitset RookMagic[64] = {\n";
	file << hex;
	//printf("const uint64 RMagic[64] = {\n");
	for (square = 0; square < 64; square++)
	{
		Bitset b = find_magic(square, RookBits[square], 0);
		printf("  0x%llxULL,\n", b);
		file << "  0x" << b << "ULL,\n";
	}
		
	//printf("};\n\n");
	file << "};\n\n";
	
	//printf("const uint64 BMagic[64] = {\n");
	file << "const Bitset BishopMagic[64] = {\n";
	for (square = 0; square < 64; square++)
	{
		Bitset b = find_magic(square, BishopBits[square], 1);
		printf("  0x%llxULL,\n", b);
		file << "  0x" << b << "ULL,\n";
	}
	//printf("};\n\n");
	file << "};\n\n";

	file.close();
}