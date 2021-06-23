#ifndef POTATO_RENDER_INSTANCE_HPP
#define POTATO_RENDER_INSTANCE_HPP

// #include "vkinclude/vulkan.hpp"

extern "C" {
    struct GLFWwindow;
}

namespace potato::render {

    class instance {
      private:
        vk::UniqueInstance               vkinstance;
        vk::UniqueDebugUtilsMessengerEXT debug_mess {};

      public:
        instance(std::vector<std::string> extensions = {});

        const vk::Instance& get();

        static std::vector<std::string> supported_extensions();

        // no copies
        instance(const instance&) = delete;
        instance& operator=(const instance&) = delete;

        // allow move
        instance(instance&&) = default;
        instance& operator=(instance&&) = default;
    };

}  // namespace potato::render

#endif
