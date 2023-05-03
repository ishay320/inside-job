#pragma once

#include <iostream>

#define UNUSED(x) (void)x

/* Logging */
#define LOG_ERROR(...)                                                                             \
    std::cout << "\033[31m" << __FILE__ << ':' << __LINE__ << " "                                  \
              << "[ERROR] \033[0m" << __VA_ARGS__ << "\n"

#define LOG_INFO(...)                                                                              \
    std::cout << "\033[32m" << __FILE__ << ':' << __LINE__ << " "                                  \
              << "[INFO] \033[0m" << __VA_ARGS__ << "\n"

#define LOG_WARNING(...)                                                                           \
    std::cout << "\033[33m" << __FILE__ << ':' << __LINE__ << " "                                  \
              << "[WARNING] \033[0m" << __VA_ARGS__ << "\n"

#define LOG_DEBUG(...)                                                                             \
    std::cout << "\033[34m" << __FILE__ << ':' << __LINE__ << " " << __FUNCTION__                  \
              << " [DEBUG] \033[0m" << __VA_ARGS__ << "\n"

/* Testing */
typedef bool (*TestFun)();
typedef struct
{
    TestFun test;
    const char* name;
} Test;

bool run_tests(Test* tests, size_t tests_len, const char* tests_name)
{
    bool fail = false;
    for (size_t i = 0; i < tests_len; i++)
    {
        if (!tests[i].test())
        {
            fail = true;
            std::cout << "\033[31m"
                      << "[-] Test " << i + 1 << '/' << tests_len << " '" << tests[i].name
                      << "' failed "
                      << " \033[0m"
                      << "\n";
        }
        else
        {
            std::cout << "\033[32m"
                      << "[+] Test " << i + 1 << '/' << tests_len << " '" << tests[i].name
                      << "' pass "
                      << " \033[0m"
                      << "\n";
        }
    }

    if (fail)
    {
        std::cout << "\033[31m ^^ Test fail was in " << tests_name << "\033[0m\n";
    }

    return fail;
}

void printByLen(void* data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%c", ((char*)data)[i]);
    }
}
