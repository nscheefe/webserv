/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHead.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreidenb <mreidenb@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 23:35:36 by nscheefe          #+#    #+#             */
/*   Updated: 2024/07/29 19:04:14 by mreidenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ResponseHead.hpp"

ResponseHead::ResponseHead(){
        setHeader("");
        setStatusCode("");
        setStatusMessage("");
        setConnectionType("");
        setContentType("");
        setContentLength("");
        setAllow("");
        setContentLanguage("");
        setContentLocation("");
        setLastModified("");
        setLocation("");
        setRetryAfter("");
        setTransferEncoding("");
        setWwwAuthenticate("");
		setCookie("");
}


ResponseHead::~ResponseHead() {}

void ResponseHead::setDefault(Config::Location location, HttpParser &parser, std::string ServerName, int numClients) {
	this->ServerName = ServerName;
	location_path = location.path;
    setStatusCode("200");
    setStatusMessage("OK");
	this->location = location;
    checkLocation(location, parser);
    checkRedirect();
    setConnectionType("close");
    setContentLength("0");
    setAllow(join(this->location.methods, ", "));
	std::string modPath = parser.getPath();
        if (modPath.find(location_path) == 0)
            modPath.erase(0, location_path.length());
    setContentLocation((parser.getPath() == location_path ? this->location.index : modPath));
    setLastModified(formatLastModifiedTime(fullPathToFile));
    setRetryAfter(calculateRetryAfter(numClients));
	checkRedirect();
	setCookie("");

	}

std::string ResponseHead::serialize(HttpParser &parser) {
    std::ostringstream oss;
	std::string version = parser.getVersion() == "" ? "HTTP/1.1" : parser.getVersion();
        oss << version << " " << getStatusCode() << " " << getStatusMessage() << "\r\n";
    if (!this->ServerName.empty())
        oss << "Server: " << this->ServerName << "\r\n";
    if (!getCurrentDate().empty())
        oss << "Date: " << getCurrentDate() << "\r\n";
    if (!getConnectionType().empty())
        oss << "Connection: " << getConnectionType() << "\r\n";
    if (!getContentType().empty())
        oss << "Content-Type: " << getContentType() << "\r\n";
    if (!getContentLength().empty())
        oss << "Content-Length: " << getContentLength() << "\r\n";
    if (!getAllow().empty())
        oss << "Allow: " << getAllow() << "\r\n";
    if (!getContentLanguage().empty())
        oss << "Content-Language: " << getContentLanguage() << "\r\n";
    if (!getContentLocation().empty())
        oss << "Content-Location: " << getContentLocation() << "\r\n";
    if (!getLastModified().empty())
        oss << "Last-Modified: " << getLastModified() << "\r\n";
    if (!getLocation().empty())
        oss << "Location: " << getLocation() << "\r\n";
    if (!getRetryAfter().empty())
        oss << "Retry-After: " << getRetryAfter() << "\r\n";
    if (!getTransferEncoding().empty())
        oss << "Transfer-Encoding: " << getTransferEncoding() << "\r\n";
    if (!getWwwAuthenticate().empty()) {
        oss << "WWW-Authenticate: " << getWwwAuthenticate() << "\r\n";
    }
    if (!getCookie().empty()){
		oss << "Set-Cookie: " << getCookie() << "\r\n";
	}
    oss << "\r\n";

    return oss.str();
}



//########################################## utils ##########################################

float ResponseHead::calculateServerLoad(int activeConnections) {
    int maxConnections = 100;
    float load = static_cast<float>(activeConnections) / maxConnections;
    return load;
}

std::string ResponseHead::calculateRetryAfter(int activeConnections) {
    float load = calculateServerLoad(activeConnections);
    int delay;

    if (load > 0.75) {
        delay = 120;
    } else if (load > 0.5) {
        delay = 60;
    } else {
        delay = 30;
    }
    std::ostringstream oss;
    oss << delay;
    return oss.str();
}

std::string ResponseHead::formatLastModifiedTime(const std::string &filePath) {
    struct stat fileInfo;
    if (stat(filePath.c_str(), &fileInfo) != 0) {
        return "";
    }

    char dateStr[80];
    std::tm *ptm = std::localtime(&fileInfo.st_mtime);
    std::strftime(dateStr, sizeof(dateStr), "%a, %d %b %Y %H:%M:%S GMT", ptm);

    return std::string(dateStr);
}

std::string ResponseHead::getCurrentDate() {
    std::time_t now = std::time(NULL);
    std::tm *timeinfo = std::gmtime(&now);

    char buffer[1000];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

    return std::string(buffer);
}

std::string ResponseHead::intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}


bool ResponseHead::filecheck(std::string fullPath,
                             std::string path) {
    std::ifstream file(fullPath.c_str());
    if (file.good()) {
        fullPathToFile = fullPath;
        setLocation(path);
        file.close();
		return true;
    }
	return false;
}

void ResponseHead::checkLocation(Config::Location location, HttpParser &parser) {

        std::string modPath = parser.getPath();
        if (modPath.find(location_path) == 0)
            modPath.erase(0, location_path.length());

        std::string fullPath = location.root + (parser.getPath() == location_path ? location.index : modPath);
		if (filecheck(fullPath + location.index, parser.getPath()))
			return;
        // std::cout << "fullpath :" << fullPath << std::endl;
		if (filecheck(fullPath, parser.getPath()))
			return;
		throw std::runtime_error("404");
}

void ResponseHead::checkRedirect() {
    if (!location.redirect_url.empty()) {
        setStatusCode(intToString(location.redirect_status));
        setLocation(location.redirect_url);
    }
}

std::string ResponseHead::join(const std::vector <std::string> &vec, const char *delim) {
    std::ostringstream os;
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        if (it != vec.begin())
            os << delim;
        os << *it;
    }
    return os.str();
}



//getter / setter

std::string ResponseHead::getHeader() const { return _header; }

void ResponseHead::setHeader(const std::string &header) { _header = header; }

std::string ResponseHead::getStatusCode() const { return _statusCode; }

void ResponseHead::setStatusCode(const std::string &statusCode) { _statusCode = statusCode; }

std::string ResponseHead::getStatusMessage() const { return _statusMessage; }

void ResponseHead::setStatusMessage(const std::string &statusMessage) { _statusMessage = statusMessage; }

std::string ResponseHead::getConnectionType() const { return _connectionType; }

void ResponseHead::setConnectionType(const std::string &connectionType) { _connectionType = connectionType; }

std::string ResponseHead::getContentType() const { return _contentType; }

void ResponseHead::setContentType(const std::string &contentType) { _contentType = contentType; }

std::string ResponseHead::getContentLength() const { return _contentLength; }

void ResponseHead::setContentLength(const std::string &contentLength) { _contentLength = contentLength; }

std::string ResponseHead::getAllow() const { return _allow; }

void ResponseHead::setAllow(const std::string &allow) { _allow = allow; }

std::string ResponseHead::getContentLanguage() const { return _contentLanguage; }

void ResponseHead::setContentLanguage(const std::string &contentLanguage) { _contentLanguage = contentLanguage; }

std::string ResponseHead::getContentLocation() const { return _contentLocation; }

void ResponseHead::setContentLocation(const std::string &contentLocation) { _contentLocation = contentLocation; }

std::string ResponseHead::getLastModified() const { return _lastModified; }

void ResponseHead::setLastModified(const std::string &lastModified) { _lastModified = lastModified; }

std::string ResponseHead::getLocation() const { return _location; }

void ResponseHead::setLocation(const std::string &location) { _location = location; }

std::string ResponseHead::getRetryAfter() const { return _retryAfter; }

void ResponseHead::setRetryAfter(const std::string &retryAfter) { _retryAfter = retryAfter; }

std::string ResponseHead::getTransferEncoding() const { return _transferEncoding; }

void ResponseHead::setTransferEncoding(const std::string &transferEncoding) { _transferEncoding = transferEncoding; }

std::string ResponseHead::getWwwAuthenticate() const { return _wwwAuthenticate; }

void ResponseHead::setWwwAuthenticate(const std::string &wwwAuthenticate) { _wwwAuthenticate = wwwAuthenticate; }

std::string ResponseHead::getCookie() const { return _cookie; }
void ResponseHead::setCookie(const std::string &cookie) { _cookie = cookie; }
