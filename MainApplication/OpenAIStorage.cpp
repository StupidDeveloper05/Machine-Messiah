#include "pch.h"
#include "OpenAIStorage.h"
#include "FileSystem.h"
#include "FunctionManager.h"

#include <atlstr.h>
#include <algorithm>

namespace OpenAI
{
	std::string OpenAIStorage::m_key;
	OpenAIStorage* OpenAIStorage::instance;

	OpenAIStorage::OpenAIStorage()
	{
	}

	OpenAIStorage::~OpenAIStorage()
	{
		for (auto session : openAISessions)
		{
			delete session;
		}
	}
	
	OpenAIStorage* OpenAIStorage::Get()
	{
		if (instance == nullptr)
		{
			instance = new OpenAIStorage();
		}
		return instance;
	}
	
	void OpenAIStorage::Destroy()
	{
		delete instance;
	}
	
	void OpenAIStorage::Create(EndPoint eType, Json::Value& json_body, const std::string& uuid, _chatInfo* p_chatInfo)
	{
		if (openAISessions.size() == 0)
		{
			auto oai = new OpenAI(DATA["API_KEY"].asCString());
			_msgData* data = new _msgData(uuid, p_chatInfo);
			data->curl = oai->GetHandel();

			oai->SetWriteFunction(OpenAIStorage::writeFunctionChat);
			oai->SetFinishCallback(OpenAIStorage::finishCallback);
			oai->SetUserPointer(data);
			openAISessions.push_back(oai);
			std::thread(&OpenAI::Create, openAISessions[0], eType, json_body).detach();
		}
		else
		{
			bool found = false;
			for (auto session : openAISessions)
			{
				if (session->IsAvailiable())
				{
					found = true;
					auto data = static_cast<_msgData*>(session->GetUserPointer());
					data->uuid = uuid;
					data->msg = "";
					data->curl = session->GetHandel();
					data->chatInfo = p_chatInfo;
					curl_easy_setopt(data->curl, CURLOPT_TIMEOUT_MS, 0);

					session->SetWriteFunction(OpenAIStorage::writeFunctionChat);
					session->SetFinishCallback(OpenAIStorage::finishCallback);
					session->SetUserPointer(data);
					std::thread(&OpenAI::Create, session, eType, json_body).detach();
					break;
				}
			}
			if (!found)
			{
				auto oai = new OpenAI(DATA["API_KEY"].asCString());
				_msgData* data = new _msgData(uuid, p_chatInfo);
				data->curl = oai->GetHandel();

				oai->SetWriteFunction(OpenAIStorage::writeFunctionChat);
				oai->SetFinishCallback(OpenAIStorage::finishCallback);
				oai->SetUserPointer(data);
				openAISessions.push_back(oai);
				std::thread(&OpenAI::Create, openAISessions[openAISessions.size() - 1], eType, json_body).detach();
			}
		}
	}

	bool OpenAIStorage::CheckAllCompleted()
	{
		bool result = true;
		for (auto session : openAISessions)
		{
			if (!session->IsAvailiable() && static_cast<_msgData*>(session->GetUserPointer())->chatInfo->isRunning)
				result = false;
		}
		return result;
	}

	size_t OpenAIStorage::writeFunctionChat(void* ptr, size_t size, size_t nmemb, void* data)
	{
		std::string json = (char*)ptr;
		json = json.substr(0, size * nmemb); // ���� �ذ�: �����Ͱ� �ߺ����� �ʰ� �ݹ��Լ��� ���� ������ ��ŭ �����͸� �����̽� ��.
		std::string str_buf;
		std::istringstream iss(json);

		Json::Value root;
		Json::CharReaderBuilder builder;
		Json::CharReader* reader = builder.newCharReader();
		std::string errors;

		_msgData* msgData = static_cast<_msgData*>(data);

		// �ش� ä���� ���� �� �� ó��
		if (msgData->chatInfo->del)
		{
			curl_easy_setopt(msgData->curl, CURLOPT_TIMEOUT_MS, 1);
			return size * nmemb;
		}

		// �ش� ä���� ���� �Ǿ��� �� ó��
		if (msgData->chatInfo->stop)
		{
			curl_easy_setopt(msgData->curl, CURLOPT_TIMEOUT_MS, 1);
			SAFE_ACCESS;
			if (DATA["chatting"][msgData->uuid]["data"][DATA["chatting"][msgData->uuid]["data"].size() - 1]["role"] == "function")
			{
				Json::Value* ptr = nullptr;
				DATA["chatting"][msgData->uuid]["data"].removeIndex(DATA["chatting"][msgData->uuid]["data"].size() - 1, ptr);
			}
			DATA["chatting"][msgData->uuid]["data"].append(CreateMessage("assistant", msgData->msg.c_str()));
			SaveData();
			return size * nmemb;
		}

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
							if (!root["choices"][0]["delta"].isMember("function_call"))
							{
								std::string utf8 = root["choices"][0]["delta"].get("content", "").asCString();
								msgData->msg += utf8;

								if (msgData->msg != "")
								{
									auto msg = MDView::CreateMessage(
										m_key,
										msgData->uuid.c_str(),
										msgData->msg,
										"assistant",
										msgData->chatInfo->isBegin ? "start" : "running"
									);
									if (CLIENT.isConnected())
										CLIENT.send(msg);

									msgData->chatInfo->isBegin = false;
									msgData->chatInfo->started = true;
								}
							}
							else
							{
								// function called
								if (root["choices"][0]["delta"]["function_call"].isMember("name"))
									msgData->chatInfo->funcInfo.name = root["choices"][0]["delta"]["function_call"]["name"].asCString();
								msgData->chatInfo->funcInfo.arguments += root["choices"][0]["delta"]["function_call"].get("arguments", "").asCString();
							}
						}
					}
				}
				catch (std::exception&)
				{
					if (msgData->chatInfo->funcInfo.name.empty())
					{
						SAFE_ACCESS;
						// ������ role�� function�̸� ���� -> �̹� ����� �Լ��� ��û���� �ʴ� Ư�� �ذ� -> �׻� �Լ� �����ϵ���
						if (DATA["chatting"][msgData->uuid]["data"][DATA["chatting"][msgData->uuid]["data"].size() - 1]["role"] == "function")
						{
							Json::Value* ptr = nullptr;
							DATA["chatting"][msgData->uuid]["data"].removeIndex(DATA["chatting"][msgData->uuid]["data"].size() - 1, ptr);
						}
						DATA["chatting"][msgData->uuid]["data"].append(CreateMessage("assistant", msgData->msg.c_str()));
						SaveData();
					}
					else
					{
						// �Լ� ����
						SmartMode::FunctionManager::Get()->Call(msgData->chatInfo->funcInfo.name, msgData->chatInfo->funcInfo.arguments, msgData);
					}
				}
			}
		}
		else
		{
			msgData->chatInfo->isRunning = false;
			bool parsing_successful = reader->parse(json.c_str(), json.c_str() + json.size(), &root, &errors);
			if (parsing_successful) {
				auto msg = MDView::CreateMessage(
					m_key,
					msgData->uuid.c_str(),
					std::string("> ") + std::string(root["error"]["message"].asCString()),
					"assistant",
					"start"
				);
				if (CLIENT.isConnected())
					CLIENT.send(msg);
			}
		}

		return size * nmemb;
	}
	
	void OpenAIStorage::finishCallback(void* userPtr)
	{
		_msgData* msgData = static_cast<_msgData*>(userPtr);
		msgData->chatInfo->isRunning = false;
		msgData->chatInfo->isBegin = true;
		msgData->chatInfo->started = false;
	}
}