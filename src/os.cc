//
// Created by benoit on 18/10/21.
//

#include "os.h"

namespace snooz {

// me prefer empty strings.
std::string getenv(std::string env_name) {
    const char* value = std::getenv(env_name.c_str());

    if (value) {
        return std::string{value};
    } else {
        return "";
    }
}

}