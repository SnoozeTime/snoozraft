//
// Created by benoit on 18/10/20.
//

#pragma once

#include <stdexcept>

namespace snooz {

/// This is what happens when you don't configure correctly!
class ConfigException : std::runtime_error {};

}