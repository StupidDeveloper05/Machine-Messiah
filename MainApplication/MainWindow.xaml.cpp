// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Windowing.h>

#include <Client.h>
#include "FileSystem.h"
#include "OpenAIStorage.h"

using namespace winrt;
using namespace Windows::UI::Xaml::Interop;
using namespace Microsoft::UI::Xaml;
using namespace std::chrono_literals;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

MDView::Client CLIENT;
//Json::Value DATA;

namespace winrt::MainApplication::implementation
{

    MainWindow::MainWindow()
    {
        InitializeComponent();

        this->AppWindow().Closing({ this, &MainWindow::AppClosed });

        ExtendsContentIntoTitleBar(true);
        SetTitleBar(AppTitleBar());

        voice().Content(box_value(L"음성 명령"));
        chat().Content(box_value(L"채팅"));

        mainNav().Header(box_value(L"음성 명령"));
        contentFrame().Navigate(xaml_typename<MainApplication::VoiceCommand>());
    }

    void MainWindow::mainNav_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        if (args.IsSettingsSelected())
        {
            sender.Header(box_value(L"설정"));
            contentFrame().Navigate(xaml_typename<MainApplication::Settings>());
        }
        else
        {
            auto itemTag = args.SelectedItem().as<Controls::NavigationViewItem>().Tag().as<hstring>();

            if (itemTag == L"Voice Command")
            {
                sender.Header(box_value(L"음성 명령"));
                contentFrame().Navigate(xaml_typename<MainApplication::VoiceCommand>());
            }
            else if (itemTag == L"Chat GPT")
            {
                sender.Header(box_value(L"채팅 목록"));
                Windows::Foundation::Collections::ValueSet parameters;
                parameters.Insert(L"port", box_value(m_port));
                parameters.Insert(L"key", box_value(m_wkey.c_str()));
                contentFrame().Navigate(xaml_typename<MainApplication::ChattingList>(), parameters);
            }
        }
    }

    Windows::Foundation::IAsyncAction MainWindow::AppClosed(winrt::Microsoft::UI::Windowing::AppWindow const& sender, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs const& args)
    {
        if (!args.Cancel())
        {
            args.Cancel(true);
            if (OpenAI::OpenAIStorage::Get()->CheckAllCompleted())
            {
                Controls::ContentDialog dialog;
                dialog.XamlRoot(this->Content().XamlRoot());
                dialog.Title(box_value(L"프로그램 종료"));
                dialog.Content(box_value(L"프로그램을 종료하시겠습니까?"));
                dialog.PrimaryButtonText(L"종료");
                dialog.CloseButtonText(L"취소");
                auto result = co_await dialog.ShowAsync();
                if (result == Controls::ContentDialogResult::Primary)
                {
                    SaveData();
                    UserData::UserDataFile::Get()->Destroy();
                    OpenAI::OpenAIStorage::Get()->Destroy();

                    // close md viewer
                    std::string command = std::string("curl -X GET \"http://localhost:") + std::to_string(m_port) + std::string("/shutdown?key=") + m_key + std::string("\"");
                    system(command.c_str());

                    Close();
                }
            }
            else
            {
                Controls::ContentDialog dialog;
                dialog.XamlRoot(this->Content().XamlRoot());
                dialog.Title(box_value(L"응답 생성중"));
                dialog.Content(box_value(L"응답이 생성중 입니다.\n종료하려면 모든 응답을 중지하거나 완료될 때까지 기다려 주세요."));
                dialog.PrimaryButtonText(L"확인");
                auto result = co_await dialog.ShowAsync();
            }
        }
    }

    unsigned short MainWindow::get_port()
    {
        return m_port;
    }
}
