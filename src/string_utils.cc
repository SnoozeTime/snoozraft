//
// Created by benoit on 18/10/16.
//

#include "string_utils.h"
#include <sstream>

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

}