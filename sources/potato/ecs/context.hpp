#ifndef POTATO_ECS_CONTEXT_HPP
#define POTATO_ECS_CONTEXT_HPP

#include "utils.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace {
    struct empty {};
}  // namespace

namespace ecs {

    class context {
      private:
        // context stores all the component arrays that this context is
        // responsible for
        std::unordered_map<uint64_t, std::unique_ptr<icomponents>>
          component_arrays {};

      public:
        context()  = default;
        ~context() = default;

        // no copy
        context(const context&) = delete;
        context& operator=(const context&) = delete;

        // allow move
        context(context&&) = default;
        context& operator=(context&&) = default;

        template<component_type T>
        context& add_component() {
            if ( component_arrays.contains(T::signature) ) {
                throw std::runtime_error("Context cannot contain more than one "
                                         "component store of same type");
            }
            component_arrays[T::signature] = std::make_unique<components<T>>();
            return *this;
        }

        template<component_type T>
        components<T>& get_component() {
            auto&& a = component_arrays.at(T::signature).get();
            return *reinterpret_cast<components<T>*>(a);
        }

        template<component_type T>
        components<T>& get_component() const {
            return component_arrays[T::signature];
        }

        entity create_entity();
        void remove_entity(const entity&);
    };

}  // namespace ecs

#endif
