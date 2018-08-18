/*
 * Copyright (c) 2018 https://github.com/dscat/cuitool
 *
 * Licensed under the MIT License: http://www.opensource.org/licenses/mit-license.php
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef DSCAT_LIB_BASE64_HPP
#define DSCAT_LIB_BASE64_HPP

#include <string>
#include <cassert>
#include <limits>
#include <cctype>

namespace dscat {

    static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char reverse_table[128] = {
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
            64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
            64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
            64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
    };

    int base64_encode(const std::string& bindata, std::string& output) {

        if (bindata.size() > (std::numeric_limits<std::string::size_type>::max() / 4u) * 3u) {
            return -1; // too large
        }

        const std::size_t binlen = bindata.size();
        std::string retval((((binlen + 2) / 3) * 4), '=');
        std::size_t outpos = 0;
        int bits_collected = 0;
        unsigned int accumulator = 0;
        const std::string::const_iterator binend = bindata.end();

        for (std::string::const_iterator i = bindata.begin(); i != binend; ++i) {
            accumulator = (accumulator << 8) | (*i & 0xffu);
            bits_collected += 8;
            while (bits_collected >= 6) {
                bits_collected -= 6;
                retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
            }
        }

        if (bits_collected > 0) {
            assert(bits_collected < 6);
            accumulator <<= 6 - bits_collected;
            retval[outpos++] = b64_table[accumulator & 0x3fu];
        }

        assert(outpos >= (retval.size() - 2));
        assert(outpos <= retval.size());
        output = retval;

        return 0;
    }

    int base64_decode(const std::string& ascdata, std::string& output) {

        std::string retval;
        const std::string::const_iterator last = ascdata.end();
        int bits_collected = 0;
        unsigned int accumulator = 0;

        for (std::string::const_iterator i = ascdata.begin(); i != last; ++i) {
            const int c = *i;
            if (std::isspace(c) || c == '=') {
                continue;
            }
            if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
                return -1; // wrong format
            }
            accumulator = (accumulator << 6) | reverse_table[c];
            bits_collected += 6;
            if (bits_collected >= 8) {
                bits_collected -= 8;
                retval += (char) ((accumulator >> bits_collected) & 0xffu);
            }
        }
        output = retval;
        return 0;
    }

} // ns::dscat

#endif //DSCAT_LIB_BASE64_HPP
