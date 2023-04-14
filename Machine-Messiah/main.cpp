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
		TurnOntheLight: �µ����� ��ȭ�� ���� �ʰ�, �ֺ��� ��� ����� �ִ� ����� �Ͽ� ��ü�� �� ���̰� �ϴ� ��ɵ� �Ѵ�.
		TurnOfftheLight: �µ����� ��ȭ�� ���� �ʰ�, �ֺ��� ��Ӱ� ����� �ִ� ����� �Ͽ� ��ü�� �� �� ���̰� �ϴ� ��ɵ� �Ѵ�.
		OpenTheDoor: ���� ���� �µ��� ���߾� �ִ� ����� �Ѵ�. �ݵ�� �µ��� ��������.
		CloseTheDoor: ���� �ݰ� �µ��� ���� �ִ� ����� �Ѵ�. �ݵ�� �µ��� ��������.
		���� ���� ���� �Լ����� �̸��� �����̾�. �� �Լ����� ���� ���� ���� ��ɸ� ������.
		���� ��� �µ��� ���̷��� CloseTheDoor�Լ���, �µ��� ���߷��� OpenTheDoor, ��� ������� TurnOntheLight, ��Ӱ� �Ϸ��� TurnOfftheLight�Լ��� �� �� �־�.
		�ʴ� �Լ��� �̸��� ���� �ٰ��Ͽ� ������ ����� �ؾ���. ������ ���״� �������.
	)");

	while (true)
	{
		std::cout << "User : ";
		std::string input;
		getline(std::cin, input);

		nlbfr.process_input(input);
	}
}