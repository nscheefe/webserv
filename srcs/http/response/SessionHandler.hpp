#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib> // For srand, rand
#include <ctime> // For time
    static std::string base64_chars ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#include "ResponseHead.hpp"
#include "../parser/HttpParser.hpp"
struct Login {
    std::string Base64Login;
    std::string username;
    std::string password;
    Login(const std::string& user, const std::string& pass) : username(user), password(pass) {
        Base64Login = encodeLogin(username, password);
    }

    static std::string base64_encode(const std::string &in) {
        std::string out;

        int val = 0, valb = -6;
        for (size_t i = 0; i < in.length(); i++) {
            unsigned char c = in[i];
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back(base64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4) out.push_back('=');
        return out;
    }

    static std::string encodeLogin(const std::string& username, const std::string& password) {
        return base64_encode(username + ":" + password);
    }

    bool operator==(const Login& other) const {
        return username == other.username && password == other.password;
    }
};




class SessionHandler {
private:
    std::vector<Login> _logins;
    std::map<std::string, std::string> _sessionStorage;


public:
    SessionHandler() {
        _logins.push_back(Login("user1", "password1"));
        _logins.push_back(Login("user2", "password2"));
    }

    SessionHandler(const SessionHandler& other)
        : _logins(other._logins), _sessionStorage(other._sessionStorage) {
    }

    std::map<std::string, std::string> parseBody(const std::string& body) {
        std::map<std::string, std::string> parsedBody;
        std::istringstream bodyStream(body);
        std::string pair;

        while (std::getline(bodyStream, pair, '&')) {
            std::size_t delimiterPos = pair.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = pair.substr(0, delimiterPos);
                std::string value = pair.substr(delimiterPos + 1);
                parsedBody[key] = value;
            }
        }

        return parsedBody;
    }
    SessionHandler& operator=(const SessionHandler& other) {
        if (this != &other) {
            _logins = other._logins;
            _sessionStorage = other._sessionStorage;
        }
        return *this;
    }




std::string generateSession(const std::string& incomingCreds) {
    if (validateCredentials(incomingCreds)) {
        std::string session_id = generateUniqueSessionId();

        _sessionStorage[session_id] = "is existent shit session ";
		return ("session_id=" + session_id + "; HttpOnly; SameSite=Strict; Max-Age=86400");
    }else
	{
		throw std::runtime_error("401");
	}
}

    bool validateCredentials(const std::string& incomingCreds) {
        for (size_t i = 0; i < _logins.size(); ++i) {
            if (_logins[i].Base64Login == incomingCreds) {
                return true;
            }
        }
        return false;
    }

 bool checkSession(std::string cookie) {
	 std::string session_id = cookie.substr(cookie.find("session_id=") + 11);
	if (_sessionStorage.find(session_id) != _sessionStorage.end()) {
		return true;
	} else {
		return false;
	}
	return false;
 }

bool deleteSession(std::string cookie) {
	 std::string session_id = cookie.substr(cookie.find("session_id=") + 11);
    if (_sessionStorage.find(session_id) != _sessionStorage.end()) {
        _sessionStorage.erase(session_id);
        return true;
    } else {
        return false;
    }
}
~SessionHandler() {
}
    std::string generateUniqueSessionId() {
        std::stringstream ss;
        ss << time(nullptr) << rand();
        return ss.str();
    }
};
