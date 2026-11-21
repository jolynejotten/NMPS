#include "ObsClient.h"

#include <ixwebsocket/IXNetSystem.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

using nlohmann::json;

namespace
{
    std::mutex logMutex;

    void LogObs(const std::string& text)
    {
        std::lock_guard<std::mutex> lock(logMutex);

        char* userProfile = nullptr;
        size_t size = 0;
        const errno_t result = _dupenv_s(&userProfile, &size, "USERPROFILE");

        std::filesystem::path logPath = "obs_log.txt";
        if (result == 0 && userProfile != nullptr)
        {
            logPath = std::filesystem::path(userProfile) / "Desktop" / "obs_log.txt";
        }

        std::free(userProfile);

        std::ofstream log(logPath, std::ios::app);
        if (log)
        {
            log << text << '\n';
        }
    }
}

ObsClient::ObsClient() = default;

ObsClient::~ObsClient()
{
    Disconnect();
}

void ObsClient::Connect()
{
    ix::initNetSystem();

    LogObs("Starting connection to ws://127.0.0.1:4455");

    socket_.setUrl("ws://127.0.0.1:4455");
    socket_.setOnMessageCallback(
        [this](const ix::WebSocketMessagePtr& message)
        {
            if (message->type == ix::WebSocketMessageType::Open)
            {
                LogObs("WebSocket opened");
                return;
            }

            if (message->type == ix::WebSocketMessageType::Close)
            {
                LogObs("WebSocket closed");
                return;
            }

            if (message->type == ix::WebSocketMessageType::Error)
            {
                LogObs("WebSocket error: " + message->errorInfo.reason);
                return;
            }

            if (message->type != ix::WebSocketMessageType::Message)
            {
                return;
            }

            LogObs("Received from OBS: " + message->str);

            const json incoming = json::parse(message->str, nullptr, false);
            if (incoming.is_discarded() || !incoming.contains("op"))
            {
                LogObs("Could not parse OBS message or it had no op field");
                return;
            }

            const int op = incoming.value("op", -1);

            if (op == 0)
            {
                LogObs("OBS Hello received; sending Identify");
                socket_.send(R"({"op":1,"d":{"rpcVersion":1}})");
            }
            else if (op == 2)
            {
                LogObs("OBS Identified successfully");

                std::string sceneToSend;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    identified_ = true;
                    sceneToSend = pendingScene_;
                    pendingScene_.clear();
                }

                if (!sceneToSend.empty())
                {
                    LogObs("Sending queued scene request: " + sceneToSend);
                    SendSceneRequest(sceneToSend);
                }
            }
            else if (op == 7)
            {
                LogObs("OBS request response: " + message->str);
            }
        });

    socket_.start();
}

void ObsClient::SetCurrentProgramScene(const std::string& sceneName)
{
    LogObs("SetCurrentProgramScene called: " + sceneName);

    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!identified_)
        {
            pendingScene_ = sceneName;
            LogObs("Not identified yet; queued scene: " + sceneName);
            return;
        }
    }

    SendSceneRequest(sceneName);
}

void ObsClient::SendSceneRequest(const std::string& sceneName)
{
    const std::string requestId =
        "nsc-" + std::to_string(++requestCounter_);

    const json request = {
        { "op", 6 },
        { "d",{
            { "requestType", "SetCurrentProgramScene" },
            { "requestId", requestId },
            { "requestData",{
                { "sceneName", sceneName }
            } }
        } }
    };

    LogObs("Sending request: " + request.dump());
    socket_.send(request.dump());
}

void ObsClient::Disconnect()
{
    socket_.stop();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        identified_ = false;
        pendingScene_.clear();
    }

    ix::uninitNetSystem();
}