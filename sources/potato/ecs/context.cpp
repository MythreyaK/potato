#include "context.hpp"

#include "component.hpp"
#include "entity.hpp"

namespace ecs {
    entity context::create_entity() {
        return entity { *this };
    }

    void context::remove_entity(const entity& e) {
        for ( auto& i : component_arrays ) {
            i.second->remove(e);
        }
    }
}  // namespace ecs
