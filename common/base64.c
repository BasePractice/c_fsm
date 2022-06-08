/*
 * base64.c: Base64 converting functions
 *
 * Copyright (C) 2001, 2002  Juha Yrjölä <juha.yrjola@iki.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static const uint8_t base64_table[66] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
        "0123456789+/=";

static const uint8_t bin_table[128] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xE0, 0xD0, 0xFF, 0xFF, 0xD0, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xF2, 0xFF, 0x3F,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
        0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF,
        0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
        0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
        0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
        0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
        0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static void to_base64(unsigned int i, uint8_t *out, unsigned int fillers) {
    unsigned int s = 18, c;

    for (c = 0; c < 4; c++) {
        if (fillers >= 4 - c)
            *out = base64_table[64];
        else
            *out = base64_table[(i >> s) & 0x3f];
        out++;
        s -= 6;
    }
}

static int from_base64(const char *in, unsigned int *out, int *skip) {
    unsigned int res = 0, c, s = 18;
    const char *in0 = in;

    for (c = 0; c < 4; c++, in++) {
        uint8_t b;
        int k = *in;

        if (k < 0 || k >= (int) sizeof(bin_table))
            return -1;
        if (k == 0 && c == 0)
            return 0;
        b = bin_table[k];
        if (b == 0xC0) /* '=' */
            break;
        switch (b) {
            case 0xD0: /* '\n' or '\r' */
                c--;
                continue;
        }
        if (b > 0x3f)
            return -1;

        res |= b << s;
        s -= 6;
    }
    *skip = in - in0;
    *out = res;
    return c * 6 / 8;
}

size_t base64_encode(const uint8_t *in, size_t len, uint8_t *out, size_t out_len, int line_length) {
    unsigned int chars = 0;
    unsigned int i, c;
    uint8_t *offset = out;

    line_length -= line_length & 0x03;
    while (len >= 3) {
        i = in[2] + (in[1] << 8) + (in[0] << 16);
        in += 3;
        len -= 3;
        if (out_len < 4)
            return 0;
        to_base64(i, out, 0);
        out += 4;
        out_len -= 4;
        chars += 4;
        if (chars >= line_length && line_length > 0) {
            if (out_len < 1)
                return 0;
            *out = '\n';
            out++;
            out_len--;
            chars = 0;
        }
    }
    i = c = 0;
    while (c < len)
        i |= ((unsigned int) *in++) << ((2 - c++) << 3);
    if (len) {
        if (out_len < 4)
            return 0;
        to_base64(i, out, 3 - len);
        out += 4;
        out_len -= 4;
        chars += 4;
    }
    if (chars && line_length > 0) {
        if (out_len < 1)
            return 0;
        *out = '\n';
        out++;
        out_len--;
    }
    if (out_len < 1)
        return 0;
    *out = 0;

    return  out - offset;
}

int base64_decode(const char *in, uint8_t *out, size_t *out_len) {
    int len = 0, r, skip;
    unsigned int i;
    uint8_t *offset = out;

    while ((r = from_base64(in, &i, &skip)) > 0) {
        int finished = 0, s = 16;

        if (r < 3)
            finished = 1;
        while (r--) {
            if ((*out_len) <= 0)
                return 0;
            *out++ = i >> s;
            s -= 8;
            (*out_len)--;
            len++;
        }
        in += skip;
        if (finished || *in == 0) {
            (*out_len) = out - offset;
            return 1;
        }
    }
    if (r == 0) {
        (*out_len) = out - offset;
        return 1;
    }
    return 0;
}
