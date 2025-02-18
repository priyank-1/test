#include <iostream>
#include <string.h>
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  cout << "$ ";

  string input;
  std::getline(std::cin, input);
  cout << input << ": command not found" << endl;
  // cin.ignore();

  while(true){
    cout << "$ ";
    string input;
    getline(std::cin, input);
    cout << input << ": command not found" << endl;
  }
}
