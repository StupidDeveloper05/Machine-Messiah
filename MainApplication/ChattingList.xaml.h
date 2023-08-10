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

        // 채팅 목록 패널 열고 닫기
        void _OpenPane();
        void _ClosePane();

        // 항상 위에 유지 모드 켜고 끄기
        void _AlwaysTopOn();
        void _AlwaysTopOff();

    private:
        ChatThumbnail   m_selectedChat;
        std::wstring    m_activatedChatUuid;
        std::wstring    m_newChatUuid;

        // 마크다운 서버 포트 및 접근 키
        unsigned short  m_port;
        std::wstring    m_key;
        Json::Value*    m_data;
        
        bool            m_shiftDown; // shift 키 눌림 체크용 -> shift + Enter면 줄바꿈하기 위함
        bool            m_isPaneOpen; // 채팅 목록이 열려있는지 여부
        bool            m_beforePaneOpen; // 항상 위에 유지 모드 전의 상태 기록
        bool            m_isAlwaysTop; // 현재 항상 위에 유지 모드인지
        bool            m_loaded;

        winrt::Microsoft::UI::Xaml::DispatcherTimer timer; // 이벤트 생성용

        std::unordered_map<std::string, OpenAI::_chatInfo> m_chatInfo; // 해당 uuid의 채팅 정보

    public:
        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        // 채팅 목록에서 아이템을 클릭 했을때 처리
        void ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        // 채팅이 입력되면 전송 아이콘 변경
        void input_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        // 새 채팅, 제목 변경, 삭제 버튼 크기 조정용
        void ListPart_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        // 전송 버튼 클릭 이벤트
        void Send(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // 새 채팅 생성 버튼 클릭 이벤트
        void NewChatPage(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // 제목 변경 클릭 이벤트 -> dialog를 띄워서 처리하기 위해서 비동기임
        Windows::Foundation::IAsyncAction EditTitle_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // 채팅 삭제 클릭 이벤트 -> dialog를 띄워서 처리하기 위해서 비동기임
        Windows::Foundation::IAsyncAction Delete_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // Tool버튼은 때에 따라 채팅을 중지하거나 답변을 재생성 하는 기능을 함. Update는 아이콘을 적절히 변환하기 위함
        void Tool_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ToolBtnUpdate(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&);
        // 채팅 입력창에서 키를 인식하여 적절히 처리하기 위함
        void input_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        // 채팅 입력창의 크기에 따라 웹뷰의 크기도 조정하기 위함
        void input_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        // shift키 처리용
        void Page_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        // 마크다운 뷰어에서 우클릭 및 개발자 툴 열지 못하게 막기 위함
        Windows::Foundation::IAsyncAction mdViewer_CoreWebView2Initialized(winrt::Microsoft::UI::Xaml::Controls::WebView2 const& sender, winrt::Microsoft::UI::Xaml::Controls::CoreWebView2InitializedEventArgs const& args);
        // 항상 위에 유지모드 버튼 클릭 이벤트
        void AlwaysTop_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // 채팅목록 열고 닫기 버튼 클릭 이벤트
        void ClosePane_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        // api 키 입력 이벤트
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
