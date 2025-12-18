#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
namespace fs = std::filesystem;

vector<string> split_path(const string &str, char delimiter) {
    vector<string> our_path;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        if (!token.empty())
            our_path.push_back(token);
    }
    return our_path;
}

void search_executable_in_path(const string& command) {
    const char* path_env = getenv("PATH");
    if (!path_env) {
        cout << "PATH environment variable is not set.\n";
        return;
    }
    vector<string> path_dirs = split_path(path_env, ':');
    for (const auto& dir : path_dirs) {
        fs::path full_path = fs::path(dir) / command;
        if (!fs::exists(dir))
            continue;
        if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
            if ((fs::status(full_path).permissions() & fs::perms::owner_exec) != fs::perms::none) {
                cout << command << " is " << full_path.string() << endl;
                return;
            }
        }
    }
    cout << command << ": not found\n";
}

bool exist (const string &dir) {
    fs::path our_path(dir);
     return fs::exists(our_path) && fs::is_directory(our_path);
}

bool change_dir(std::string dir) {
    dir = expand_home(dir);

    if (chdir(dir.c_str()) != 0) {
        cerr << "cd: " << dir << ": No such file or directory" <<endl;
        return false;
    }
    return true;
}
void external_program(const string &command) {
    vector<string> tokens = split_path(command, ' ');
    if (tokens.empty())
        return;
    vector<char*> argv;
    for (auto &t : tokens)
        argv.push_back(const_cast<char*>(t.c_str()));
    argv.push_back(nullptr);
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv.data());
        cout << argv[0] << ": command not found\n";
        _exit(1);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    }
}


int main() {
    cout << unitbuf;
    cerr << unitbuf;
    string command;
    cout << "$ ";
    while (getline(cin, command)) {
        if (command.empty()) {
            cout << "$ ";
            continue;
        }
        if (command == "exit") {
            break;
        } else if (command.rfind("echo", 0) == 0) {
            size_t space = command.find(' ');
            if (space != string::npos)
                cout << command.substr(space + 1);
            cout << "\n";
        } else if (command.rfind("type", 0) == 0) {
            size_t space = command.find(' ');
            if (space != string::npos) {
                string func = command.substr(space + 1);
                if (func == "echo" || func == "exit" || func == "type" || func == "pwd") {
                    cout << func << " is a shell builtin\n";
                } else {
                    search_executable_in_path(func);
                }
            }
        } else if (command == "pwd") {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                cout << cwd << "\n";
            }
        } else {
            external_program(command);
        }
        cout << "$ ";
    }
    return 0;
}
