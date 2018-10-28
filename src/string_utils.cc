//
// Created by benoit on 18/10/16.
//

#include "string_utils.h"
#include <sstream>
#include <algorithm>
#include <iostream>

namespace snooz {

std::string join(const std::vector<std::string>& vec, std::string delim) {
    std::stringstream ss;
    for (int i = 0; i < vec.size(); i++) {
        ss << vec[i];
        if (i != vec.size() - 1) {
            ss << delim;
        }
    }
    auto result = ss.str();
    return result;
}

std::vector<std::string> split(const std::string& str, std::string delim) {
    if (str.empty()) {
        return std::vector<std::string>();
    }


    std::vector<std::string> elements;

    size_t start_index = 0;
    size_t it = 0;
    do {
        it = str.find_first_of(delim, start_index);
        auto length = it - start_index;

        auto substr = str.substr(start_index, length);

        if (!substr.empty()) {
            elements.push_back(substr);
        }

        start_index = it + delim.size();
    } while (it != std::string::npos);

    return elements;
}

std::string lower(const std::string& target) {

    std::cout << "Before lower -> " << target << std::endl;
    std::stringstream ss;
    std::for_each(target.begin(), target.end(), [&ss] (const char& c) {
        ss << (char) ::tolower(c);
    });
    std::cout << "After lower -> " << ss.str() << std::endl;
    return ss.str();
}

std::string trim_left(const std::string& input) {
    auto idx = input.find_first_not_of(' ');
    if (idx != std::string::npos) {
        return input.substr(idx);
    } else {
        return input;
    }
}

std::string trim_right(const std::string& input) {
    auto idx = input.find_last_not_of(' ');
    if (idx != std::string::npos) {
        return input.substr(0, idx+1);
    } else {
        return input;
    }
}

std::string trim(const std::string& input) {
    auto trimmed_left = trim_left(input);
    return trim_right(input);
}

}