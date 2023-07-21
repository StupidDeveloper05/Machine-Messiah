// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ChattingList.xaml.h"
#if __has_include("ChattingList.g.cpp")
#include "ChattingList.g.cpp"
#endif

#include <winrt/Microsoft.Web.WebView2.Core.h>

#include <iostream>
#include <ctime>
#include <regex>

#include "utils.h"
#include "FileSystem.h"
#include "OpenAIStorage.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    ChattingList::ChattingList()
        : m_selectedChat{nullptr}
        , m_activatedChatUuid(L"")
    {
        InitializeComponent();

        // new chat용 uuid 생성
        m_activatedChatUuid = GenerateUUID();
        m_newChatUuid = m_activatedChatUuid;

        // 채팅 기록 로드
        m_data = &DATA["chatting"];
        for (auto key : m_data->getMemberNames())
        {
            ChatList().Items().Append(ChatThumbnail(GetWStringFromString((*m_data)[key.c_str()]["title"].asCString()), (*m_data)[key.c_str()]["created_time"].asInt64(), GetWStringFromString(key.c_str())));
        }
        input().PlaceholderText(L"질문을 입력하세요.");
    }

    void ChattingList::ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
    {
        if (e.AddedItems().Size() > 0)
        {
            for (int i = 0; i < e.AddedItems().Size(); ++i)
            {
                m_selectedChat = e.AddedItems().GetAt(i).as<ChatThumbnail>();
                if (!(m_activatedChatUuid == m_selectedChat.Uuid())) {
                    m_activatedChatUuid = m_selectedChat.Uuid();
                    mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
                }
            }
        }
        if (e.RemovedItems().Size() > 0)
        {
            for (int i = 0; i < e.RemovedItems().Size(); ++i)
            {
                auto removedObject = e.RemovedItems().GetAt(i).as<ChatThumbnail>();
                auto str = removedObject.Title();
            }
        }
    }
    
    void ChattingList::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        auto params = e.Parameter().as<Windows::Foundation::Collections::ValueSet>();
        m_port = params.Lookup(L"port").as<unsigned short>();
        m_key = params.Lookup(L"key").as<hstring>().c_str();
        mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
    }
    
    void ChattingList::Send(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (input().Text().size() != 0)
        {
            if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning)
                return;

            if (m_activatedChatUuid == m_newChatUuid)
                CreateNewChat();

            // copy text and clear input
            auto inputText = AnsiToUtf8(GetStringFromWstring(input().Text().c_str()));
            inputText = std::regex_replace(inputText, std::regex("\r"), "\n");        

            // 현재 chat을 작동 중으로 변경
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning = true;

            // send user request to markdown viewer server
            auto msg = MDView::CreateMessage(
                GetStringFromWstring(m_key),
                GetStringFromWstring(m_activatedChatUuid),
                inputText,
                "user",
                "start",
                false
            );
            if (CLIENT.isConnected())
                CLIENT.send(msg);

            // save to DATA
           (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"].append(OpenAI::CreateMessage("user", inputText.c_str()));

            // send user request to openai api
            Json::Value json_body;
            json_body["model"] = "gpt-3.5-turbo";
            json_body["messages"] = (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"];
            json_body["stream"] = true;
            OpenAI::OpenAIStorage::Get()->Create(OpenAI::EndPoint::Chat, json_body, GetStringFromWstring(m_activatedChatUuid), &m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning);

            // save File
            SaveData();

            // clear input
            input().Text(L"");
        }
    }

    void ChattingList::input_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e)
    {
        if (input().Text().size() != 0)
        {
            SendIcon().Glyph(L"\uE725");
        }
        else
        {
            SendIcon().Glyph(L"\uE724");
        }
    }

    void ChattingList::ListPart_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
    {
        NewChat().Width(ListPart().ActualWidth() - 10);
    }

    void ChattingList::NewChatPage(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        ChatList().SelectedItem(nullptr);
        m_activatedChatUuid = m_newChatUuid;
        mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
    }

    void ChattingList::CreateNewChat()
    {
        auto t = time(NULL);
        ChatList().Items().Append(ChatThumbnail(L"Untitled Chat", t, m_newChatUuid));
        ChatList().SelectedItem(ChatList().Items().GetAt(ChatList().Items().Size() - 1));

        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["title"] = "Untitled Chat";
        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["created_time"] = t;
        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["data"] = Json::Value(Json::arrayValue);

        m_newChatUuid = GenerateUUID();
    }

    std::string ChattingList::GetStringFromWstring(const std::wstring& str)
    {
        std::string result(CW2A(str.c_str()).m_psz);
        return result;
    }

    std::wstring ChattingList::GetWStringFromString(const std::string& str)
    {
        std::wstring result(CA2W(str.c_str()).m_psz);
        return result;
    }
    
    std::wstring ChattingList::GenerateUUID()
    {
        srand(time(NULL));
        std::wstring alphabet = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::wstring key;
        for (int i = 0; i < 32; ++i)
        {
            key += alphabet[rand() % alphabet.size()];
        }

        if (DATA["chatting"].isMember(GetStringFromWstring(key)))
        {
            key = GenerateUUID();
        }

        return key;
    }
}