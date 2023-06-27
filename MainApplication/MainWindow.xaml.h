// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::MainApplication::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        void mainNav_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        void AppClosed(winrt::Microsoft::UI::Windowing::AppWindow const& sender, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs const& args);
        
        unsigned short get_port();

    private:
        unsigned short createPort();
        void open_markdown_server_and_load_data();
        void TerminatePreviousProcess(const std::wstring& processName, const std::wstring& fullPath);
        //Windows::Foundation::IAsyncAction open_markdown_server();

    private:
        unsigned short m_port;
        std::string m_key;
        std::wstring m_wkey;

        PROCESS_INFORMATION pi;
        STARTUPINFO si = { sizeof(STARTUPINFO) };
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
