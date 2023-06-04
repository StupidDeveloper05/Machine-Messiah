// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ChattingList.xaml.h"
#if __has_include("ChattingList.g.cpp")
#include "ChattingList.g.cpp"
#endif

#include <ctime>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MainApplication::implementation
{
    ChattingList::ChattingList()
        : m_selectedChat{L"No Chat Selected", 0}
    {
        InitializeComponent();
        
        std::vector<hstring> Titles = { L"인공지능에 대하여", L"인사와 질문" };
        std::vector<int64_t> dates = { time(NULL), time(NULL) };
        for (int i = 0; i < Titles.size(); ++i)
        {
            ChatList().Items().Append(ChatThumbnail(Titles[i], dates[i]));
        }
    }

    void ChattingList::ChatList_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
    {
        if (e.AddedItems().Size() > 0)
        {
            for (int i = 0; i < e.AddedItems().Size(); ++i)
            {
                m_selectedChat = e.AddedItems().GetAt(i).as<ChatThumbnail>();
            }
        }
        if (e.RemovedItems().Size() > 0)
        {
            for (int i = 0; i < e.RemovedItems().Size(); ++i)
            {
                auto removedObject = e.RemovedItems().GetAt(i).as<ChatThumbnail>();
                auto str = removedObject.Title();
            }
        }
    }
}
