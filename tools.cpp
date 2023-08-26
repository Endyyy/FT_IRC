#include "tools.hpp"

bool	have_whitespaces(std::string arg)
{
	for (unsigned long i = 0; i < arg.size(); i++)
	{
		if (isspace(arg[i]))
			return (false);
	}
	return (true);
}

bool	check_port(const char* arg)
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

bool	check_password(std::string arg)
{
	//check if string size ranging from LEN_PWD_MIN and LEN_PORT_MAX
	if (arg.empty() || arg.size() < LEN_PWD_MIN || arg.size() > LEN_PORT_MAX)
		return (false);

	//check if no whitespaces
	if (!have_whitespaces(arg))
		return (false);

	return (true);
}

bool	check_valid_limit(std::string arg, int max)
{
	int size = 0;
	int checker = max;
	while (checker)
	{
		checker = checker / 10;
		size++;
	}
	if (arg.size() > size)
		return (false);

	for (int i = 0; arg[i]; i++)
		if (strchr("0123456789", arg[i]) == NULL)
			return (false);

	if (atoi(arg.c_str()) > max)
		return (false);

	return (true);
}
