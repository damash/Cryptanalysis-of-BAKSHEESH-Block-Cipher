#include "Generating_random_numbers_AES.h"
#include <math.h>
typedef unsigned long long int u64;
typedef unsigned char u8;
typedef unsigned short int u16;

#define RIGHTROTONEBIT(a,b) ((a)<<63)^((b)>>1)
#define ACTIVE (u64)1<<14
#define FILTERTIME 4
#define GUESSKEYNUM (u64)1<<16
#define DATAPAIRSNUM (u64)1<<32
#define ROUND 7

const int p[128] = {
		0, 33, 66, 99, 96, 1, 34, 67, 64, 97, 2, 35, 32, 65, 98, 3, 4, 37, 70, 103, 100, 5, 38, 71, 68, 101, 6, 39, 36, 69, 102, 7,
		8, 41, 74, 107, 104, 9, 42, 75, 72, 105, 10, 43, 40, 73, 106, 11, 12, 45, 78, 111, 108, 13, 46, 79, 76, 109, 14, 47, 44, 77, 110, 15,
		16, 49, 82, 115, 112, 17, 50, 83, 80, 113, 18, 51, 48, 81, 114, 19, 20, 53, 86, 119, 116, 21, 54, 87, 84, 117, 22, 55, 52, 85, 118, 23,
		24, 57, 90, 123, 120, 25, 58, 91, 88, 121, 26, 59, 56, 89, 122, 27, 28, 61, 94, 127, 124, 29, 62, 95, 92, 125, 30, 63, 60, 93, 126, 31
};
u8 sbox[16] = { 0x3,0x0,0x6,0xD,0xB,0x5,0x8,0xE,0xC,0xF,0x9,0x2,0x4,0xA,0x7,0x1 };
u8 isbox[16] = { 1, 15, 11, 0, 12, 5, 2, 14, 6, 10, 13, 4, 8, 3, 7, 9 };
u8 ssbox[256] = { 0 };
u64 pstab[16][256][2] = { 0 };
const int rc[35] = { 2, 33, 16, 9, 36, 19, 40, 53, 26, 13, 38, 51, 56, 61, 62, 31, 14, 7, 34, 49, 24, 45, 54, 59, 28, 47, 22, 43, 20, 11, 4, 3, 32, 17, 8 };
const int TP[6] = { 8, 13, 19, 35, 67, 106 };
u64 RC[35][2] = { 0 };
int K_[8][16] = {
	{0, 33, 66, 99, 8, 41, 74, 107, 16, 49, 82, 115, 24, 57, 90, 123},
	{96, 1, 34, 67, 104, 9, 42, 75, 112, 17, 50, 83,120, 25, 58, 91},
	{64, 97, 2, 35, 72, 105, 10, 43, 80, 113, 18, 51,88, 121, 26, 59},
	{32, 65, 98, 3, 40, 73, 106, 11, 48, 81, 114, 19,56, 89, 122, 27},
	{4, 37, 70, 103, 12, 45, 78, 111, 20, 53, 86, 119,28, 61, 94, 127},
	{100, 5, 38, 71, 108, 13, 46, 79, 116, 21, 54, 87,124, 29, 62, 95},
	{68, 101, 6, 39, 76, 109, 14, 47, 84, 117, 22, 55,92, 125, 30, 63},
	{36, 69, 102, 7, 44, 77, 110, 15, 52, 85, 118, 23,60, 93, 126, 31}
};

int balancedBits[8][4] = {
	{0,1,2,3},
	{0,1,2,3},
	{0,1,2,3},
	{0,1,2,3},
	{0,1,2,3},
	{0,1,2,3},
	{0,1,2,3},
	{0,1,2,3}
};

void get_ssbox();
void get_pstab();
void get_round_constant();
void encrypt(u64 pt[2], u64 rk[][2], u64 ct[2], int R);
void testIntegral();
void _9roundAttack();

int main() {
	get_ssbox();
	get_pstab();
	get_round_constant();
	_9roundAttack();
	return 0;
}

void encrypt(u64 pt[2], u64 rk[][2], u64 ct[2], int R) {
	ct[0] = pt[0] ^ rk[0][0];
	ct[1] = pt[1] ^ rk[0][1];
	for (int r = 0; r < R; r++) {
		pt[0] = pstab[0][0xff & ct[0]][0] ^ pstab[1][0xff & (ct[0] >> 8)][0] ^ pstab[2][0xff & (ct[0] >> 16)][0] ^ pstab[3][0xff & (ct[0] >> 24)][0]
			^ pstab[4][0xff & (ct[0] >> 32)][0] ^ pstab[5][0xff & (ct[0] >> 40)][0] ^ pstab[6][0xff & (ct[0] >> 48)][0] ^ pstab[7][0xff & (ct[0] >> 56)][0]
			^ pstab[8][0xff & ct[1]][0] ^ pstab[9][0xff & (ct[1] >> 8)][0] ^ pstab[10][0xff & (ct[1] >> 16)][0] ^ pstab[11][0xff & (ct[1] >> 24)][0]
			^ pstab[12][0xff & (ct[1] >> 32)][0] ^ pstab[13][0xff & (ct[1] >> 40)][0] ^ pstab[14][0xff & (ct[1] >> 48)][0] ^ pstab[15][0xff & (ct[1] >> 56)][0];
		pt[1] = pstab[0][0xff & ct[0]][1] ^ pstab[1][0xff & (ct[0] >> 8)][1] ^ pstab[2][0xff & (ct[0] >> 16)][1] ^ pstab[3][0xff & (ct[0] >> 24)][1]
			^ pstab[4][0xff & (ct[0] >> 32)][1] ^ pstab[5][0xff & (ct[0] >> 40)][1] ^ pstab[6][0xff & (ct[0] >> 48)][1] ^ pstab[7][0xff & (ct[0] >> 56)][1]
			^ pstab[8][0xff & ct[1]][1] ^ pstab[9][0xff & (ct[1] >> 8)][1] ^ pstab[10][0xff & (ct[1] >> 16)][1] ^ pstab[11][0xff & (ct[1] >> 24)][1]
			^ pstab[12][0xff & (ct[1] >> 32)][1] ^ pstab[13][0xff & (ct[1] >> 40)][1] ^ pstab[14][0xff & (ct[1] >> 48)][1] ^ pstab[15][0xff & (ct[1] >> 56)][1];
		ct[0] = pt[0] ^ rk[r + 1][0];
		ct[1] = pt[1] ^ rk[r + 1][1];
	}
}

void keySchedule(u64 key[2], u64 rk[][2], int R) {
	rk[0][0] = key[0];
	rk[0][1] = key[1];
	for (int r = 0; r < R; r++) {
		rk[r + 1][0] = (rk[r][1] << 63) ^ (rk[r][0] >> 1); //^ RC[r][0];
		rk[r + 1][1] = (rk[r][0] << 63) ^ (rk[r][1] >> 1); //^ RC[r][1];
	}
}

u16 getCorrectKey(u64 key[2], int s) {
	u16 k = 0;
	for (int i = 0; i < 16; i++) {
		k ^= ((key[K_[s][i] >> 6] >> (K_[s][i] & 0x3f)) & 1) << i;
	}
	return k;
}

void generateData(u64 ct[][ACTIVE][2], u64 rk[][2], int R, int position) {
	for (int j = 0; j < FILTERTIME; j++) {
		u64 i;
		u64 tmp[2] = { GenerateRandomValue(), GenerateRandomValue() };
		u64 pt[2];
		for (i = 0; i < ACTIVE; i++) {
			pt[0] = tmp[0] ^ (i << position);
			pt[1] = tmp[1];
			encrypt(pt, rk, ct[j][i], R);
		}
	}
}

int filter(u16 gk[], int candidateNum, u64 ct[][ACTIVE][2], int K[], int balancedBit[4], int b, int time) {
	int rk[16], x[12], y[16], t, i, j, k;
	int tp[12] = { 0,5,10,12,1,6,8,13,2,4,9,14 };
	int cnt = 0;
	for (int index = 0; index < candidateNum; index++) {
		u8  bit[4] = { 0 }, bt = 0;
		for (i = 0; i < 16; i++) rk[i] = 1 & (gk[index] >> i);
		for (i = 0; i < (1 << 12); i++) {
			for (j = 0; j < 4; j++) {
				k = 4 * j;

				y[k] = rk[k] ^ (1 & (ct[time][i][K[k] >> 6] >> (K[k] & 0x3f)));
				y[k + 1] = rk[k + 1] ^ (1 & (ct[time][i][K[k + 1] >> 6] >> (K[k + 1] & 0x3f)));
				y[k + 2] = rk[k + 2] ^ (1 & (ct[time][i][K[k + 2] >> 6] >> (K[k + 2] & 0x3f)));
				y[k + 3] = rk[k + 3] ^ (1 & (ct[time][i][K[k + 3] >> 6] >> (K[k + 3] & 0x3f)));

				t = (y[k + 3] << 3) ^ (y[k + 2] << 2) ^ (y[k + 1] << 1) ^ y[k];
				t = isbox[t];

				y[k] = t & 1;
				y[k + 1] = (t >> 1) & 1;
				y[k + 2] = (t >> 2) & 1;
			}
			for (j = 0; j < 4; j++) {
				k = 3 * j;
				x[k] = y[tp[k]];
				x[k + 1] = y[tp[k + 1]];
				x[k + 2] = y[tp[k + 2]];
				bit[j] ^= x[k] ^ x[k + 1] ^ x[k + 2];
			}
		}
		for (j = 0; j < b; j++) bt |= bit[balancedBit[j]];
		if (bt == 0) {
			gk[cnt] = gk[index];
			cnt++;
		}
	}
	return cnt;
}

/* t: 过滤次数；balancedBit: 7轮平衡位置 */
int recoverKey(int K[16], u64 ct[][ACTIVE][2], int balancedBit[4], int b, u16 candidate[16]) {
	/* gk: 密钥猜测值 */
	int i;
	int candidateNum = GUESSKEYNUM;
	u16 gk[GUESSKEYNUM] = { 0 };
	for (i = 0; i < GUESSKEYNUM; i++) gk[i] = i;
	int time = 0;
	while (candidateNum > 16 && time < FILTERTIME) {
		candidateNum = filter(gk, candidateNum, ct, K, balancedBit, b, time);
		time++;
		printf("filtering %d, |candidates| = 2^{%0.1f}\n", time, log2((double)candidateNum));
	}
	for (i = 0; i < candidateNum; i++)
	{
		printf("%04x ", gk[i]);
		candidate[i] = gk[i];
	}
	return candidateNum;
}

int determineCorrectFromCandidate(u16 candidate[][16], int subkey[128], u64 rk[][2]) {
	int i[8];
	u64 pt[2], ct[2], tpt[2], tct[2], key[2], grk[10][2];
	pt[0] = GenerateRandomValue();
	pt[1] = GenerateRandomValue();
	tpt[0] = pt[0];
	tpt[1] = pt[1];
	encrypt(tpt, rk, ct, ROUND + 2);
	
	for (i[0] = 0; i[0] < 16; i[0]++) {
		//printf("K[0]: %04x\n", candidate[0][i[0]]);
		for (int j = 0; j < 16; j++) subkey[K_[0][j]] = 1 & (candidate[0][i[0]] >> j);
		for (i[1] = 0; i[1] < 16; i[1]++) {
			//printf("K[1]: %04x\n", candidate[1][i[1]]);
			for (int j = 0; j < 16; j++) subkey[K_[1][j]] = 1 & (candidate[1][i[1]] >> j);
			for (i[2] = 0; i[2] < 16; i[2]++) {
				//printf("K[2]: %04x\n", candidate[2][i[2]]);
				for (int j = 0; j < 16; j++) subkey[K_[2][j]] = 1 & (candidate[2][i[2]] >> j);
				for (i[3] = 0; i[3] < 16; i[3]++) {
					//printf("K[3]: %04x\n", candidate[3][i[3]]);
					for (int j = 0; j < 16; j++) subkey[K_[3][j]] = 1 & (candidate[3][i[3]] >> j);
					for (i[4] = 0; i[4] < 16; i[4]++) {
						//printf("K[4]: %04x\n", candidate[4][i[4]]);
						for (int j = 0; j < 16; j++) subkey[K_[4][j]] = 1 & (candidate[4][i[4]] >> j);
						for (i[5] = 0; i[5] < 16; i[5]++) {
							//printf("K[5]: %04x\n", candidate[5][i[5]]);
							for (int j = 0; j < 16; j++) subkey[K_[5][j]] = 1 & (candidate[5][i[5]] >> j);
							for (i[6] = 0; i[6] < 16; i[6]++) {
								//printf("K[6]: %04x\n", candidate[6][i[6]]);
								for (int j = 0; j < 16; j++) subkey[K_[6][j]] = 1 & (candidate[6][i[6]] >> j);
								for (i[7] = 0; i[7] < 16; i[7]++) {
									/*for (int j = 0; j < 8; j++) printf("%04x ", candidate[j][i[j]]);
									printf("\n");*/
									for (int j = 0; j < 16; j++) subkey[K_[7][j]] = 1 & (candidate[7][i[7]] >> j);
									key[0] = 0;
									key[1] = 0;
									for (int k = 0; k < 64; k++) {
										key[1] ^= ((u64)subkey[56 + k]) << k;
										key[0] ^= ((u64)subkey[(120 + k) % 128]) << k;
									}
									keySchedule(key, grk, ROUND + 2);
									tpt[0] = pt[0];
									tpt[1] = pt[1];
									encrypt(tpt, grk, tct, ROUND + 2);
									if ((ct[0] == tct[0]) && (ct[1] == tct[1]))
									{
										printf("recovered key: %016llx %016llx\n", key[1], key[0]);
										return 0;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	printf("Not found!!!\n");
	return 0;
}

void _9roundAttack() {
	u64 ct[FILTERTIME][ACTIVE][2], rk[36][2], key[2] = {0xffffffffffffffff,0xffffffffffffffff};
	int time = 1;
	int position = 0;
	int b[8] = { 4,4,4,4,4,4,4,4 };
	u16 correctrk[8];
	clock_t s, e;
	//key[0] = GenerateRandomValue();
	//key[1] = GenerateRandomValue();
	keySchedule(key, rk, ROUND + 2);
	printf("Correct subkey: ");
	for (int s = 0; s < 8; s++) {
		correctrk[s] = getCorrectKey(rk[ROUND + 2], s);
		printf("%04x ", correctrk[s]);
	}
	printf("\n");
	while(time > 0) {
		int flag = 1;
		s = clock();
		int subkey[128];
		printf("Number: %d\n", time);
		printf("Generating data......\n");
		generateData(ct, rk, ROUND + 2, position);
		printf("Done!!!!!!\n");
		u16 candidate[8][16];
		int candidateNum;		
		for (int s = 0; s < 8; s++) {
			printf("Now filtering guessed key in K'[%d], 16-bit correct key: %04x\n", s, correctrk[s]);
			candidateNum = recoverKey(K_[s], ct, balancedBits[s], b[s], candidate[s]);
			printf("candidateNum: %d\n", candidateNum);
			if (candidateNum != 16) {
				flag = 0;
				break;
			}
			printf("\n");
		}
		if (flag == 1) {
			printf("Now filtering candidates......\n");
			flag = determineCorrectFromCandidate(candidate, subkey, rk);
			printf("Correct subkey: ");
			for (int s = 0; s < 8; s++) printf("%04x ", correctrk[s]);
			printf("\n");
			printf("Correct key: %016llx %016llx\n", key[1], key[0]);
			e = clock();
			printf("time: %f\n", (float)(e - s) / 1000);
			break;
		}
		printf("Once again!!!!!!\n");
		time++;
	}
}

void get_ssbox() {
	int i;
	for (i = 0; i < 256; i++) {
		ssbox[i] = (sbox[i >> 4] << 4) | sbox[i & 0xf];
	}
}
void get_pstab() {

	int i, j, k, l, m, n;
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 256; j++) {
			for (k = 0; k < 8; k++) {
				l = 1 & (ssbox[j] >> k);
				m = i * 8 + k;
				n = p[m];
				pstab[i][j][n / 64] ^= ((u64)l) << (n % 64);
			}
		}
	}
}

void get_round_constant() {
	int i, j;
	for (i = 0; i < 35; i++) {
		for (j = 0; j < 6; j++) {
			RC[i][TP[j] / 64] ^= ((u64)((rc[i] >> j) & 1)) << (TP[j] % 64);
		}
	}
}