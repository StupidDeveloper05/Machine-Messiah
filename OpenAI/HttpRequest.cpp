#include "HttpRequest.h"

namespace SmartMode
{
	size_t writeFunctionDefault(void* ptr, size_t size, size_t nmemb, void* data)
	{
		static_cast<std::string*>(data)->append(std::string((char*)ptr).substr(0, size * nmemb).c_str(), size * nmemb);
		return size * nmemb;
	}

	HttpRequest::HttpRequest()
	{
		curl = curl_easy_init();
	}

	HttpRequest::~HttpRequest()
	{
		curl_easy_cleanup(curl);
	}

	Json::Value HttpRequest::Get(const std::string& url)
	{
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunctionDefault);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

			res = curl_easy_perform(curl);
			if (res == CURLE_OK) {
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
	}
}