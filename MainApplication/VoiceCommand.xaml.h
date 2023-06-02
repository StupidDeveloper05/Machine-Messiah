// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "VoiceCommand.g.h"

namespace winrt::MainApplication::implementation
{
    struct VoiceCommand : VoiceCommandT<VoiceCommand>
    {
        VoiceCommand();

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::MainApplication::factory_implementation
{
    struct VoiceCommand : VoiceCommandT<VoiceCommand, implementation::VoiceCommand>
    {
    };
}
