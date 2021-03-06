#pragma once

class UIManager {
  public:
    void BeginFrame();
    void EndFrame();
    void Draw();

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
