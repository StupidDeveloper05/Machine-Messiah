#include "OpenAI.h"

// file NLBFR : Natural Language Based Function Recommender (자연어기반 함수 추천기)
class NLBFR
{
public:
	NLBFR(const std::string& descriptions);
	~NLBFR() = default;

public:
	void process_input(const std::string& input);

private:
	std::string function_descriptions;
	Json::Value messages;
};