#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

namespace SmartMode
{
	class HttpRequest
	{
	public:
		HttpRequest();
		~HttpRequest();

	public:
		Json::Value Get(const std::string& url);

	private:
		CURL* curl;
		CURLcode res;
		std::string response;
	};
}

#endif