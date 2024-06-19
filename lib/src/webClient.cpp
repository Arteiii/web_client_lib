//
// Created by arteii on 6/18/24.
//

#include "../include/webClient.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

HttpClient::HttpClient(const wchar_t *user_agent)
        : user_agent_(user_agent), session_(nullptr), connection_(nullptr), request_(nullptr), logging_enabled_(false) {
    session_ = WinHttpOpen(user_agent,
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                           WINHTTP_NO_PROXY_NAME,
                           WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session_) {
        throw std::runtime_error("Failed to open WinHTTP session.");
    }
    log(L"WinHTTP session opened.");
}

HttpClient::~HttpClient() {
    closeHandles();
    log(L"WinHTTP session closed.");
}

HttpClient &HttpClient::get(const std::wstring &url) {
    log(L"Setting up GET request for URL: " + url);
    return request(L"GET", url);
}

HttpClient &HttpClient::post(const std::wstring &url, const std::wstring &data) {
    log(L"Setting up POST request for URL: " + url);
    return request(L"POST", url, data);
}

HttpClient &HttpClient::put(const std::wstring &url, const std::wstring &data) {
    log(L"Setting up PUT request for URL: " + url);
    return request(L"PUT", url, data);
}

HttpClient &HttpClient::del(const std::wstring &url) {
    log(L"Setting up DELETE request for URL: " + url);
    return request(L"DELETE", url);
}

HttpClient &HttpClient::header(const std::map<std::wstring, std::wstring> &headers) {
    log(L"Adding headers:");
    for (const auto &header: headers) {
        std::wstring full_header = header.first + L": " + header.second;
        if (!WinHttpAddRequestHeaders(request_, full_header.c_str(), (ULONG) - 1L, WINHTTP_ADDREQ_FLAG_ADD)) {
            throw std::runtime_error("Failed to add header: " + std::string(header.first.begin(), header.first.end()));
        }
        log(L" - " + full_header);
    }
    return *this;
}

std::wstring HttpClient::send() {
    log(L"Sending request...");
    if (!WinHttpSendRequest(request_, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        throw std::runtime_error("Failed to send request.");
    }

    if (!WinHttpReceiveResponse(request_, nullptr)) {
        throw std::runtime_error("Failed to receive response.");
    }

    DWORD size = 0;
    DWORD downloaded = 0;
    std::vector<char> response;

    do {
        if (!WinHttpQueryDataAvailable(request_, &size)) {
            throw std::runtime_error("Failed to query data availability.");
        }

        if (size == 0) {
            break;
        }

        std::vector<char> buffer(size);
        if (!WinHttpReadData(request_, buffer.data(), size, &downloaded)) {
            throw std::runtime_error("Failed to read data.");
        }

        response.insert(response.end(), buffer.begin(), buffer.begin() + downloaded);
        log(L"Downloaded: " + std::to_wstring(downloaded) + L" bytes");

    } while (size > 0);

    log(L"Request completed.");
    return std::wstring(response.begin(), response.end());
}

HttpClient &HttpClient::request(const std::wstring &method, const std::wstring &url, const std::wstring &data) {
    log(L"Creating request with method: " + method + L" and URL: " + url);

    URL_COMPONENTS url_components = {sizeof(URL_COMPONENTS)};
    wchar_t host_name[256], url_path[2048];
    url_components.lpszHostName = host_name;
    url_components.dwHostNameLength = _countof(host_name);
    url_components.lpszUrlPath = url_path;
    url_components.dwUrlPathLength = _countof(url_path);

    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &url_components)) {
        throw std::runtime_error("Failed to crack URL.");
    }

    connection_ = WinHttpConnect(session_, host_name, url_components.nPort, 0);
    if (!connection_) {
        throw std::runtime_error("Failed to connect to server.");
    }

    request_ = WinHttpOpenRequest(connection_, method.c_str(), url_components.lpszUrlPath,
                                  nullptr, WINHTTP_NO_REFERER,
                                  WINHTTP_DEFAULT_ACCEPT_TYPES,
                                  url_components.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
    if (!request_) {
        throw std::runtime_error("Failed to open request.");
    }

    log(L"Request created.");

    if (!data.empty() && (method == L"POST" || method == L"PUT")) {
        if (!WinHttpSendRequest(request_, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                (LPVOID) data.c_str(), data.length() * sizeof(wchar_t),
                                data.length() * sizeof(wchar_t), 0)) {
            throw std::runtime_error("Failed to send request with data.");
        }
    }

    return *this;
}

void HttpClient::closeHandles() {
    if (request_) {
        WinHttpCloseHandle(request_);
        log(L"Request handle closed.");
    }
    if (connection_) {
        WinHttpCloseHandle(connection_);
        log(L"Connection handle closed.");
    }
    if (session_) {
        WinHttpCloseHandle(session_);
        log(L"Session handle closed.");
    }
}

HttpClient &HttpClient::logging(bool enable) {
    logging_enabled_ = enable;
    return *this;
}

void HttpClient::log(const std::wstring &message) const {
    if (logging_enabled_) {
        std::wcerr << message << L"\n";
    }
}