#include <iostream>

int main()
{
	std::wstring a = L"";
	std::wcout << L"Sfs" << std::endl;
	std::wcout << a.substr(0, a.size() < 20 ? a.size() : 20) << std::endl;
	return 0;
}