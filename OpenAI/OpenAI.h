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
		Embedding
	};

	typedef size_t(*WriteFunc)(void*, size_t, size_t, std::string*);
	class OpenAI_Http_Context
	{
	public:
		OpenAI_Http_Context() = default;
		~OpenAI_Http_Context();

		static OpenAI_Http_Context* Get();

	void SetWriteFunction(WriteFunc customFuncPtr = nullptr);

	private:
		static size_t writeFunctionDefault(void* ptr, size_t size, size_t nmemb, std::string* data);

	public:
		CURL* curl;
		CURLcode res;

		WriteFunc write_func = writeFunctionDefault;
	};

	class OpenAI_Post_Helper
	{
	public:
		OpenAI_Post_Helper();
		~OpenAI_Post_Helper();

		static OpenAI_Post_Helper* Get();

	public:
		void AppendHeader(const std::string& header);

		Json::Value Create(EndPoint eType, Json::Value& json_body);

	private:
		void SetEndPoint(EndPoint endPointType);

		Json::Value MakeRequest(Json::Value& json_body);

	private:
		std::string mainUrl;
		std::string endpoint;

		std::string response;
		curl_slist* headers;
	};

	bool Init(const std::string& api_key, const std::string& authorization);

	Json::Value Create(EndPoint eType, Json::Value& json_body);

	Json::Value CreateMessage(const char* role, const char* content);
}

#endif
