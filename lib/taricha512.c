/*
The MIT License (MIT)

Copyright (c) 2014 Scott Hulberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "taricha.h"
#include "taricha512_common.h"


static inline void taricha512_transform(uint64_t x[8])
{
	uint64_t y[8] = {
		0x243F6A8885A308D3,
		0x13198A2E03707344,
		0xA4093822299F31D0,
		0x082EFA98EC4E6C89,
		0x452821E638D01377,
		0xBE5466CF34E90C6C,
		0xC0AC29B7C97C50DD,
		0x3F84D5B5B5470917
	};

	mix512x(y,x[3],x[0],x[2],x[7],x[4],x[5],x[6],x[1],56,48, 8, 8,48,56,48,40);
	mix512a(y,x[1],x[2],x[6],x[3],x[0],x[7],x[5],x[4],24,56,32, 8,32,56,24, 8);
	mix512x(y,x[0],x[4],x[7],x[1],x[5],x[6],x[2],x[3],16,32,48,16,24,48,48, 8);
	mix512a(y,x[7],x[6],x[3],x[5],x[1],x[2],x[4],x[0],56, 8,56,56,56,48,32,24);
	mix512x(y,x[4],x[1],x[5],x[2],x[3],x[0],x[7],x[6],48,16,32,32,24,56,56,16);
	mix512a(y,x[2],x[7],x[4],x[0],x[6],x[3],x[1],x[5],32,56, 8,56,32,32,24,56);
	mix512x(y,x[6],x[5],x[0],x[4],x[2],x[1],x[3],x[7], 8,32,40,56,48,48,56,48);
	mix512a(y,x[5],x[3],x[1],x[6],x[7],x[4],x[0],x[2], 8, 8, 8, 8, 8, 8, 8, 8);

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

	mix512x(x,y[0],y[2],y[4],y[1],y[5],y[7],y[6],y[3],48,56,24,40,48,48,40,24);
	mix512a(x,y[2],y[7],y[5],y[6],y[0],y[1],y[3],y[4],24,32,48, 8,40, 8, 8,24);
	mix512x(x,y[4],y[3],y[0],y[7],y[6],y[2],y[5],y[1], 8,16,40, 8,32,16,32,32);
	mix512a(x,y[7],y[4],y[3],y[0],y[1],y[6],y[2],y[5], 8,32,48,16,16,32, 8,40);
	mix512x(x,y[5],y[1],y[6],y[2],y[3],y[4],y[0],y[7],32,24,48,16, 8,48, 8,16);
	mix512a(x,y[6],y[0],y[7],y[3],y[4],y[5],y[1],y[2],40,48,24, 8,16, 8,40,24);
	mix512x(x,y[1],y[6],y[2],y[5],y[7],y[3],y[4],y[0], 8,32,16,48, 8,48,48, 8);

	x[0] += y[3];
	x[1] += y[5];
	x[2] += y[1];
	x[3] += y[4];
	x[4] += y[2];
	x[5] += y[0];
	x[6] += y[7];
	x[7] += y[6];
}


static inline void taricha512_block(const uint8_t in[64],
		struct taricha512_state *s)
{
	uint64_t *a = (uint64_t *)in;
	uint64_t x[8];
	int i;

	for (i=0; i<8; i++)
	{
		x[i] = s->s[i] ^ correct_bytes(a[i]);
	}

	taricha512_transform(x);

	for (i=0; i<8; i++)
	{
		s->s[i] ^= x[i];
	}
}

struct taricha512_state taricha512_init()
{
	struct taricha512_state s = {
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

void taricha512_append(const uint8_t *in, size_t length,
		struct taricha512_state *s)
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

			taricha512_block(s->buffer, s);

			in += s-> buffer_free;
			length -= s->buffer_free;

			s->buffer_free = 64;
		}
	}
	while (length >= 64)
	{
		taricha512_block(in, s);

		in += 64;
		length -= 64;
	}
	if (length > 0)
	{
		memcpy(s->buffer, in, length);
		s->buffer_free = 64-length;
	}
}

size_t taricha512_finalize(uint8_t *out, size_t length,
		struct taricha512_state *s)
{
	int i;
	memset(s->buffer+(64-s->buffer_free), s->buffer_free, s->buffer_free);
	taricha512_block(s->buffer, s);

	if (length > 64)
		length = 64;

	for (i=0; i<8; i++)
	{
		s->s[i] = correct_bytes(s->s[i]);
	}

	memcpy(out, s->s, length);

	return length;
}

size_t taricha512_full(const uint8_t *in, uint8_t *out,
		size_t in_length, size_t out_length)
{
	struct taricha512_state s = taricha512_init();
	taricha512_append(in, in_length, &s);
	return taricha512_finalize(out, out_length, &s);
}

uint64_t taricha512_64_finalize(struct taricha512_state *s)
{
	uint64_t x[8];
	int i;

	memset(s->buffer+(64-s->buffer_free), s->buffer_free, s->buffer_free);

	for (i=0; i<8; i++)
	{
		x[i] = s->s[i] ^ s->buffer[i];
	}

	taricha512_transform(x);

	return s->s[0] ^ x[0];
}

uint64_t taricha512_64_full(const uint8_t *in, size_t in_length)
{
	struct taricha512_state s = taricha512_init();
	taricha512_append(in, in_length, &s);
	return taricha512_64_finalize(&s);
}
