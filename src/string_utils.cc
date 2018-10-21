//
// Created by benoit on 18/10/16.
//

#include "string_utils.h"
#include <sstream>
#include <algorithm>

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
    std::string lowered;
    std::transform(target.begin(), target.end(), lowered.begin(), ::tolower);
    return lowered;
}

}