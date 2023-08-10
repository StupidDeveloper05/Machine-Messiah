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
        void _ReGenerate();

        // ä�� ��� �г� ���� �ݱ�
        void _OpenPane();
        void _ClosePane();

        // �׻� ���� ���� ��� �Ѱ� ����
        void _AlwaysTopOn();
        void _AlwaysTopOff();

    private:
        ChatThumbnail   m_selectedChat;
        std::wstring    m_activatedChatUuid;
        std::wstring    m_newChatUuid;

        // ��ũ�ٿ� ���� ��Ʈ �� ���� Ű
        unsigned short  m_port;
        std::wstring    m_key;
        Json::Value*    m_data;
        
        bool            m_shiftDown; // shift Ű ���� üũ�� -> shift + Enter�� �ٹٲ��ϱ� ����
        bool            m_isPaneOpen; // ä�� ����� �����ִ��� ����
        bool            m_beforePaneOpen; // �׻� ���� ���� ��� ���� ���� ���
        bool            m_isAlwaysTop; // ���� �׻� ���� ���� �������
        bool            m_loaded;

        winrt::Microsoft::UI::Xaml::DispatcherTimer timer; // �̺�Ʈ ������

        std::unordered_map<std::string, OpenAI::_chatInfo> m_chatInfo; // �ش� uuid�� ä�� ����

    public:
        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        // ä�� ��Ͽ��� �������� Ŭ�� ������ ó��
        void ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        // ä���� �ԷµǸ� ���� ������ ����
        void input_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        // �� ä��, ���� ����, ���� ��ư ũ�� ������
        void ListPart_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        // ���� ��ư Ŭ�� �̺�Ʈ
        void Send(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // �� ä�� ���� ��ư Ŭ�� �̺�Ʈ
        void NewChatPage(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // ���� ���� Ŭ�� �̺�Ʈ -> dialog�� ����� ó���ϱ� ���ؼ� �񵿱���
        Windows::Foundation::IAsyncAction EditTitle_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // ä�� ���� Ŭ�� �̺�Ʈ -> dialog�� ����� ó���ϱ� ���ؼ� �񵿱���
        Windows::Foundation::IAsyncAction Delete_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // Tool��ư�� ���� ���� ä���� �����ϰų� �亯�� ����� �ϴ� ����� ��. Update�� �������� ������ ��ȯ�ϱ� ����
        void Tool_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ToolBtnUpdate(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&);
        // ä�� �Է�â���� Ű�� �ν��Ͽ� ������ ó���ϱ� ����
        void input_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        // ä�� �Է�â�� ũ�⿡ ���� ������ ũ�⵵ �����ϱ� ����
        void input_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        // shiftŰ ó����
        void Page_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        // ��ũ�ٿ� ���� ��Ŭ�� �� ������ �� ���� ���ϰ� ���� ����
        Windows::Foundation::IAsyncAction mdViewer_CoreWebView2Initialized(winrt::Microsoft::UI::Xaml::Controls::WebView2 const& sender, winrt::Microsoft::UI::Xaml::Controls::CoreWebView2InitializedEventArgs const& args);
        // �׻� ���� ������� ��ư Ŭ�� �̺�Ʈ
        void AlwaysTop_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // ä�ø�� ���� �ݱ� ��ư Ŭ�� �̺�Ʈ
        void ClosePane_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // api Ű �Է� �̺�Ʈ
        void apiKey_PasswordChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        Windows::Foundation::IAsyncAction ApiKeyEdit_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct ChattingList : ChattingListT<ChattingList, implementation::ChattingList>
    {
    };
}
