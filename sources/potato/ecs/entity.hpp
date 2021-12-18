#ifndef POTATO_ECS_ENTITY_HPP
#define POTATO_ECS_ENTITY_HPP

#include "context.hpp"
#include "utils.hpp"

#include <bitset>
#include <cstddef>

namespace ecs {
    // TODO: Thread safety

    class entity {
      public:
        using id = uint32_t;

      private:
        static id next_eid;
        static id count;

        id              entity_id { 0 };
        context*        entity_context;
        std::bitset<64> components;

      public:
        explicit entity(context& c);
        ~entity();

        id get_id() const;

        // no copy
        entity(const entity&) = delete;
        entity& operator=(const entity&) = delete;

        // allow move
        entity(entity&&);
        entity& operator=(entity&&);

        template<component_type T, typename... Args>
        T& add_component(Args... args) {
            entity_context->get_component<T>().add(*this,
                                                   std::forward<Args>(args)...);
            return get_component<T>();
        }

        template<typename T>
        requires component_type<T> T& get_component() {
            return entity_context->get_component<T>().get(*this);
        }

        template<typename T>
        requires component_type<T> T& get_component() const {
            return entity_context->get_component<T>().get(*this);
        }

        // template<typename T>
        // requires component_type<T>
        // const T& get_component() const;

        // template<typename T>
        // requires component_type<T>
        // void remove_component();
    };

}  // namespace ecs

#endif
