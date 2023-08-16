// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ChattingList.xaml.h"
#if __has_include("ChattingList.g.cpp")
#include "ChattingList.g.cpp"
#endif

#include <winrt/Microsoft.UI.Windowing.h>
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
#include "FunctionManager.h"

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
        , m_isPaneOpen(true)
        , m_beforePaneOpen(true)
        , m_isAlwaysTop(false)
        , m_loaded(false)
    {
        InitializeComponent();

        // new chat�� uuid ����
        m_activatedChatUuid = GenerateUUID();
        m_newChatUuid = m_activatedChatUuid;

        // ä�� ��� �ε�
        m_data = &DATA["chatting"];
        
        // ���� �ð��� �������� Ű ����
        std::vector<std::pair<std::string, time_t>> uuids;
        for (auto key : m_data->getMemberNames())
            uuids.push_back({ key, (*m_data)[key.c_str()]["created_time"].asInt64() });
        std::sort(uuids.begin(), uuids.end(), cmp);

        // ä�� ����Ʈ�信 �߰�
        for (auto pair : uuids)
        {
            ChatList().Items().Append(ChatThumbnail(GetWStringFromString(Utf8ToAnsi((*m_data)[pair.first.c_str()]["title"].asCString())), pair.second, GetWStringFromString(pair.first.c_str())));
            m_chatInfo[pair.first.c_str()].smart_mode = (*m_data)[pair.first.c_str()]["smart_mode"].asBool();
        }

        // �ѱ۶����� ���⼭ ó��
        input().PlaceholderText(L"������ �Է��ϼ���. (�ٹٲ�: Shift + Enter, ���� �Է�: Windows + H)");
        createApiKey().Content(box_value(L"API Ű �����ϱ�"));

        // �� �� ����
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"�� ä��"));
            Controls::ToolTipService::SetToolTip(NewChat(), tooltip);
        }
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"���� ����"));
            Controls::ToolTipService::SetToolTip(EditTitle(), tooltip);
        }
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"����"));
            Controls::ToolTipService::SetToolTip(Delete(), tooltip);
        }
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"����"));
            Controls::ToolTipService::SetToolTip(SendBtn(), tooltip);
        }
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"�ٽ� ����"));
            Controls::ToolTipService::SetToolTip(Tool(), tooltip);
        }
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"�׻� ���� ����"));
            Controls::ToolTipService::SetToolTip(AlwaysTop(), tooltip);
        }
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"ä�� ��� �ݱ�"));
            Controls::ToolTipService::SetToolTip(ClosePane(), tooltip);
        }
        {
            Controls::ToolTip tooltip{};
            tooltip.Content(box_value(L"API Ű ����"));
            Controls::ToolTipService::SetToolTip(ApiKeyEdit(), tooltip);
        }

        // �̺�Ʈ ���
        timer.Tick({ this, &ChattingList::ToolBtnUpdate });
        timer.Interval(100ms);
        timer.Start();
    }

    void ChattingList::AlwaysTop_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        // ������ ��ġ �� ũ�� ����
        auto position = Application::Current().as<App>()->window.as<MainWindow>()->AppWindow().Position();
        auto size = Application::Current().as<App>()->window.as<MainWindow>()->AppWindow().Size();

        auto presenter = Application::Current().as<App>()->window.as<MainWindow>()->AppWindow().Presenter().as<Microsoft::UI::Windowing::OverlappedPresenter>();
        if (!presenter.IsAlwaysOnTop()) {
            presenter.IsMaximizable(false);
            presenter.IsMinimizable(false);
            presenter.IsAlwaysOnTop(true);

            Application::Current().as<App>()->window.as<MainWindow>()->AppWindow().MoveAndResize({ position.X + size.Width - 480, position.Y, 480, 600 });
            Application::Current().as<App>()->window.as<MainWindow>()->mainNav().PaneDisplayMode(Controls::NavigationViewPaneDisplayMode::LeftMinimal);

            _AlwaysTopOn();
            m_beforePaneOpen = m_isPaneOpen;
            if (m_isPaneOpen)
                _ClosePane();
        }
        else
        {
            presenter.IsMaximizable(true);
            presenter.IsMinimizable(true);
            presenter.IsAlwaysOnTop(false);
            
            Application::Current().as<App>()->window.as<MainWindow>()->AppWindow().MoveAndResize({ position.X + size.Width - 1200, position.Y, 1200, 720 });
            Application::Current().as<App>()->window.as<MainWindow>()->mainNav().PaneDisplayMode(Controls::NavigationViewPaneDisplayMode::LeftCompact);

            _AlwaysTopOff();
            if (m_beforePaneOpen != m_isPaneOpen)
            {
                if (m_beforePaneOpen)
                    _OpenPane();
            }
        }
    }

    void ChattingList::ClosePane_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_isPaneOpen)
            _ClosePane();
        else
            _OpenPane();
    }

    void ChattingList::_OpenPane()
    {
        m_isPaneOpen = true;
        ClosePaneIcon().Glyph(L"\uEA49");
        ListPart().Visibility(Visibility::Visible);
        ListColumn().Width({ 0.25, GridUnitType::Star });
        ViewColumn().Width({ 0.75, GridUnitType::Star });

        Controls::ToolTip tooltip{};
        tooltip.Content(box_value(L"ä�� ��� �ݱ�"));
        Controls::ToolTipService::SetToolTip(ClosePane(), tooltip);
    }

    void ChattingList::_ClosePane()
    {
        m_isPaneOpen = false;
        ClosePaneIcon().Glyph(L"\uEA5B");
        ListPart().Visibility(Visibility::Collapsed);
        ListColumn().Width({ 0, GridUnitType::Pixel });
        ViewColumn().Width({ 1, GridUnitType::Star });

        Controls::ToolTip tooltip{};
        tooltip.Content(box_value(L"ä�� ��� ����"));
        Controls::ToolTipService::SetToolTip(ClosePane(), tooltip);
    }

    void ChattingList::_AlwaysTopOn()
    {
        m_isAlwaysTop = true;
        AlwaysTopIcon().Glyph(L"\uEE47");
        Controls::ToolTip tooltip{};
        tooltip.Content(box_value(L"������� ���ư���"));
        Controls::ToolTipService::SetToolTip(AlwaysTop(), tooltip);
    }

    void ChattingList::_AlwaysTopOff()
    {
        m_isAlwaysTop = false;
        AlwaysTopIcon().Glyph(L"\uEE49");
        Controls::ToolTip tooltip{};
        tooltip.Content(box_value(L"�׻� ���� ����"));
        Controls::ToolTipService::SetToolTip(AlwaysTop(), tooltip);
    }

    void ChattingList::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        // mdviewer setting
        auto params = e.Parameter().as<Windows::Foundation::Collections::ValueSet>();
        m_port = params.Lookup(L"port").as<unsigned short>();
        m_key = params.Lookup(L"key").as<hstring>().c_str();
        mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));

        if (!m_loaded)
        {
            m_loaded = true;
            return;
        }

        // change header text
        if (m_selectedChat != nullptr)
            Application::Current().as<App>()->window.as<MainWindow>()->mainNav().Header(box_value(m_selectedChat.Title()));

        // check is always top
        auto presenter = Application::Current().as<App>()->window.as<MainWindow>()->AppWindow().Presenter().as<Microsoft::UI::Windowing::OverlappedPresenter>();
        if (presenter.IsAlwaysOnTop() && !m_isAlwaysTop)
        {
            _AlwaysTopOn();
            m_beforePaneOpen = m_isPaneOpen;
            if (m_isPaneOpen)
                _ClosePane();
        }
        else if (!presenter.IsAlwaysOnTop() && m_isAlwaysTop)
        {
            _AlwaysTopOff();
            if (m_beforePaneOpen != m_isPaneOpen)
            {
                if (m_beforePaneOpen)
                    _OpenPane();
            }
        }
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
                    SmartMode().IsChecked(m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].smart_mode);
                    SmartMode().IsEnabled(false);
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

    Windows::Foundation::IAsyncAction ChattingList::mdViewer_CoreWebView2Initialized(winrt::Microsoft::UI::Xaml::Controls::WebView2 const& sender, winrt::Microsoft::UI::Xaml::Controls::CoreWebView2InitializedEventArgs const& args)
    {
        // webview ��Ŭ�� ����
        co_await mdViewer().EnsureCoreWebView2Async();
        mdViewer().CoreWebView2().Settings().AreBrowserAcceleratorKeysEnabled(false);
        mdViewer().CoreWebView2().Settings().AreDefaultContextMenusEnabled(false);
        mdViewer().CoreWebView2().Settings().AreDevToolsEnabled(false);
        
        if (std::string(DATA["API_KEY"].asCString()).empty())
        {
            ApiKeyInput().Title(box_value(L"OpenAI API Ű �Է�"));
            ApiKeyInput().PrimaryButtonText(L"Ȯ��");
            ApiKeyInput().IsPrimaryButtonEnabled(false);
            apiKey().PlaceholderText(L"OpenAI API Ű�� �Է��ϼ���.");
            auto result = co_await ApiKeyInput().ShowAsync();
            if (result == Controls::ContentDialogResult::Primary)
            {
                DATA["API_KEY"] = GetStringFromWstring(apiKey().Password().c_str());
                SaveData();
            }
        }
    }

    void ChattingList::apiKey_PasswordChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (!apiKey().Password().empty())
        {
            ApiKeyInput().IsPrimaryButtonEnabled(true);
        }
    }

    Windows::Foundation::IAsyncAction ChattingList::ApiKeyEdit_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        ApiKeyInput().Title(box_value(L"OpenAI API Ű ����"));
        ApiKeyInput().PrimaryButtonText(L"Ȯ��");
        ApiKeyInput().CloseButtonText(L"���");
        apiKey().PlaceholderText(L"OpenAI API Ű�� �Է��ϼ���.");
        apiKey().Password(to_hstring(DATA["API_KEY"].asCString()));
        auto result = co_await ApiKeyInput().ShowAsync();
        if (result == Controls::ContentDialogResult::Primary)
        {
            DATA["API_KEY"] = GetStringFromWstring(apiKey().Password().c_str());
            SaveData();
        }
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

            // ���� chat�� �۵� ������ ����
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning = true;
            // stop�� false�� ���� true�̸� ������ ������
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].stop = false;
            // del�� false�� ���� true�̸� �ش� ä���� �������� �ǹ�
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].del = false;
            // isBegin �̸� ���ο� ��ǳ�� ���� �ƴϸ� �̾ �ۼ�
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isBegin = true;
            // started�� ���� ���� ������ �̷�������� ������. �̸� ���� stop, regenerate ���θ� �Ǵ�
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started = false;
            // function ���� - �Լ� ���� ���� �ʱ�ȭ
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].funcInfo.name = "";
            m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].funcInfo.arguments = "";

            // send user request to markdown viewer server
            auto msg = MDView::CreateMessage(
                GetStringFromWstring(m_key),
                GetStringFromWstring(m_activatedChatUuid),
                inputText,
                "user",
                "start"
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
            if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].smart_mode)
            {
                json_body["model"] = "gpt-3.5-turbo-0613";
                json_body["functions"] = SmartMode::FunctionManager::Get()->functions;
            }
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

    void ChattingList::_ReGenerate()
    {
        auto msg = MDView::CreateMessage(
            GetStringFromWstring(m_key),
            GetStringFromWstring(m_activatedChatUuid),
            "",
            "clear_until_user",
            "start"
        );
        if (CLIENT.isConnected())
            CLIENT.send(msg);

        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isRunning = true;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].stop = false;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].del = false;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].isBegin = true;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started = false;
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].funcInfo.name = "";
        m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].funcInfo.arguments = "";

        // send user request to openai api
        Json::Value json_body;
        json_body["model"] = "gpt-3.5-turbo";
        json_body["messages"] = (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"];
        json_body["stream"] = true;
        if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].smart_mode)
        {
            json_body["model"] = "gpt-3.5-turbo-0613";
            json_body["functions"] = SmartMode::FunctionManager::Get()->functions;
        }
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
            if (std::string((*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"][size - 1]["role"].asCString()) == "user")
                _ReGenerate();
            else
            {
                Json::Value* ptr = nullptr;
                for (int i = size - 1; i >= 0; --i)
                {
                    if (std::string((*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"][i]["role"].asCString()) == "user")
                        break;
                    (*m_data)[GetStringFromWstring(m_activatedChatUuid)]["data"].removeIndex(i, ptr);
                }
                _ReGenerate();
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
            {
                ToolIcon().Glyph(L"\uE72C");

                Controls::ToolTip tooltip{};
                tooltip.Content(box_value(L"�ٽ� ����"));
                Controls::ToolTipService::SetToolTip(Tool(), tooltip);
            }
        }
        else
        {
            if (m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].started && ToolIcon().Glyph() == L"\uE72C")
            {
                ToolIcon().Glyph(L"\uE71A");

                Controls::ToolTip tooltip{};
                tooltip.Content(box_value(L"�亯 ����"));
                Controls::ToolTipService::SetToolTip(Tool(), tooltip);
            }
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
        GridLength length {};
        length.GridUnitType = GridUnitType::Pixel;
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
        Application::Current().as<App>()->window.as<MainWindow>()->mainNav().Header(box_value(L"ä�� ���"));
        ChatList().SelectedItem(nullptr);
        m_selectedChat = nullptr;
        m_activatedChatUuid = m_newChatUuid;
        SmartMode().IsChecked(false);
        SmartMode().IsEnabled(true);
        mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
    }

    void ChattingList::CreateNewChat(const std::wstring& _title)
    {
        std::wstring title = _title.substr(0, _title.size() < 30 ? _title.size() : 30);
        auto t = time(NULL);
        ChatList().Items().InsertAt(0, ChatThumbnail(title, t, m_newChatUuid));
        ChatList().SelectedItem(ChatList().Items().GetAt(0));

        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["title"] = AnsiToUtf8(GetStringFromWstring(title));
        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["created_time"] = t;
        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["smart_mode"] = SmartMode().IsChecked().GetBoolean();
        DATA["chatting"][GetStringFromWstring(m_newChatUuid)]["data"] = Json::Value(Json::arrayValue);
        m_chatInfo[GetStringFromWstring(m_newChatUuid)].smart_mode = SmartMode().IsChecked().GetBoolean();
        SmartMode().IsEnabled(false);

        m_newChatUuid = GenerateUUID();
    }

    Windows::Foundation::IAsyncAction ChattingList::EditTitle_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        if (m_selectedChat == nullptr)
            return;

        EditTitleContentDialog().Title(box_value(L"ä�� ���� ����"));
        EditTitleContentDialog().PrimaryButtonText(L"����");
        EditTitleContentDialog().CloseButtonText(L"���");
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
            dialog.Title(box_value(L"ä�� ��� ����"));
            dialog.Content(box_value(L"�ش� ä�� ����� �����Ͻðڽ��ϱ�?"));
            dialog.PrimaryButtonText(L"����");
            dialog.CloseButtonText(L"���");
            auto result = co_await dialog.ShowAsync();
            if (result == Controls::ContentDialogResult::Primary)
            {
                // ���� ����
                m_chatInfo[GetStringFromWstring(m_activatedChatUuid)].del = true;

                // ä�� ����Ʈ���� ����
                uint32_t index;
                ChatList().Items().IndexOf(m_selectedChat, index);
                ChatList().Items().RemoveAt(index);
                m_selectedChat = nullptr;

                // navigation ��� ����
                Application::Current().as<App>()->window.as<MainWindow>()->mainNav().Header(box_value(L"ä�� ���"));

                // ����Ʈ ��� ����
                SmartMode().IsChecked(false);
                SmartMode().IsEnabled(true);

                // �ش� chat�� ������ �� ����
                SAFE_ACCESS;
                DATA["chatting"].removeMember(GetStringFromWstring(m_activatedChatUuid));
                SaveData();

                // �ڵ����� new chat �������� �̵�
                ChatList().SelectedItem(nullptr);
                m_activatedChatUuid = m_newChatUuid;
                mdViewer().Source(Windows::Foundation::Uri((L"http://localhost:" + std::to_wstring(m_port) + L"/chat/" + m_activatedChatUuid + L"?key=" + m_key).c_str()));
            }
        }
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
