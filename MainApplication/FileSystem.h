#ifndef F_SYS_H
#define F_SYS_H

#include <mutex>
#include <json/json.h>

#define DATA UserData::UserDataFile::Get()->data
#define SAFE_ACCESS std::lock_guard<std::mutex>(UserData::UserDataFile::Get()->m)
#define SaveData() UserData::UserDataFile::Get()->Save()

namespace UserData
{
	class UserDataFile
	{
	public:
		UserDataFile();
		~UserDataFile();

	public:
		static UserDataFile* Get();
		void Destroy();

		void Save();

	public:
		bool saving;
		Json::Value data;
		std::mutex m;

	private:
		static UserDataFile* instance;
	};
}

#endif