#pragma once
#include <json/json.h>

namespace SmartMode
{
	class ProgramManager
	{
	private:
		ProgramManager();
		~ProgramManager();

	public:
		static ProgramManager* Get();
		static void Destroy();
		Json::Value Search(Json::Value& names);
		void Run(const std::string& name);

	private:
		static ProgramManager* instance;
		static std::unordered_map<std::string, std::vector<std::string>> appList;
	};
}
