// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "ChattingList.g.h"
#include <json/json.h>
#include <unordered_map>

namespace winrt::MainApplication::implementation
{
    struct _chatInfo
    {
        bool isRunning;
    };

    struct ChattingList : ChattingListT<ChattingList>
    {
        ChattingList();

    private:
        std::string GetStringFromWstring(const std::wstring& str);
        std::wstring GetWStringFromString(const std::string& str);
        std::wstring GenerateUUID();
        void CreateNewChat();

    private:
        ChatThumbnail   m_selectedChat;
        std::wstring    m_activatedChatUuid;
        std::wstring    m_newChatUuid;

        unsigned short  m_port;
        std::wstring    m_key;
        Json::Value*    m_data;

        std::unordered_map<std::string, _chatInfo> m_chatInfo; // 해당 uuid의 채팅 정보

    public:
        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void input_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void ListPart_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void Send(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NewChatPage(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct ChattingList : ChattingListT<ChattingList, implementation::ChattingList>
    {
    };
}
