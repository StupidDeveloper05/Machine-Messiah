#ifndef OPENAI_H
#define OPENAI_H

#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

#define HttpContext OpenAI::OpenAI_Http_Context::Get()
#define PostHelper OpenAI::OpenAI_Post_Helper::Get()

namespace OpenAI {

	enum class EndPoint
	{
		Chat,
		Embedding,
		Image,
		Whisper
	};

	typedef size_t(*WriteFunc)(void*, size_t, size_t, void*);
	class OpenAI_Http_Context
	{
	public:
		OpenAI_Http_Context() = default;
		~OpenAI_Http_Context();

		static OpenAI_Http_Context* Get();

		void SetWriteFunction(WriteFunc customFuncPtr = nullptr);

	private:
		static size_t writeFunctionDefault(void* ptr, size_t size, size_t nmemb, void* data);

	public:
		CURL* curl;
		CURLcode res;

		WriteFunc write_func = writeFunctionDefault;
	};

	class OpenAI_Post_Helper
	{
	public:
		OpenAI_Post_Helper();
		~OpenAI_Post_Helper() = default;

		static OpenAI_Post_Helper* Get();

	public:
		void Init(const std::string& api_key, const std::string& _authorizaion);

		Json::Value Create(EndPoint eType, Json::Value& json_body);
		void SetUserPointer(void* _userPtr);

	private:
		void SetEndPoint(EndPoint endPointType);

		Json::Value MakeRequest(Json::Value& json_body);

	public:
		std::string response;

	private:
		void* user_ptr;

		// post setting
		std::string mainUrl;
		std::string endpoint;
		std::string contentType;

		// authorization setting
		std::string apiKey;
		std::string authorization;
	};

	bool Init(const std::string& api_key, const std::string& authorization);

	Json::Value Create(EndPoint eType, Json::Value& json_body);

	Json::Value CreateMessage(const char* role, const char* content);
}

#endif
