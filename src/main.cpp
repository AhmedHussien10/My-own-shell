#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <Windows.h> // for system()

using namespace std;
namespace fs = std::filesystem;

vector<string> split_path(const string &str, char delimiter) {
    vector<string> our_path{};
    stringstream ss(str);
    string token;
    
    while (getline(ss, token, delimiter)) {
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

    vector<string> path_dirs = split_path(path_env, ';'); // Use ';' for Windows PATH separator

    for (const auto& dir : path_dirs) {
        fs::path full_path = fs::path(dir) / command;

        if (!fs::exists(dir)) continue;

        if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
            if ((fs::status(full_path).permissions() & fs::perms::owner_exec) != fs::perms::none) {
                cout << command << " is " << full_path.string() << endl;
                return;
            }
        }
    }

    cout << command << ": not found\n";
}

void run_external_program(const string& command, const vector<string>& args) {
    string full_command = command;
    
    // Add arguments to the command string
    for (const string& arg : args) {
        full_command += " " + arg;
    }

    // Execute the command using system() in Windows
    int result = system(full_command.c_str());

    if (result != 0) {
        cerr << "Error: Unable to execute " << command << endl;
    }
}

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    string command;

    cout << "$ ";
    while (getline(cin, command)) {
        if (command.empty()) {
            cout << "$ ";
            continue;
        }

        if (command == "exit") {
            break;
        }

        if (command.find("echo") == 0) {
            size_t space = command.find(' ');
            if (space != string::npos) {
                string toprint = command.substr(space + 1);
                cout << toprint;
            }
            cout << "\n";
        } else if (command.find("type") == 0) {
            size_t space = command.find(' ');
            if (space != string::npos) {
                string func = command.substr(space + 1);
                if (func == "echo" || func == "exit" || func == "type") {
                    cout << func << " is a shell builtin\n";
                } else {
                    search_executable_in_path(func);
                }
            }
        } else {
            size_t space = command.find(' ');
            string cmd_name = command.substr(0, space);

            vector<string> args;
            if (space != string::npos) {
                stringstream ss(command.substr(space + 1));
                string arg;
                while (getline(ss, arg, ' ')) {
                    args.push_back(arg);
                }
            }

            if (cmd_name == "exit") {
                break;
            }

            search_executable_in_path(cmd_name);
            run_external_program(cmd_name, args);
        }

        cout << "$ ";
    }

    return 0;
}
