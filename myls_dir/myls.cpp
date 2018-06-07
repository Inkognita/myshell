#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <ctime>



using namespace std;
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
boost::filesystem::path curr{"."};

void help() {
    cout<<"myls: usage: myls [path|mask] [-l] [-h|--help] [--sort=U|S|t|X|D|s] [-r]"<<endl;
    cout<<"List information about the FILEs (the current directory by default)."<<endl;
    cout<<"-h|--help - info"<<endl;
    cout<<"-l - use a long listing format"<<endl;
    cout<<"-r - reverse order while sorting"<<endl;
    cout<<"--sort=U|S|t|X|D|s - sorting"<<endl;
}

bool sort_t(boost::filesystem::path file, boost::filesystem::path file2){


    return(boost::filesystem::last_write_time(file)<boost::filesystem::last_write_time(file2));
}

bool sort_s(boost::filesystem::path file, boost::filesystem::path file2){
    int size1 = 0;
    int size2 = 0;

    if(!boost::filesystem::is_directory(file)){
      size1=(int) boost::filesystem::file_size(file);
    }
    if(!boost::filesystem::is_directory(file2)){
        size2=(int) boost::filesystem::file_size(file2);
    }
    return (size1<size2);
}

bool sort_n(boost::filesystem::path file, boost::filesystem::path file2){
    return (file.filename().string()<file2.filename().string());
}

bool sort_x(boost::filesystem::path file, boost::filesystem::path file2){
    return (boost::filesystem::extension(file)< boost::filesystem::extension(file2));
}
void sort_files_by(vector<boost::filesystem::path> &paths){
    if(S){
        sort(paths.begin(), paths.end(), sort_s);
    } else if(t){
        sort(paths.begin(), paths.end(), sort_t);

    }else if(N){
        sort(paths.begin(), paths.end(), sort_n);

    }else if(X) {
        sort(paths.begin(), paths.end(), sort_x);
    }}

void special_ls(boost::filesystem::path file ){
    time_t t = boost::filesystem::last_write_time(file);
    int size = 0;
    string s = "";
    if(boost::filesystem::is_symlink(file)){
        s = "@";
    }
    else if(boost::filesystem::is_directory(file)){
        s = "/";
    }
    else if(!boost::filesystem::is_directory(file)){
        s = "";
    }

    if(boost::filesystem::is_regular_file(file)){
        size=(int) boost::filesystem::file_size(file);

    }

    cout << s;
    cout<< file.filename().string();
    if(L) {
        cout << "     " << size << " "
             << ctime(&t);
    } else{cout<<endl;}

}


int files_ls( boost::filesystem::path file){
    if ( boost::filesystem::exists(file) ){

        special_ls(file);
    }

    else{
        std::cout << "Can't find this file!" << std::endl;

    }
    return 0;
}

int directory_ls(boost::filesystem::path file){
    vector<boost::filesystem::path> files;
    //cout<<"fund"<<endl;
    if (R) {
        cout<<file.filename().string()<<":"<<endl;
        boost::filesystem::recursive_directory_iterator end;

        for (boost::filesystem::recursive_directory_iterator i(file); i != end; ++i) {
            boost::filesystem::path current_file = i->path();

            files.push_back(current_file);
        }
    }else{

        boost::filesystem::directory_iterator end_itr{};
        cout<<file.filename().string()<<":"<<endl;

        for (auto itr = boost::filesystem::directory_iterator(file); itr != end_itr; ++itr) {

            boost::filesystem::path current_file = itr->path();

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

int is_obj(boost::filesystem::path p){
    int merrno = 0;

    if(boost::filesystem::is_directory(p)){
        merrno=directory_ls(p);
        if(merrno!=0){ return merrno;}
    } else{

        merrno=files_ls(p);
        if(merrno!=0){ return merrno;}
    }
    return 0;
}



int listing(vector<boost::filesystem::path> &paths){
    int merrno = 0;


    for (auto path : paths) {
        merrno = is_obj(path);
        if(merrno!=0){ return merrno;}
    }

    return merrno;

}

int main(int argc, char *argv[]) {

    vector<boost::filesystem::path> paths;
    string cur;
    int merrno = 0;

    vector<boost::filesystem::path> files;

    //if [-h |--help] - print help and exit with errno 0
    for (int i = 1; i < argc; i++) {
        string stmp{argv[i]};

        boost::filesystem::path p{stmp};
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

        } else if (!strcmp(argv[i], "-l")) {
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
                // sorting with param
                N = true;
            }

        }else if (!strcmp(argv[i], "-U")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // sorting with param
                U = true;
            }

        }else if (!strcmp(argv[i], "-s")) {
            if (double_dash) {
                paths.push_back(p);
            } else {
                // sorting with param
                s = true;
            }


        }
        else {


            string stmp{argv[i]};

            boost::filesystem::path p{stmp};

            paths.push_back(p);
        }
    }
    if(paths.size()< 1){

        paths.push_back(curr);
    }
    merrno = listing(paths);

    return merrno;

}