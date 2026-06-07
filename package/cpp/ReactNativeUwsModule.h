#pragma once

#include <ReactNativeUwsSpecsJSI.h>
#include "app/TemplatedAppObject.h"

namespace facebook::react {

class ReactNativeUwsModule : public NativeReactNativeUwsCxxSpec<ReactNativeUwsModule> {

public:
  ReactNativeUwsModule(std::shared_ptr<CallInvoker> jsInvoker);

  react_native_uws::TemplatedAppObject App(facebook::jsi::Runtime &rt,
                                           std::optional<facebook::jsi::Object> appOptions);

  facebook::jsi::Object SSLApp(facebook::jsi::Runtime &rt,
                              facebook::jsi::Object appOptions);

  facebook::jsi::Object getParts(facebook::jsi::Runtime &rt,
                facebook::jsi::Object body,
                facebook::jsi::Object contentType);

  void us_listen_socket_close(facebook::jsi::Runtime &rt,
                                              facebook::jsi::Object listenSocket);

  void us_socket_local_port(facebook::jsi::Runtime &rt,
                                            facebook::jsi::Object socket);

};

}
