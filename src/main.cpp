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

    cout << command << ": command not found" << std::endl;

    cout << "$ ";
  }

  return 0;
}
