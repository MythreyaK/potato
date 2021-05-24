#ifndef POTATO_RENDER_UTILS_HPP
#define POTATO_RENDER_UTILS_HPP

#include <string>
#include <vector>

namespace potato::render {
    std::vector<const char*> to_vecchar(const std::vector<std::string>& s);

    bool has_required_items(const std::string&              info,
                            const std::vector<std::string>& supported_items,
                            const std::vector<std::string>& required_items);
}  // namespace potato::render

#endif
