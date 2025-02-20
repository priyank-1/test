#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <cstdlib>  // For getenv()
#include <unistd.h> // For fork(), execvp(), access()
#include <sys/wait.h> // For waitpid()
// #include <pwd.h> // For getting home directory

// Built-in commands
std::unordered_set<std::string> builtins = {"echo", "exit", "type", "pwd", "cd"};

//  `echo` command
void handleEcho(const std::string& input) {
    std::cout << std::string_view(input).substr(5) << '\n';
}

// `pwd` command
void handlePwd() {
    const size_t size = 1024;
    char buffer[size];
    if (getcwd(buffer, size) != nullptr) {
        std::cout << buffer << std::endl;
    } else {
        std::cerr << "Error: Failed to get current working directory\n";
    }
}

// `cd` command
void handleCd(const std::string& input) {
    std::string path = input.substr(3); 

    if (path == "~") {
        const char* home = getenv("HOME");
        if(home != nullptr){
            if(chdir(home)!=0)
            std::perror("chdir");
        }
        else{
            std::perror("getenv");
        }
    } else if (path == "..") {
        if (chdir("..") != 0) {
            std::perror("chdir");
        }
    } else {
        if (access(path.c_str(), F_OK) == 0) {
            if (chdir(path.c_str()) != 0) {
                std::perror("chdir");
            }
        } else {
            std::cout << "cd: " << path << ": No such file or directory\n";
        }
    }
}

// `type` command
void handleType(const std::string& input) {
    std::string cmd = input.substr(5);

    if (builtins.find(cmd) != builtins.end()) {
        std::cout << cmd << " is a shell builtin\n";
        return;
    }

    const char* path = std::getenv("PATH");
    if (path != nullptr) {
        std::stringstream ss(path);
        std::string token;
        while (std::getline(ss, token, ':')) {
            std::string cmd_path = token + "/" + cmd;
            if (access(cmd_path.c_str(), X_OK) == 0) {
                std::cout << cmd << " is " << cmd_path << std::endl;
                return;
            }
        }
    }

    std::cout << cmd << ": not found\n";
}

//  external commands
void executeCommand(const std::string& input) {
    std::stringstream ss(input);
    std::vector<std::string> args;
    std::string arg;

    while (ss >> arg) {
        args.push_back(arg);
    }

    if (args.empty()) return;

    std::string cmd = args[0];
    const char* path = std::getenv("PATH");
    if (path == nullptr) {
        std::cout << cmd << ": not found\n";
        return;
    }

    std::string executablePath;
    std::stringstream path_ss(path);
    std::string dir;
    bool found = false;

    while (std::getline(path_ss, dir, ':')) {
        std::string full_path = dir + "/" + cmd;
        if (access(full_path.c_str(), X_OK) == 0) {
            executablePath = full_path;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << cmd << ": not found\n";
        return;
    }

    // Convert vector<string> to vector<char*>
    std::vector<char*> argv;
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);

    // Fork and execute 
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Error: Failed to fork process\n";
        return;
    }

    if (pid == 0) {
        execvp(executablePath.c_str(), argv.data());
        std::cerr << cmd << ": command not found\n";
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string input;

    while (true) {
        std::cout << "$ " << std::flush;
        std::getline(std::cin, input);

        if (input == "exit 0") {
            break;
        }

        if (input.starts_with("echo ")) {
            handleEcho(input);
            continue;
        }

        if (input.starts_with("type ")) {
            handleType(input);
            continue;
        }

        if (input == "pwd") {
            handlePwd();
            continue;
        }

        if (input.starts_with("cd ")) {
            handleCd(input);
            continue;
        }

        //  external command
        executeCommand(input);
    }

    return 0;
}
