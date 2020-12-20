class TimeManager {
  public:
    int frameCount = 0;
    int elapsedSeconds = 0;
    double elapsedTime = 0;
    double deltaTime = 0;

    double GetFPS();
    void Tick(); // Called every frame

    static TimeManager& GetInstance();
    // Deleted to prevent copies
    TimeManager(const TimeManager&) = delete;
    void operator=(const TimeManager&) = delete;
  private:
    TimeManager() = default;
    ~TimeManager() = default;
};