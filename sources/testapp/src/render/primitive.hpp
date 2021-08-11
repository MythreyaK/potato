#ifndef POTATO_VERTEX_HPP
#define POTATO_VERTEX_HPP

#include <core/utils.hpp>
#include <ecs/ecs.hpp>
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

    class model {

      private:
        using shared_ptr_device =
          std::shared_ptr<const potato::graphics::device>;
        mesh              m_mesh {};
        shared_ptr_device potato_device;
        vk::Buffer        vertex_bufer {};
        vk::DeviceMemory  vertex_device_mem {};

      public:
        static constexpr auto signature = potato::utils::bit(4);

        model(shared_ptr_device device, mesh model_mesh);
        ~model();

        model(const model&) = delete;
        model& operator=(const model&) = delete;

        model(model&&);
        model& operator=(model&&);
        bool   operator==(const model&) const;

        friend void swap(model&, model&);

        void bind(const vk::CommandBuffer& cmdbuffer) const;
        void draw(const vk::CommandBuffer& cmdbuffer) const;
    };

    struct push_constants {
        glm::mat4 transform { 1.f };
        alignas(16) glm::vec3 color;
    };

}  // namespace testapp

#endif
