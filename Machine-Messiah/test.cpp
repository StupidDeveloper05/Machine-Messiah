#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "utils.h"

#include "OpenAIInstanced.h"
#include "MicRecord.h"

std::string reply;
Json::Value messages;

size_t writeFunctionForChat(void* ptr, size_t size, size_t nmemb, void* data)
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
	else 
	{
		bool parsing_successful = reader->parse(json.c_str(), json.c_str() + json.size(), &root, &errors);
		if (parsing_successful) {
			std::cout << root << std::endl;
			std::cout << root["error"]["message"] << std::endl;
		}
	}

	//static_cast<std::string*>(data)->append(json.c_str(), size * nmemb);
	return size * nmemb;
}

void micRecord()
{
	// Microphone capturing
	MIC::MicRecord mic(1, 16000, 128);

	std::cout << "Press Enter to Speak";
	std::cin.ignore();
	mic.Start();

	std::cout << "Press Enter to Stop" << std::endl;
	std::cin.ignore();
	mic.Pause();
}

std::string speach_to_text(auto& instance)
{
	// create json body
	Json::Value json_body;
	json_body["model"] = "whisper-1";
	json_body["file"] = "output.wav";

	// http post request
	auto result = instance.Create(OpenAI::EndPoint::Whisper, json_body);
	return result["text"].asCString();
}

int main()
{
	OpenAI::OpenAI instance { 
		"sk-apikey",
		"org-1XK4EGAKbk9RBmHca7zf6HLK" 
	};

	instance.SetWriteFunction(writeFunctionForChat);

	while (true)
	{
		std::cout << "User : ";
		std::string input;
		getline(std::cin, input);

		/*micRecord();
		std::string utf8 = speach_to_text();

		std::cout << "User : ";
		std::cout << Utf8ToAnsi(utf8) << std::endl;*/

		// create message
		std::string utf8 = AnsiToUtf8(input);
		messages.append(OpenAI::CreateMessage("user", utf8.c_str()));

		// create json body
		Json::Value json_body;
		json_body["model"] = "gpt-3.5-turbo";
		json_body["messages"] = messages;
		json_body["stream"] = true;

		// http post request
		std::cout << "AI : ";
		auto result = instance.Create(OpenAI::EndPoint::Chat, json_body);
	}
}
