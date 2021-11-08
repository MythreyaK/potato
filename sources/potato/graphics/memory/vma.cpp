#include "vma.hpp"

#include "internal.hpp"

namespace vma {
    bool init(vk::PhysicalDevice pd, vk::Device d) {
        internal::phy_device = pd;
        internal::device     = d;

        return true;
    }
}  // namespace vma
