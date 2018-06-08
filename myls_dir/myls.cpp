#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>



using namespace std;
bool D = false;
bool L = false;
bool f = false;
bool R = false;
bool r = false;
bool S = false;
bool U = false;
bool t = false;
bool X = false;
bool N = true;
bool s = false;
bool double_dash = false;
bool sort_file = false;
namespace fs = boost::filesystem;
fs::path curr{"."};

void help() {
    cout<<"myls: usage: myls [path|mask] [-l] [-h|--help] [--sort=U|S|t|X|D|s] [-r]"<<endl;
    cout<<"List information about the FILEs (the current directory by default)."<<endl;
    cout<<"-h|--help - info"<<endl;
    cout<<"-l - use a long listing format"<<endl;
    cout<<"-r - reverse order while sorting"<<endl;
    cout<<"--sort=U|S|t|X|D|s - sorting"<<endl;
}

bool sort_t(fs::path file, fs::path file2){
    if(D){
        if(fs::is_regular_file(file) && fs::is_directory(file2)){
            return false;
        }
        else if(fs::is_regular_file(file2) && fs::is_directory(file)){
            return true;
        }
    }

    return(fs::last_write_time(file)<fs::last_write_time(file2));
}

bool sort_s(fs::path file, fs::path file2){
    if(D){
        if(fs::is_regular_file(file) && fs::is_directory(file2)){
            return false;
        }
        else if(fs::is_regular_file(file2) && fs::is_directory(file)){
            return true;
        }
    }
    int size1 = 0;
    int size2 = 0;

    if(!fs::is_directory(file)){
      size1=(int) fs::file_size(file);
    }
    if(!fs::is_directory(file2)){
        size2=(int) fs::file_size(file2);
    }

    return (size1<size2);
}

bool sort_n(fs::path file, fs::path file2){
    if(D){
        if(fs::is_regular_file(file) && fs::is_directory(file2)){
            return false;
        }
        else if(fs::is_regular_file(file2) && fs::is_directory(file)){
            return true;
        }
    }
    return (file.filename().string()<file2.filename().string());
}

bool sort_x(fs::path file, fs::path file2){
    if(D){
        if(fs::is_regular_file(file) && fs::is_directory(file2)){
            return false;
        }
        else if(fs::is_regular_file(file2) && fs::is_directory(file)){
            return true;
        }
    }
    return (fs::extension(file)< fs::extension(file2));
}
void sort_files_by(vector<fs::path> &paths){
    if(U){N= false;}
    else if(S){
        sort(paths.begin(), paths.end(), sort_s);
    } else if(t){
        sort(paths.begin(), paths.end(), sort_t);

    }else if(N){
        sort(paths.begin(), paths.end(), sort_n);

    }else if(X) {
        sort(paths.begin(), paths.end(), sort_x);
    }}

bool is_executable(const char *file)
{
    struct stat  st;


    if ((st.st_mode & S_IEXEC) != 0)
        return true;
    return false;
}

bool is_socket(const char *file)
{
    struct stat  st;
    stat(file, &st);

    if (S_ISSOCK(st.st_mode) == 0) {
        return true;
    }
    return false;
}

bool is_pipe(const char *file)
{
    struct stat  st;
    stat(file, &st);

    if (S_ISFIFO(st.st_mode) == 0) {
        return true;
    }
    return false;
}


bool is_symlink(const char *file)
{
    struct stat  st;
    stat(file, &st);

    if (S_ISLNK(st.st_mode) == 0) {
        return true;
    }
    return false;
}


void special_ls(fs::path file ){
    time_t t = fs::last_write_time(file);
    int size = 0;
    string s = "";
    if(fs::is_directory(file)){
        s = "/";
    }else if(is_executable(file.filename().string().c_str())){
        s = "*";
    }
    else if(is_socket(file.filename().string().c_str())){
        s = "=";
    }
    else if(is_pipe(file.filename().string().c_str())){
        s = "|";
    }
    else if(is_symlink(file.filename().string().c_str())){
        s = "@";
    }else if(!fs::is_regular_file(file)){
        s = "?";
    }



    if(fs::is_regular_file(file)){
        size=(int) fs::file_size(file);

    }

    cout << s;
    cout<< file.filename().string();
    if(L) {
        cout << "       " << size << "   "
             << ctime(&t);
    } else{cout<<endl;}

}


int files_ls( fs::path file){
    if ( fs::exists(file) ){

        special_ls(file);
    }

    else{
        std::cout << "Can't find this file!" << std::endl;

    }
    return 0;
}

int directory_ls(fs::path file){
    vector<fs::path> files;
    //cout<<"fund"<<endl;
    if (R) {
        cout<<file.filename().string()<<":"<<endl;
        fs::recursive_directory_iterator end;

        for (fs::recursive_directory_iterator i(file); i != end; ++i) {
            fs::path current_file = i->path();

            files.push_back(current_file);
        }
    }else{

        fs::directory_iterator end_itr{};
        cout<<file.filename().string()<<":"<<endl;

        for (auto itr = fs::directory_iterator(file); itr != end_itr; ++itr) {

            fs::path current_file = itr->path();

            files.push_back(current_file);
        }
    }

    if(sort_file){
        sort_files_by(files);

    }

    for(auto file : files){
        files_ls(file);
    }

    return 0;
}

int is_obj(fs::path p){
    int merrno = 0;

    if(fs::is_directory(p)){
        merrno=directory_ls(p);
        if(merrno!=0){ return merrno;}
    } else{

        merrno=files_ls(p);
        if(merrno!=0){ return merrno;}
    }
    return 0;
}



int listing(vector<fs::path> &paths){
    int merrno = 0;


    for (auto path : paths) {
        merrno = is_obj(path);
        if(merrno!=0){ return merrno;}
    }

    return merrno;

}

int main(int argc, char *argv[]) {

    vector<fs::path> paths;
    string cur;
    int merrno = 0;

    vector<fs::path> files;

    //if [-h |--help] - print help and exit with errno 0
    for (int i = 1; i < argc; i++) {
        string stmp{argv[i]};

        fs::path p{stmp};
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                help();
                return 0;
            }
        } else if (!strcmp(argv[i], "-F")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // крім того, що перед директоріями виводить /, вказує і типи спеціальних файлів
                f = true;
            }
        } else if (!strcmp(argv[i], "-R")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // recursive dir listing

                R = true;
            }
        } else if (!strcmp(argv[i], "--")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                double_dash = true;
            }

        }else if (!strcmp(argv[i], "-D")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                D = true;
            }

        }
        else if (!strcmp(argv[i], "-l")) {
            L = true;
            if (double_dash) {
                paths.push_back(p);
            } else {
                // long listing
                L = true;
            }

        } else if (!strcmp(argv[i], "-r")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // reverse listing
                r = true;
            }

        } else if (!strcmp(argv[i], "--sort")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // sorting with param
                sort_file = true;
            }

        }else if (!strcmp(argv[i], "-S")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // sorting with param
                S = true;
            }

        }else if (!strcmp(argv[i], "-X")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // sorting with param
                X = true;
            }

        }else if (!strcmp(argv[i], "-t")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // sorting with param
                t = true;
            }

        }else if (!strcmp(argv[i], "-N")) {
            if (double_dash) {
                paths.push_back(p);
            } else {

                N = true;
            }

        }else if (!strcmp(argv[i], "-U")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                U = true;
            }

        }else if (!strcmp(argv[i], "-s")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // ToDo : special files goes separate
                s = true;
            }


        }
        else {


            string stmp{argv[i]};

            fs::path p{stmp};


            paths.push_back(p);
        }
    }
    if(paths.size()< 1){

        paths.push_back(curr);
    }
    merrno = listing(paths);

    return merrno;

}