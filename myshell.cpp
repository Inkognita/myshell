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
#include <cstdlib>
#include <climits>
#include <readline/history.h>
#include <readline/readline.h>
#include <map>
#include <sys/stat.h>
#include <fcntl.h>


int ERRNO = 0;

#define BUFF_SIZE 10000
using namespace std;
extern char **environ;
map<string, string> vars_local;
map<string, string> vars_export;


struct single_subprocess {
    string input_file;
    string err_output_file;
    string output_file;
    vector<string> args;
    bool deattach;
};

std::vector<std::string> expand_env(const std::string &var) {
    std::vector<std::string> vars;

    wordexp_t p;
    if (!wordexp(var.c_str(), &p, 0)) {
        if (p.we_wordc) {
            for (char **exp = p.we_wordv; *exp; ++exp) {
                vars.emplace_back(exp[0]);
            }
        }

        wordfree(&p);
    }

    return vars;
}

bool check_redirection(single_subprocess &subprocess, string &cur) {
//    cout <<"HEA" << cur.find("<") << endl;
//    cout <<"HEA" << cur << endl;

    if(cur == "&") {
        subprocess.deattach = true;
        return true;
    }
    if (cur.find("1>") == 0 && cur.size() != 2) {
        subprocess.output_file = cur.substr(2);
    } else if (cur.find('>') == 0 && cur.size() != 1) {
        subprocess.output_file = cur.substr(1);
    } else if (cur.find("2>") == 0) {

        if (cur.substr(2) == "&1") {
            subprocess.err_output_file = subprocess.output_file;
        } else {
            subprocess.err_output_file = cur.substr(2);
        }
    } else if (cur.find("<") == 0) {
        subprocess.input_file = cur.substr(1);
    } else {
        return false;
    }
    return true;
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
    int correct = chdir(path.c_str());
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

void dot(vector<string> params) {
    ERRNO = 0;
    if (params.size() == 1) {
        cerr << "No arguments given" << endl;
        ERRNO = 1;
        return;
    }
    if (process_script_file(params[1]) > 0) {
        ERRNO = 1;
    }
}


void mecho(vector<string> args) {
    ERRNO = 0;
    int i = 1;
    while (i < args.size() - 1) {
        cout << args[i] << " ";
        i++;
    }
    if (args.size() > 1) {
        cout << args[args.size() - 1];
    }
    cout << endl;
}

void set_var_as_export(string &var);

void mexport(vector<string> params) {
    // To DO:  додати змінну до блоку змінних середовища дочірнього процесу
    ERRNO = 0;
    if (params.size() > 1) {
        if ((params[1].find('=') != std::string::npos) && (params[1].find("\\=") == std::string::npos)) {
            // creating and exporting
            set_var_as_export(params[1]);
        } else {
            // export first var
            map<string, string>::iterator pair;
            pair = vars_local.find(params[1]);
            if (pair != vars_local.end()) {
                vars_export[pair->first] = pair->second;
                setenv(pair->first.c_str(), pair->second.c_str(), 1);
            } else {
                vars_export[params[1]] = "";
                setenv(params[1].c_str(), "", 1);
            }
        }
    } else {
        ERRNO = 1;
        cerr << "parse error" << endl;
    }
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

string check_var(string arg) {
    // there is a fail To Do : fix
    if (arg.size() > 1 && arg[0] == '$') {
        map<string, string>::iterator pair;
        pair = vars_local.find(arg.substr(1));
        if (pair != vars_local.end()) {
            return pair->second;
        } else if (std::getenv(arg.substr(1).c_str())) {
            return string(std::getenv(arg.substr(1).c_str()));
        } else {
            cout << "Have no variable : " << arg.substr(1) << endl;
            return "";
        }
    }
    return arg;
}


void set_var_as_local(const string &var) {
    string first = var.substr(0, var.find('='));
    string second = var.substr(var.find('=') + 1);
    vars_local[first] = second;
}

void set_var_as_export(string &var) {
    string first = var.substr(0, var.find('='));
    string second = var.substr(var.find('=') + 1);
    vars_export[first] = second;
    set_var_as_local(var);
    setenv(first.c_str(), second.c_str(), 1);
}

std::vector<std::string> post_process_args(std::vector<std::string> args, single_subprocess &ss) {
    std::vector<std::string> res;
    if (args.size() > 1 && args[0] == "mexport") {
        mexport(args);
        return res;
    }
    for (auto &arg : args) {
        if (arg == "&") {
            ss.deattach = true;
        }
        if ((arg.find('=') != std::string::npos) && (arg.find("\\=") == std::string::npos)) {
            // making var
            set_var_as_local(arg);
        } else {
            //To Do : fix
            res.push_back(arg);
        }
    }
    return res;
}


std::vector<single_subprocess> divide_into_argumens(std::string &line) {
    ERRNO = 0;
    std::vector<std::string> parameters;
    std::vector<single_subprocess> subprocesses;
    if (line.length() == 0) {
        return subprocesses;
    }
    single_subprocess current_subproc{};
    current_subproc.deattach = false;
    std::string current_word{};
    bool opened_single_quotes = false;
    bool opened_double_quotes = false;
    bool previous_backslash = false;
    for (auto symbol : line) {
        if (symbol == '\\') {
            if (previous_backslash) {
                current_word += "\\";
                previous_backslash = false;
            } else {
                previous_backslash = true;
            }
        } else if (symbol == '\'') {
            if (previous_backslash || opened_double_quotes) {
                current_word += '\'';
            } else {
                if (opened_single_quotes) {
                    opened_single_quotes = false;
                    if (!current_word.empty()) {
                        parameters.push_back(current_word);
                        if (!check_redirection(current_subproc, current_word)) {
//                            cout << "H0:" << current_word << endl;
                            parameters.push_back(current_word);
                        } else {
//                            cout << "badH1:" << current_word<< endl;
                        }
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
                    if (!current_word.empty()) {
                        if (!parameters.empty()) {
                            current_word = check_var(current_word);
                            if (!check_redirection(current_subproc, current_word)) {

//                                cout << "H0:" << current_word << endl;
//                                current_subproc.args.push_back(current_word);
                                for (auto &arg : expand_env(current_word)) {
                                    parameters.push_back(arg);
                                }
                            } else {
//                                cout << "badH1:" << current_word<< endl;
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
                if (!current_word.empty()) {
                    if (!parameters.empty()) {
                        current_word = check_var(current_word);
                        if (!check_redirection(current_subproc, current_word)) {
//                            cout << "H0:" << current_word << endl;
//                            current_subproc.args.push_back(current_word);
                            for (auto &arg : expand_env(current_word)) {
                                parameters.push_back(arg);
                            }
                        } else {
//                            cout << "badH1:" << current_word<< endl;
                        }

                    } else {
                        current_word = check_var(current_word);
                        parameters.push_back(current_word);
                    }
                    current_word = "";
                }
            }
        } else if (symbol == '|') {
            previous_backslash = false;
            if (opened_double_quotes || opened_single_quotes) {
                current_word += symbol;
            } else {
                if (!current_word.empty()) {
                    parameters.push_back(current_word);
                    current_word.clear();
                }
                if (parameters.empty()) {
                    ERRNO = 1;
                    return subprocesses;
                }
//                current_subproc.program_name = parameters[0];
                for (int i = 0; i < parameters.size(); ++i) {
                    if (!check_redirection(current_subproc, parameters[i])) {
//                        cout << "H0:" << parameters[i] << endl;
                        current_subproc.args.push_back(parameters[i]);
                    } else {
//                        cout << "badH1:" << parameters[i]<< endl;
                    }
                }
                subprocesses.push_back(current_subproc);
                current_subproc = single_subprocess();
                current_subproc.deattach = false;
                parameters.clear();
            }
        } else if (symbol == '#') {
            previous_backslash = false;
            if (opened_double_quotes || opened_single_quotes) {
                current_word += symbol;
            } else {
                if (!current_word.empty()) {
                    current_word += symbol;
                } else {
                    return subprocesses;
                }
            }
        } else if (symbol == '=') {

            if (opened_double_quotes || opened_single_quotes || previous_backslash) {
                current_word += "\\";
            }
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
        return subprocesses;
    }
    if (!current_word.empty()) {
        if (!parameters.empty()) {
            current_word = check_var(current_word);
            if (!check_redirection(current_subproc, current_word)) {
//                cout << "H0:" << current_word << endl;
                for (auto &arg : expand_env(current_word)) {
                    parameters.push_back(arg);
                }
            } else {
//                cout << "badH1" << current_word << endl;
            }
        } else {
            current_word = check_var(current_word);
            parameters.push_back(current_word);
        }
//        current_subproc.program_name = parameters[0];
        for (int i = 0; i < parameters.size(); ++i) {
            current_subproc.args.push_back(parameters[i]);
        }
        subprocesses.push_back(current_subproc);
    }
//    for(auto&el:subprocesses[0].args) {
//        cout << "a:" << el << endl;
//    }
    return subprocesses;
}

void create_subprocess(single_subprocess subproc) {
    if (subproc.args.empty() || subproc.args[0].empty()) {
        return;
    }

    string program_name = subproc.args[0];

//    pid_t parent = getpid();
    pid_t pid = fork();
    vector<const char *> args;
    for (string &el: subproc.args) {
        args.push_back(el.c_str());
    }
    args.push_back(nullptr);
    if (pid == -1) {
        std::cerr << "Failed to fork()" << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status = 0;
        if (!subproc.deattach) {
            waitpid(pid, &status, 0);
        }
        ERRNO = status;
    } else {
        int cin_desc, cout_desc, cerr_desc;
        if (subproc.deattach) {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
        }
        if (!subproc.input_file.empty()) {
            close(STDIN_FILENO);
            if ((cin_desc = open(subproc.input_file.c_str(), O_RDONLY | O_CREAT,
                                 S_IWOTH | S_IRGRP | S_IWGRP | S_IROTH | S_IRUSR | S_IWUSR)) < 0)
                cerr << "no file " << subproc.input_file;
            dup2(cin_desc, STDIN_FILENO);
        }
        if (!subproc.output_file.empty()) {
            close(STDOUT_FILENO);
            if ((cout_desc = open(subproc.output_file.c_str(), O_WRONLY | O_CREAT,
                                  S_IWOTH | S_IRGRP | S_IWGRP | S_IROTH | S_IRUSR | S_IWUSR)) < 0)
                cerr << "no file " << subproc.output_file;
            dup2(cout_desc, STDOUT_FILENO);
        }
        if (!subproc.err_output_file.empty()) {
            close(STDERR_FILENO);
            if ((cerr_desc = open(subproc.err_output_file.c_str(), O_WRONLY | O_CREAT,
                                  S_IWOTH | S_IRGRP | S_IWGRP | S_IROTH | S_IRUSR | S_IWUSR)) < 0)
                cerr << "no file " << subproc.output_file;
            dup2(cerr_desc, STDERR_FILENO);
        }
        execvp(program_name.c_str(), const_cast<char *const *>(args.data()));
        cerr << program_name << " :no such program" << endl;
        exit(EXIT_FAILURE);
    }


}


inline bool file_exists(const std::string &name) {
    struct stat buffer{};
    return (stat(name.c_str(), &buffer) == 0);
}


void process_single_line(string line) {
    vector<single_subprocess> params;

    single_subprocess sub_proc;

    params = divide_into_argumens(line);
    for (auto &subproc: params) {
        subproc.args = post_process_args(subproc.args, subproc);
    }
//
//    for (auto & p: params) {
//        cout << "From:" << p.input_file << endl;
//        cout << "ToFile:" << p.output_file << endl;
//        cout << "ToErrFile:" << p.err_output_file << endl;
//        for(auto & a: p.args) {
//            cout << a << endl;
//        }
//    }
//    return;

    if (params.empty()) {
        return;
    } else if (params.size() == 1) {
        sub_proc = params[0];
        if (sub_proc.args[0] == "help") {
            help();
        } else if (sub_proc.args[0] == "mexit") {
            mexit(sub_proc.args);
        } else if (sub_proc.args[0] == "mcd") {
            mcd(sub_proc.args);
        } else if (sub_proc.args[0] == "mpwd") {
            mpwd(sub_proc.args);
        } else if (sub_proc.args[0] == "merrno") {
            merrno(sub_proc.args);
        } else if (sub_proc.args[0] == "mecho") {
            mecho(params[0].args);
        } else if (sub_proc.args[0] == ".") {
            dot(sub_proc.args);
        } else {
            create_subprocess(sub_proc);
        }
    } else {
        create_subprocess(params[0]);
    }
}

int process_script_file(const string &filename) {
    ifstream in(filename);

    if (!in) {
        cerr << "Cannot open file:" << filename << endl;
        return 1;
    }

    string str;
    while (std::getline(in, str)) {
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


    char *line;

    while ((line = readline(get_identifier().c_str())) != nullptr) {
        if (*line) add_history(line);
        cur = string(line);

        free(line);
        process_single_line(cur);
    }

    cout << endl << "Bye bye" << endl;
    return 0;
}
