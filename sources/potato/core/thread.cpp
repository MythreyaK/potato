#include "thread.hpp"

#include "platform.h"

#include <iostream>
#include <thread>

namespace {
#if defined(WINDOWS) && defined(NTDDI_WIN10_RS1)
    bool platform_set_thread_name(const std::string& name) {
        const std::wstring a { name.cbegin(), name.cend() };
        return !FAILED(SetThreadDescription(::GetCurrentThread(), a.c_str()));
    }
#endif

}  // namespace

namespace std::this_thread {

    bool set_name(const std::string& name) {
        return platform_set_thread_name(name);
    }

}  // namespace std::this_thread
