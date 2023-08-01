#include "OpenAIInstanced.h"

namespace OpenAI {

	//////////////////// JSON Format /////////////////////
	// 													//
	// EndPoint: Chat { "model", "messages", "stream" }	//
	// EndPoint: Whisper { "model", "file" }			//
	// EndPoint: Image { "prompt", "n", "size" }		//
	// 													//
	//////////////////////////////////////////////////////

	OpenAI::OpenAI(const std::string& api_key, const std::string& _authorization)
	{
		apiKey = api_key;
		authorization = _authorization;
		mainUrl = "https://api.openai.com/v1/";
		isAvailable = true;

		//curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();
	}

	OpenAI::~OpenAI()
	{
		curl_easy_cleanup(curl);
		delete userPtr;
	}
	
	Json::Value OpenAI::Create(EndPoint eType, Json::Value& json_body)
	{
		isAvailable = false;
		SetEndPoint(eType);
		auto result = MakeRequest(json_body);
		isAvailable = true;
		return result;
	}

	void OpenAI::SetWriteFunction(WriteFunc customFuncPtr)
	{
		if (customFuncPtr != nullptr)
			write_func = customFuncPtr;
		else
			write_func = writeFunctionDefault;
	}

	void OpenAI::SetUserPointer(void* _userPtr)
	{
		userPtr = _userPtr;
	}

	void* OpenAI::GetUserPointer()
	{
		return userPtr;
	}

	CURL* OpenAI::GetHandel()
	{
		return curl;
	}

	bool OpenAI::IsAvailiable()
	{
		return isAvailable;
	}
	
	void OpenAI::SetEndPoint(EndPoint endPointType)
	{
		switch (endPointType)
		{
		case EndPoint::Chat:
			endpoint = "chat/completions";
			contentType = "application/json";
			break;
		case EndPoint::Embedding:
			endpoint = "embeddings";
			contentType = "application/json";
			break;
		case EndPoint::Image:
			endpoint = "images/generations";
			contentType = "application/json";
			break;
		case EndPoint::Whisper:
			endpoint = "audio/transcriptions";
			contentType = "multipart/form-data";
		default:
			break;
		}
	}
	
	Json::Value OpenAI::MakeRequest(Json::Value& json_body)
	{
		response.clear();

		Json::FastWriter writer;
		std::string body_str = writer.write(json_body);

		// header settings
		curl_slist* headers = NULL;
		headers = curl_slist_append(headers, std::string("Authorization: Bearer " + apiKey).c_str());
		headers = curl_slist_append(headers, std::string("OpenAI-Organization: " + authorization).c_str());
		headers = curl_slist_append(headers, std::string("Content-Type: " + contentType).c_str());

		// process mutipart form data
		curl_mime* mime_form = NULL;
		if (contentType == "multipart/form-data")
		{
			headers = curl_slist_append(headers, "Expect:");
			mime_form = curl_mime_init(curl);

			curl_mimepart* field = NULL;
			field = curl_mime_addpart(mime_form);
			curl_mime_name(field, "file");
			curl_mime_filedata(field, json_body["file"].asCString());

			field = curl_mime_addpart(mime_form);
			curl_mime_name(field, "model");
			curl_mime_data(field, json_body["model"].asCString(), CURL_ZERO_TERMINATED);

			curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime_form);
		}
		else
		{
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body_str.length());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_str.c_str());
		}

		curl_easy_setopt(curl, CURLOPT_URL, (mainUrl + endpoint).c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, userPtr == nullptr ? &response : userPtr);

		// debugging
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);
		if (res == CURLE_OK) {
			Json::Value root;
			Json::CharReaderBuilder builder;
			Json::CharReader* reader = builder.newCharReader();
			std::string errors;
			bool parse_successed = reader->parse(response.c_str(), response.c_str() + response.size(), &root, &errors);
			if (parse_successed) {
				curl_mime_free(mime_form);
				return root;
			}
			else
			{
				curl_mime_free(mime_form);
				return Json::Value{ "error" };
			}
		}
		else
		{
			std::cout << curl_easy_strerror(res) << std::endl;
		}

		curl_mime_free(mime_form);
	}
	
	size_t OpenAI::writeFunctionDefault(void* ptr, size_t size, size_t nmemb, void* data)
	{
		static_cast<std::string*>(data)->append(std::string((char*)ptr).substr(0, size * nmemb).c_str(), size * nmemb);
		return size * nmemb;
	}

	Json::Value CreateMessage(const char* role, const char* content)
	{
		Json::Value message;
		message["role"] = role;
		message["content"] = content;
		return message;
	}
}