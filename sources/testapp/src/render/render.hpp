#ifndef TESTAPP_RENDER_HPP
#define TESTAPP_RENDER_HPP

#include "camera.hpp"
#include "primitive.hpp"

#include <graphics/pipeline.hpp>

namespace testapp {

    class render_system {
      private:
        potato::graphics::pipeline m_pipeline;

        void create_pipeline(const vk::Device&, const vk::RenderPass&);

      public:
        render_system(const vk::Device&, const vk::RenderPass&);

        void render_objects(const vk::CommandBuffer&,
                            const std::vector<testapp::model>&,
                            const camera&);
    };

}  // namespace testapp
#endif
