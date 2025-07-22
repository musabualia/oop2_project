// ================================
// More State - State Pattern with FFmpeg Video Playback Integration
// ================================
#pragma once

#include "States/IState.h"
#include "UI/Button.h"
#include <SFML/Graphics.hpp>

// FFmpeg headers for video playback functionality
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class Game;
class StateMachine;

// Extended content state with video playback capabilities using FFmpeg
class MoreState : public IState {
public:
    MoreState(Game* game, StateMachine* machine);
    ~MoreState();

    // State lifecycle management
    void onEnter() override;
    void onExit() override;

    // Core state interface implementation
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    // Core system references
    Game* m_game;
    StateMachine* m_machine;

    // UI Elements
    std::unique_ptr<Button> m_backButton;                    // Return to previous state
    sf::Sprite m_backgroundSprite;                          // Background when no video

    // FFmpeg Video Playback System
    AVFormatContext* m_formatCtx = nullptr;                 // Video format context
    AVCodecContext* m_codecCtx = nullptr;                   // Video codec context
    const AVCodec* m_codec = nullptr;                       // Video codec
    AVFrame* m_frame = nullptr;                             // Video frame buffer
    AVPacket* m_packet = nullptr;                           // Video packet buffer
    SwsContext* m_swsCtx = nullptr;                         // Video scaling context
    int m_videoStreamIndex = -1;                           // Video stream index in file
    bool m_videoFinished = false;                           // Video playback completion flag

    // SFML Integration for Video Display
    sf::Texture m_videoTexture;                            // Texture for video frames
    sf::Sprite m_videoSprite;                              // Sprite for video display
    uint8_t* m_rgbBuffer = nullptr;                        // RGB buffer for frame conversion

    // Private methods for video management would be implemented here
    // (Video loading, frame processing, cleanup, etc.)
};