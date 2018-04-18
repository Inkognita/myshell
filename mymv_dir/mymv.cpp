#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <stdio.h>
#include <vector>

namespace fs = std::experimental::filesystem;
using namespace std;


bool get_user_answer(string question) {
    string answer;
    while (answer != "Y" && answer != "N") {
        cout << question << " ? Y/N :";
        getline(cin, answer);
    }
    if (answer == "Y") {
        return true;
    }
    return false;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        cout << "mv: missing file operand\n"
                "Try 'mv --help' for more information.\n";
        exit(EXIT_FAILURE);
    }

    bool forced = false;

    vector<string> words;

    for (int i = 1; i < argc; ++i) {
        string cur = argv[i];
        if (cur == "-h" || cur == "--help") {
            cout << "mymv [-h|--help] [-f] <oldfile> <newfile>\n"
                    "\t -- changing file name or moving a single file\n"
                    "mymv [-h|--help] [-f] <oldfile_or_dir_1> "
                    "<oldfile_or_dir_oldfile2> <oldfile_or_dir_oldfile3>.... <dir>\n"
                    "\t -- moving multiple files to a single dir\n";
            return 0;
        } else if (cur == "-f") {
            forced = true;
        } else {
            words.push_back(cur);
        }
    }

    if (words.size() == 2 && fs::is_regular_file(words[0]) && !fs::is_directory(words[1])) {
        if (fs::exists(words[1])) {
            if (forced || get_user_answer("Overwrite the file " + words[1])) {
                fs::copy_file(words[0], words[1], fs::copy_options::overwrite_existing);
                fs::remove(fs::path(words[0]));
            }
        } else {
            fs::copy_file(words[0], words[1]);
            fs::remove(fs::path(words[0]));
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

                    if (fs::exists(path_to_create)) {
                        if (forced || get_user_answer("Overwrite the dir " + path_to_create)) {
                            fs::copy(words[i], path_to_create,
                                     fs::copy_options::overwrite_existing | fs::copy_options::recursive);
                            fs::remove_all(fs::path(words[i]));
                        }
                    } else {
                        fs::copy(words[i], path_to_create, fs::copy_options::recursive);
                        fs::remove_all(fs::path(words[i]));
                    }
                } else if (fs::is_regular_file(words[i])) {
                    string path_to_create = fs::path(last_word) / words[i];
                    if (fs::exists(path_to_create)) {
                        if (forced || get_user_answer("Overwrite the dir " + path_to_create)) {
                            fs::copy_file(words[i], path_to_create, fs::copy_options::overwrite_existing);
                            fs::remove(fs::path(words[i]));
                        }
                    } else {
                        fs::copy_file(words[i], path_to_create);
                        fs::remove(fs::path(words[i]));
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
