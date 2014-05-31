#include <stdlib.h>
#include <string.h>
#include "taricha.h"

static uint64_t rotl(uint64_t x, uint8_t r)
{
	return (x<<r) | (x>>(64-r));
}

static void mix_xor(uint64_t y[8],
		uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, 
		uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7,
		uint8_t  r0, uint8_t  r1, uint8_t  r2, uint8_t  r3,
		uint8_t  r4, uint8_t  r5, uint8_t  r6, uint8_t  r7)
{
	y[0] = rotl(y[0] ^ x0, r0);
	y[1] = rotl(y[1] ^ x1, r1);
	y[2] = rotl(y[2] ^ x2, r2);
	y[3] = rotl(y[3] ^ x3, r3);
	y[4] = rotl(y[4] ^ x4, r4);
	y[5] = rotl(y[5] ^ x5, r5);
	y[6] = rotl(y[6] ^ x6, r6);
	y[7] = rotl(y[7] ^ x7, r7);
}

static void mix_add(uint64_t y[8],
		uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, 
		uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7,
		uint8_t  r0, uint8_t  r1, uint8_t  r2, uint8_t  r3,
		uint8_t  r4, uint8_t  r5, uint8_t  r6, uint8_t  r7)
{
	y[0] = rotl(y[0] + x0, r0);
	y[1] = rotl(y[1] + x1, r1);
	y[2] = rotl(y[2] + x2, r2);
	y[3] = rotl(y[3] + x3, r3);
	y[4] = rotl(y[4] + x4, r4);
	y[5] = rotl(y[5] + x5, r5);
	y[6] = rotl(y[6] + x6, r6);
	y[7] = rotl(y[7] + x7, r7);
}

static void f(uint64_t x[8])
{
	uint64_t y[8] = {
		0x243F6A8885A308D3, 0x13198A2E03707344,
		0xA4093822299F31D0, 0x082EFA98EC4E6C89,
		0x452821E638D01377, 0xBE5466CF34E90C6C,
		0xC0AC29B7C97C50DD, 0x3F84D5B5B5470917
	};

	mix_xor(y,x[3],x[0],x[2],x[7],x[4],x[5],x[6],x[1],56,48, 8, 8,48,56,48,40);
	mix_add(y,x[1],x[2],x[6],x[3],x[0],x[7],x[5],x[4],24,56,32, 8,32,56,24, 8);
	mix_xor(y,x[0],x[4],x[7],x[1],x[5],x[6],x[2],x[3],16,32,48,16,24,48,48, 8);
	mix_add(y,x[7],x[6],x[3],x[5],x[1],x[2],x[4],x[0],56, 8,56,56,56,48,32,24);
	mix_xor(y,x[4],x[1],x[5],x[2],x[3],x[0],x[7],x[6],48,16,32,32,24,56,56,16);
	mix_add(y,x[2],x[7],x[4],x[0],x[6],x[3],x[1],x[5],32,56, 8,56,32,32,24,56);
	mix_xor(y,x[6],x[5],x[0],x[4],x[2],x[1],x[3],x[7], 8,32,40,56,48,48,56,48);
	mix_add(y,x[5],x[3],x[1],x[6],x[7],x[4],x[0],x[2], 8, 8, 8, 8, 8, 8, 8, 8);

	y[0] *= 0xB7E151628AED2A6B;
	y[1] *= 0xBF7158809CF4F3C7;
	y[2] *= 0x62E7160F38B4DA57;
	y[3] *= 0xA784D9045190CFEF;
	y[4] *= 0x324E7738926CFBE5;
	y[5] *= 0xF4BF8D8D8C31D763;
	y[6] *= 0xDA06C80ABB1185EB;
	y[7] *= 0x4F7C7B5757F59585;	
	
	x[0] = 0x9216D5D98979FB1B;
	x[1] = 0xD1310BA698DFB5AC;
	x[2] = 0x2FFD72DBD01ADFB7;
	x[3] = 0xB8E1AFED6A267E96;
	x[4] = 0xBA7C9045F12C7F99;
	x[5] = 0x24A19947B3916CF7;
	x[6] = 0x0801F2E2858EFC16;
	x[7] = 0x636920D871574E69;

	mix_xor(x,y[0],y[2],y[4],y[1],y[5],y[7],y[6],y[3],48,56,24,40,48,48,40,24);
	mix_add(x,y[2],y[7],y[5],y[6],y[0],y[1],y[3],y[4],24,32,48, 8,40, 8, 8,24);
	mix_xor(x,y[4],y[3],y[0],y[7],y[6],y[2],y[5],y[1], 8,16,40, 8,32,16,32,32);
	mix_add(x,y[7],y[4],y[3],y[0],y[1],y[6],y[2],y[5], 8,32,48,16,16,32, 8,40);
	mix_xor(x,y[5],y[1],y[6],y[2],y[3],y[4],y[0],y[7],32,24,48,16, 8,48, 8,16);
	mix_add(x,y[6],y[0],y[7],y[3],y[4],y[5],y[1],y[2],40,48,24, 8,16, 8,40,24);
	mix_xor(x,y[1],y[6],y[2],y[5],y[7],y[3],y[4],y[0], 8,32,16,48, 8,48,48, 8);
	mix_add(x,y[3],y[5],y[1],y[4],y[2],y[0],y[7],y[6], 8, 8, 8, 8, 8, 8, 8, 8);
}


static void taricha_block(const uint8_t in[64], struct taricha_state *s)
{
	uint64_t *a = (uint64_t *)in;
	uint64_t x[8];
	int i;

	for (i=0; i<8; i++)
	{
		x[i] = s->s[i] ^ a[i];
	}
		
	f(x);

	for (i=0; i<8; i++)
	{
		s->s[i] ^= x[i];
	}
}

struct taricha_state taricha_init()
{
	struct taricha_state s = {
		.buffer_free = 64,
		.s = {
			0x9E3779B97F4A7C15, 0xF39CC0605CEDC834, 
			0x1082276BF3A27251, 0xF86C6A11D0C18E95,
			0x2767F0B153D27B7F, 0x0347045B5BF1827F,
			0x01886F0928403002, 0xC1D64BA40F335E36
		}
	};
	return s;
}

void taricha_append(const uint8_t *in, size_t length,
		struct taricha_state *s)
{
	if (s->buffer_free < 64)
	{
		if (s->buffer_free > length)
		{
			memcpy(s->buffer+(64-s->buffer_free), in, length);
			s->buffer_free -= length;
			return;
		}
		else
		{
			memcpy(s->buffer+(64-s->buffer_free), in, s->buffer_free);

			taricha_block(s->buffer, s);

			in += s-> buffer_free;
			length -= s->buffer_free;

			s->buffer_free = 64;
		}
	}
	while (length >= 64)
	{
		taricha_block(in, s);

		in += 64;
		length -= 64;
	}
	if (length > 0)
	{
		memcpy(s->buffer, in, length);
		s->buffer_free = 64-length;
	}
}

void taricha_finalize(uint8_t *out, size_t length,
		struct taricha_state *s)
{
	memset(s->buffer+(64-s->buffer_free), s->buffer_free, s->buffer_free);
	taricha_block(s->buffer, s);

	if (length > 64)
		length = 64;

	memcpy(out, s->s, length);
}

void taricha_full(uint8_t *in, uint8_t *out,
		size_t in_length, size_t out_length)
{
	struct taricha_state s = taricha_init();
	taricha_append(in, in_length, &s);
	taricha_finalize(out, out_length, &s);
}
