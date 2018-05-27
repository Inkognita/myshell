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

int files_ls( boost::filesystem::path file){
    if ( boost::filesystem::exists(file) ){

        if(L){
            // long listing
            if(boost::filesystem::is_directory(file)){
                cout << file << "     " << "0" << " "
                     << boost::filesystem::last_write_time(file) << endl;
            } else {

                cout << file << "     " << boost::filesystem::file_size(file) << " "
                     << boost::filesystem::last_write_time(file) << endl;
            }
            return 0;
        } else{

            cout<<file<<endl;
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

    boost::filesystem::directory_iterator end_itr{};
    //cout<<"kkk"<<endl;

//    cout <<"j"<< file.filename() << endl;
    cout << "name: " << file.string() << endl;

    cout << boost::filesystem::exists(file) << endl;
    cout << boost::filesystem::is_regular_file(file) << endl;
    cout << boost::filesystem::is_directory(file) << endl;

   // boost::filesystem::recursive_directory_iterator itr(file);
// ЦІ рядки дають помилку
   // boost::filesystem::recursive_directory_iterator end_iter;
    for (auto itr = boost::filesystem::directory_iterator(file); itr != end_itr; ++itr) {
        cout << itr->path().string() << endl;
////        cout<<"ppp"<<endl;
        boost::filesystem::path current_file = itr->path();
        cout<<current_file<<endl;
        int merrno=files_ls(current_file);
        return merrno;
    }
    return 0;
}


int main(int argc, char *argv[]) {


    string cur;
    int merrno = 0;


    if (argc==1){
        cout<<"invalid number of args"<<endl;

        return 1 ;
    }
    //if [-h |--help] - print help and exit with errno 0
    for(int i =1; i<argc; i++){
        if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")){
            help();
            return 0;
        } else if(!strcmp(argv[i],"-F")){
            // крім того, що перед директоріями виводить /, вказує і типи спеціальних файлів
            f = true;
        } else if(!strcmp(argv[i],"-R")){
            // recursive dir listing
            R = true;
        }else if(!strcmp(argv[i],"--")){
            double_dash= true;

        } else if(!strcmp(argv[i],"-l")){
            // long listing
            L= true;

        }else if(!strcmp(argv[i],"-r")){
            // reverse listing
            r= true;

        }else if(!strcmp(argv[i],"--sort")){
            // sorting with param
            sort_file= true;

        }else{
            // if contains "-", but not an option - pass

            //checking file or dir
            string stmp{argv[i]};
            cout << "|" << stmp << "|" << endl;
            boost::filesystem::path p{stmp};

            //cout << "DFD: " << stmp << endl;
            //cout<<p.filename().string()<< endl;
            cout << p.string() << endl;
            //merrno=files_ls(p);
            if(boost::filesystem::is_directory(p)){
                try {
                    boost::filesystem::directory_iterator end_itr{};
                    for (auto itr = boost::filesystem::directory_iterator(p);
                                    itr != end_itr; ++itr) {
                        cout << itr->path().string() << endl;
                        boost::filesystem::path current_file = itr->path();
                        cout<<current_file<<endl;
                        //int merrno=files_ls(current_file);
                        //return merrno;
                    }
//                    merrno = directory_ls(p);
#if 1
                }
                catch(boost::filesystem::filesystem_error& e)
                {
                    std::cerr << e.what() << std::endl;
                    std::cerr << e.code() << std::endl;
                    std::cerr << e.path1() << std::endl;
                    std::cerr << e.path2() << std::endl;
                    throw;
                }
#endif
                if(merrno!=0){ return merrno;}
            } else{

                merrno=files_ls(p);
                if(merrno!=0){ return merrno;}
            }
        }
    }





    return merrno;
}