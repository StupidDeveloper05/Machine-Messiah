#include "pch.h"
#include "FunctionManager.h"
#include "HttpRequest.h"

#include <sstream>
#include <atlstr.h>
#include "ProgramManager.h"
#include "FileSystem.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::MainApplication::implementation;
namespace SmartMode
{
	FunctionManager* FunctionManager::instance;
	FunctionManager::FunctionManager()
	{
		{
			Json::Value parameters;
			parameters["type"] = "object";
			parameters["properties"] = Json::Value(Json::objectValue);
			Append("get_current_time", "Get current time information", parameters);
		}
		{
			Json::Value parameters;
			parameters["type"] = "object";
			parameters["properties"] = Json::Value(Json::objectValue);
			parameters["properties"]["location"] = Parameter("string", "The city and state, e.g. Seoul, KR");
			parameters["required"] = Json::Value(Json::arrayValue);
			parameters["required"].append("location");
			Append("get_current_weather", "Get the current weather in a given location", parameters);
		}
		{
			Json::Value parameters;
			parameters["type"] = "object";
			parameters["properties"] = Json::Value(Json::objectValue);
			parameters["properties"]["name"] = Parameter("array", "A list of program names requested by the user. At least three things must be presented, including the unaltered word of the program name requested by the user, the English translation of the word, and the English pronunciation of the word.", "string");
			parameters["required"] = Json::Value(Json::arrayValue);
			parameters["required"].append("name");
			Append("run_program", "Searches for a program with a given name and creates an actionable button. As a result, the 'count' item contains the number of programs searched, and if 'count' is 0, it means that there is no matching program. The user must be notified of only the number of programs searched using the 'count' value.", parameters);
		}
	}

	FunctionManager::~FunctionManager()
	{
	}
	
	FunctionManager* FunctionManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new FunctionManager();
		}
		return instance;
	}
	
	void FunctionManager::Destroy()
	{
		delete instance;
	}

	void FunctionManager::Call(const std::string& name, const std::string& args, OpenAI::_msgData* _pData)
	{
		Json::Value arguments;
		std::string errors;
		Json::CharReaderBuilder builder;
		Json::CharReader* reader = builder.newCharReader();
		if (reader->parse(args.c_str(), args.c_str() + args.size(), &arguments, &errors))
		{
			if (name == "get_current_time")
			{
				Json::Value info;
				time_t timer = time(NULL);
				tm* t = localtime(&timer);
				info["year"] = t->tm_year + 1900;
				info["month"] = t->tm_mon + 1;
				info["day"] = t->tm_mday;
				info["hour"] = t->tm_hour;
				info["minute"] = t->tm_min;
				info["second"] = t->tm_sec;
				info["day_of_the_week"] = t->tm_wday;

				Process(info, _pData);
			}
			else if (name == "get_current_weather")
			{
				std::string location = arguments["location"].asCString();
				location.erase(remove(location.begin(), location.end(), ' '), location.end());
				std::string url = "https://api.openweathermap.org/data/2.5/weather?q=" + location + "&appid=apikey&units=metric";
				HttpRequest request;
				auto result = request.Get(url);

				Json::Value info;
				info = result["main"];
				info["weather"] = Json::Value(Json::objectValue);
				info["weather"]["main"] = result["weather"][0]["main"];
				info["weather"]["description"] = result["weather"][0]["description"];
				info["wind"] = result["wind"];

				Process(info, _pData);
			}
			else if (name == "run_program")
			{
				auto result = ProgramManager::Get()->Search(arguments["name"]);
				Json::StreamWriterBuilder builder;
				builder.settings_["emitUTF8"] = true;
				std::string content = CW2A(CA2W(Json::writeString(builder, result).c_str()).m_psz, CP_UTF8).m_psz;

				if (result.size() != 0)
				{
					auto msg = MDView::CreateMessage(
						Application::Current().as<App>()->window.as<MainWindow>()->m_key,
						_pData->uuid.c_str(),
						content.c_str(),
						"function",
						_pData->chatInfo->isBegin ? "start" : "running"
					);
					if (CLIENT.isConnected())
						CLIENT.send(msg);

					DATA["chatting"][_pData->uuid]["data"].append(OpenAI::CreateMessage("function", content.c_str(), "run_program"));
				}

				_pData->chatInfo->isBegin = true;

				Json::Value info;
				info["count"] = result.size();
				Process(info, _pData);
			}
		}
	}
	
	void FunctionManager::Append(const std::string& name, const std::string& description, Json::Value& parameters)
	{
		Json::Value func;
		func["name"] = name;
		func["description"] = description;
		func["parameters"] = parameters;
		functions.append(func);
	}
	
	Json::Value FunctionManager::Parameter(const std::string& type, const std::string& desctiption, const std::string& childtype)
	{
		Json::Value param;
		param["type"] = type;
		param["description"] = desctiption;
		if (!childtype.empty())
		{
			param["items"] = Json::Value(Json::objectValue);
			param["items"]["type"] = childtype;
		}
		return param;
	}
	
	void FunctionManager::Process(Json::Value& result, OpenAI::_msgData* _pData)
	{
		Json::FastWriter writer;
		std::string content = writer.write(result);

		// 메시지에 추가
		DATA["chatting"][_pData->uuid]["data"].append(OpenAI::CreateMessage("function", content.c_str(), _pData->chatInfo->funcInfo.name.c_str()));
		SaveData();

		// function 정보 초기화
		_pData->chatInfo->funcInfo.name = "";
		_pData->chatInfo->funcInfo.arguments = "";

		// 재전송 -> 적절한 답변 생성
		Json::Value json_body;
		json_body["model"] = "gpt-3.5-turbo-0613";
		json_body["messages"] = DATA["chatting"][_pData->uuid]["data"];
		json_body["stream"] = true;
		json_body["functions"] = SmartMode::FunctionManager::Get()->functions;
		json_body["function_call"] = "none";
		OpenAI::OpenAIStorage::Get()->Create(OpenAI::EndPoint::Chat, json_body, _pData->uuid, _pData->chatInfo);
	}
}