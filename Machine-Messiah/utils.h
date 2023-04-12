#pragma once
// 구버전 인코딩 코드
// 여러모로 오류가 많이남. 특히 알수없는 메모리 오류들..
#include <string>
#include <atlstr.h>

// ansi->unicode->utf8 인코딩
std::string AnsiToUtf8(std::string ansi_str)
{
    return CW2A(CA2W(ansi_str.c_str()).m_psz, CP_UTF8).m_psz;
}

// utf8 -> unicode -> ansi 디코딩
std::string Utf8ToAnsi(std::string utf8_str)
{
    return CW2A(CA2W(utf8_str.c_str(), CP_UTF8).m_psz).m_psz;
}

//#include <Windows.h>
//
//std::string AnsiToUtf8(const std::string& str)
//{
//    int length = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
//    wchar_t* buffer = new wchar_t[length];
//    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, length);
//
//    length = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
//    char* utf8_buffer = new char[length];
//    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, utf8_buffer, length, nullptr, nullptr);
//
//    std::string result(utf8_buffer);
//
//    delete[] buffer;
//    delete[] utf8_buffer;
//
//    return result;
//}

//
//std::string Utf8ToAnsi(const std::string& str)
//{
//    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
//    wchar_t* buffer = new wchar_t[length];
//    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, length);
//
//    length = WideCharToMultiByte(CP_ACP, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
//    char* ansi_buffer = new char[length];
//    WideCharToMultiByte(CP_ACP, 0, buffer, -1, ansi_buffer, length, nullptr, nullptr);
//
//    std::string result(ansi_buffer);
//
//    delete[] buffer;
//    delete[] ansi_buffer;
//
//    return result;
//}