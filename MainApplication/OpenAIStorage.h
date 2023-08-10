#ifndef OPENAI_STORAGE_H
#define OPENAI_STORAGE_H

#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <OpenAIInstanced.h>

namespace OpenAI
{
	struct _function
	{
		std::string name;
		std::string arguments;
	};

	struct _chatInfo
	{
		bool isRunning;
		bool stop;
		bool del;
		bool isBegin = true;  // 이어서 작성할지 새로운 말풍선을 만들지 여부
		bool started = false; // 최초로 문자열이 socket 전송이 이루어졌는지 여부 ""가 전송된것은 제외함
		bool smart_mode = false;
		_function	funcInfo; // 스마트 모드일때만 작동
	};

	struct _msgData
	{
		std::string	uuid;
		std::string	msg;
		CURL*		curl;
		_chatInfo*	chatInfo;

		_msgData() = default;
		_msgData(const std::string& _uuid, _chatInfo* p_chatInfo)
			: uuid(_uuid)
			, msg("")
			, chatInfo(p_chatInfo)
		{}
	};

	class OpenAIStorage
	{
	private:
		OpenAIStorage();
		~OpenAIStorage();

	public:
		static OpenAIStorage* Get();
		void Destroy();

		void Create(EndPoint eType, Json::Value& json_body, const std::string& uuid, _chatInfo* p_chatInfo);
		bool CheckAllCompleted();

	private:
		static size_t writeFunctionChat(void* ptr, size_t size, size_t nmemb, void* data);
		static void finishCallback(void* userPtr);

	public:
		static std::string	m_key;

	private:
		static OpenAIStorage*	instance;
		std::vector<OpenAI*>	openAISessions;
	};
}

#endif