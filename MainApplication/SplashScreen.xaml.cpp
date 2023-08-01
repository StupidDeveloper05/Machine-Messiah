// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "SplashScreen.xaml.h"
#if __has_include("SplashScreen.g.cpp")
#include "SplashScreen.g.cpp"
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

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::chrono_literals;
using namespace Microsoft::UI::Windowing;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MainApplication::implementation
{
    SplashScreen::SplashScreen()
    {
        InitializeComponent();
        
        auto displayArea = DisplayArea::GetFromWindowId(AppWindow().Id(), DisplayAreaFallback::Nearest).WorkArea();
        AppWindow().MoveAndResize({(displayArea.Width - 470) / 2, (displayArea.Height - 250) / 2, 470, 250 });

        auto presenter = AppWindow().Presenter().as<Microsoft::UI::Windowing::OverlappedPresenter>();
        presenter.IsMaximizable(false);
        presenter.IsMinimizable(false);
        presenter.IsResizable(false);
        presenter.SetBorderAndTitleBar(false, false);

        timer.Tick({ this, &SplashScreen::Splash });
        timer.Interval(1s);
        timer.Start();
    }

    void SplashScreen::Splash(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&)
    {
        if (!loaded)
            return;

        timer.Stop();

        Application::Current().as<App>()->window = make<MainWindow>();
        Application::Current().as<App>()->window.as<MainWindow>()->m_key = m_key;
        Application::Current().as<App>()->window.as<MainWindow>()->m_wkey = m_wkey;
        Application::Current().as<App>()->window.as<MainWindow>()->m_port = m_port;
        Application::Current().as<App>()->window.Activate();

        this->Close();
    }

    Windows::Foundation::IAsyncAction SplashScreen::Window_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args)
    {
        if (isFirstTime)
        {
            isFirstTime = false;
            co_await open_markdown_server_and_load_data_async();
        }
    }

    Windows::Foundation::IAsyncAction SplashScreen::open_markdown_server_and_load_data_async()
    {
        status().Text(L"준비중...");

        // create random key
        srand(time(NULL));
        std::wstring alphabet = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        for (int i = 0; i < 32; ++i)
        {
            m_wkey += alphabet[rand() % alphabet.size()];
        }
        m_key.assign(m_wkey.begin(), m_wkey.end());
        OpenAI::OpenAIStorage::Get()->m_key = m_key;

        status().Text(L"사용자 정보를 불러오는 중...");
        // get appx path
        auto temp = Windows::Storage::ApplicationData::Current().TemporaryFolder().Path();
        auto user = Windows::System::User::GetDefault();
        Windows::Storage::StorageFolder storageFolder = co_await Windows::Storage::StorageFolder::GetFolderFromPathForUserAsync(user, temp);

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

        status().Text(L"프로그램을 시작하는 중...");
        // kill remain process
        TerminatePreviousProcess(L"app.exe", (temp + L"\\app.exe").c_str());

        // get mdViewer path from installed path
        auto pkgPath = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
        auto mdPath = pkgPath + L"\\app.exe";
        Windows::Storage::StorageFile mdViewer = co_await Windows::Storage::StorageFile::GetFileFromPathAsync(mdPath.c_str());

        // copy mdViewer to temp folder
        co_await mdViewer.CopyAsync(storageFolder, L"app.exe", Windows::Storage::NameCollisionOption::ReplaceExisting);

        // create availiable port
        m_port = createPort();

        // run markdown viewer server
        CreateProcess((temp + L"\\app.exe").c_str(), (LPWSTR)(std::wstring(L"\"app\" --key ") + m_wkey + std::wstring(L" --port ") + std::to_wstring(m_port) + std::wstring(L" --file ") + fNameW).c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

        // connect client
        CLIENT.connect("http://localhost:" + std::to_string(m_port));
        while (!CLIENT.isConnected()) {
            co_await 1s;
        }

        loaded = true;
    }

    unsigned short SplashScreen::createPort()
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

    void SplashScreen::TerminatePreviousProcess(const std::wstring& processName, const std::wstring& fullPath)
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
}
