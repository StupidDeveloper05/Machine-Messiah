#include <Windows.h>
#include <gdiplus.h>

#include <iostream>
#include <atlbase.h>
#include <ShlGuid.h>
#include <Shlwapi.h>
#include <ShObjIdl.h>
#include <ShObjIdl_core.h>
#include <thumbcache.h>

#include <propkey.h>
#include <propvarutil.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Propsys.lib")
#pragma comment(lib, "gdiplus.lib")

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;          // 이미지 인코더 개수
    UINT size = 0;         // 이미지 인코더 배열 사이즈
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // 실패

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // 실패

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // 성공
        }
    }

    free(pImageCodecInfo);
    return -1;  // 실패
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
        GetEncoderClsid(L"image/png", &bmpClsid); // BMP 포맷을 위한 CLSID를 얻습니다.

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

int main()
{
    TCHAR programpath[_MAX_PATH];
    GetCurrentDirectory(_MAX_PATH, programpath);

    CoInitialize(NULL);
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
                    wprintf(L"%s\n", name);
                }

                CComPtr<IShellItemImageFactory> factory;
                if (SUCCEEDED(item->QueryInterface(IID_IShellItemImageFactory, (void**)&factory)))
                {
                    HBITMAP bit;
                    factory->GetImage({256, 256}, SIIGBF_ICONONLY, &bit);
                    SaveHBITMAPToFile(bit, (std::wstring(programpath) + L"\\image\\" + std::wstring(name.m_pData) + L".png").c_str());
                }

                /*IThumbnailCache* thumbnail;
                HRESULT hr = CoCreateInstance(CLSID_LocalThumbnailCache,
                                                NULL,
                                                CLSCTX_INPROC_SERVER,
                                                IID_PPV_ARGS(&thumbnail));
                if (SUCCEEDED(hr))
                {
                    ISharedBitmap* bitmap = NULL;
                    if (SUCCEEDED(thumbnail->GetThumbnail(item, 150, WTS_FORCEEXTRACTION, &bitmap, NULL, NULL)))
                    {
                        HBITMAP img;
                        bitmap->GetSharedBitmap(&img);
                        SaveHBITMAPToFile(img, (std::wstring(programpath) + L"\\image\\" + std::wstring(name.m_pData) + L".png").c_str());
                    }

                    thumbnail->Release();
                }*/

                //// dump all properties
                //CComPtr<IPropertyStore> store;
                //if (SUCCEEDED(item->BindToHandler(NULL, BHID_PropertyStore, IID_PPV_ARGS(&store))))
                //{
                //    DWORD count = 0;
                //    store->GetCount(&count);
                //    for (DWORD i = 0; i < count; i++) {
                //        PROPERTYKEY pk;
                //        if (SUCCEEDED(store->GetAt(i, &pk)))
                //        {
                //            CComHeapPtr<wchar_t> pkName;
                //            PSGetNameFromPropertyKey(pk, &pkName); // needs propsys.lib

                //            PROPVARIANT pv;
                //            PropVariantInit(&pv);
                //            if (SUCCEEDED(store->GetValue(pk, &pv)))
                //            {
                //                CComHeapPtr<wchar_t> pvs;
                //                pvs.Allocate(512);
                //                PropVariantToString(pv, pvs, 512); // needs propvarutil.h and propsys.lib
                //                PropVariantClear(&pv);
                //                wprintf(L" %s=%s\n", pkName, pvs);
                //            }
                //            else
                //            {
                //                wprintf(L" %s=???\n", pkName);
                //            }
                //        }
                //    }
                //}
            }
        }
    }

    CoUninitialize();
}