#pragma once

#include "GLFW/glfw3.h"

class UIManager {
  public:
    static GLFWwindow* window;
    static UIManager& GetInstance();
    // Deleted to prevent copies
    UIManager(const UIManager&) = delete;
    void operator=(const UIManager&) = delete;

  private:
    UIManager();
    ~UIManager();
};
