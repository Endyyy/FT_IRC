#include "tools.hpp"

bool    have_whitespaces(std::string arg)
{
    for (unsigned long i = 0; i < arg.size(); i++)
    {
        if (isspace(arg[i]))
            return (false);
    }
    return (true);
}

bool    check_port(const char* arg)
{
    std::string copy = arg;
    char* endptr;
    long value = 0;

    //check if string size ranging from 1 to 5
    if (copy.empty() || copy.size() > LEN_PORT_MAX)
        return (false);

    //check if digits only
    value = std::strtol(arg, &endptr, 10);
    if (*endptr != '\0' || arg[0] == '+')
        return (false);

    //check if port ranging from 1024 to 65536
    if (value < PORT_MIN || value > PORT_MAX)
        return (false);
    return (true);
}

bool    check_password(std::string arg)
{    
    //check if string size ranging from LEN_PWD_MIN and LEN_PORT_MAX
    if (arg.empty() || arg.size() < LEN_PWD_MIN || arg.size() > LEN_PORT_MAX)
        return (false);

    //check if no whitespaces
    if (!have_whitespaces(arg))
        return (false);
        
    return (true);
}