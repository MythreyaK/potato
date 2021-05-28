#include "render.hpp"

#include <vector>
namespace potato::render {

    render_instance::render_instance(
      const std::vector<std::string> required_extensions,
      GLFWwindow*                    window_handle)
      : render_context(window_handle)
      , render_device(render_context.get_instance(), window_handle)
      , render_pipeline(render_device,
                        "shader.vert.spv",
                        "shader.frag.spv",
                        pipeline::default_pipeline_info({ 800, 600 })) {}

}  // namespace potato::render
