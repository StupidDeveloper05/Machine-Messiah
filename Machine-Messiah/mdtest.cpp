#include <iostream>
#include <Windows.h>
#include <Client.h>
#include "utils.h"

#define KEYW L"hello"
#define KEY "hello"

int main()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(STARTUPINFO), };
	CreateProcess(L"Markdown Viewer.exe", (LPWSTR)(std::wstring(L"\"Markdown Viewer\" --key ") + std::wstring(KEYW)).c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	MDView::Client client;
	client.connect("http://127.0.0.1:5000");

	while (!client.isConnected()) {}
	
	std::cout << "Connected!!!" << std::endl;
	
	auto msg = MDView::CreateMessage(
		KEY,
		"uuidTest",
		"Hello?",
		"user",
		"start"
	);

	int a;
	std::cin >> a;
	client.send(msg);
	
	std::cin >> a;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	
	std::cin >> a;
	msg = MDView::CreateMessage(
		KEY,
		"uuidTest",
		"Hello?",
		"user",
		"start"
	);
	client.send(msg);
	std::cin >> a;
	return 0;
}