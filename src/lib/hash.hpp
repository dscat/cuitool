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

#ifndef DSCAT_HASH_HPP
#define DSCAT_HASH_HPP

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <openssl/evp.h>

namespace dscat {

    bool computeHash(const std::vector<char>& source, unsigned char(&hash)[64]) {
        bool success = false;

#ifdef OPENSSL_COMPATIBLE_11
        EVP_MD_CTX *context = EVP_MD_CTX_new();
#elif OPENSSL_COMPATIBLE_10
        EVP_MD_CTX *context = EVP_MD_CTX_create();
#else
#error must be defined OPENSSL_COMPATIBLE_10 or OPENSSL_COMPATIBLE_11.
#endif

        if (context != NULL) {
            if (EVP_DigestInit_ex(context, EVP_sha256(), NULL)) {
                if (EVP_DigestUpdate(context, &source[0], source.size())) {
                    unsigned int lengthOfHash = 0;
                    if (EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
                        success = true;
                    }
                }
            }

#ifdef OPENSSL_COMPATIBLE_11
            EVP_MD_CTX_free(context);
#elif OPENSSL_COMPATIBLE_10
            EVP_MD_CTX_destroy(context);
#else
#error must be defined OPENSSL_COMPATIBLE_10 or OPENSSL_COMPATIBLE_11.
#endif

        }

        return success;
    }

    bool computeHash(const std::vector<char> &source, std::string &vhash) {
        bool success = false;

#ifdef OPENSSL_COMPATIBLE_11
        EVP_MD_CTX *context = EVP_MD_CTX_new();
#elif OPENSSL_COMPATIBLE_10
        EVP_MD_CTX *context = EVP_MD_CTX_create();
#else
#error must be defined OPENSSL_COMPATIBLE_10 or OPENSSL_COMPATIBLE_11.
#endif

        if (context != NULL) {
            if (EVP_DigestInit_ex(context, EVP_sha256(), NULL)) {
                if (EVP_DigestUpdate(context, &source[0], source.size())) {
                    unsigned char hash[EVP_MAX_MD_SIZE];
                    unsigned int lengthOfHash = 0;
                    if (EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
                        std::stringstream ss;
                        for (unsigned int i = 0; i < lengthOfHash; ++i) {
                            ss << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];
                        }
                        vhash = ss.str();
                        success = true;
                    }
                }
            }

#ifdef OPENSSL_COMPATIBLE_11
            EVP_MD_CTX_free(context);
#elif OPENSSL_COMPATIBLE_10
            EVP_MD_CTX_destroy(context);
#else
#error must be defined OPENSSL_COMPATIBLE_10 or OPENSSL_COMPATIBLE_11.
#endif

        }

        return success;
    }


    bool computeHash(const std::string &source, std::string &vhash) {
        bool success = false;

#ifdef OPENSSL_COMPATIBLE_11
        EVP_MD_CTX *context = EVP_MD_CTX_new();
#elif OPENSSL_COMPATIBLE_10
        EVP_MD_CTX *context = EVP_MD_CTX_create();
#else
#error must be defined OPENSSL_COMPATIBLE_10 or OPENSSL_COMPATIBLE_11.
#endif

        if (context != NULL) {
            if (EVP_DigestInit_ex(context, EVP_sha256(), NULL)) {
                if (EVP_DigestUpdate(context, source.c_str(), source.length())) {
                    unsigned char hash[EVP_MAX_MD_SIZE];
                    unsigned int lengthOfHash = 0;

                    if (EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
                        std::stringstream ss;
                        for (unsigned int i = 0; i < lengthOfHash; ++i) {
                            ss << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];
                        }

                        vhash = ss.str();
                        success = true;
                    }
                }
            }

#ifdef OPENSSL_COMPATIBLE_11
            EVP_MD_CTX_free(context);
#elif OPENSSL_COMPATIBLE_10
            EVP_MD_CTX_destroy(context);
#else
#error must be defined OPENSSL_COMPATIBLE_10 or OPENSSL_COMPATIBLE_11.
#endif

        }

        return success;
    }

} // ns::dscat

#endif //DSCAT_HASH_HPP
