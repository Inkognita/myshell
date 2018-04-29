#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <wordexp.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <wordexp.h>
#include <string>
#include <stdlib.h>
#include <limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <map>
#include <sys/stat.h>


int ERRNO = 0;

// якщо баш запускають, хай чекає аргументи
#define BUFF_SIZE 10000
using namespace std;
extern char **environ;
map <string, string> vars_local;
map <string, string> vars_export;

std::vector<std::string> expand_env(const std::string &var) {
    std::vector<std::string> vars;

    wordexp_t p;
    if (!wordexp(var.c_str(), &p, 0)) {
        if (p.we_wordc){
            for (char **exp = p.we_wordv; *exp; ++exp)
            {
                vars.push_back(exp[0]);
            }
        }

        wordfree(&p);
    }

    return vars;
}

void mcd(vector<string> params) {
    ERRNO = 0;
    if (params.size() == 1) {
        cout << "invalid number of args" << endl;
        ERRNO = -1;
        return;
    }
    for (int i = 1; i < params.size(); i++) {
        if (params[i] == "-h" || params[i] == "--help") {
            printf("mcd: usage: mcd <path> [-h|--help]\n");
            printf("changes the directory\n");
            ERRNO = 0;
            return;
        }
    }
    string path = params[1];
    int correct = 1;
    correct = chdir(path.c_str());
    ERRNO = correct;
}

void mpwd(vector<string> params) {
    ERRNO = 0;
    for (int i = 1; i < params.size(); i++) {
        if (params[i] == "-h" || params[i] == "--help") {
            printf("mpwd: usage: mpwd [-h|--help]\n");
            printf("exits the shell\n");
        }
    }
    char sub_buf[BUFF_SIZE];
    getcwd(sub_buf, BUFF_SIZE);
    cout << sub_buf << endl;
}

string get_identifier() {
    char sub_buf[BUFF_SIZE];
    getcwd(sub_buf, BUFF_SIZE);
    return string(sub_buf) + "$ ";
}

int process_script_file(const string &filename);

void dot(vector<string> params){
    // To DO:  виконати його в поточному інтерпретаторі
    ERRNO = 0;
    if (params.size() == 1) {
        cerr << "No arguments given" << endl;
        ERRNO = 1;
        return;
    }
    if(process_script_file(params[1]) > 0) {
        ERRNO = 1;
    }
}


void mecho(vector<string> args){
    // вивід вмісту змінної
    ERRNO = 0;
    int i = 1;
    while (i < args.size() - 1) {
        cout << args[i] << " ";
        i++;
    }
    if(args.size() > 1) {
        cout << args[args.size() - 1];
    }
    cout << endl;
}


void mexport(string param){
    // To DO:  додати змінну до блоку змінних середовища дочірнього процесу
}


void mexit(vector<string> params) {
    ERRNO = 0;
    if (params.size() == 1) {
        exit(0);
    }
    for (int i = 1; i < params.size(); i++) {
        if (params[i] == "-h" || params[i] == "--help") {
            printf("mexit: usage: mexit [exit code] [-h|--help]\n");
            printf("prints working directory name\n");
            return;
        }
    }
    if (!(std::find_if(params[1].begin(),
                      params[1].end(),
                      [](char c) { return !std::isdigit(c); }) == params[1].end()
            )) {
        ERRNO = -1;
        cout << "Code must be a digit" << endl;
        return;
    }
    int code = atoi(params[1].c_str());
    cout << "bye :=)" << endl;
    exit(code);
}

void merrno(vector<string> params) {
    for (int i = 1; i < params.size(); i++) {
        if (params[i] == "-h" || params[i] == "--help") {
            printf("merrno: usage: merrno [-h|--help]\n");
            printf("returns exit code of last func or program called\\n\"");
            ERRNO = 0;
            return;
        }
    }
    cout << ERRNO << endl;
    ERRNO = 0;
}

void help() {
    ERRNO = 0;
    printf("*******Welcome to my shell********)\n");
    printf("help - prints the list of commands supported by trial_shell\n");
    printf("mcd   - changes the directory\n");
    printf("mpwd  - prints working directory name\n");
    printf("mexit - exits the shell\n");
    printf("merrno - returns exit code of last func or program called\n");
}

string check_var(string arg){
    // there is a fail To Do : fix
   if(arg.size() > 0 && arg[0] == '$'){
       map<string, string>::iterator pair;
       pair = vars_local.find(arg.substr(1));
       if(pair != vars_local.end()){
            return pair->second;
       }
       else{
           cout<< "Have no variable : "<< arg.substr(1)<< endl;
           return "";
       }
   }
   return arg;
}



void set_var_as_local(const string &var){
    string first = var.substr(0, var.find("="));
    string second = var.substr( var.find("="));
    vars_local[first]=second;
}
void set_var_as_export(string var){
    string first = var.substr(0, var.find("="));
    string second = var.substr( var.find("="));
    vars_export[first]=second;
    set_var_as_local(var);
}

std::vector<std::string> post_process_args(std::vector<std::string> args) {
    int i = 0;
    std::vector<std::string> res;
    if (args.size() > 0 && args[0] == "mexport") {
        if(args.size() > 1) {
            if((args[1].find("=") != std::string::npos) && (args[1].find("\\=") == std::string::npos)) {
                // creating and exporting
                set_var_as_export(args[1]);
            }
            else {
                // export first var
                vars_export[args[1]];
            }
        }
        else {
            cout << "parse error" << endl;
        }
        return res;
    }
    for(int i = 1; i<args.size();i++) {
        if((args[i].find("=") != std::string::npos) && (args[i].find("\\=") == std::string::npos)) {
            // making var
            set_var_as_local(args[i]);
        }
        else {
            //To Do : fix
            string arg = check_var(args[i]);
            res.push_back(arg);
        }
    }
    return res;
}

std::vector<std::string> divide_into_argumens(std::string line) {
    std::vector<std::string> parameters;
    if (line.length() == 0) {
        return parameters;
    }
    std::string current_word = "";
    bool opened_single_quotes = false;
    bool opened_double_quotes = false;
    bool previous_backslash = false;
    bool var = false; // ira
    for (auto symbol : line) {
        cout << "C:" << symbol << ":C" << endl;
        if (symbol == '\\') {
            if (previous_backslash) {
                current_word += "\\";
                previous_backslash = false;
            }
            else {
                previous_backslash = true;
            }
        } else if (symbol == '\'') {
            if (previous_backslash || opened_double_quotes) {
                current_word += '\'';
            } else {
                if (opened_single_quotes) {
                    opened_single_quotes = false;
                    if (current_word.size() > 0) {
                        parameters.push_back(current_word);
                        current_word = "";
                    }
                } else {
                    opened_single_quotes = true;
                }
            }
            previous_backslash = false;
        } else if (symbol == '"') {
            if (previous_backslash || opened_single_quotes) {
                current_word += '"';
            } else {
                if (opened_double_quotes) {
                    opened_double_quotes = false;
                    if (current_word.size() > 0) {
                        if (parameters.size() > 0) {
                            current_word = check_var(current_word);
                            for (auto arg : expand_env(current_word)) {
                                parameters.push_back(arg);
                                
                            }
                        } else {
                            current_word = check_var(current_word);
                            parameters.push_back(current_word);
                        }
                        current_word = "";
                    }
                } else {
                    opened_double_quotes = true;
                }
            }
            previous_backslash = false;
        } else if (symbol == ' ') {
            previous_backslash = false;
            if (opened_double_quotes || opened_single_quotes) {
                current_word += "\\";
                current_word += symbol;
            } else {
                if (current_word.size() > 0) {
                    if (parameters.size() > 0) {
                        current_word = check_var(current_word);
                        for (auto arg : expand_env(current_word)) {

                            parameters.push_back(arg);

                        }
                    } else {
                        current_word = check_var(current_word);
                        parameters.push_back(current_word);
                    }
                    current_word = "";
                }
            }
        }else if (symbol == '#') {
            previous_backslash = false;
            if (opened_double_quotes || opened_single_quotes) {
                current_word += symbol;
            } else {
                if (current_word.size() > 0) {
                    current_word += symbol;
                }
                else {
                    return parameters;
                }
            }
        }else if (symbol == '=') {

            if (opened_double_quotes || opened_single_quotes || previous_backslash) {
                current_word += "\\";}
            current_word += symbol;
            previous_backslash = false;
        } else {
            if (previous_backslash) {
                current_word += '\\';
                previous_backslash = false;
            }
            current_word += symbol;
        }
    }
    if (opened_double_quotes || opened_single_quotes || previous_backslash) {
        ERRNO = 1;
        std::cout << "Error\n";
        return parameters;
    }
    if (current_word.size() > 0) {
        if (parameters.size() > 0) {
            current_word = check_var(current_word);
            cout << "I am here:" << current_word << endl;
            for (auto arg : expand_env(current_word)) {
                parameters.push_back(arg);
            }
        } else {
            current_word = check_var(current_word);
            parameters.push_back(current_word);
        }
    }
    return parameters;
}

void create_subprocess(vector<string> params) {
    if (params[0].size() == 0) {
        return;
    }

    string program_name = params[0];

    pid_t parent = getpid();
    pid_t pid = fork();
    vector<const char *> args;
    for (string &el: params) {
        args.push_back(el.c_str());
    }
    args.push_back(nullptr);
    if (pid == -1) {
        std::cerr << "Failed to fork()" << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        ERRNO = status;
    } else {
        execvp(program_name.c_str(), const_cast<char *const *>(args.data()));
        cerr << program_name << " :no such program" << endl;
        exit(EXIT_FAILURE);
    }
}


inline bool file_exists (const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}


void process_single_line(string line) {
    vector<string> params;

    params = divide_into_argumens(line);
    for (string p:params) {
        cout << "PC:" << p << ":CP" << endl;
    }
    cout << params.size() << endl;
    params = post_process_args(params);
    for (string p:params) {
        cout << "P:" << p << ":P" << endl;
    }
    cout << params.size() << endl;

    if (params.size() == 0) {
        return;
    } else if (params[0] == "help") {
        help();
    } else if (params[0] == "mexit") {
        mexit(params);
    } else if (params[0] == "mcd") {
        mcd(params);
    } else if (params[0] == "mpwd") {
        mpwd(params);
    } else if (params[0] == "merrno") {
        merrno(params);
    } else if (params[0] == "mecho") {
        mecho(params);
    } else if (params[0] == ".") {
        dot(params);
    } else {
        create_subprocess(params);
    }
}

int process_script_file(const string &filename) {
    ifstream in(filename);

    if(!in) {
        cerr << "Cannot open file:" << filename << endl;
        return 1;
    }

    string str;
    while(std::getline(in, str)) {
        process_single_line(str);
    }
    in.close();
    return 0;
}

int main(int argc, char *argv[]) {
    char sub_buf[BUFF_SIZE];
    getcwd(sub_buf, BUFF_SIZE);

    string path_to_subprograms = sub_buf;
    if (const char *env_p = std::getenv("PATH")) {
        string env_p_s = env_p;
        env_p_s = path_to_subprograms + ":" + env_p_s;
        setenv("PATH", env_p_s.c_str(), 1);
    }

    if (argc > 1) {
        ERRNO = process_script_file(string(argv[1]));
        exit(ERRNO);
    }

    std::string cur;


    char* line;

//    while (getline(cin, cur)) {
    while ((line = readline(get_identifier().c_str())) != nullptr) {
        if (*line) add_history(line);
        cur = string(line);

        free(line);
        process_single_line(cur);
        /*vector<string> params;

        params = divide_into_argumens(cur);

        if (params.size() == 0) {
            continue;
        } else if (params[0] == "help") {
            help();
        } else if (params[0] == "mexit") {
            mexit(params);
        } else if (params[0] == "mcd") {
            mcd(params);
        } else if (params[0] == "mpwd") {
            mpwd(params);
        } else if (params[0] == "merrno") {
            merrno(params);
        } else if (params[0] == "mecho") {
            mecho(params[1]);
        } else if (params[0] == ".") {
            dot(params);
        } else {
            create_subprocess(params);
        }*/
    }

    cout << endl << "Bye bye" << endl;
    return 0;
}
