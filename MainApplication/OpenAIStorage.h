#ifndef OPENAI_STORAGE_H
#define OPENAI_STORAGE_H

#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <OpenAIInstanced.h>

namespace OpenAI
{
	struct _chatInfo
	{
		bool isRunning;
		bool stop;
		bool del;
		bool isBegin = true;  // �̾ �ۼ����� ���ο� ��ǳ���� ������ ����
		bool started = false; // ���� socket ������ �̷�������� ����
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
	public:
		OpenAIStorage();
		~OpenAIStorage();

	public:
		static OpenAIStorage* Get();
		void Destroy();

		void Create(EndPoint eType, Json::Value& json_body, const std::string& uuid, _chatInfo* p_chatInfo);
		bool CheckAllCompleted();

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