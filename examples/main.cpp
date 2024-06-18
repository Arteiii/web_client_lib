#include "../lib/include/webClient.hpp"
#include <iostream>

int main() {
    try {
        HttpClient client(L"ExampleClient");

        std::wstring getResponse = client.get(L"https://jsonplaceholder.typicode.com/posts/1")
                .header({{L"Content-Type", L"application/json"}})
                .send();

        std::wcout << L"GET Response: " << getResponse << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::system("pause");

    return 0;
}

