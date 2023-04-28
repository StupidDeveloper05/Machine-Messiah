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
	// ������ �ϼ���Ŵ
	messages.append(OpenAI::CreateMessage("user", AnsiToUtf8("\"" + input + "\"" + "��� ���� ������ ��Ȳ�� �ذ����� �Լ��� ������ ��õ����.").c_str()));
	
	Json::Value json_body;
	json_body["model"] = "gpt-3.5-turbo";
	json_body["messages"] = messages;
	json_body["temperature"] = 0;
	auto result = OpenAI::Create(OpenAI::EndPoint::Chat, json_body);
	std::string response = Utf8ToAnsi(result["choices"][0]["message"].get("content", "").asCString());
	std::cout << response << std::endl;

	messages.removeIndex(2, nullptr);
}
