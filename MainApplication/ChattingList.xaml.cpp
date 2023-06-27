// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ChattingList.xaml.h"
#if __has_include("ChattingList.g.cpp")
#include "ChattingList.g.cpp"
#endif

#include <iostream>
#include <ctime>
#include <regex>
#include "utils.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    ChattingList::ChattingList()
        : m_selectedChat{L"No Chat Selected", 0, L""}
        , m_activatedChatUuid(L"untitled")
    {
        InitializeComponent();

        m_data = &DATA["chatting"];
        for (auto key : m_data->getMemberNames())
        {
            ChatList().Items().Append(ChatThumbnail(GetWStringFromString((*m_data)[key.c_str()]["title"].asCString()), (*m_data)[key.c_str()]["created_time"].asInt64(), GetWStringFromString(key.c_str())));
        }
        m_activatedChatUuid = GetWStringFromString(m_data->getMemberNames()[0].c_str());

        input().PlaceholderText(L"질문을 입력하세요.");
    }

    void ChattingList::ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
    {
        if (e.AddedItems().Size() > 0)
        {
            for (int i = 0; i < e.AddedItems().Size(); ++i)
            {
                m_selectedChat = e.AddedItems().GetAt(i).as<ChatThumbnail>();
                m_activatedChatUuid = m_selectedChat.Uuid();
                mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
                SendToClient();
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
        SendToClient();
    }
    
    void ChattingList::Send(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (input().Text().size() != 0)
        {
            auto inputText = AnsiToUtf8(GetStringFromWstring(input().Text().c_str()));
            inputText = std::regex_replace(inputText, std::regex("\r"), "\n");

            // send user request to openai api
            

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
            Json::Value data; data["user"] = inputText;
            (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"].append(data);

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
        NewChat().Width(ListPart().ActualWidth());
    }

    void ChattingList::CreateNewChat(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        auto uuid = GenerateUUID();
        auto t = time(NULL);
        ChatList().Items().Append(ChatThumbnail(L"Untitled Chat", t, uuid));
        ChatList().SelectedItem(ChatList().Items().GetAt(ChatList().Items().Size() - 1));

        DATA["chatting"][GetStringFromWstring(uuid)]["title"] = "Untitled Chat";
        DATA["chatting"][GetStringFromWstring(uuid)]["created_time"] = t;
        DATA["chatting"][GetStringFromWstring(uuid)]["data"] = Json::Value(Json::arrayValue);

        m_activatedChatUuid = uuid;
        mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
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
    
    void ChattingList::SendToClient()
    {
        // clear chatting page
        auto clear = MDView::CreateMessage(
            GetStringFromWstring(m_key),
            GetStringFromWstring(m_activatedChatUuid),
            "", "", "start", true
        );
        if (CLIENT.isConnected())
            CLIENT.send(clear);

        for (auto const& message : (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"])
        {
            for (auto const& key : message.getMemberNames())
            {
                auto msg = MDView::CreateMessage(
                    GetStringFromWstring(m_key),
                    GetStringFromWstring(m_activatedChatUuid),
                    message[key].asCString(),
                    key,
                    "start",
                    false
                );
                if (CLIENT.isConnected())
                    CLIENT.send(msg);
            }
        }
    }
}