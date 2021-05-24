#include "utils.hpp"

#include "version.hpp"

#include <iostream>

namespace potato::render {

    bool has_required_items(const std::string&              name,
                            const std::vector<std::string>& supported_items,
                            const std::vector<std::string>& required_items) {

        int found_extns { 0 };

        std::cout << name << '\n';

        for ( const auto& supported_item : supported_items ) {

            bool ext_present { false };

            for ( const auto& required_item : required_items ) {

                if ( supported_item == required_item ) {
                    found_extns += 1;
                    ext_present = true;
                    break;
                }
            }

            const auto prnt { "[%s] %s\n" };
            if ( ext_present ) {

                std::printf(prnt, "Enabled ", supported_item.c_str());
            }
            else {
                std::printf(prnt, "Disabled", supported_item.c_str());
            }
        }
        std::cout << '\n';
        return found_extns == required_items.size();
    }

    // Make sure not to pass temporaries! Argument
    // MUST outlive the return value
    std::vector<const char*> to_vecchar(const std::vector<std::string>& s) {
        std::vector<const char*> ret {};
        ret.reserve(s.size());

        for ( const auto& i : s ) {
            ret.push_back(i.c_str());
        }

        return ret;
    }
}  // namespace potato::render
