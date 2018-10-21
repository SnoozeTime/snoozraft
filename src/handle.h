/// Handles are used to refer to objects that are stored elsewhere. For example, Timer objects are
/// created in the ZmqLoop but the caller get a handle back. Handle avoid the problem of dangling
/// pointers

#pragma once

#include <vector>
#include <memory>
#include <algorithm>
namespace snooz {

// Type ?
struct Handle {
    int version;
    size_t idx;

    bool operator==(const Handle& other) {
        return version == other.version && idx == other.idx;
    }
};

template <typename T>
class HandleManager {

public:

    HandleManager() {
        objects_.reserve(INITIAL_SIZE);
        handles_.reserve(INITIAL_SIZE);

        for(size_t i = 0; i < INITIAL_SIZE; i++) {
            objects_.push_back(nullptr);
            handles_.push_back({0, i});
        }
    }

    // Add a new object to the manager and return a handle to it.
    Handle add(std::unique_ptr<T> ptr) {

        // Find the first null object. We are going to use the handle associated to this and increase
        // the version number. If there is no available handle, we need to increase the number of handles
        auto it = std::find_if(objects_.begin(), objects_.end(), [] (const auto& ob) { return ob == nullptr;});
        if (it != objects_.end()) {
            auto index = it - objects_.begin();

            handles_[index].version += 1;
            objects_[index] = std::move(ptr);
            return handles_[index];
        } else {
            // nothing available :(
            auto index = objects_.size();
            handles_.push_back({1, index});
            objects_.push_back(std::move(ptr));

            return handles_[index];
        }
    }

    /// Will remove the object that pointed from the handle
    bool remove(const Handle&  handle) {
        if (!is_handle_valid(handle)) {
            return false;
        }

        // ok we can remove.
        objects_[handle.idx] = nullptr;
        return true;
    }


    ///
    /// \param handle to the object
    /// \return either pointer to the object or nullptr.
    T* get(const Handle& handle) const {
        if (!is_handle_valid(handle)) {
            return nullptr;
        }

        return objects_[handle.idx].get();
    }

private:

    bool is_handle_valid(const Handle& h) const {
        if (h.idx >= handles_.size()) {
            // What are you trying to do?
            return false;
        }

        // Do not remove old objects.
        return h.version == handles_[h.idx].version;
    }

    // Will initialize 20 handles to begin with.
    constexpr static size_t INITIAL_SIZE = 20;
    using TPtr = std::unique_ptr<T>;

    // These two are kept in sync. Whenever there is no free handle, we
    // will increase the size of both of these vectors.
    std::vector<TPtr> objects_;
    std::vector<Handle> handles_;
};


}



