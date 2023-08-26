#include <iostream>
#include <sstream>

int main(void)
{
	std::string arg = "MODE -i #room zied   ";
	std::stringstream stream(arg);

	std::string cmd;
	std::string option;
	std::string channel;
	std::string user;
	std::string end;
	std::string after1;
	std::string after2;

	try
	{
		stream >> cmd;
		stream >> option;
		stream >> channel;
		stream >> user;
		stream >> end;
		std::cout << after1 << after2;
		std::cout << "OVER" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cout << "catched" << std::endl;
	}

	return (0);
}
