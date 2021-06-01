#ifndef POTATO_INSTANCE_RENDER_CONTEXT_HPP
#define POTATO_INSTANCE_RENDER_CONTEXT_HPP

#include "vkinclude/vulkan.hpp"

extern "C" {
    struct GLFWwindow;
}

namespace potato::render {

    class context {
      private:
        vk::UniqueInstance               instance;
        vk::UniqueDebugUtilsMessengerEXT debug_mess {};

      public:
        context(GLFWwindow*              window_handle,
                std::vector<std::string> extensions = {});

        const vk::Instance& get_instance();

        static std::vector<std::string> supported_extensions();

        // no copies
        context(const context&) = delete;
        context& operator=(const context&) = delete;

        // allow move
        context(context&&) = default;
        context& operator=(context&&) = default;
    };

}  // namespace potato::render

#endif
