module potato.core:utils;

import std.core;
import std.filesystem;

namespace potato::utils {

    using sloc = std::source_location;

    std::vector<std::byte> read_file(const std::string& fname) {

        namespace fs = std::filesystem;

        const auto     file_size { fs::file_size(fname) };
        constexpr auto FILE_FAIL_OPEN = static_cast<std::uintmax_t>(-1);

        if ( file_size != FILE_FAIL_OPEN ) {
            std::vector<std::byte> file_buffer { file_size };

            std::ifstream file { fname, std::ios::binary };

            if ( !file.is_open() ) {
                throw std::runtime_error("Could not open file for reading");
            }

            file.read(reinterpret_cast<char*>(file_buffer.data()),
                      file_buffer.size());

            file.close();

            return file_buffer;
        }

        else {
            throw std::runtime_error("Invalid file path");
        }
    }

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

    // Make sure not to pass temporaries! Argument MUST outlive the return value
    std::vector<const char*> to_vecchar(const std::vector<std::string>& s) {
        std::vector<const char*> ret {};
        ret.reserve(s.size());

        for ( const auto& i : s ) {
            ret.push_back(i.c_str());
        }

        return ret;
    }

    //void check_result(vk::Result&& res, const sloc loc) {

    //    const auto file_info {
    //        std::format("{}({}): ", loc.file_name(), loc.line())
    //    };

    //    const auto error_info { std::format("{}"
    //                                        "Error '{}' in {}\n",
    //                                        file_info,
    //                                        vk::to_string(res),
    //                                        loc.function_name()) };

    //    if ( res != vk::Result::eSuccess ) {
    //        std::cout << error_info << '\n';
    //    }
    //}

}  // namespace potato::utils
