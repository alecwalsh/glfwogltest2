#pragma once

#include <queue>
#include <functional>
#include <utility>

class UIManager {
  public:
    void Draw();

    static UIManager& GetInstance();
    // Deleted to prevent copies
    UIManager(const UIManager&) = delete;
    void operator=(const UIManager&) = delete;

    bool guiActive = false;

    // Call before any drawing
    void Initialize();
    void Destroy();

    // Registers a function that draws UI elements
    template<typename F>
    void AddToUI(F&& f) {
        registeredFunctions.push(std::forward<F>(f));
    }
  private:
    UIManager() = default;
    ~UIManager();

    void BeginFrame();
    void EndFrame();
    void DrawOptional();      // Draw UI elements whose visibility can be toggled
    void DrawAlwaysVisible(); // Draw UI elements that are always visible
    void DrawStats();
    // Call all registered UI functions, and clears the queue
    void DrawMisc();

    // Registered UI functions
    std::queue<std::function<void()>> registeredFunctions;

    char textBuffer[64] = {};
    unsigned int displayFPS = 0;

    bool initialized = false;
};

inline UIManager& uiManager = UIManager::GetInstance();