#include "NLBFR.h"

#include "utils.h"

NLBFR::NLBFR(const std::string& descriptions)
{
	function_descriptions = descriptions;

	messages.append(OpenAI::CreateMessage("user", AnsiToUtf8(function_descriptions).c_str()));

	Json::Value json_body;
	json_body["model"] = "gpt-3.5-turbo";
	json_body["messages"] = messages;
	json_body["temperature"] = 0;
	auto result = OpenAI::Create(OpenAI::EndPoint::Chat, json_body);
	messages.append(OpenAI::CreateMessage("assistant", result["choices"][0]["message"].get("content", "").asCString()));
}

void NLBFR::process_input(const std::string& input)
{
	// 질문을 완성시킴
	messages.append(OpenAI::CreateMessage("user", AnsiToUtf8("\"" + input + "\"" + "라는 말이 나오는 상황을 해결해줄 함수를 위에서 추천해줘.").c_str()));
	
	Json::Value json_body;
	json_body["model"] = "gpt-3.5-turbo";
	json_body["messages"] = messages;
	json_body["temperature"] = 0;
	auto result = OpenAI::Create(OpenAI::EndPoint::Chat, json_body);
	std::string response = Utf8ToAnsi(result["choices"][0]["message"].get("content", "").asCString());
	std::cout << response << std::endl;

	messages.removeIndex(2, nullptr);
}
