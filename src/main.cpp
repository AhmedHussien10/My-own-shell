#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

// Function to split a string by a delimiter (for splitting PATH)
vector<string> split_path(const string &str, char delimiter) {
    vector<string> our_path{};
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        our_path.push_back(token);
    }

    return our_path;
}

// Function to search for an executable in the directories listed in PATH
void search_executable_in_path(const string& command) {
    // Get the PATH environment variable
    const char* path_env = getenv("PATH");

    if (!path_env) {
        cout << "PATH environment variable is not set.\n";
        return;
    }

    // Split the PATH into directories (use os-agnostic separator)
    vector<string> path_dirs = split_path(path_env, ':'); // ':' is Unix-style, change if needed for Windows

    // Iterate through each directory in PATH
    for (const auto& dir : path_dirs) {
        fs::path full_path = fs::path(dir) / command;

        // Check if the directory exists
        if (!fs::exists(dir)) {
            continue; // Skip if the directory doesn't exist
        }

        // Check if the file exists and is executable
        if (fs::exists(full_path) && fs::is_regular_file(full_path)) {
            // Check execute permissions (for Unix-like systems)
            if ((fs::status(full_path).permissions() & fs::perms::owner_exec) != fs::perms::none) {
                // Print the command and its full path without quotes
                cout << command << " is " << full_path.string() << endl;
                return;
            }
        }
    }

    // If the command was not found in any PATH directory
    cout << command << ": not found\n";
}

int main() {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    string command;

    // Print the prompt once before the first input
    std::cout << "$ ";

    while (getline(cin, command)) {
        if (command.empty()) {
            cout << "$ ";
            continue;
        }

        if (command == "exit 0") {
            break;
        }

        // Handle the 'echo' command
        else if (command.find("echo") == 0) {
            size_t space = command.find(' ');

            if (space != string::npos) {
                string toprint = command.substr(space + 1);
                cout << toprint;
            }

            cout << "\n";
        }

        // Handle the 'type' command
        else if (command.find("type") == 0) {
            size_t space = command.find(' ');
            if (space != string::npos) {
                string func = command.substr(space + 1); // argument after 'type'

                // Check if it's a builtin command
                if (func == "echo" || func == "exit" || func == "type") {
                    cout << func << " is a shell builtin\n";
                } else {
                    // Otherwise, search for executable in PATH
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
