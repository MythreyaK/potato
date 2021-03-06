#include "primitive.hpp"

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
    model::model(shared_ptr_device device, const mesh& mesh)
      : potato_device(device)
      , vertex_count(mesh.size()) {

        assert(vertex_count >= 3 && "Vertex count must be at least 3");

        vk::DeviceSize buffer_size = sizeof(mesh[0]) * vertex_count;

        constexpr auto host_visible_coherent {
            vk::MemoryPropertyFlagBits::eHostVisible
            | vk::MemoryPropertyFlagBits::eHostCoherent
        };

        // TOOD: MEMORY
        // device->create_buffer(buffer_size,
        //                       vk::BufferUsageFlagBits::eVertexBuffer,
        //                       host_visible_coherent,
        //                       vertex_bufer,
        //                       vertex_device_mem);
        // void* data;
        // vertex_devi
        // auto  result = device->logical->mapMemory(vertex_device_mem,
        //                                          0,
        //                                          buffer_size,
        //                                          {},
        //                                          &data);
        // if ( result != vk::Result::eSuccess ) {
        //     throw std::runtime_error(
        //       "Failed to map vertex buffer memory to GPU");
        // }

        // std::memcpy(data, mesh.data(), buffer_size);
        // device->logical->unmapMemory(vertex_device_mem);
        vertex_bufer = std::move(device->logical->createBuffer({
          .size        = buffer_size,
          .usage       = vk::BufferUsageFlagBits::eVertexBuffer,
          .sharingMode = vk::SharingMode::eExclusive,
        }));

        auto mem_req { device->logical->getBufferMemoryRequirements(
          vertex_bufer) };

        vertex_device_mem =
          std::move(vma::memory<>(mem_req, host_visible_coherent));

        vertex_device_mem.bind(vertex_bufer)
          .map()
          .write_to_gpu(mesh.data(), buffer_size)
          .unmap();
    }

    model::~model() {
        potato_device->logical->waitIdle();
        potato_device->logical->destroyBuffer(vertex_bufer);
        // potato_device->logical->freeMemory(vertex_device_mem);
        vertex_device_mem.free();
    }

    void model::draw(const vk::CommandBuffer& cmd_buffer) const {
        cmd_buffer.draw(vertex_count, 1, 0, 0);
    }

    void model::bind(const vk::CommandBuffer& cmd_buffer) const {
        vk::Buffer     buffers[] = { vertex_bufer };
        vk::DeviceSize offsets[] = { 0 };
        cmd_buffer.bindVertexBuffers(0, 1, buffers, offsets);
    }

}  // namespace testapp
