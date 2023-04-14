#pragma once
#include <string>
#include <atlstr.h>

// ansi->unicode->utf8 ���ڵ�
std::string AnsiToUtf8(std::string ansi_str)
{
    return CW2A(CA2W(ansi_str.c_str()).m_psz, CP_UTF8).m_psz;
}

// utf8 -> unicode -> ansi ���ڵ�
std::string Utf8ToAnsi(std::string utf8_str)
{
    return CW2A(CA2W(utf8_str.c_str(), CP_UTF8).m_psz).m_psz;
}