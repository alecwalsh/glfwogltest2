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

  private:
    UIManager();
    ~UIManager();
};
