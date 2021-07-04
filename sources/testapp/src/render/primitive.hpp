#ifndef POTATO_VERTEX_HPP
#define POTATO_VERTEX_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <graphics/vkinclude/vulkan.hpp>
#include <vector>

namespace potato::graphics {
    class device;
}

namespace testapp {
    struct vertex;
    using mesh = std::vector<vertex>;

    struct vertex {
        glm::vec3 position;
        glm::vec3 color;

        static std::vector<vk::VertexInputBindingDescription>
        binding_descriptions();

        static std::vector<vk::VertexInputAttributeDescription>
        attribute_descriptions();
    };

    struct model_transform {
        glm::vec3 translation {};
        glm::vec3 scale { 1.f, 1.f, 1.f };
        glm::vec3 rotation {};

        glm::mat4 mat4() const {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);

            return glm::mat4 {
                {
                  scale.x * (c1 * c3 + s1 * s2 * s3),
                  scale.x * (c2 * s3),
                  scale.x * (c1 * s2 * s3 - c3 * s1),
                  0.0f,
                },
                {
                  scale.y * (c3 * s1 * s2 - c1 * s3),
                  scale.y * (c2 * c3),
                  scale.y * (c1 * c3 * s2 + s1 * s3),
                  0.0f,
                },
                {
                  scale.z * (c2 * s1),
                  scale.z * (-s2),
                  scale.z * (c1 * c2),
                  0.0f,
                },
                { translation.x, translation.y, translation.z, 1.0f }
            };
        }
    };

    class model {

      private:
        using shared_ptr_device =
          std::shared_ptr<const potato::graphics::device>;

        uint32_t          vertex_count {};
        shared_ptr_device potato_device;
        vk::Buffer        vertex_bufer {};
        vk::DeviceMemory  vertex_device_mem {};

      public:
        model_transform transform {};

        model(shared_ptr_device device, const mesh& model_mesh);
        ~model();

        model(const model&) = delete;
        model& operator=(const model&) = delete;

        model(model&&) = default;
        model& operator=(model&&) = default;

        void bind(const vk::CommandBuffer& cmdbuffer) const;
        void draw(const vk::CommandBuffer& cmdbuffer) const;
    };

    struct push_constants {
        glm::mat4 transform { 1.f };
        alignas(16) glm::vec3 color;
    };

}  // namespace testapp

#endif
