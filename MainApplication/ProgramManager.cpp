#include "pch.h"
#include "ProgramManager.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <atlbase.h>
#include <gdiplus.h>
#include <ShlGuid.h>
#include <Shlwapi.h>
#include <ShObjIdl.h>
#include <ShObjIdl_core.h>

#include <propkey.h>
#include <propvarutil.h>

#include <winrt/Windows.Storage.h>

#pragma comment(lib, "Propsys.lib")
#pragma comment(lib, "gdiplus.lib")

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;
    UINT size = 0;    
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

bool SaveHBITMAPToFile(HBITMAP hBitmap, const wchar_t* filePath)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    bool success = false;
    Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
    if (bitmap)
    {
        CLSID bmpClsid;
        GetEncoderClsid(L"image/png", &bmpClsid);

        Gdiplus::Status status = bitmap->Save(filePath, &bmpClsid, NULL);
        if (status == Gdiplus::Ok)
        {
            success = true;
        }
        delete bitmap;
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return success;
}

namespace SmartMode
{
    ProgramManager* ProgramManager::instance;
    std::unordered_map<std::string, std::vector<std::string>> ProgramManager::appList;
	ProgramManager::ProgramManager()
	{
        std::wstring path = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path().c_str();

        CoInitialize(NULL);

        //ShellExecute(NULL, L"open", L"explorer.exe", L"shell:appsFolder\\Microsoft.MSPaint_8wekyb3d8bbwe!Microsoft.MSPaint", 0, SW_HIDE);

        CComPtr<IShellItem> folder;
        if (SUCCEEDED(SHCreateItemFromParsingName(L"shell:appsFolder", nullptr, IID_PPV_ARGS(&folder))))
        {
            CComPtr<IEnumShellItems> enumItems;
            if (SUCCEEDED(folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&enumItems))))
            {
                IShellItem* items;
                while (enumItems->Next(1, &items, nullptr) == S_OK)
                {
                    CComPtr<IShellItem> item = items;
                    CComHeapPtr<wchar_t> name;
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)))
                    {
                        CComPtr<IPropertyStore> store;
                        if (SUCCEEDED(item->BindToHandler(NULL, BHID_PropertyStore, IID_PPV_ARGS(&store))))
                        {
                            PROPVARIANT pv;
                            PropVariantInit(&pv);
                            if (SUCCEEDED(store->GetValue(INIT_PKEY_AppUserModel_ID, &pv)))
                            {
                                CComHeapPtr<wchar_t> pvs;
                                pvs.Allocate(512);
                                PropVariantToString(pv, pvs, 512); // needs propvarutil.h and propsys.lib
                                PropVariantClear(&pv);

                                std::string key = CW2A(name.m_pData).m_psz;
                                std::transform(key.begin(), key.end(), key.begin(), tolower);
                                key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
                                appList[key] = std::vector<std::string>{ CW2A(name.m_pData).m_psz, CW2A(pvs.m_pData).m_psz };
                            }
                        }
                    }

                    if (!std::filesystem::exists((path + L"\\app\\static\\img\\" + std::wstring(name.m_pData) + L".png").c_str()))
                    {
                        CComPtr<IShellItemImageFactory> factory;
                        if (SUCCEEDED(item->QueryInterface(IID_IShellItemImageFactory, (void**)&factory)))
                        {
                            HBITMAP bit;
                            factory->GetImage({ 256, 256 }, SIIGBF_ICONBACKGROUND | SIIGBF_ICONONLY, &bit);
                            SaveHBITMAPToFile(bit, (path + L"\\app\\static\\img\\" + std::wstring(name.m_pData) + L".png").c_str());
                        }
                    }
                }
            }
        }

        CoUninitialize();
	}

	ProgramManager::~ProgramManager()
	{
	}
	
	ProgramManager* ProgramManager::Get()
	{
        if (instance == nullptr)
        {
            instance = new ProgramManager();
        }
		return instance;
	}
	
	void ProgramManager::Destroy()
	{
        delete instance;
	}

    Json::Value ProgramManager::Search(Json::Value& names)
    {
        Json::Value result;
        
        std::unordered_map<std::string, bool> cache;
        for (auto name : names)
        {
            std::string nm = CW2A(CA2W(name.asCString(), CP_UTF8).m_psz).m_psz;
            std::transform(nm.begin(), nm.end(), nm.begin(), tolower);
            nm.erase(std::remove(nm.begin(), nm.end(), ' '), nm.end());
            if (nm.empty())
                continue;
            if (cache[nm])
                continue;
            cache[nm] = true;

            for (auto app : appList)
            {
                if (app.first.find(nm) != std::string::npos)
                {
                    result[app.first] = Json::Value(Json::arrayValue);
                    for (auto arg : app.second)
                    {
                        result[app.first].append(arg);
                    }
                }
            }
        }

        return result;
    }

    void ProgramManager::Run(const std::string& name)
    {
        if (appList.find(name) != appList.end())
        {
            std::string id = std::string("shell:appsFolder\\") + appList[name][1];
            ShellExecuteA(NULL, "open", "explorer.exe", id.c_str(), 0, SW_HIDE);
        }
    }
}