#ifndef POTATO_CORE_TIME_HPP
#define POTATO_CORE_TIME_HPP

#include <chrono>
#include <iostream>
#include <type_traits>

namespace potato::chrono {

    using nanoseconds  = std::chrono::duration<float, std::nano>;
    using microseconds = std::chrono::duration<float, std::micro>;
    using milliseconds = std::chrono::duration<float, std::milli>;

    using clock_type = std::chrono::high_resolution_clock;

    class scoped_timer {
      private:
        // TODO: Might not be monotonic or accurate on some systems

        std::string                         m_name {};
        std::chrono::time_point<clock_type> m_start_time;

      public:
        scoped_timer(std::string timer_name);
        ~scoped_timer();
    };

    class timer {
      private:
        std::string_view               m_name {};
        clock_type::time_point         m_start {};
        mutable clock_type::time_point m_prev_time {};

      public:
        timer(std::string name);
        milliseconds elapsed() const;
    };

}  // namespace potato::chrono

#endif
