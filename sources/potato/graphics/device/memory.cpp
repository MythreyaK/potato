#include "device.hpp"

namespace potato::graphics {

    vk::Format
    device::find_supported_format(const std::vector<vk::Format>& candidates,
                                  vk::ImageTiling                tiling,
                                  vk::FormatFeatureFlags features) const {

        for ( const auto& format : candidates ) {
            vk::FormatProperties props {
                physical.getFormatProperties2(format).formatProperties
            };

            if ( tiling == vk::ImageTiling::eLinear
                 && (props.linearTilingFeatures & features) == features )
            {
                return format;
            }
            else if ( tiling == vk::ImageTiling::eOptimal
                      && (props.optimalTilingFeatures & features) == features )
            {
                return format;
            }
        }

        throw std::runtime_error("Could not find supported format");
    }

}  // namespace potato::graphics
