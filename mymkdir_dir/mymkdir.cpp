#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        cout << "A name of directory should be in input" << "\n";
        return 1;
    }
    vector <string> arg_vec;
    for (int i = 1; i < argc; i++) {
        arg_vec.push_back(string(argv[i]));
    }
    if (find(arg_vec.begin(), arg_vec.end(), "--help") != arg_vec.end() ||
        find(arg_vec.begin(), arg_vec.end(), "-h") != arg_vec.end()) {
        cout << "mymkdir [-h|--help] [-p]  <dirname>\n"
             << "-p  - creating all p\n"
             << " --help/-h     display this help and exit\n";
        return 0;
    }
    string dirr = arg_vec[arg_vec.size() - 1];
    if (find(arg_vec.begin(), arg_vec.end(), "-p") != arg_vec.end() && argc == 3) {
        const int dir_err = mkdir(dirr.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (dir_err == -1 && errno == EEXIST) { // Success
            return 0;
        } else if (dir_err == -1 && errno == ENOENT) {

            istringstream iss(dirr);
            vector <string> dirs;
            string dir;

            while (getline(iss, dir, '/')) {
                if (!dir.empty())
                    dirs.push_back(dir);
            }
            string p = dirs[0];
            int nerr = mkdir(p.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (errno == EEXIST) {
                int c = 0;
                while (c != dirs.size() - 1 && errno == EEXIST) {
                    p += "/" + dirs[c + 1];
                    nerr = mkdir(p.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                    c++;
                }
            }
            return 0;
        } else if (dir_err == -1) {
            cout << "Error." << "\n";
            return 1;
        }
    } else if (argc == 2) {
        const int dir_err = mkdir(dirr.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (dir_err == -1) {
            cout << "Wrong input." << "\n";
            return 1;
        }
    } else {
        cout << "Wrong input." << "\n";
        return 1;
    }
    return 0;
}