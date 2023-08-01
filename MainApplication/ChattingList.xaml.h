// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "ChattingList.g.h"
#include <json/json.h>
#include <unordered_map>

#include "OpenAIStorage.h"

#include <winrt/Microsoft.UI.Xaml.Input.h>

namespace winrt::MainApplication::implementation
{
    struct ChattingList : ChattingListT<ChattingList>
    {
        ChattingList();

    private:
        std::string GetStringFromWstring(const std::wstring& str);
        std::wstring GetWStringFromString(const std::string& str);
        std::wstring GenerateUUID();
        void CreateNewChat(const std::wstring& _title);
        void _Send();
        void _ReGenerate(bool _lastIsAssistant);

    private:
        ChatThumbnail   m_selectedChat;
        std::wstring    m_activatedChatUuid;
        std::wstring    m_newChatUuid;

        unsigned short  m_port;
        std::wstring    m_key;
        Json::Value*    m_data;

        bool            m_shiftDown;

        winrt::Microsoft::UI::Xaml::DispatcherTimer timer;

        std::unordered_map<std::string, OpenAI::_chatInfo> m_chatInfo; // 해당 uuid의 채팅 정보

    public:
        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void input_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void ListPart_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void Send(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NewChatPage(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        Windows::Foundation::IAsyncAction EditTitle_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        Windows::Foundation::IAsyncAction Delete_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Tool_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void input_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void Page_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void input_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        Windows::Foundation::IAsyncAction mdViewer_CoreWebView2Initialized(winrt::Microsoft::UI::Xaml::Controls::WebView2 const& sender, winrt::Microsoft::UI::Xaml::Controls::CoreWebView2InitializedEventArgs const& args);
        void ToolBtnUpdate(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&);
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct ChattingList : ChattingListT<ChattingList, implementation::ChattingList>
    {
    };
}
