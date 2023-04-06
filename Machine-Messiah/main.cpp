#include <iostream>
#include <sstream>
#include <atlstr.h>

#include <openai.hpp>

std::string reply;
auto messages = nlohmann::json::array({
    { {"role", "system"}, {"content", "You are a kind helpful assistant."} }
});

// ansi->unicode->utf8 인코딩
std::string ANSI2UTF8(std::string ansi_str)
{
    return CW2A(CA2W(ansi_str.c_str()).m_psz, CP_UTF8).m_psz;
}

// utf8 -> unicode -> ansi 디코딩
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

        // 데이터를 안전하게 utf8로 인코딩함.
        std::string utf8 = ANSI2UTF8(input);

        // utf8로 인코딩된 사용자의 입력을 POST할 데이터에 집어넣음.
        messages.push_back({ {"role", "user"}, {"content", utf8.c_str()} });

        // openai 라이브러리를 수정하여 curl의 write function을 커스터마이징 할 수 있게 함.
        openai::set_func(writeFunctionForChat);
        
        std::cout << "AI : ";
        // chat gpt에 데이터 POST
        auto chat = openai::chat().create({
            {"model", "gpt-3.5-turbo"},
            {"messages", messages},
            {"temperature", 0},
            {"stream", true}
        });
    }
}