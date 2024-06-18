//
// Created by arteii on 6/18/24.
//

#ifndef WEB_CLIENT
#define WEB_CLIENT

#include <map>
#include <stdexcept>
#include <string>
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

/**
 * @brief A class for making HTTP requests using the WinHTTP library.
 */
class HttpClient {
public:
    /**
     * @brief Constructs a new WebClient object with a specified user agent.
     * @param user_agent The user agent string for the HTTP requests.
     */
    explicit HttpClient(const wchar_t *user_agent = L"WebClient");

    /**
     * @brief Destroys the WebClient object and closes any open handles.
     */
    ~HttpClient();

    /**
     * @brief Prepares a GET request.
     * @param url The URL for the GET request.
     * @return A reference to the WebClient object.
     */
    HttpClient &get(const std::wstring &url);

    /**
     * @brief Prepares a POST request.
     * @param url The URL for the POST request.
     * @param data The data to send in the POST request.
     * @return A reference to the WebClient object.
     */
    HttpClient &post(const std::wstring &url, const std::wstring &data);

    /**
     * @brief Prepares a PUT request.
     * @param url The URL for the PUT request.
     * @param data The data to send in the PUT request.
     * @return A reference to the WebClient object.
     */
    HttpClient &put(const std::wstring &url, const std::wstring &data);

    /**
     * @brief Prepares a DELETE request.
     * @param url The URL for the DELETE request.
     * @return A reference to the WebClient object.
     */
    HttpClient &del(const std::wstring &url);

    /**
     * @brief Adds headers to the request.
     * @param headers A map of headers to add to the request.
     * @return A reference to the WebClient object.
     */
    HttpClient &header(const std::map<std::wstring, std::wstring> &headers);

    /**
     * @brief Sends the prepared request and retrieves the response.
     * @return The response from the server as a wide string.
     */
    std::wstring send();

private:
    std::wstring
            user_agent_;         /**< The user agent string for the HTTP requests. */
    HINTERNET session_{};    /**< The WinHTTP session handle. */
    HINTERNET connection_{}; /**< The WinHTTP connection handle. */
    HINTERNET request_{};    /**< The WinHTTP request handle. */

    /**
     * @brief Prepares a request with the specified HTTP method and URL.
     * @param method The HTTP method (GET, POST, PUT, DELETE).
     * @param url The URL for the request.
     * @param data The data to send in the request (if applicable).
     * @return A reference to the WebClient object.
     */
    HttpClient &request(const std::wstring &method,
                        const std::wstring &url,
                        const std::wstring &data = L"");

    /**
     * @brief Closes any open handles
     */
    void closeHandles();
};

#endif // WEB_CLIENT
