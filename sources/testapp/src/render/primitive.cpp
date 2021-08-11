#include "primitive.hpp"

#include <format>
#include <graphics/device/device.hpp>

namespace testapp {

    // struct vertex
    std::vector<vk::VertexInputBindingDescription>
    vertex::binding_descriptions() {
        return { {
          .binding   = 0,
          .stride    = sizeof(vertex),
          .inputRate = vk::VertexInputRate::eVertex,
        } };
    }

    std::vector<vk::VertexInputAttributeDescription>
    vertex::attribute_descriptions() {
        return { {
                   .location = 0,
                   .binding  = 0,
                   .format   = vk::Format::eR32G32B32Sfloat,
                   .offset   = offsetof(vertex, position),

                 },
                 {
                   .location = 1,
                   .binding  = 0,
                   .format   = vk::Format::eR32G32B32Sfloat,
                   .offset   = offsetof(vertex, color),

                 } };
    }

    // class model
    model::model(shared_ptr_device device, mesh mesh)
      : m_mesh { mesh }
      , potato_device { device } {

        assert(m_mesh.size() >= 3 && "Vertex count must be at least 3");

        vk::DeviceSize buffer_size = sizeof(m_mesh[0]) * m_mesh.size();

        constexpr auto host_visible_coherent {
            vk::MemoryPropertyFlagBits::eHostVisible
            | vk::MemoryPropertyFlagBits::eHostCoherent
        };

        potato_device->create_buffer(buffer_size,
                                     vk::BufferUsageFlagBits::eVertexBuffer,
                                     host_visible_coherent,
                                     vertex_bufer,
                                     vertex_device_mem,
                                     mesh.data());
    }

    model::model(model&& other) {
        if ( *this != other ) swap(*this, other);
    }

    model& model::operator=(model&& other) {
        if ( *this != other ) swap(*this, other);
        return *this;
    }

    bool model::operator==(const model& other) const {
        return vertex_bufer == other.vertex_bufer
            && vertex_device_mem == other.vertex_device_mem;
    }

    model::~model() {
        potato_device->logical->destroyBuffer(vertex_bufer);
        potato_device->logical->freeMemory(vertex_device_mem);
    }

    void model::draw(const vk::CommandBuffer& cmd_buffer) const {
        cmd_buffer.draw(m_mesh.size(), 1, 0, 0);
    }

    void model::bind(const vk::CommandBuffer& cmd_buffer) const {
        vk::Buffer     buffers[] = { vertex_bufer };
        vk::DeviceSize offsets[] = { 0 };
        cmd_buffer.bindVertexBuffers(0, 1, buffers, offsets);
    }

    void swap(model& a, model& b) {
        using std::swap;
        swap(a.m_mesh, b.m_mesh);
        swap(a.potato_device, b.potato_device);
        swap(a.vertex_bufer, b.vertex_bufer);
        swap(a.vertex_device_mem, b.vertex_device_mem);
    }

}  // namespace testapp
