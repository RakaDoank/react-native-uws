#pragma once

#include <optional>
#include <mutex>
#include <thread>
#include "uWebSockets/App.h"

namespace react_native_uws {

/**
 * This simple wrapper helps consumer to run the uWebSockets server in an another thread.
 * UI Thread and JavaScript thread must not be blocked by the app run.
 */
class AppRunner {

private:
  std::thread serverThread;

  /**
   * The main loop of the uWebSockets.
   * Useful to wake the uWS Loop immediately to execute queued tasks.
   */
  uWS::Loop *serverLoop = nullptr;

  // +++++ for socket closing +++++
  std::mutex listenSocketMutex;
  // ----- for socket closing -----

public:
  /**
   * Be careful. Once the `listen` method from this class instance has been invoked,
   * this member has moved into another rvalue.
   *
   * Do not run the uWebSockets server manually from this instance.
   * If you do, you don't need this class.
   */
  uWS::App app;

  us_listen_socket_t *listenSocket = nullptr;

  void listen(const std::optional<std::string> &host,
              const int port,
              const std::optional<int> options,
              std::function<void (us_listen_socket_t *listenedSocket)> &&handler) {
    // To prevent the UI thread is getting blocked,
    // run the server from another thread
    // https://github.com/uNetworking/uWebSockets/issues/1858#issuecomment-2907728248
    this->serverThread = std::thread([this, host, options, port, handler]{
      uWS::App internalApp = uWS::App(std::move(this->app));
      this->serverLoop = internalApp.getLoop();

      std::function<void (us_listen_socket_t *listenedSocket)> listenHandler = [this, handler](auto *listenedSocket) {
        this->listenSocket = listenedSocket;
        handler(this->listenSocket);
      };

      if(host.has_value()) {
        if(options.has_value()) {
          internalApp.listen(host.value(), port, options.value(), std::move(listenHandler));
        } else {
          internalApp.listen(host.value(), port, std::move(listenHandler));
        }
      } else {
        internalApp.listen(port, std::move(listenHandler));
      }

      this->serverLoop->run();
    });

    this->serverThread.detach();
  };

  void close(std::function<void ()> &&onClose) {
    if(this->serverLoop) {
      this->serverLoop->defer([this, onClose]() {
        std::lock_guard<std::mutex> lock(this->listenSocketMutex);
        if(this->listenSocket) {
          us_listen_socket_close(0, this->listenSocket);
          this->listenSocket = nullptr;
          onClose();
        }
      });
    }
  };

};

} // namespace
