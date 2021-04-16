#pragma once

class UIManager {
  public:
    void BeginFrame();
    void EndFrame();
    void Draw();
    void DrawOptional(); // Draw UI elements whose visibility can be toggled
    void DrawAlwaysVisible(); // Draw UI elements that are always visible

    static UIManager& GetInstance();
    // Deleted to prevent copies
    UIManager(const UIManager&) = delete;
    void operator=(const UIManager&) = delete;

    bool guiActive = false;
  private:
    UIManager();
    ~UIManager();

    void DrawStats();

    char textBuffer[64] = {};

    unsigned int displayFPS = 0;
};
