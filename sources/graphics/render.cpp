#include "render.hpp"

#include <vector>
namespace potato::render {

    render_instance::render_instance(
      const std::vector<std::string> required_extensions,
      GLFWwindow*                    window_handle)
      : context(window_handle)
      , render_device(context.get_instance(), window_handle)
      , render_surface(window_handle, render_device) {}

}  // namespace potato::render
