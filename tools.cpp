#include "tools.hpp"

// bool	isInt(char *av)
// {
// 	std::string str = av;
//     if (str.size() > 4 || str.empty())
//     {
//         return (true);
//     }
// 	for (int i = 0; str[i]; i++)
// 	{
// 		if (!isdigit(str[i]))
// 			return (true);
// 	}
//     if (atoi(av) > 9999)
//     {
//         return (true);
//     }
// 	return (false);
// }

// int checkArgs(char **av)
// {
//     if (isInt(av[1]))
//         return (1);
//     return (0);
// }

bool    have_whitespaces(std::string arg)
{
    for (int i = 0; arg[i]; i++)
        if (arg[i] < 33 || arg[i] == 127)
            return (false);
    return (true);
}

bool    check_port(const char* arg)
{
    std::string copy = arg;
    char* endptr;
    long val = 0;

    //one: taille arg comprise entre 1 et 4 (voir si 5)
    if (copy.empty() || copy.size() > LEN_PORT_MAX)
        return (false);

    //two: que des chiffres
    val = std::strtol(arg, &endptr, 10);
    if (*endptr !='\0')
        return (false);

    //three: atoisable entre PORT_MIN jusqu'a PORT_MAX (voir laquelle)
    if (val < PORT_MIN || val > PORT_MAX)
        return (false);

    return (true);
}

bool    check_password(std::string arg)
{    
    //one: taille comprise entre LEN_PWD_MIN et LEN_PWD_MAX
    if (arg.empty() || arg.size() < LEN_PWD_MIN || arg.size() > LEN_PORT_MAX)
        return (false);

    //two: aucun whitespaces
    if (have_whitespaces(arg))
        return (false);
        
    return (true);
}