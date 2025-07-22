#include "States/MoreState.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "States/MenuState.h"
#include "States/StateMachine.h"
#include "Core/Game.h"
#include <iostream>

MoreState::MoreState(Game* game, StateMachine* machine)
    : m_game(game), m_machine(machine) {

    sf::Font& font = ResourceManager::getInstance().getFont("bruce");
    sf::Texture& btnTexture = ResourceManager::getInstance().getTexture("btn.png");

    m_backButton = std::make_unique<Button>(
        sf::Vector2f(20.f, 20.f),
        sf::Vector2f(140.f, 40.f),
        btnTexture,
        sf::IntRect(),
        font,
        "Back"
    );

    m_backButton->setClickCallback([this]() {
        m_machine->popState();
        });

    avformat_network_init();

    if (avformat_open_input(&m_formatCtx, "more.mp4", nullptr, nullptr) != 0) {
        std::cerr << "Failed to open video file!" << std::endl;
        m_videoFinished = true;
        return;
    }

    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        std::cerr << "Failed to find stream info!" << std::endl;
        m_videoFinished = true;
        return;
    }

    for (unsigned int i = 0; i < m_formatCtx->nb_streams; ++i) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }

    if (m_videoStreamIndex == -1) {
        std::cerr << "No video stream found!" << std::endl;
        m_videoFinished = true;
        return;
    }

    AVCodecParameters* codecParams = m_formatCtx->streams[m_videoStreamIndex]->codecpar;
    m_codec = avcodec_find_decoder(codecParams->codec_id);

    if (!m_codec) {
        std::cerr << "Unsupported codec!" << std::endl;
        m_videoFinished = true;
        return;
    }

    m_codecCtx = avcodec_alloc_context3(m_codec);
    avcodec_parameters_to_context(m_codecCtx, codecParams);

    if (avcodec_open2(m_codecCtx, m_codec, nullptr) < 0) {
        std::cerr << "Failed to open codec!" << std::endl;
        m_videoFinished = true;
        return;
    }

    m_frame = av_frame_alloc();
    m_packet = av_packet_alloc();

    m_swsCtx = sws_getContext(
        m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
        m_codecCtx->width, m_codecCtx->height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_codecCtx->width, m_codecCtx->height, 1);
    m_rgbBuffer = (uint8_t*)av_malloc(numBytes);

    m_videoTexture.create(m_codecCtx->width, m_codecCtx->height);
    m_videoSprite.setTexture(m_videoTexture);
    m_videoSprite.setScale(
        static_cast<float>(m_game->getWindow().getSize().x) / m_codecCtx->width,
        static_cast<float>(m_game->getWindow().getSize().y) / m_codecCtx->height
    );
}

MoreState::~MoreState() {
    if (m_rgbBuffer) av_free(m_rgbBuffer);
    if (m_frame) av_frame_free(&m_frame);
    if (m_packet) av_packet_free(&m_packet);
    if (m_codecCtx) avcodec_free_context(&m_codecCtx);
    if (m_formatCtx) avformat_close_input(&m_formatCtx);
    if (m_swsCtx) sws_freeContext(m_swsCtx);
}

void MoreState::onEnter() {
    // Pre-decode the first frame
    bool frameLoaded = false;
    while (!frameLoaded && av_read_frame(m_formatCtx, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            if (avcodec_send_packet(m_codecCtx, m_packet) >= 0) {
                if (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {
                    // Convert to RGBA
                    uint8_t* dest[4] = { m_rgbBuffer, nullptr, nullptr, nullptr };
                    int linesize[4] = { 4 * m_codecCtx->width, 0, 0, 0 };

                    sws_scale(
                        m_swsCtx,
                        m_frame->data,
                        m_frame->linesize,
                        0,
                        m_codecCtx->height,
                        dest,
                        linesize
                    );

                    m_videoTexture.update(m_rgbBuffer);
                    frameLoaded = true;
                }
            }
        }
        av_packet_unref(m_packet);
    }

    // Optional: Play synced audio here if needed
    AudioManager::getInstance().playSound("more", AudioManager::AudioCategory::Music);
}



void MoreState::onExit() {
    AudioManager::getInstance().stopAllSounds();

}

void MoreState::handleEvent(const sf::Event& event) {
    if (m_backButton)
        m_backButton->handleEvent(event);
}

void MoreState::update(float dt) {
    if (m_videoFinished) return;

    if (av_read_frame(m_formatCtx, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            if (avcodec_send_packet(m_codecCtx, m_packet) >= 0) {
                while (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {
                    uint8_t* dest[4] = { m_rgbBuffer, nullptr, nullptr, nullptr };
                    int linesize[4] = { 4 * m_codecCtx->width, 0, 0, 0 };

                    sws_scale(m_swsCtx, m_frame->data, m_frame->linesize, 0,
                        m_codecCtx->height, dest, linesize);

                    m_videoTexture.update(m_rgbBuffer);
                }
            }
        }
        av_packet_unref(m_packet);
    }
    else {
        // Video finished
        m_videoFinished = true;
        m_machine->popState();
    }

    if (m_backButton)
        m_backButton->update(dt);
}

void MoreState::render(sf::RenderWindow& window) {
    window.draw(m_videoSprite);
    if (m_backButton)
        m_backButton->draw(window);
}