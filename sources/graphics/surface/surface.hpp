#ifndef POTATO_RENDER_SURFACE_HPP
#define POTATO_RENDER_SURFACE_HPP

#include <memory>

extern "C" {
    struct GLFWwindow;
}

namespace potato::render {

    class surface : public std::enable_shared_from_this<surface> {
      private:
        GLFWwindow*          window_handle {};
        vk::UniqueSurfaceKHR vksurface {};

      public:
        surface(const vk::Instance&, GLFWwindow*);

        surface(const surface&) = delete;
        surface& operator=(const surface&) = delete;

        surface(surface&&) = default;
        surface& operator=(surface&&) = default;

        std::shared_ptr<surface>       make_shared();
        std::shared_ptr<const surface> make_shared() const;
        const vk::SurfaceKHR&          get() const;

        vk::Extent2D framebuffer_size(const vk::PhysicalDevice&) const;

        bool can_present(const vk::PhysicalDevice&, uint32_t queue_inx) const;
        vk::PresentModeKHR get_present_mode(const vk::PhysicalDevice&) const;
        uint32_t           swapimage_count(const vk::PhysicalDevice&,
                                           uint32_t request_count) const;

        std::pair<vk::Format, vk::ColorSpaceKHR>
        get_present_setting(const vk::PhysicalDevice&) const;
    };

}  // namespace potato::render

#endif
