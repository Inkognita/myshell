#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <regex>

namespace boost_po = boost::program_options;
using namespace std;

int line_f(string &line, string &ptrn, bool ic, bool invrt, bool regexi, bool file) {
    if (ic) {
        boost::algorithm::to_lower(ptrn);
        boost::algorithm::to_lower(line);
    }
    if (!regexi) {
        if ((line.find(ptrn) == string::npos)== invrt) {
            if (!file) cout << "Found: ";
            cout << line << endl;
            return 1;
        }
    } else {
        regex e (ptrn);
        if (regex_search(line, e) == !invrt) {
            if (!file) cout << "Found: ";
            cout << line << endl;
            return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        cerr << "Usage: grep [OPTION]... PATTERN [FILE]...\n"
             << "Try 'grep --help' for more information." << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> path_a;
    bool regexi, ic, fname, invrt, r = false;
    boost_po::variables_map vm;
    string ptrn, sfname;
    const char *divider_cmd = "-";

    boost_po::options_description help("Options");
    help.add_options()
            ("help,h", "Search for PATTERN in each FILE or standard input.\n"
                    "PATTERN is, by default, a basic regular expression (BRE).\n"
                    "Example: grep -i 'hello world' menu.h main.c\n"
                    "\n"
                    "Regexp selection and interpretation:\n"
                    "  --regexp              obtain PATTERN for matching"
                    "  --file=FILE           obtain PATTERN from FILE\n"
                    "  -i, --ignore-case         ignore case distinctions\n"
                    "\n"
                    "Miscellaneous:\n"
                    "  -v, --invert-match        select non-matching lines\n"
                    "  -h, --help                display this help text and exit\n"
                    "\n")
            ("ignore-case,i", "ignore case distinctions")
            ("file", boost_po::value<std::string>(), "obtain PATTERN from FILE")
            ("regexp", boost_po::value<std::string>(), "obtain PATTERN for matching")
            ("invert-match,v", "select non-matching lines");
    try {
        auto p = boost_po::command_line_parser(argc, argv).options(help).run();
        path_a = boost_po::collect_unrecognized(p.options, boost_po::include_positional);
        boost_po::store(p, vm);
        boost_po::notify(vm);
        if (vm.count("help")) {
            cout << help << endl;
            exit(EXIT_SUCCESS);
        } else {
            if (vm.count("invert-match")) {invrt = true; }
            if (vm.count("ignore-case")) {ic = true; }
            if (vm.count("file")) { sfname = vm["file"].as<string>(); fname = true; }
            if (vm.count("regexp")) { ptrn = vm["regexp"].as<string>(); regexi = true; }
        }
    } catch (boost_po::error &error) {
        cerr << "Something went wrong: " << error.what() << endl << help << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        string tmp(argv[i]);
        if (tmp.substr(0, 1) != divider_cmd) {
            if (r) { cerr << "One string to compare needed!" << endl; exit(EXIT_FAILURE); }
            if (regexi) { cerr << "Either regexp or string is needed!" << endl; exit(EXIT_FAILURE); }
            r = true;
            ptrn = tmp;
        }
    }
    int repeats = 1;
    if (fname) {
        ifstream input(sfname);
        if (!input.good()) { cerr << "Wrong name of file " << endl; exit(EXIT_FAILURE); }
        for (string line; getline(input, line);) { repeats += line_f(ref(line), ref(ptrn), ic, invrt, regexi, fname); }
    } else {
        for (string line; getline(cin, line);) { repeats += line_f(ref(line), ref(ptrn), ic, invrt, regexi, fname); }
    }
    if (repeats <= 1) { cout << "No repeats" << endl; }
    return 0;
}
