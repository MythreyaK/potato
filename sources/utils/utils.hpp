#ifndef POTATO_UTILS_HPP
#define POTATO_UTILS_HPP

#include <string>
#include <vector>

// Yeah yeah shouldn't do this, but vulkan uses it so often,
// it becomes distracting after a while
#define UINTSIZE(X) static_cast<uint32_t>(X.size())

namespace potato::utils {
    std::vector<const char*> to_vecchar(const std::vector<std::string>& s);

    // Reads file in binary mode
    std::vector<std::byte> read_file(const std::string& fname);

    bool has_required_items(const std::string&              info,
                            const std::vector<std::string>& supported_items,
                            const std::vector<std::string>& required_items);

}  // namespace potato::utils

#endif
