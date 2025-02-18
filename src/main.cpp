#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  cout << "$ ";

  string input;
  getline(std::cin, input);
  cout << input << ": command not found" << endl;

  while(true){
    string input;
    getline(std::cin, input);
    cout << input << ": command not found" << endl;
  }
}
