#include <iostream>
#include <string>
using namespace std;

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

    if (command == "exit 0")
    {
      break;
    }
    
    else if (command.find("echo") == 0)
    {
      size_t space = command.find(' ');

      if (space!=string::npos)
      {
        string toprint = command.substr(space + 1);
        cout<<toprint;
      }
      
      cout<<"\n";
    }

   else if (command.find("type") == 0) {
            size_t space = command.find(' ');
            if (space != string::npos) {
                string func = command.substr(space + 1); // argument after type

                if (func == "echo" || func == "exit") {
                    cout << func << " is a shell builtin\n";
                } else {
                    cout << func << ": not found\n";
                }
            }
        }
        
    else
      cout << command << ": command not found" << std::endl;
    cout << "$ ";
  }

  return 0;
}
