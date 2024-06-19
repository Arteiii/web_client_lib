#include "../lib/include/webClient.hpp"
#include <iostream>

int main() {
    try {
        HttpClient client(L"ExampleClient");

        std::wstring getResponse = client.get(
                        L"https://gist.githubusercontent.com/Arteiii/687d82cddd3b39834ed0b0ae4c15f138/raw/2a7add1e778e12f1c49e7b6bd7ecd3eaeea14c2f/hello_world.txt")
                .header({{L"Content-Type", L"application/json"}})
                .logging()
                .send();

        std::wcout << L"GET Response: " << getResponse << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::system("pause");

    return 0;
}
