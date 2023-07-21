// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <Psapi.h>
#include <tlhelp32.h>

#include <io.h>
#include <fstream>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.System.UserProfile.h>

#include <Client.h>
#include "FileSystem.h"
#include "OpenAIStorage.h"

using namespace winrt;
using namespace Windows::UI::Xaml::Interop;
using namespace Microsoft::UI::Xaml;

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

        open_markdown_server_and_load_data();

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
    
    void MainWindow::AppClosed(winrt::Microsoft::UI::Windowing::AppWindow const& sender, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs const& args)
    {
        auto cancel = args.Cancel();
        if (!cancel) {
            SaveData();
            UserData::UserDataFile::Get()->Destroy();
            OpenAI::OpenAIStorage::Get()->Destroy();

            // close md viewer
            std::string command = std::string("curl -X GET \"http://localhost:") + std::to_string(m_port) + std::string("/shutdown?key=") + m_key + std::string("\"");
            system(command.c_str());
        }
    }

    void MainWindow::open_markdown_server_and_load_data()
    {
        // create random key
        srand(time(NULL));
        std::wstring alphabet = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (int i = 0; i < 32; ++i)
        {
            m_wkey += alphabet[rand() % alphabet.size()];
        }
        m_key.assign(m_wkey.begin(), m_wkey.end());
        OpenAI::OpenAIStorage::Get()->m_key = m_key;

        // get appx path
        auto temp = Windows::Storage::ApplicationData::Current().TemporaryFolder().Path();
        auto user = Windows::System::User::GetDefault();
        Windows::Storage::StorageFolder storageFolder = Windows::Storage::StorageFolder::GetFolderFromPathForUserAsync(user, temp).get();

        // load user data
        auto local = Windows::Storage::ApplicationData::Current().LocalFolder().Path();
        auto fNameW = std::wstring((local + L"\\userData.json").c_str());
        std::string fName; fName.assign(fNameW.begin(), fNameW.end());
        if (_access(fName.c_str(), 0) != -1)
        {
            std::fstream jsonData;
            jsonData.open(fName.c_str(), std::ifstream::in | std::ifstream::binary);
            jsonData >> DATA;
            jsonData.close();
        }
        else
        {
            DATA["name"] = "YOUR NAME";
            DATA["API_KEY"] = "YOUR_API_KEY";
            DATA["ORGANZATION_ID"] = "YOURS";
            DATA["chatting"] = Json::Value(Json::objectValue);

            Json::StyledWriter writer;
            auto str = writer.write(DATA);

            std::ofstream output(fName.c_str());
            output << str;
            output.close();
        }

        // kill remain process
        TerminatePreviousProcess(L"app.exe", (temp + L"\\app.exe").c_str());

        // get mdViewer path from installed path
        auto pkgPath = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
        auto mdPath = pkgPath + L"\\app.exe";
        Windows::Storage::StorageFile mdViewer = Windows::Storage::StorageFile::GetFileFromPathAsync(mdPath.c_str()).get();

        // copy mdViewer to temp folder
        mdViewer.CopyAsync(storageFolder, L"app.exe", Windows::Storage::NameCollisionOption::ReplaceExisting).get();

        // create availiable port
        m_port = createPort();

        // run markdown viewer server
        CreateProcess((temp + L"\\app.exe").c_str(), (LPWSTR)(std::wstring(L"\"app\" --key ") + m_wkey + std::wstring(L" --port ") + std::to_wstring(m_port) + std::wstring(L" --file ") + fNameW).c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

        // connect client
        CLIENT.connect("http://localhost:" + std::to_string(m_port));
        while (!CLIENT.isConnected()) {}
    }

    void MainWindow::TerminatePreviousProcess(const std::wstring& processName, const std::wstring& fullPath)
    {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE)
        {
            return;
        }

        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &entry))
        {
            do
            {
                if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, entry.th32ProcessID);
                    if (hProcess != NULL)
                    {
                        wchar_t  filePath[MAX_PATH];
                        if (GetModuleFileNameEx(hProcess, NULL, filePath, MAX_PATH))
                        {
                            if (fullPath.compare(filePath) == 0)
                            {
                                TerminateProcess(hProcess, 0);
                            }
                        }
                        CloseHandle(hProcess);
                    }
                }
            } while (Process32Next(hSnapshot, &entry));
        }

        CloseHandle(hSnapshot);
    }


    //Windows::Foundation::IAsyncAction MainWindow::open_markdown_server()
    //{        
    //    // create random key
    //    srand(time(NULL));
    //    std::wstring alphabet = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    //    for (int i = 0; i < 32; ++i)
    //    {
    //        m_wkey += alphabet[rand() % alphabet.size()];
    //    }
    //    m_key.assign(m_wkey.begin(), m_wkey.end());

    //    // get mdViewer path from installed path
    //    auto pkgPath = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
    //    auto mdPath = pkgPath + L"\\app.exe";
    //    Windows::Storage::StorageFile mdViewer = co_await Windows::Storage::StorageFile::GetFileFromPathAsync(mdPath.c_str());

    //    // copy mdViewer to temp folder
    //    auto local = Windows::Storage::ApplicationData::Current().TemporaryFolder().Path();
    //    auto user = Windows::System::User::GetDefault();
    //    Windows::Storage::StorageFolder storageFolder = co_await Windows::Storage::StorageFolder::GetFolderFromPathForUserAsync(user, local);
    //    co_await mdViewer.CopyAsync(storageFolder, L"app.exe", Windows::Storage::NameCollisionOption::ReplaceExisting);

    //    // create availiable port
    //    m_port = createPort();
    //    CreateProcess((local + L"\\app.exe").c_str(), (LPWSTR)(std::wstring(L"\"app\" --key ") + m_wkey + std::wstring(L" --port ") + std::to_wstring(m_port)).c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);        
    //}

    unsigned short MainWindow::get_port()
    {
        return m_port;
    }
    
    unsigned short MainWindow::createPort()
    {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        SOCKET tempSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        serverAddress.sin_port = 0;
        bind(tempSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        listen(tempSocket, 1);
        sockaddr_in tempAddress{};
        int addressSize = sizeof(tempAddress);
        getsockname(tempSocket, (struct sockaddr*)&tempAddress, &addressSize);
        unsigned short portNumber = ntohs(tempAddress.sin_port);
        closesocket(tempSocket);
        WSACleanup();
        return portNumber;
    }
}
