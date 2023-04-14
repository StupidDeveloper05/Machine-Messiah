#include <iostream>
#include <sstream>

#include "openai_api.hpp"
#include "NLBFR.h"


int main()
{
	bool successed = OpenAI::Init(
		"api key",
		"org-1XK4EGAKbk9RBmHca7zf6HLK"
	);
	if (!successed)
		return -1;

	NLBFR nlbfr(R"(
		TurnOntheLight: 온도에는 변화를 주지 않고, 주변을 밝게 만들어 주는 기능을 하여 물체가 잘 보이게 하는 기능도 한다.
		TurnOfftheLight: 온도에는 변화를 주지 않고, 주변을 어둡게 만들어 주는 기능을 하여 물체가 잘 안 보이게 하는 기능도 한다.
		OpenTheDoor: 문을 열고 온도를 낮추어 주는 기능을 한다. 반드시 온도는 낮아진다.
		CloseTheDoor: 문을 닫고 온도를 높여 주는 기능을 한다. 반드시 온도는 높아진다.
		위는 내가 가진 함수들의 이름과 설명이야. 이 함수들은 오직 설명에 쓰인 기능만 수행해.
		예를 들어 온도를 높이려면 CloseTheDoor함수를, 온도를 낮추려면 OpenTheDoor, 밝게 만들려면 TurnOntheLight, 어둡게 하려면 TurnOfftheLight함수를 쓸 수 있어.
		너는 함수의 이름과 설명에 근거하여 질문에 대답을 해야해. 질문을 할테니 대기해줘.
	)");

	while (true)
	{
		std::cout << "User : ";
		std::string input;
		getline(std::cin, input);

		nlbfr.process_input(input);
	}
}