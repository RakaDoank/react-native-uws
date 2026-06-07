#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <uWebSockets/App.h>

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
  us_listen_socket_t *listenSocket = nullptr;

  std::mutex listenSocketMutex;
  // ----- for socket closing -----

public:
  /**
   * Be careful. Once the `run` method has been invoked,
   * this member has moved into another rvalue.
   */
  uWS::App app;

  void run() {
    // TODO
  };

  void close() {
    // TODO
  };

};

} // namespace
