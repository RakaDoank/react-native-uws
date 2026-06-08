#include "ReactNativeUwsModule.h"
#include <memory>
#include <vector>
#include <jsi/jsi.h>
#include "RequestHostObject.h"
#include "app/AppHost.h"
#include "app/TemplatedAppObject.h"
#include "uWebSockets/HttpContextData.h"
#include "uWebSockets/HttpResponse.h"

namespace facebook::react {

namespace {

std::vector<std::shared_ptr<react_native_uws::AppHost>> appHosts;

} // namespace

ReactNativeUwsModule::ReactNativeUwsModule(std::shared_ptr<CallInvoker> jsInvoker)
  : NativeReactNativeUwsCxxSpec(std::move(jsInvoker)) {}

react_native_uws::TemplatedAppObject ReactNativeUwsModule::App(facebook::jsi::Runtime &rt,
                                                               std::optional<facebook::jsi::Object> appOptions) {
  auto appHost = std::make_shared<react_native_uws::AppHost>();
  appHosts.emplace_back(appHost);
  return appHost->getTemplatedAppObject(rt, this->jsInvoker_);
}

facebook::jsi::Object ReactNativeUwsModule::getParts(facebook::jsi::Runtime &rt,
                                                    facebook::jsi::Object body,
                                                    facebook::jsi::Object contentType) {
  // TODO
  return facebook::jsi::Object(rt);
}

void ReactNativeUwsModule::us_listen_socket_close(facebook::jsi::Runtime &rt,
                                                  facebook::jsi::Object listenSocket) {
  // TODO
}

void ReactNativeUwsModule::us_socket_local_port(facebook::jsi::Runtime &rt,
                                                facebook::jsi::Object socket) {
  // TODO
}

}