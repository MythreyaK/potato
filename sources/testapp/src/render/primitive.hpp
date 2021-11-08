#ifndef POTATO_VERTEX_HPP
#define POTATO_VERTEX_HPP

#include "graphics/memory/vma.hpp"

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
        glm::quat rotation {};

        void euler_rotate(glm::vec3 euler_angles) {
            rotation = glm::toQuat(
              glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z));
        }

        glm::mat4 mat4() const {
            return glm::translate(glm::mat4 { 1.f }, translation)
                 * glm::toMat4(rotation)
                 * glm::scale(glm::mat4 { 1.0f }, scale);
        }
    };

    class model {

      private:
        using shared_ptr_device =
          std::shared_ptr<const potato::graphics::device>;

        uint32_t          vertex_count {};
        shared_ptr_device potato_device;
        vk::Buffer        vertex_bufer {};
        vma::memory<>     vertex_device_mem {};

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
