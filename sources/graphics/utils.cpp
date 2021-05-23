#include "utils.hpp"

#include "version.hpp"

#include <iostream>

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
