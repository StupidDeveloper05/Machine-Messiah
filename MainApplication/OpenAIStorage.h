#ifndef OPENAI_STORAGE_H
#define OPENAI_STORAGE_H

#include <string>
#include <thread>
#include <vector>
#include <OpenAIInstanced.h>

namespace OpenAI
{
	struct _msgData
	{
		bool		isBegin;
		std::string	uuid;
		std::string	msg;
		bool*		isRunning;

		_msgData() = default;
		_msgData(bool begin, const std::string& _uuid, bool* p_isRunning)
			: isBegin(begin)
			, uuid(_uuid)
			, msg("")
			, isRunning(p_isRunning)
		{}
	};

	class OpenAIStorage
	{
	public:
		OpenAIStorage();
		~OpenAIStorage();

	public:
		static OpenAIStorage* Get();
		void Destroy();

		void Create(EndPoint eType, Json::Value& json_body, const std::string& uuid, bool* p_isRunning);

	private:
		static size_t writeFunctionChat(void* ptr, size_t size, size_t nmemb, void* data);

	public:
		static std::string	m_key;

	private:
		static OpenAIStorage*	instance;
		std::vector<OpenAI*>	openAISessions;

	};
}

#endif