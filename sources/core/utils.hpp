#ifndef POTATO_UTILS_HPP
#define POTATO_UTILS_HPP

#include "version.hpp"

#include <concepts>
#include <string>
#include <vector>

namespace potato::utils {

    template<typename T>
    concept has_size = requires(T a) {
        a.size();
    };

    // Returns size of container as uint32_t so that
    // vulkan can work happily
    template<typename T>
    requires has_size<T>
    constexpr uint32_t vksize(const T& x) {
        // TODO: Check if any overhead in release mode?
        // Can make this more specific?

        // Yeah yeah shouldn't do this, but vulkan uses it so often,
        // it becomes distracting after a while

        return static_cast<uint32_t>(x.size());
    }

    std::vector<const char*> to_vecchar(const std::vector<std::string>& s);

    // Reads file in binary mode
    std::vector<std::byte> read_file(const std::string& fname);

    bool has_required_items(const std::string&              info,
                            const std::vector<std::string>& supported_items,
                            const std::vector<std::string>& required_items);

}  // namespace potato::utils

#endif
