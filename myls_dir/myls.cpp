#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <boost/filesystem.hpp>



using namespace std;
bool L = false;
bool f = false;
bool R = false;
bool r = false;
bool double_dash = false;
bool sort_file = false;

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


}

bool sort_n(boost::filesystem::path file, boost::filesystem::path file2){
    return (file.filename()<file2.filename());
}

bool sort_x(boost::filesystem::path file, boost::filesystem::path file2){
    return (boost::filesystem::extension(file)< boost::filesystem::extension(file2));
}



int files_ls( boost::filesystem::path file){
    if ( boost::filesystem::exists(file) ){

        if(L){
            // long listing
            if(boost::filesystem::is_directory(file)){
                cout << file.filename() << "     " << "0" << " "
                     << boost::filesystem::last_write_time(file) << endl;
            } else {

                cout << file.filename() << "     " << boost::filesystem::file_size(file) << " "
                     << boost::filesystem::last_write_time(file) << endl;
            }
            return 0;
        } else{

            cout<<file.filename()<<endl;
        }

    }

    else{
        std::cout << "Can't find this file!" << std::endl;
        return 1;
    }
    return 0;
}

int directory_ls(boost::filesystem::path file){
    //cout<<"fund"<<endl;
    if (R) {
        boost::filesystem::recursive_directory_iterator end;

        for (boost::filesystem::recursive_directory_iterator i(file); i != end; ++i) {
            boost::filesystem::path current_file = i->path();

            int merrno=files_ls(current_file);
            return merrno;
        }
    }else{

        boost::filesystem::directory_iterator end_itr{};

    // ЦІ рядки дають помилку
       // boost::filesystem::recursive_directory_iterator end_iter;
        for (auto itr = boost::filesystem::directory_iterator(file); itr != end_itr; ++itr) {

            boost::filesystem::path current_file = itr->path();

            int merrno=files_ls(current_file);
            return merrno;
        }
    }
    return 0;
}

int is_obj(boost::filesystem::path p){
    int merrno = 0;
    cout<<"ebat"<<endl;

    if(boost::filesystem::is_directory(p)){
        merrno=directory_ls(p);
        if(merrno!=0){ return merrno;}
    } else{
        cout<<"ebat kolo"<<endl;
        merrno=files_ls(p);
        if(merrno!=0){ return merrno;}
    }
    return 0;
}

int listing(vector<boost::filesystem::path> &files){
    int merrno = 0;
    cout<<"ebat kopat"<<endl;
    for(auto file : files){
        cout<<"typo klasnyi kod"<<endl;
        merrno=is_obj(file);
        if(merrno!=0){ return merrno;}
    }

    return 0;
}

int main(int argc, char *argv[]) {


    string cur;
    int merrno = 0;

    vector<boost::filesystem::path> files;


    if (argc==1){

        merrno=directory_ls(".");
        if(merrno!=0){ return merrno;}
    }
    //if [-h |--help] - print help and exit with errno 0
    for(int i =1; i<argc; i++){
        string stmp{argv[i]};

        boost::filesystem::path p{stmp};
        if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")){
            if(double_dash){
                files.emplace_back(p);
            }else {
                help();

                return 0;
            }
        } else if(!strcmp(argv[i],"-F")){
            if(double_dash){
                files.emplace_back(p);
            }else{
            // крім того, що перед директоріями виводить /, вказує і типи спеціальних файлів
                 f = true;}
        } else if(!strcmp(argv[i],"-R")){
            if(double_dash){
                files.emplace_back(p);
            }else{
            // recursive dir listing
                R = true;}
        }else if(!strcmp(argv[i],"--")){
            if(double_dash){
                files.emplace_back(p);
            }else{
                double_dash= true;}

        } else if(!strcmp(argv[i],"-l")){
            if(double_dash){
                files.emplace_back(p);
            }else{
            // long listing
                L= true;
            }

        }else if(!strcmp(argv[i],"-r")){
            if(double_dash){
                files.emplace_back(p);
            }else{
            // reverse listing
                r= true;}

        }else if(!strcmp(argv[i],"--sort")){
            if(double_dash){
                files.emplace_back(p);
            }else{
            // sorting with param
                sort_file= true;}

        }else{
            files.emplace_back(p);
        }
    }
    cout<<"ebis veselis"<<endl;
    merrno=listing(files);
    if(merrno!=0){ return merrno;}





    return merrno;
}