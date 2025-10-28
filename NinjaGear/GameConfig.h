#pragma once

class GameConfig
{
public:
    static constexpr int WINDOW_WIDTH = 1920;
    static constexpr int WINDOW_HEIGHT = 1080;

    // UI takes 15% of game height at bottom
    static constexpr float UI_HEIGHT_PERCENT = 0.15f;
    static constexpr int GAME_WIDTH = 1280;
    static constexpr int GAME_HEIGHT = static_cast<int>(GAME_WIDTH * 0.85f);  // 85% for gameplay, 15% for UI
    static constexpr int UI_HEIGHT = GAME_WIDTH - GAME_HEIGHT;  // Bottom 15%

    // FIXED: Camera should be 20 tiles × 16 pixels = 320 pixels
    static constexpr int CAMERA_WIDTH = 320;   // 20 tiles wide
    static constexpr int CAMERA_HEIGHT = 320;  // 20 tiles tall

    static constexpr int SCREEN_X = 0;
    static constexpr int SCREEN_Y = 0;
    static constexpr float PLAYER_SIZE = 16.0f;
    static constexpr float ENEMY_SIZE = 16.0f;
    static constexpr float CENTER_X = GAME_WIDTH / 2.0f;
    static constexpr float CENTER_Y = GAME_HEIGHT / 2.0f;
    static constexpr float SCALE_FACTOR = 2.0f;
};