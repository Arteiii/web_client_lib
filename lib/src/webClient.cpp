//
// Created by arteii on 6/18/24.
//

#include "../include/webClient.hpp"

HttpClient::HttpClient(const wchar_t *user_agent)
        : user_agent_(user_agent), session_(nullptr), connection_(nullptr), request_(nullptr) {
    session_ = WinHttpOpen(user_agent,
                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                           WINHTTP_NO_PROXY_NAME,
                           WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session_) {
        throw std::runtime_error("Failed to open WinHTTP session.");
    }
}

HttpClient::~HttpClient() {
    closeHandles();
}

HttpClient &HttpClient::get(const std::wstring &url) {
    return request(L"GET", url);
}

HttpClient &HttpClient::post(const std::wstring &url, const std::wstring &data) {
    return request(L"POST", url, data);
}

HttpClient &HttpClient::put(const std::wstring &url, const std::wstring &data) {
    return request(L"PUT", url, data);
}

HttpClient &HttpClient::del(const std::wstring &url) {
    return request(L"DELETE", url);
}

HttpClient &HttpClient::header(const std::map<std::wstring, std::wstring> &headers) {
    for (const auto &header: headers) {
        std::wstring full_header = header.first + L": " + header.second;
        if (!WinHttpAddRequestHeaders(request_, full_header.c_str(), (ULONG) - 1L, WINHTTP_ADDREQ_FLAG_ADD)) {
            throw std::runtime_error("Failed to add header: " + std::string(header.first.begin(), header.first.end()));
        }
    }
    return *this;
}

std::wstring HttpClient::send() {
    if (!WinHttpSendRequest(request_, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        throw std::runtime_error("Failed to send request.");
    }

    if (!WinHttpReceiveResponse(request_, nullptr)) {
        throw std::runtime_error("Failed to receive response.");
    }

    DWORD size = 0;
    DWORD downloaded = 0;
    std::wstring response;

    do {
        if (!WinHttpQueryDataAvailable(request_, &size)) {
            throw std::runtime_error("Failed to query data availability.");
        }

        std::wstring buffer(size / sizeof(wchar_t), L'\0');

        if (!WinHttpReadData(request_, &buffer[0], size, &downloaded)) {
            throw std::runtime_error("Failed to read data.");
        }

        response.append(buffer, 0, downloaded / sizeof(wchar_t));
    } while (size > 0);

    return response;
}

HttpClient &HttpClient::request(const std::wstring &method, const std::wstring &url, const std::wstring &data) {
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
    if (request_) WinHttpCloseHandle(request_);
    if (connection_) WinHttpCloseHandle(connection_);
    if (session_) WinHttpCloseHandle(session_);
}
