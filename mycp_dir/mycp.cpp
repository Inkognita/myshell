#include <iostream>
#include <fstream>
#include <experimental/filesystem>
//#include <boost/filesystem.hpp>

#include <string>
#include <stdio.h>
#include <vector>

namespace fs = std::experimental::filesystem;
using namespace std;
//namespace fs = boost::filesystem;


int get_user_answer(string question) {
    string answer;
    while (answer != "Y" && answer != "y" && answer != "n" && answer != "N" && answer != "A" && answer != "a" && answer != "c" && answer != "C") {
        cout << question << " ? Y/N/A/C :";
        getline(cin, answer);
    }
    if (answer == "Y") {
        return 1;
    }
    else if (answer == "C") {
        return 2;
    }
    else if (answer == "A") {
        return 3;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        cout << "cp: missing file operand\n"
                "Try 'cp --help' for more information.\n";
        exit(EXIT_FAILURE);
    }

    bool forced = false;
    bool recursive = false;

    vector<string> words;

    for (int i = 1; i < argc; ++i) {
        string cur = argv[i];
        if (cur == "-h" || cur == "--help") {
            cout << "mycp [-h|--help] [-f] <file1> <file2> -- when copying the only file with\n"
                    "\t\t the other name\n"
                    "mycp [-h|--help] [-f] <file1> <file2> <file3>... <dir>] -- when copying\n"
                    "\t\t man files to the dir\n"
                    "mycp [-h|--help] [-f] -R  <dir_or_file_1> <dir_or_file_2> <dir_or_file_3>... <dir>\n"
                    "\t -- when copying dirs required argument is -R\n"
                    "-f\t -- means not to ask when overwriting the files\n";
            return 0;
        } else if (cur == "-f") {
            forced = true;
        } else if (cur == "-R") {
            recursive = true;
        } else {
            words.push_back(cur);
        }
    }

    if (words.size() == 2 && fs::is_regular_file(words[0]) && !fs::is_directory(words[1])) {
        if (fs::exists(words[1])) {
            int answ = get_user_answer("Overwrite the dir " + words[1]);
            if (forced || answ == 1) {
                fs::copy_file(words[0], words[1], fs::copy_options::overwrite_existing);
            }else if (answ== 3){
                forced = true;
            }else if (answ== 2){
                return 0;
            }
        } else {
            fs::copy_file(words[0], words[1]);
        }
    } else {
        string last_word = words[words.size() - 1];
        if (!fs::exists(last_word) || fs::is_directory(last_word)) {
            if (!fs::exists(last_word)) {
                fs::create_directory(last_word);
            }
            for (int i = 0; i < words.size() - 1; i++) {
                if (!fs::exists(words[i])) {
                    cerr << words[i] << " no such file or directory" << endl;
                    exit(EXIT_FAILURE);
                }
                if (fs::is_directory(words[i])) {
                    string path_to_create = fs::path(last_word) / words[i] / "";
                    if (recursive) {
                        if (fs::exists(path_to_create)) {
                            int answ = get_user_answer("Overwrite the dir " + path_to_create);
                            if (forced || answ == 1) {
                                fs::copy(words[i], path_to_create,
                                         fs::copy_options::overwrite_existing | fs::copy_options::recursive);
                            }else if (answ== 3){
                                forced = true;
                            }else if (answ== 2){
                                return 0;
                            }
                        } else {
                            fs::copy(words[i], path_to_create, fs::copy_options::recursive);
                        }
                    } else {
                        cerr << words[i] << " is a directory" << endl;
                        exit(EXIT_FAILURE);
                    }
                } else if (fs::is_regular_file(words[i])) {
                    string path_to_create = fs::path(last_word) / words[i];
                    if (fs::exists(path_to_create)) {
                        int answ = get_user_answer("Overwrite the dir " + path_to_create);
                        if (forced || answ == 1) {
                            fs::copy_file(words[i], path_to_create, fs::copy_options::overwrite_existing);
                        }else if (answ== 3){
                            forced = true;
                        }else if (answ== 2){
                            return 0;
                        }
                    } else {
                        fs::copy_file(words[i], path_to_create);
                    }
                }
            }
        } else {
            cerr << last_word << " exists and is not a dir" << endl;
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
