#include <iostream>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <vector>

#include "tests.h"

// TODO: add args
int execAndWait(const std::string& path, const std::string& file)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        LOG_ERROR("Failed to fork process");
        return 1;
    }
    else if (pid == 0)
    {
        char command[128]{0};
        strcat(command, path.c_str());
        strcat(command, "/");
        strcat(command, file.c_str());
        char* const argv[] = {command, nullptr};
        execvp(command, argv);
        LOG_ERROR("Failed to execute `" << command << "` command");
        exit(1);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
    LOG_ERROR("how???");
    return -1;
}

int main(int argc, char const* argv[])
{
    int ret = 0;
    for (int i = 1; i < argc; i++)
    {
        std::cout << "\033[34m[" << i << '/' << argc - 1 << "] Running `" << argv[i]
                  << "` test\033[0m" << std::endl;
        int code = execAndWait(".", argv[i]);
        if (code)
        {
            std::cout << "\033[31m[-] Test `" << argv[i] << "` failed, code " << code << "\033[0m"
                      << std::endl;
            ret++;
        }
    }

    return ret;
}
