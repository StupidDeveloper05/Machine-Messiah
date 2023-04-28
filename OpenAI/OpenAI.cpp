#include "OpenAI.h"

namespace OpenAI {
	
	OpenAI_Http_Context::~OpenAI_Http_Context()
	{
		curl_easy_cleanup(curl);
	}

	OpenAI_Http_Context* OpenAI_Http_Context::Get()
	{
		static OpenAI_Http_Context instance;
		return &instance;
	}

	void OpenAI_Http_Context::SetWriteFunction(WriteFunc customFuncPtr)
	{
		if (customFuncPtr != nullptr)
			write_func = customFuncPtr;
		else
			write_func = writeFunctionDefault;
	}

	size_t OpenAI_Http_Context::writeFunctionDefault(void* ptr, size_t size, size_t nmemb, std::string* data) {
		data->append(std::string((char*)ptr).substr(0, size * nmemb).c_str(), size * nmemb);
		return size * nmemb;
	}

	OpenAI_Post_Helper::OpenAI_Post_Helper()
	{
		mainUrl = "https://api.openai.com/v1/";
	}
	OpenAI_Post_Helper::~OpenAI_Post_Helper()
	{
		curl_slist_free_all(headers);
	}

	OpenAI_Post_Helper* OpenAI_Post_Helper::Get()
	{
		static OpenAI_Post_Helper instance;
		return &instance;
	}

	void OpenAI_Post_Helper::AppendHeader(const std::string& header)
	{
		headers = curl_slist_append(headers, header.c_str());
	}

	Json::Value OpenAI_Post_Helper::Create(EndPoint eType, Json::Value& json_body)
	{
		SetEndPoint(eType);
		return MakeRequest(json_body);
	}

	void OpenAI_Post_Helper::SetEndPoint(EndPoint endPointType)
	{
		switch (endPointType)
		{
		case EndPoint::Chat:
			endpoint = "chat/completions";
			break;
		case EndPoint::Embedding:
			endpoint = "embeddings";
			break;
		default:
			break;
		}
	}

	Json::Value OpenAI_Post_Helper::MakeRequest(Json::Value& json_body)
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
		else
		{
			std::cout << curl_easy_strerror(HttpContext->res) << std::endl;
		}
	}

	bool Init(const std::string& api_key, const std::string& authorization)
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

	Json::Value Create(EndPoint eType, Json::Value& json_body)
	{
		return PostHelper->Create(eType, json_body);
	}

	Json::Value CreateMessage(const char* role, const char* content)
	{
		Json::Value message;
		message["role"] = role;
		message["content"] = content;
		return message;
	}
}