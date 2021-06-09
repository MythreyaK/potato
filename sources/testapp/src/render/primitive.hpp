#ifndef POTATO_VERTEX_HPP
#define POTATO_VERTEX_HPP

#include <glm/glm.hpp>
#include <graphics/vkinclude/vulkan.hpp>
#include <vector>

namespace potato::render {
    class device;
}

namespace potato {
    struct vertex;
    using mesh = std::vector<vertex>;

    struct vertex {
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<vk::VertexInputBindingDescription>
        binding_descriptions();

        static std::vector<vk::VertexInputAttributeDescription>
        attribute_descriptions();
    };

    class model {

      private:
        using shared_ptr_device = std::shared_ptr<const potato::render::device>;

        uint32_t          vertex_count {};
        shared_ptr_device potato_device;
        vk::Buffer        vertex_bufer {};
        vk::DeviceMemory  vertex_device_mem {};

      public:
        model(shared_ptr_device device, const mesh& model_mesh);
        ~model();

        model(const model&) = delete;
        model& operator=(const model&) = delete;

        model(model&&) = default;
        model& operator=(model&&) = default;

        void bind(const vk::CommandBuffer& cmdbuffer) const;
        void draw(const vk::CommandBuffer& cmdbuffer) const;
    };

    struct PushConstantData {
        glm::mat2 transform { 1.f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

}  // namespace potato

#endif
