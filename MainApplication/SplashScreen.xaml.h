// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "SplashScreen.g.h"

namespace winrt::MainApplication::implementation
{
    struct SplashScreen : SplashScreenT<SplashScreen>
    {
        SplashScreen();

        void Splash(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&);
        Windows::Foundation::IAsyncAction open_markdown_server_and_load_data_async();
        Windows::Foundation::IAsyncAction Window_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);

        unsigned short createPort();
        void TerminatePreviousProcess(const std::wstring& processName, const std::wstring& fullPath);

    private:
        winrt::Microsoft::UI::Xaml::DispatcherTimer timer;
        bool loaded = false;
        bool isFirstTime = true;

        unsigned short m_port;
        std::string m_key;
        std::wstring m_wkey;

        PROCESS_INFORMATION pi;
        STARTUPINFO si = { sizeof(STARTUPINFO) };
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct SplashScreen : SplashScreenT<SplashScreen, implementation::SplashScreen>
    {
    };
}
