#ifndef OPENAIINSTANCED_H
#define OPENAIINSTANCED_H

#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

namespace OpenAI {
	enum class EndPoint
	{
		Chat,
		Embedding,
		Image,
		Whisper
	};

	typedef size_t(*WriteFunc)(void*, size_t, size_t, void*);
	typedef void(*FinishFunc)(void*);
	class OpenAI
	{
	public:
		OpenAI(const std::string& api_key);
		~OpenAI();

	public:
		Json::Value Create(EndPoint eType, Json::Value& json_body);
		void SetWriteFunction(WriteFunc customFuncPtr = nullptr);
		void SetFinishCallback(FinishFunc customFuncPtr = nullptr);
		void SetUserPointer(void* _userPtr);
		void* GetUserPointer();
		CURL* GetHandel();
		bool IsAvailiable();

	private:
		void SetEndPoint(EndPoint endPointType);
		Json::Value MakeRequest(Json::Value& json_body);

	private:
		static size_t writeFunctionDefault(void* ptr, size_t size, size_t nmemb, void* data);

	public:
		std::string response;
		std::string apiKey;

	private:
		// user pointer
		void* userPtr;

		// post setting
		std::string mainUrl;
		std::string endpoint;
		std::string contentType;

		// context
		CURL* curl;
		CURLcode res;

		// available check
		bool	isAvailable;

		WriteFunc write_func = writeFunctionDefault;
		FinishFunc finish_callback = nullptr;
	};

	Json::Value CreateMessage(const char* role, const char* content, const char* name="");
}

#endif