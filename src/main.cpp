#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
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

    vector<string> path_dirs = split_path(path_env, ':');

    for (const auto& dir : path_dirs) {
        fs::path full_path = fs::path(dir) / command;

        if (!fs::exists(dir)) {
            continue;
        }

        if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
            if ((fs::status(full_path).permissions() & fs::perms::owner_exec) != fs::perms::none) {
                cout << command << " is " << full_path.string() << endl;
                return;
            }
        }
    }

    cout << command << ": not found\n";
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

        else if (command.find("echo") == 0) {
            size_t space = command.find(' ');

            if (space != string::npos) {
                string toprint = command.substr(space + 1);
                cout << toprint;
            }

            cout << "\n";
        }

        else if (command.find("type") == 0) {
            size_t space = command.find(' ');
            if (space != string::npos) {
                string func = command.substr(space + 1);

                if (func == "echo" || func == "exit" || func == "type") {
                    cout << func << " is a shell builtin\n";
                } else {
                    search_executable_in_path(func);
                }
            }
        }

        else {
            cout << command << ": command not found" << std::endl;
        }

        cout << "$ ";
    }

    return 0;
}
