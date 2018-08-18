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

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <istream>
#include <string>
#include <sstream>
#include <fstream>

#include "lib/clipp.h"
#include "lib/scatlib.hpp"
#include "lib/base64.hpp"
#include "lib/cuilog.hpp"
#include "lib/hash.hpp"

int main(int argc, char* argv[]) {

    // argv parse
    bool opt_scat = false, opt_gath = false, opt_cin = false, opt_verbose = false, opt_test = false;
    std::string opt_pieces = "", opt_output = "";
    int opt_piececnt = 0;
    auto cli = (
            (clipp::option("-s", "--scatting").set(opt_scat) |
             clipp::option("-g", "--gathering").set(opt_gath)) % "mode",
                    clipp::option("-c", "--count") & clipp::value("pieces", opt_piececnt) % "for scatting, count of pieces(1-8).",
                    clipp::option("-p", "--pieces") & clipp::value("pieces files", opt_pieces) % ("pieces file(s) comma split."),
                    clipp::option("-i", "--stdin").set(opt_cin, true).doc("input from stdin for -s."),
                    clipp::option("-o", "--output") & clipp::value("output file", opt_output) % "file name for -g.",
                    clipp::option("-v", "--verbose").set(opt_verbose).doc("verbose mode."),
                    clipp::option("-t", "--test").set(opt_test).doc("test mode (no output results).")
    );
    if (!clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]) << std::endl;
        exit(1);
    }
    std::vector<std::string> pieces;
    std::istringstream sspieces(opt_pieces);
    std::string buf;
    while (std::getline(sspieces, buf, ',')) {
        if (buf != "") pieces.push_back(buf);
    }
    if (opt_scat == opt_gath || pieces.size() != opt_piececnt
        || (opt_scat && (opt_piececnt < 1 || 8 < opt_piececnt))) {
        std::cout << clipp::make_man_page(cli, argv[0]) << std::endl;
        exit(1);
    }

    // switch logging
    opt_verbose ? cuilog::cout.enable() : cuilog::cout.disable();

    // banner
    cuilog::cout << cuilog::info("🐈 scatter v1.0") << std::endl;
    cuilog::cout << cuilog::info("Copytight (c) 2018 https://github.com/dscat/cuitool") << std::endl;
    cuilog::cout << cuilog::note("Starting ") << (opt_scat ? "scatting" : "gathering") << " mode with " << pieces.size()
                 << " pieces." << std::endl;
    if (opt_scat && opt_cin) cuilog::cout << cuilog::note("from stdin.") << std::endl;
    if (opt_scat && !opt_cin) cuilog::cout << cuilog::note("from ") << pieces.size() << " files." << std::endl;

    // read cin and calculate hash
    std::vector<char> data;
    std::string vhash;
    if (opt_cin) {
        // read
        std::cin >> std::noskipws;
        std::copy(std::istream_iterator<char>(std::cin), std::istream_iterator<char>(), std::back_inserter(data));
        cuilog::cout << cuilog::note("cin - size   : ") << data.size() << " byte(s)." << std::endl;

        // hash
        dscat::computeHash(data, vhash);
        cuilog::cout << cuilog::note("cin - sha256 : ") << vhash << std::endl;
    }


    // main
    auto scatlib = dscat::scatlib();
    if (opt_scat) {

        // make masks
        std::vector<uint8_t> ma;
        int ret = scatlib.makeMasks(ma, opt_piececnt);
        if (ret != 0) {
            cuilog::cout << cuilog::crit("Error has occurred - could not make masks.") << std::endl;
            return 1;
        }

        // scatting
        std::vector<std::string> output(opt_piececnt);
        ret = scatlib.scatString(data, ma, output);
        if (ret != 0) {
            cuilog::cout << cuilog::crit("Error has occurred - could not make pieces.") << std::endl;
            return 1;
        }

        // output
        int i = 1;
        for (auto p : output) {
            std::string b64, hash;
            ret = dscat::base64_encode(p, b64);
            ret = dscat::computeHash(p, hash);
            cuilog::cout << cuilog::note("Scatted #") << i++ << " : " << hash << std::endl;
            if (!opt_test) {
                if (pieces.size() == 0) std::cout << b64 << std::endl;
                else {
                    cuilog::cout << cuilog::note("Writing ") << pieces[i - 2] << "..." << std::endl;
                    std::ofstream ofile(pieces[i - 2]);
                    ofile << p;
                    ofile.close();
                }
            } else {
                cuilog::cout << cuilog::warn("Testing mode. No outputs.") << std::endl;
            }
        }
        cuilog::cout << cuilog::info("🐈 completed!") << std::endl;

    } else if (opt_gath) {

        // make masks
        std::vector<uint8_t> ma;
        int ret = scatlib.makeMasks(ma, opt_piececnt);
        if (ret != 0) {
            cuilog::cout << cuilog::crit("Error has occurred - could not make masks.") << std::endl;
            return 1;
        }

        // gathering
        std::vector<std::string> seps;
        for (auto sf : pieces) {
            std::ifstream ifs(sf);
            std::string ibuf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            seps.push_back(ibuf);
            cuilog::cout << cuilog::note("Loaded #") << seps.size() << " from " << sf << ". ( " << ibuf.size()
                         << " byte(s). )" << std::endl;
        }
        std::vector<char> out;
        ret = scatlib.gatherString(seps, 2336644, ma, out);
        if (ret == -1) {
            cuilog::cout << cuilog::crit("Error has occurred - some pieces has broken.") << std::endl;
            return 1;
        }
        if (ret == -2) {
            cuilog::cout << cuilog::crit("Error has occurred - hashing failed.") << std::endl;
            return 1;
        }
        if (ret == -3) {
            cuilog::cout << cuilog::crit("Error has occurred - hash mismatch.") << std::endl;
            return 1;
        }
        cuilog::cout << cuilog::note("Gathered ") << out.size() << " byte(s) file." << std::endl;

        // output
        if (!opt_test) {
            if (opt_output.size() != 0) {
                cuilog::cout << cuilog::note("Writing ") << opt_output << "..." << std::endl;
                std::ofstream ofile(opt_output);
                ofile << std::string(out.begin(), out.end());
                ofile.close();
            } else {
                std::cout << std::string(out.begin(), out.end());
            }
        } else {
            cuilog::cout << cuilog::warn("Testing mode. No outputs.") << std::endl;
        }
        cuilog::cout << cuilog::info("🐈 completed!") << std::endl;

    }
    return 0;
}
