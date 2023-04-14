#include <iostream>
#include <sstream>
#include <thread>

#include <vector>
#include <map>

#include "utils.h"
#include "openai_api.hpp"

std::map<std::string, std::vector<double>> lists;
std::vector<std::string> functions = {
	"���� �Ҵ�-�µ��� ��ȭ�� ����.�ֺ��� ������� ȿ���� �ִ�.",
	"���� ����-�µ��� ��ȭ�� ����.�ֺ��� ��ο����� ȿ���� �ִ�.",
	"���� ����-�ֺ��� Ź Ʈ�̰� �ǰ�, �µ��� �������� ȿ���� �ִ�.",
	"���� �ݴ´�-�ֺ��� ������ �ǰ�, �µ��� �����ǰų� �ö󰡴� ȿ���� �ִ�.",
};

std::vector<double> getUniqueVector(Json::Value& arr)
{
	std::vector<double> result;
	for (auto it = arr.begin(); it != arr.end(); ++it)
	{
		result.push_back(it->asDouble());
	}
	return result;
}

double cosine_similarity(const std::vector<double>& v1, const std::vector<double>& v2) {
	double dot_product = 0.0;
	double norm_v1 = 0.0;
	double norm_v2 = 0.0;
	int n = v1.size();  // ������ ����

	// ���� ���
	for (int i = 0; i < n; i++) {
		dot_product += v1[i] * v2[i];
	}

	// �� ������ �븧(norm) ���
	for (int i = 0; i < n; i++) {
		norm_v1 += pow(v1[i], 2);
		norm_v2 += pow(v2[i], 2);
	}
	norm_v1 = sqrt(norm_v1);
	norm_v2 = sqrt(norm_v2);

	// �ڻ��� ���缺 ���
	return dot_product / (norm_v1 * norm_v2);
}

std::string getBestFunc(const std::string& utf8)
{
	Json::Value json_body;
	json_body["model"] = "text-embedding-ada-002";
	json_body["input"] = utf8;

	// http post request
	auto result = OpenAI::Create(OpenAI::EndPoint::Embedding, json_body);
	auto uniqueVector = getUniqueVector(result["data"][0]["embedding"]);
	
	std::string best_fname;
	double best_score = 0.0;
	for (auto& [key, value] : lists)
	{
		double score = cosine_similarity(value, uniqueVector);
		if (score > best_score)
		{
			best_score = score;
			best_fname = key;
		}
		std::cout << key << " : " << score << std::endl;
	}
	return best_fname;
}

int main()
{
	bool successed = OpenAI::Init(
		"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", 
		"org-1XK4EGAKbk9RBmHca7zf6HLK"
	); 
	if (!successed)
		return -1;

	std::cout << "Preparing..." << std::endl;
	for (auto& fname : functions)
	{
		Json::Value json_body;
		json_body["model"] = "text-embedding-ada-002";
		json_body["input"] = AnsiToUtf8(fname).c_str();

		// http post request
		auto result = OpenAI::Create(OpenAI::EndPoint::Embedding, json_body);
		lists[fname] = getUniqueVector(result["data"][0]["embedding"]);
	}
	std::cout << "End!" << std::endl;

	while (true)
	{
		std::cout << "User : ";
		std::string input;
		getline(std::cin, input);

		std::cout << getBestFunc(AnsiToUtf8(input)) << std::endl;
	}
}
