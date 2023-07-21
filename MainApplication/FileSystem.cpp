#include "pch.h"
#include "FileSystem.h"

#include <fstream>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.ApplicationModel.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace UserData
{
	UserDataFile* UserDataFile::instance;

	UserDataFile::UserDataFile()
		: data{}
	{
	}
	
	UserDataFile::~UserDataFile()
	{
	}

	UserDataFile* UserDataFile::Get()
	{
		if (instance == nullptr)
		{
			instance = new UserDataFile();
		}

		return instance;
	}

	void UserDataFile::Destroy()
	{
		delete instance;
	}
	
	void UserDataFile::Save()
	{
		while (saving) {}
		saving = true;
		auto local = Windows::Storage::ApplicationData::Current().LocalFolder().Path();
		auto fNameW = std::wstring((local + L"\\userData.json").c_str());
		std::string fName; fName.assign(fNameW.begin(), fNameW.end());

		Json::StyledWriter writer;
		auto str = writer.write(data);
		std::ofstream output(fName.c_str());
		output << str;
		output.close();
		saving = false;
	}
}