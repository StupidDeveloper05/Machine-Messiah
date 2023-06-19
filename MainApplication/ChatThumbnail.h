#pragma once
#include "ChatThumbnail.g.h"

namespace winrt::MainApplication::implementation
{
    struct ChatThumbnail : ChatThumbnailT<ChatThumbnail>
    {
        ChatThumbnail() = default;
        ChatThumbnail(hstring const& Title, int64_t Date, hstring const& Uuid);

        // properties
        hstring Title();
        void Title(hstring const& value);        
        int64_t Date();
        void Date(int64_t value);
        hstring Uuid();
        void Uuid(hstring const& value);

        hstring DateToStr();

        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        hstring m_title;
        int64_t m_date;
        hstring m_uuid;
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;
    };
}
namespace winrt::MainApplication::factory_implementation
{
    struct ChatThumbnail : ChatThumbnailT<ChatThumbnail, implementation::ChatThumbnail>
    {
    };
}
