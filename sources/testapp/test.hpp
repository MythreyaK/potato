#ifndef TESTAPP
#define TESTAPP

#include <glfwcpp/glfw.hpp>
#include <graphics/render.hpp>
#include <iostream>
#include <memory>
#include <string>

class testapp : public glfw::window {
  private:
    potato::render::render_instance renderer;

  public:
    testapp(int w, int h, const std::string& title)
      : glfw::window(w, h, title),
        renderer(required_extns(), get_handle()) {}

    void window_loop() override {

        int count = 0;
        while ( keep_window_open() ) {
            poll_events();
        }
    }
};

#endif  // !TESTAPP
