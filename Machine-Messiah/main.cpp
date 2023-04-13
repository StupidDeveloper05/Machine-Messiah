#include <iostream>
#include <sstream>

#include "utils.h"
#include "openai_api.hpp"

std::string reply;
Json::Value messages;

size_t writeFunctionForChat(void* ptr, size_t size, size_t nmemb, std::string* data) 
{
    std::string json = (char*)ptr;
	json = json.substr(0, size * nmemb); // 버그 해결: 데이터가 중복되지 않게 콜백함수로 들어온 사이즈 만큼 데이터를 슬라이싱 함.
    std::string str_buf;
    std::istringstream iss(json);

	Json::Value root;
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	std::string errors;

    if (json.find("data") != std::string::npos)
    {
        while (getline(iss, str_buf, '\n')) {
			try
            {
				if (str_buf.find("data") != std::string::npos)
                {
					std::string generated_response = str_buf.substr(str_buf.find("{"), str_buf.rfind("}") + 1);
					bool parsing_successful = reader->parse(generated_response.c_str(), generated_response.c_str() + generated_response.size(), &root, &errors);
					if (parsing_successful)
					{
						std::string utf8 = root["choices"][0]["delta"].get("content", "").asCString();
						std::string ansi = Utf8ToAnsi(utf8);
						reply += utf8;
						std::cout << ansi.c_str();
					}                    
                }
            }
            catch (std::exception&)
            {
                std::cout << std::endl;
                messages.append(OpenAI::CreateMessage("assistant", reply.c_str()));
                reply = "";
			}
        }
    }
    
    data->append(json.c_str(), size * nmemb);
    return size * nmemb;
}


int main()
{
	bool successed = OpenAI::Init(
		"API KEY", 
		"org-1XK4EGAKbk9RBmHca7zf6HLK"
	); 
	if (!successed)
		return -1;
	
	// setting basic system message
	messages.append(OpenAI::CreateMessage("system", "You are very smart teacher!"));

	HttpContext->SetWriteFunction(writeFunctionForChat);

	while (true)
	{
		std::cout << "User : ";
		std::string input;
		getline(std::cin, input);

		// create message
		std::string utf8 = AnsiToUtf8(input);
		messages.append(OpenAI::CreateMessage("user", utf8.c_str()));
		
		// create json body
		Json::Value json_body;
		json_body["model"] = "gpt-3.5-turbo";
		json_body["messages"] = messages;
		json_body["temperature"] = 0;
		json_body["stream"] = true;

		// http post request
		auto result = OpenAI::Create(OpenAI::EndPoint::Chat, json_body);
	}
}
