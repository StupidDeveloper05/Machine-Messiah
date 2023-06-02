// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "Settings.g.h"

namespace winrt::MainApplication::implementation
{
    struct Settings : SettingsT<Settings>
    {
        Settings();

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct Settings : SettingsT<Settings, implementation::Settings>
    {
    };
}
