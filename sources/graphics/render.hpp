#ifndef POTATO_RENDER_HPP
#define POTATO_RENDER_HPP

#include "device.hpp"
#include "instance.hpp"
#include "vkinclude/vulkan.hpp"

#include <vector>

namespace potato::render {

    class render_instance {
        vk::UniqueInstance               instance {};
        vk::UniqueDebugUtilsMessengerEXT debug_mess {};
        device                           render_device;

      public:
        render_instance(const std::vector<std::string> required_extensions,
                        GLFWwindow*                    window_handle)
          : instance(create_instance(required_extensions)),
            debug_mess(register_debug_callabck(*instance)),
            render_device(*instance, window_handle) {}
    };

}  // namespace potato::render

#endif  // !POTATO_RENDER_HPP
