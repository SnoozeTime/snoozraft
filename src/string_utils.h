//
// Created by benoit on 18/10/16.
//

#ifndef SNOOZRAFT2_STRING_UTILS_H
#define SNOOZRAFT2_STRING_UTILS_H

#include <vector>
#include <string>

namespace snooz {

    /// Join a vector of strings into one string, each element separated by delim
    ///
    /// \param vec
    /// \param delim separates the elements of vec
    /// \return join string
    std::string join(const std::vector<std::string>& vec, std::string delim);

    /// Split a string into multiple elements. Delim determine what to split on.
    ///
    /// \param str
    /// \param delim
    /// \return
    std::vector<std::string> split(const std::string& str, std::string delim);


    /// Return a new string only with lowercase letters. This is not taking into
    /// account locale or whatever encoding. Just for ASCII configuration strings for
    /// example.
    ///
    /// \param target
    /// \return
    std::string lower(const std::string& target);

    /// Return a string with spaces on the left trimmed
    ///
    /// \param input
    /// \return
    std::string trim_left(const std::string& input);

    /// Return a string with spaces on the right trimmed
    ///
    /// \param input
    /// \return
    std::string trim_right(const std::string& input);

    /// Return a string with spaces on left and right trimmed
    ///
    /// \param input
    /// \return
    std::string trim(const std::string& input);
}


#endif //SNOOZRAFT2_STRING_UTILS_H
