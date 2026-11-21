#pragma once

#include <ixwebsocket/IXWebSocket.h>
#include <atomic>
#include <mutex>
#include <string>

class ObsClient
{
public:
    ObsClient();
    ~ObsClient();

    void Connect();
    void SetCurrentProgramScene(const std::string& sceneName);
    void Disconnect();

private:
    void SendSceneRequest(const std::string& sceneName);

    ix::WebSocket socket_;
    std::mutex mutex_;
    bool identified_ = false;
    std::string pendingScene_;
    std::atomic<unsigned> requestCounter_{ 0 };
};