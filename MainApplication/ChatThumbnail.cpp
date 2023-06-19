#include "pch.h"
#include "ChatThumbnail.h"
#include "ChatThumbnail.g.cpp"

#include <ctime>
#include <sstream>

namespace winrt::MainApplication::implementation
{
    ChatThumbnail::ChatThumbnail(hstring const& Title, int64_t Date, hstring const& Uuid)
        : m_title(Title)
        , m_date(Date)
        , m_uuid(Uuid)
    {
    }
    
    hstring ChatThumbnail::Title()
    {
        return m_title;
    }
    void ChatThumbnail::Title(hstring const& value)
    {
        if (m_title != value)
        {
            m_title = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Title" });
        }
    }

    int64_t ChatThumbnail::Date()
    {
        return m_date;
    }
    void ChatThumbnail::Date(int64_t value)
    {
        if (m_date != value)
        {
            m_date = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Date" });
        }
    }

    hstring ChatThumbnail::Uuid()
    {
        return m_uuid;
    }

    void ChatThumbnail::Uuid(hstring const& value)
    {
        if (m_uuid != value)
        {
            m_uuid = value;
            m_propertyChanged(*this, Windows::UI::Xaml::Data::PropertyChangedEventArgs{ L"Uuid" });
        }
    }

    hstring ChatThumbnail::DateToStr()
    {
        tm* pTimeinfo = localtime(&m_date);
        std::wstringstream timeForm;
        timeForm << std::to_wstring(pTimeinfo->tm_year + 1900) << L".";
        timeForm << std::to_wstring(pTimeinfo->tm_mon + 1) << L".";
        timeForm << std::to_wstring(pTimeinfo->tm_mday) << L" ";
        if (pTimeinfo->tm_hour > 12)
            timeForm << L"오후 " << std::to_wstring(pTimeinfo->tm_hour - 12) << L":";
        else
            timeForm << L"오전 " << std::to_wstring(pTimeinfo->tm_hour) << L":";
        timeForm << std::to_wstring(pTimeinfo->tm_min) << L":";
        timeForm << std::to_wstring(pTimeinfo->tm_sec);
        return to_hstring(timeForm.str().c_str());
    }

    winrt::event_token ChatThumbnail::PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
    {
        return m_propertyChanged.add(handler);
    }
    void ChatThumbnail::PropertyChanged(winrt::event_token const& token) noexcept
    {
        m_propertyChanged.remove(token);
    }
}
