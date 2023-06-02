// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml::Interop;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MainApplication::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();

        ExtendsContentIntoTitleBar(true);
        SetTitleBar(AppTitleBar());

        voice().Content(box_value(L"���� ���"));
        chat().Content(box_value(L"ä��"));

        mainNav().Header(box_value(L"���� ���"));
        contentFrame().Navigate(xaml_typename<MainApplication::VoiceCommand>());
    }

    void MainWindow::mainNav_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        if (args.IsSettingsSelected())
        {
            sender.Header(box_value(L"����"));
            contentFrame().Navigate(xaml_typename<MainApplication::Settings>());
        }
        else
        {
            auto itemTag = args.SelectedItem().as<Controls::NavigationViewItem>().Tag().as<hstring>();
            
            if (itemTag == L"Voice Command")
            {
                sender.Header(box_value(L"���� ���"));
                contentFrame().Navigate(xaml_typename<MainApplication::VoiceCommand>());
            }
            else if (itemTag == L"Chat GPT")
            {
                sender.Header(box_value(L"ä�� ���"));
                contentFrame().Navigate(xaml_typename<MainApplication::ChattingList>());
            }
        }
    }
}
