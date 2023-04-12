#ifndef OPENAI_API_HPP
#define OPENAI_API_HPP

#define HttpContext OpenAI::OpenAI_Http_Context::Get()
#define PostHelper OpenAI::OpenAI_Post_Helper::Get()

#include <iostream>
#include <curl.h>
#include <json/json.h>

namespace OpenAI {

	enum class EndPoint
	{
		Chat
	};

	typedef size_t(*WriteFunc)(void*, size_t, size_t, std::string*);
	class OpenAI_Http_Context
	{
	public:
		OpenAI_Http_Context() = default;
		~OpenAI_Http_Context()
		{
			curl_easy_cleanup(curl);
		}

		static OpenAI_Http_Context* Get()
		{
			static OpenAI_Http_Context instance;
			return &instance;
		}

		void SetWriteFunction(WriteFunc customFuncPtr = nullptr)
		{
			if (customFuncPtr != nullptr)
				write_func = customFuncPtr;
			else
				write_func = writeFunctionDefault;
		}

	private:
		static size_t writeFunctionDefault(void* ptr, size_t size, size_t nmemb, std::string* data) {
			data->append((char*)ptr, size * nmemb);
			return size * nmemb;
		}

	public:
		CURL* curl;
		CURLcode res;

		WriteFunc write_func = writeFunctionDefault;
	};

	class OpenAI_Post_Helper
	{
	public:
		OpenAI_Post_Helper()
		{
			mainUrl = "https://api.openai.com/v1/";
		}
		~OpenAI_Post_Helper()
		{
			curl_slist_free_all(headers);
		}

		static OpenAI_Post_Helper* Get()
		{
			static OpenAI_Post_Helper instance;
			return &instance;
		}

	public:
		void AppendHeader(const std::string& header) 
		{ 
			headers = curl_slist_append(headers, header.c_str()); 
		}

		Json::Value Create(EndPoint eType, Json::Value& json_body)
		{
			SetEndPoint(eType);
			return MakeRequest(json_body);
		}

	private:
		void SetEndPoint(EndPoint endPointType)
		{
			switch (endPointType)
			{
			case EndPoint::Chat:
				endpoint = "chat/completions";
				break;
			default:
				break;
			}
		}

		Json::Value MakeRequest(Json::Value& json_body)
		{
			response.clear();

			Json::FastWriter writer;
			std::string body_str = writer.write(json_body);

			curl_easy_setopt(HttpContext->curl, CURLOPT_URL, (mainUrl + endpoint).c_str());
			curl_easy_setopt(HttpContext->curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(HttpContext->curl, CURLOPT_POST, 1L);
			curl_easy_setopt(HttpContext->curl, CURLOPT_POSTFIELDSIZE, body_str.length());
			curl_easy_setopt(HttpContext->curl, CURLOPT_POSTFIELDS, body_str.c_str());

			curl_easy_setopt(HttpContext->curl, CURLOPT_WRITEFUNCTION, HttpContext->write_func);
			curl_easy_setopt(HttpContext->curl, CURLOPT_WRITEDATA, &response);

			HttpContext->res = curl_easy_perform(HttpContext->curl);
			if (HttpContext->res == CURLE_OK) {
				Json::Value root;
				Json::CharReaderBuilder builder;
				Json::CharReader* reader = builder.newCharReader();
				std::string errors;
				bool parse_successed = reader->parse(response.c_str(), response.c_str() + response.size(), &root, &errors);
				if (parse_successed) {
					return root;
				}
			}
		}

	private:
		std::string mainUrl;
		std::string endpoint;

		std::string response;
		curl_slist* headers;
	};

	inline bool Init(const std::string& api_key, const std::string& authorization)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		HttpContext->curl = curl_easy_init();
		if (!HttpContext->curl)
		{
			std::cout << "Failed to Initialize Curl" << std::endl;
			return false;
		}

		PostHelper->AppendHeader("Content-Type: application/json");
		PostHelper->AppendHeader("Authorization: Bearer " + api_key);
		PostHelper->AppendHeader("OpenAI-Organization: " + authorization);
		return true;
	}

	inline Json::Value Create(EndPoint eType, Json::Value& json_body)
	{
		return PostHelper->Create(eType, json_body);
	}
	
	inline Json::Value CreateMessage(const char* role, const char* content)
	{
		Json::Value message;
		message["role"] = role;
		message["content"] = content;
		return message;
	}
}

#endif