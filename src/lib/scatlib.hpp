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

#ifndef DSCAT_LIB_SCATLIB_HPP
#define DSCAT_LIB_SCATLIB_HPP

#include <string>
#include <vector>
#include "hash.hpp"

namespace dscat {

    class scatlib {

    public:

        template <typename T>
        int makeMasks(std::vector<T>& ma, int count) {

            // allocate work
            const int maxbits = sizeof(T) * 8;
            std::vector<T> lm(count, 0);

            // bound check
            if (count > maxbits || count < 2) return -1;

            // make mask
            int mi = 0;
            int rv = 1;
            for (int i = 0; i < maxbits; i++) {
                lm[mi] += rv;
                if (mi == count - 1) mi = 0; else mi++;
                rv <<= 1;
            }

            // succeeded
            ma.clear();
            std::copy(lm.begin(), lm.end(), std::back_inserter(ma));
            return 0;

        }

        typedef struct {
            char s = 'D';
            char i = 'S';
            char g = 'C';
            char n = 'T';
            uint64_t filesize = 0;
            char hash[64] = {0};
        } block_meta;

        typedef std::vector<uint8_t> blocks;
        int scatString(std::vector<char>& src, std::vector<uint8_t >& maskarray, std::vector<std::string>& output) {

            // init
            auto apply_output = [&output](blocks bufs) {
                int idx = 0;
                for (auto c : bufs) {
                    output[idx++] += c;
                }
            };

            // calc hash
            std::string hash;
            if (!dscat::computeHash(src, hash)) return -2; // hashing error

            // make meta
            block_meta meta;
            meta.filesize = src.size();
            std::memcpy(meta.hash, &hash[0], 64);

            // adding meta
            std::vector<char> vmeta(sizeof(block_meta));
            std::memcpy(&vmeta[0], &meta, sizeof(block_meta));
            src.resize(src.size() + sizeof(block_meta));
            src.insert(src.begin(), vmeta.begin(), vmeta.end());

            std::ofstream test("/tmp/of");
            test << std::string(src.begin(), src.end());
            test.close();

            // bound check
            if (output.size() != maskarray.size()) return -1; // size mismatch

            // scatting
            int rotate = 0;
            blocks init(maskarray.size(), 0);
            blocks buf(maskarray.size(), 0);
            for (std::string::iterator it = src.begin(); it != src.end(); ++it ) {

                int idx = 0 + rotate;
                for (auto mask : maskarray) {
                    buf[idx] |= mask & *it;
                    if (++idx == maskarray.size()) idx = 0;
                }
                if (++rotate == maskarray.size()) {
                    apply_output(buf);
                    buf = init;
                    rotate = 0;
                }

            }
            if (rotate != 0) apply_output(buf);

            // succeeded
            return 0;

        }

        int gatherString(std::vector<std::string>& srcs, int destlen, std::vector<uint8_t >& maskarray, std::vector<char>& dest) {

            // check size
            int baselen = srcs[0].length();
            for (auto s : srcs) {
                if (s.length() != baselen) return -1; // illegal file error
            }

            // gathering
            for (int i = 0; i < srcs[0].length(); i++) {

                int rotate = 0;
                blocks init(maskarray.size(), 0);
                blocks buf(maskarray.size(), 0);
                for (int k = 0; k < maskarray.size(); k++) {

                    int idx = 0 + rotate;
                    for (auto mask : maskarray) {
                        buf[k] |= srcs[idx][i] & mask;
                        if (++idx == maskarray.size()) idx = 0;
                    }
                    if (++rotate == maskarray.size()) rotate = 0;

                }
                for (auto c : buf) {
                    dest.push_back(static_cast<char>(c));
                }
                buf = init;

            }

            // get metadata
            if (destlen <= sizeof(block_meta)) return -1; // illegal file error
            uint64_t    m_fs = reinterpret_cast<block_meta*>(&dest[0])->filesize;
            std::string m_hash(reinterpret_cast<block_meta*>(&dest[0])->hash, 64);

            // trim data
            dest.erase(dest.begin(), dest.begin() + sizeof(block_meta));
            dest.erase(dest.begin() + m_fs, dest.end());

            // check data
            std::string hash;
            if (!dscat::computeHash(dest, hash)) return -1; // hashing error
            if (hash.compare(m_hash) != 0) return -2; // hash mismatch

            // succeeded
            return 0;

        }

    };

} //  ns::dscat

#endif //DSCAT_LIB_SCATLIB_HPP
