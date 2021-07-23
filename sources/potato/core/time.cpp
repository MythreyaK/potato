module potato.core:time;

namespace {
    static potato::chrono::clock_type GLOBAL_CLOCK {};
    constexpr potato::chrono::milliseconds MAX_FRAME_TIME { 30 };
}

namespace potato::chrono {

    inline milliseconds millicast(std::chrono::nanoseconds&& delta) {
        return std::chrono::duration_cast<milliseconds>(delta);
    }

    inline milliseconds microcast(std::chrono::nanoseconds&& delta) {
        return std::chrono::duration_cast<microseconds>(delta);
    }

    scoped_timer::scoped_timer(std::string timer_name)
      : m_name { timer_name }
      , m_start_time { GLOBAL_CLOCK.now() } {}

    scoped_timer::~scoped_timer() {
        auto time_delta = GLOBAL_CLOCK.now() - m_start_time;

        std::cout << std::format("[{}] took {}\n", m_name, time_delta);
    }

    timer::timer(std::string name)
      : m_name { name }
      , m_start { GLOBAL_CLOCK.now() } {}

    potato::chrono::milliseconds timer::elapsed() const {

        auto time_delta { millicast(GLOBAL_CLOCK.now() - m_prev_time) };

        m_prev_time = GLOBAL_CLOCK.now();

        return time_delta > MAX_FRAME_TIME ? MAX_FRAME_TIME : time_delta;
    }

}  // namespace potato::chrono
