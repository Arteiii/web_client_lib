# WebClient

A simple C++ library for making HTTP requests using the WinHTTP library.

## Features

- Supports GET, POST, PUT, DELETE requests
- Easy to use interface
- Windows only (uses WinHTTP)

## Installation

### Using CMake

```sh
git clone https://github.com/arteiii/WebClient.git
cd WebClient
mkdir build
cd build
cmake ..
make
```

## Usage

```cpp
#include "/include/webClient.hpp"
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

```

## License
This project is licensed under the MIT License - 
see the [LICENSE](LICENSE-MIT) file for details.