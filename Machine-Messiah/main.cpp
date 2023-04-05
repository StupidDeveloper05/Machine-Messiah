#include <iostream>

#include <openai.hpp>

int main()
{
	openai::start(
        "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",
        "org-1XK4EGAKbk9RBmHca7zf6HLK"
    );

    auto completion = openai::completion().create(R"({
        "model": "text-davinci-003",
        "prompt": "Say this is a test",
        "max_tokens": 7,
        "temperature": 0
    })"_json); // Using user-defined (raw) string literals
    std::cout << "Response is:\n" << completion.dump(2) << '\n';

    auto image = openai::image().create({
        { "prompt", "A cute koala playing the violin"},
        { "n", 1 },
        { "size", "512x512" }
        }); // Using initializer lists
    std::cout << "Image URL is: " << image["data"][0]["url"] << '\n';
}