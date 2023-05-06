#pragma once
#include <QtWidgets/qlabel.h>
#include <json/json.h>
#include <string>
#include <atlstr.h>

#include "ui_GUIMain.h"

std::string reply;
Json::Value messages;

// ansi->unicode->utf8 인코딩
std::string AnsiToUtf8(std::string ansi_str)
{
	return CW2A(CA2W(ansi_str.c_str()).m_psz, CP_UTF8).m_psz;
}

// utf8 -> unicode -> ansi 디코딩
std::string Utf8ToAnsi(std::string utf8_str)
{
	return CW2A(CA2W(utf8_str.c_str(), CP_UTF8).m_psz).m_psz;
}

size_t writeFunctionForChat(void* ptr, size_t size, size_t nmemb, void* data);

void gptFromNatural(void* user, bool* gpt_proc)
{
	*gpt_proc = true;

	// create json body
	Json::Value json_body;
	json_body["model"] = "whisper-1";
	json_body["file"] = "output.wav";

	// http post request
	auto result = OpenAI::Create(OpenAI::EndPoint::Whisper, json_body);
	static_cast<QLabel*>(user)->setText(result["text"].asCString());
	messages.append(OpenAI::CreateMessage("user", result["text"].asCString()));

	HttpContext->SetWriteFunction(writeFunctionForChat);

	// create json body
	Json::Value body_data;
	body_data["model"] = "gpt-3.5-turbo";
	body_data["messages"] = messages;
	body_data["stream"] = true;
	OpenAI::Create(OpenAI::EndPoint::Chat, body_data);
	
	HttpContext->SetWriteFunction();

	*gpt_proc = false;
}

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

	auto ui = static_cast<Ui::GUIMainClass*>(data);

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
						reply += utf8;
						ui->gpt_answer->setText(ui->gpt_answer->text() + utf8.c_str());
					}
				}
			}
			catch (std::exception&)
			{
				ui->pushButton->setText("말하기");
				messages.append(OpenAI::CreateMessage("assistant", reply.c_str()));
				reply = "";
			}
		}
	}

	PostHelper->Get()->response.append(json.c_str(), size * nmemb);
	return size * nmemb;
}