#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <wordexp.h>
#include <iostream>


#include <boost/filesystem.hpp>



using namespace std;
bool R = false;
bool f = false;

bool get_user_answer(string question) {
    string answer;
    while (answer != "Y" && answer != "y" && answer != "n" && answer != "N") {
        cout << question << " ? Y/N :";
        getline(cin, answer);
    }
    if (answer == "Y") {
        return true;
    }
    return false;
}

int rm_dir(string file){
    // check if file exist
    string check;
    if ( boost::filesystem::exists(file ) ){
        if(f){
            boost::filesystem::remove_all( file);
            return 0;
        }

        if(get_user_answer("Are you sure")){
            boost::filesystem::remove_all( file);
            return 0;
        }
    }

    else{
        std::cout << "Can't find this dir!" << std::endl;
        return 1;
    }
    return 0;
}

int rm_file(string file){
    string check;
    if ( boost::filesystem::exists(file ) ){
        if(f){
            boost::filesystem::remove( file);
                return 0;
        }

        if(get_user_answer("Are you sure")){
            boost::filesystem::remove( file);
            return 0;
        }

    }

    else{
        std::cout << "Can't find this file!" << std::endl;
        return 1;
    }
    return 0;
}


void help() {
    cout<<"myrm: usage: myrm [-h|--help] [-f] [-R] <file1> <file2> <file3>"<<endl;
    cout<<"removes files or dirs"<<endl;
    cout<<"-h|--help - info"<<endl;
    cout<<"-f - removes without check"<<endl;
    cout<<"-R - removes dir"<<endl;}

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
        } else if(!strcmp(argv[i],"-f")){
            f = true;
        } else if(!strcmp(argv[i],"-R")){
            R = true;
        } else{
            //files.push_back(params[i]);
            if(R){
                merrno = rm_dir(argv[i]);
                if(merrno!=0){ return merrno;}
            } else{
                merrno=rm_file(argv[i]);
                if(merrno!=0){ return merrno;}
            }
        }
    }





    return merrno;
}