#ifndef POTATO_CORE_ECS_HPP
#define POTATO_CORE_ECS_HPP

#include "component.hpp"
#include "context.hpp"
#include "core/utils.hpp"
#include "entity.hpp"
#include "utils.hpp"

struct transform {
    static constexpr auto signature = 1 << 1;

    glm::vec3 translation {};
    glm::vec3 scale { 1.0f };
    glm::quat rotation {};

    void euler_rotate(glm::vec3 euler_angles) {
        rotation = glm::toQuat(
          glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z));
    }

    glm::mat4 mat4() const {
        return glm::translate(glm::mat4 { 1.f }, translation)
             * glm::toMat4(rotation) * glm::scale(glm::mat4 { 1.0f }, scale);
    }
};

#endif
