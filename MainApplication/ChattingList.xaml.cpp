// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ChattingList.xaml.h"
#if __has_include("ChattingList.g.cpp")
#include "ChattingList.g.cpp"
#endif

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Microsoft.Web.WebView2.Core.h>

#include <vector>
#include <regex>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <Rpc.h>

#include "utils.h"
#include "FileSystem.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::chrono_literals;

bool cmp(std::pair<std::string, time_t>& a, std::pair<std::string, time_t>& b)
{
    return a.second > b.second;
}

namespace winrt::MainApplication::implementation
{
    ChattingList::ChattingList()
        : m_selectedChat{nullptr}
        , m_activatedChatUuid(L"")
        , m_shiftDown(false)
    {
        InitializeComponent();

        // new chat용 uuid 생성
        m_activatedChatUuid = GenerateUUID();
        m_newChatUuid = m_activatedChatUuid;

        // 채팅 기록 로드
        m_data = &DATA["chatting"];
        
        // 생성 시간을 기준으로 키 정렬
        std::vector<std::pair<std::string, time_t>> uuids;
        for (auto key : m_data->getMemberNames())
            uuids.push_back({ key, (*m_data)[key.c_str()]["created_time"].asInt64() });
        std::sort(uuids.begin(), uuids.end(), cmp);

        // 채팅 리스트뷰에 추가
        for (auto pair: uuids)
            ChatList().Items().Append(ChatThumbnail(GetWStringFromString(Utf8ToAnsi((*m_data)[pair.first.c_str()]["title"].asCString())), pair.second, GetWStringFromString(pair.first.c_str())));
        
        input().PlaceholderText(L"질문을 입력하세요. (줄바꿈 Shift + Enter)");

        timer.Tick({ this, &ChattingList::ToolBtnUpdate });
        timer.Interval(100ms);
        timer.Start();
    }

    void ChattingList::ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
    {
        if (e.AddedItems().Size() > 0)
        {
            for (int i = 0; i < e.AddedItems().Size(); ++i)
            {
                m_selectedChat = e.AddedItems().GetAt(i).as<ChatThumbnail>();
                Application::Current().as<App>()->window.as<MainWindow>()->mainNav().Header(box_value(m_selectedChat.Title()));
                if (!(m_activatedChatUuid == m_selectedChat.Uuid())) {
                    m_activatedChatUuid = m_selectedChat.Uuid();
                    if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning)
                        ToolIcon().Glyph(L"\uE71A");
                    else
                        ToolIcon().Glyph(L"\uE72C");
                    mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
                }
            }
        }
        if (e.RemovedItems().Size() > 0)
        {
            for (int i = 0; i < e.RemovedItems().Size(); ++i)
            {
                auto removedObject = e.RemovedItems().GetAt(i).as<ChatThumbnail>();
            }
        }
    }
    
    void ChattingList::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        if (m_selectedChat != nullptr)
            Application::Current().as<App>()->window.as<MainWindow>()->mainNav().Header(box_value(m_selectedChat.Title()));
        auto params = e.Parameter().as<Windows::Foundation::Collections::ValueSet>();
        m_port = params.Lookup(L"port").as<unsigned short>();
        m_key = params.Lookup(L"key").as<hstring>().c_str();
        mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
    }

    Windows::Foundation::IAsyncAction ChattingList::mdViewer_CoreWebView2Initialized(winrt::Microsoft::UI::Xaml::Controls::WebView2 const& sender, winrt::Microsoft::UI::Xaml::Controls::CoreWebView2InitializedEventArgs const& args)
    {
        // webview 우클릭 방지
        co_await mdViewer().EnsureCoreWebView2Async();
        mdViewer().CoreWebView2().Settings().AreBrowserAcceleratorKeysEnabled(false);
        mdViewer().CoreWebView2().Settings().AreDefaultContextMenusEnabled(false);
        mdViewer().CoreWebView2().Settings().AreDevToolsEnabled(false);
    }
    
    void ChattingList::_Send()
    {
        if (input().Text().size() != 0)
        {
            if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning)
                return;

            if (m_activatedChatUuid == m_newChatUuid)
                CreateNewChat(input().Text().c_str());

            // copy text and clear input
            auto inputText = AnsiToUtf8(GetStringFromWstring(input().Text().c_str()));
            inputText = std::regex_replace(inputText, std::regex("\r"), "\n");

            // 현재 chat을 작동 중으로 변경
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning = true;
            // stop을 false로 설정 true이면 응답을 중지함
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].stop = false;
            // del을 false로 설정 true이면 해당 채팅은 삭제됨을 의미
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].del = false;
            // isBegin 이면 새로운 말풍선 생성 아니면 이어서 작성
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isBegin = true;
            // started는 최초 소켓 전송이 이루어졌는지 여부임. 이를 통해 stop, regenerate 여부를 판단
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started = false;

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
            OpenAI::OpenAIStorage::Get()->Create(OpenAI::EndPoint::Chat, json_body, GetStringFromWstring(m_activatedChatUuid), &m_chatInfo[GetStringFromWstring(m_activatedChatUuid)]);

            // save File
            SaveData();

            // clear input
            input().Text(L"");
        }
    }

    void ChattingList::Send(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        _Send();
    }

    void ChattingList::_ReGenerate(bool _lastIsAssistant)
    {
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning = true;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].stop = false;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].del = false;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isBegin = !_lastIsAssistant;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started = false;

        // send user request to openai api
        Json::Value json_body;
        json_body["model"] = "gpt-3.5-turbo";
        json_body["messages"] = (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"];
        json_body["stream"] = true;
        OpenAI::OpenAIStorage::Get()->Create(OpenAI::EndPoint::Chat, json_body, GetStringFromWstring(m_activatedChatUuid), &m_chatInfo[GetStringFromWstring(m_activatedChatUuid)]);
    }
    
    void ChattingList::Tool_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started)
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].stop = true;
        else if (!m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started && !m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning)
        {
            int size = (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"].size();
            if (size == 0)
                return;
            if ((*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"][size - 1]["role"].asCString() == "user")
                _ReGenerate(false);
            else
            {
                bool isLastAssistant = true;
                if (std::string((*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"][size - 1]["content"].asCString()).empty())
                    isLastAssistant = false;

                Json::Value* ptr = nullptr;
                (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"].removeIndex(size - 1, ptr);
                _ReGenerate(isLastAssistant);
            }
        }
    }

    void ChattingList::ToolBtnUpdate(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&)
    {
        if (m_selectedChat == nullptr)
            return;

        if (!m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning)
        {
            if (ToolIcon().Glyph() == L"\uE71A")
                ToolIcon().Glyph(L"\uE72C");
        }
        else
        {
            if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started && ToolIcon().Glyph() == L"\uE72C")
                ToolIcon().Glyph(L"\uE71A");
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

    void ChattingList::input_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
    {
        if (e.Key() == Windows::System::VirtualKey::Shift)
            m_shiftDown = true;
        
        if (e.Key() == Windows::System::VirtualKey::Enter)
        {
            if (m_shiftDown)
            {
                input().Text(input().Text() + L"\r");
                input().Select(input().Text().size(), 0);
            }
            else
                _Send();
        }
    }

    void ChattingList::input_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
    {
        auto size = e.NewSize();
        Microsoft::UI::Xaml::GridLength length {};
        length.GridUnitType = Microsoft::UI::Xaml::GridUnitType::Pixel;
        length.Value = size.Height;
        inputBoxRow().Height(length);
    }

    void ChattingList::Page_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
    {
        if (e.Key() == Windows::System::VirtualKey::Shift)
            m_shiftDown = false;
    }

    void ChattingList::ListPart_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
    {
        NewChat().Width((ListPart().ActualWidth() - 15) / 3);
        EditTitle().Width((ListPart().ActualWidth() - 15) / 3);
        Delete().Width((ListPart().ActualWidth() - 15) / 3);
    }

    void ChattingList::NewChatPage(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        Application::Current().as<App>()->window.as<MainWindow>()->mainNav().Header(box_value(L"채팅 목록"));
        ChatList().SelectedItem(nullptr);
        m_selectedChat = nullptr;
        m_activatedChatUuid = m_newChatUuid;
        mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
    }

    Windows::Foundation::IAsyncAction ChattingList::EditTitle_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_selectedChat == nullptr)
            return;

        EditTitleContentDialog().Title(box_value(L"채팅 제목 변경"));
        EditTitleContentDialog().PrimaryButtonText(L"변경");
        EditTitleContentDialog().CloseButtonText(L"취소");
        EditTitleContentDialog().DefaultButton(Controls::ContentDialogButton::Primary);
        titleInput().PlaceholderText(m_selectedChat.Title());
        auto result = co_await EditTitleContentDialog().ShowAsync();
        if (result == Controls::ContentDialogResult::Primary && titleInput().Text().size() != 0)
        {
            time_t t = m_selectedChat.Date();

			uint32_t index;
			ChatList().Items().IndexOf(m_selectedChat, index);
			ChatList().Items().RemoveAt(index);

			ChatList().Items().InsertAt(index, ChatThumbnail(titleInput().Text(), t, m_activatedChatUuid));
			ChatList().SelectedItem(ChatList().Items().GetAt(index));

			SAFE_ACCESS;
			DATA["chatting"][GetStringFromWstring(m_activatedChatUuid)]["title"] = AnsiToUtf8(GetStringFromWstring(titleInput().Text().c_str()));
			SaveData();

            titleInput().Text(L"");
        }
    }

    Windows::Foundation::IAsyncAction ChattingList::Delete_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_activatedChatUuid != m_newChatUuid)
        {
            Controls::ContentDialog dialog;
            dialog.XamlRoot(this->XamlRoot());
            dialog.Title(box_value(L"채팅 기록 삭제"));
            dialog.Content(box_value(L"해당 채팅 기록을 삭제하시겠습니까?"));
            dialog.PrimaryButtonText(L"삭제");
            dialog.CloseButtonText(L"취소");
            auto result = co_await dialog.ShowAsync();
            if (result == Controls::ContentDialogResult::Primary)
            {
                // 응답 중지
                m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].del = true;

                // 채팅 리스트에서 삭제
                uint32_t index;
                ChatList().Items().IndexOf(m_selectedChat, index);
                ChatList().Items().RemoveAt(index);
                m_selectedChat = nullptr;

                // navigation 헤더 변경
                Application::Current().as<App>()->window.as<MainWindow>()->mainNav().Header(box_value(L"채팅 목록"));

                // 해당 chat을 삭제한 후 저장
                SAFE_ACCESS;
                DATA["chatting"].removeMember(GetStringFromWstring(m_activatedChatUuid));
                SaveData();

                // 자동으로 new chat 페이지로 이동
                ChatList().SelectedItem(nullptr);
                m_activatedChatUuid = m_newChatUuid;
                mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
            }
        }
    }

    void ChattingList::CreateNewChat(const std::wstring& _title)
    {
        std::wstring title = _title.substr(0, _title.size() < 30 ? _title.size() : 30);
        auto t = time(NULL);
        ChatList().Items().InsertAt(0, ChatThumbnail(title, t, m_newChatUuid));
        ChatList().SelectedItem(ChatList().Items().GetAt(0));

        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["title"] = AnsiToUtf8(GetStringFromWstring(title));
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
        UUID uuid;
        UuidCreate(&uuid);
        const wchar_t* key;
        UuidToStringW(&uuid, (RPC_WSTR*)&key);

        if (DATA["chatting"].isMember(GetStringFromWstring(key)))
        {
            key = GenerateUUID().c_str();
        }    
        return key;
    }
}
