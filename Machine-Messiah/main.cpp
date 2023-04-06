#include <iostream>
#include <sstream>
#include <atlstr.h>

#include <openai.hpp>

std::string reply;
auto messages = nlohmann::json::array({
    { {"role", "system"}, {"content", "You are a kind helpful assistant."} }
});

// ansi->unicode->utf8 ���ڵ�
std::string ANSI2UTF8(std::string ansi_str)
{
    return CW2A(CA2W(ansi_str.c_str()).m_psz, CP_UTF8).m_psz;
}

// utf8 -> unicode -> ansi ���ڵ�
std::string UTF82ANSI(std::string utf8_str)
{
    return CW2A(CA2W(utf8_str.c_str(), CP_UTF8).m_psz).m_psz;
}

size_t writeFunctionForChat(void* ptr, size_t size, size_t nmemb, std::string* data) 
{
    std::string json = (char*)ptr;
    std::string str_buf;
    std::istringstream iss(json);
    if (json.find("data") != std::string::npos)
    {
        while (getline(iss, str_buf, '\n')) {
            try
            {
                if (str_buf.find("data") != std::string::npos)
                {
                    std::string utf8 = nlohmann::json::parse(str_buf.substr(str_buf.find("{"), str_buf.rfind("}") + 1))["choices"][0]["delta"].value("content", "");
                    std::string ansi = UTF82ANSI(utf8);
                    reply += ansi;
                    std::cout << ansi.c_str();
                }
            }
            catch (std::exception&)
            {
                std::cout << std::endl;
                messages.push_back({ {"role", "assistant"}, {"content", ANSI2UTF8(reply).c_str()} });
                reply = "";
                openai::set_func(nullptr);
            }
        }
    }
    
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

int main()
{
	openai::start(
        "--- API key ---",
        "-- organization ID --"
    );
    
    while (true)
    {
        std::cout << "User : ";
        std::string input;
        getline(std::cin, input);

        // �����͸� �����ϰ� utf8�� ���ڵ���.
        std::string utf8 = ANSI2UTF8(input);

        // utf8�� ���ڵ��� ������� �Է��� POST�� �����Ϳ� �������.
        messages.push_back({ {"role", "user"}, {"content", utf8.c_str()} });

        // openai ���̺귯���� �����Ͽ� curl�� write function�� Ŀ���͸���¡ �� �� �ְ� ��.
        openai::set_func(writeFunctionForChat);
        
        std::cout << "AI : ";
        // chat gpt�� ������ POST
        auto chat = openai::chat().create({
            {"model", "gpt-3.5-turbo"},
            {"messages", messages},
            {"temperature", 0},
            {"stream", true}
        });
    }
}