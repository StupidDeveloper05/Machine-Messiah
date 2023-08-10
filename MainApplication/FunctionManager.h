#pragma once

#include <string>
#include <json/json.h>
#include "OpenAIStorage.h"

namespace SmartMode
{
	class FunctionManager
	{
	private:
		FunctionManager();
		~FunctionManager();

	public:
		static FunctionManager* Get();
		void Destroy();
		void Call(const std::string& name, const std::string& args, OpenAI::_msgData* _pData);

	private:
		void Append(const std::string& name, const std::string& description, Json::Value& parameters);
		Json::Value Parameter(const std::string& type, const std::string& desctiption, const std::string& childtype="");
		void Process(Json::Value& result, OpenAI::_msgData* _pData);

	private:
		static FunctionManager* instance;

	public:
		Json::Value functions;
	};
}