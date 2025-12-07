#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h> // Only on Windows
#else
#include <unistd.h> // For POSIX systems (Linux/macOS)
#include <sys/wait.h> // For wait() function on Unix systems
#endif

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

    vector<string> path_dirs = split_path(path_env, (sizeof(char) == 1 && *path_env == '/') ? ':' : ';'); // ':' for Unix and ';' for Windows

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
    vector<const char*> c_args;
    c_args.push_back(command.c_str());

    // Convert each argument to const char* and add it to the vector
    for (const string& arg : args) {
        c_args.push_back(arg.c_str());
    }

    c_args.push_back(nullptr); // Null-terminate the array of arguments

#ifdef _WIN32
    // Windows: use system to run the command
    int result = system(command.c_str());
#else
    // Unix-like: use fork and execvp
    pid_t pid = fork();

    if (pid == 0) {
        execvp(command.c_str(), const_cast<char* const*>(c_args.data()));
        exit(0); // execvp will replace the current process, so this is just a fallback.
    } else if (pid > 0) {
        wait(NULL); // Parent process waits for the child to finish
    } else {
        cerr << "Fork failed!" << endl;
    }
#endif

    // If execvp or system fails
#ifdef _WIN32
    if (result != 0) {
        cerr << "Error: Unable to execute " << command << endl;
    }
#endif
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
