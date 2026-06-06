#include "ReactNativeUwsModule.h"
#include <android/looper.h>
#include <algorithm>
#include <chrono>
#include <future>
#include <memory>
#include <iterator>
#include <jsi/jsi.h>
#include <string>
#include <utility>
#include <vector>
#include "RequestHostObject.h"
#include "http/RouteState.h"
#include "http/Server.h"
#include "uWebSockets/HttpContextData.h"
#include "uWebSockets/HttpResponse.h"

namespace facebook::react {

namespace {

std::vector<std::shared_ptr<react_native_echo::Server>> servers;

std::shared_ptr<react_native_echo::Server> getServerByID(std::string &serverID) {
  auto serverPtr = std::find_if(servers.begin(), servers.end(), [&serverID](auto &item) {
    return serverID == item->id;
  });
  return *serverPtr;
}

void commonRouteHandler(const AsyncCallback<> &asyncCallback,
                        uWS::HttpResponse<false> *res,
                        uWS::HttpRequest *req) {

  auto aborted = std::make_shared<bool>(false);
  /**
   * I don't know why without this,
   * uWebSockets can't wait the route handler to be finished.
   * I thought `onAborted` is just a callback.
   */
  res->onAborted([aborted]() {
    *aborted = true;
  });

  asyncCallback.call([aborted, res, req](facebook::jsi::Runtime &rt1, facebook::jsi::Function &cb) mutable {
    auto requestHostObject = std::make_shared<react_native_echo::RequestHostObject>(res, req);

    if(!*aborted) {
      cb.call(rt1,
              // requestObject
              facebook::jsi::Object::createFromHostObject(rt1, std::move(requestHostObject)),

              // responseNotifier
              facebook::jsi::Function::createFromHostFunction(rt1,
                                                              facebook::jsi::PropNameID::forUtf8(rt1, "responseNotifier"),
                                                              1,
                                                              [aborted, res](facebook::jsi::Runtime &rt2,
                                                                      const facebook::jsi::Value &thisValue,
                                                                      const facebook::jsi::Value *arguments,
                                                                      size_t count) mutable -> facebook::jsi::Value {
                                                                /**
                                                                 * Check the JS object definition at /src/modules/http/_response-to-object.ts
                                                                 */
                                                                auto responseObject = facebook::jsi::Object(arguments[0].asObject(rt2));

                                                                if(!*aborted) {
                                                                  // +++++ Status +++++
                                                                  {
                                                                    // Status needs to be written first before the header
                                                                    // https://github.com/uNetworking/uWebSockets/issues/1808

                                                                    auto status = std::to_string(static_cast<int>(responseObject.getProperty(rt2, "status").asNumber()));
                                                                    auto statusText = responseObject.getProperty(rt2, "statusText").asString(rt2).utf8(rt2);

                                                                    if(!statusText.empty()) {
                                                                      status += " " + statusText;
                                                                    }

                                                                    res->writeStatus(status);
                                                                  }
                                                                  // ----- Status -----

                                                                  // +++++ Write Header +++++
                                                                  {
                                                                    auto headers = responseObject.getProperty(rt2, "headers").asObject(rt2);
                                                                    const auto headerNames = headers.getPropertyNames(rt2);
                                                                    for(int i = 0; i < static_cast<int>(headerNames.length(rt2)); i++) {
                                                                      facebook::jsi::String headerNameJsString = headerNames.getValueAtIndex(rt2, i).asString(rt2);
                                                                      std::string headerValueStr = headers.getProperty(rt2, headerNameJsString).asString(rt2).utf8(rt2);

                                                                      res->writeHeader(std::move(headerNameJsString).utf8(rt2), std::move(headerValueStr));
                                                                    }
                                                                  }
                                                                  // ----- Write Header -----

                                                                  // +++++ Write Response +++++
                                                                  {
                                                                    auto body = responseObject.getProperty(rt2, "body");
                                                                    auto bodyType = responseObject.getProperty(rt2, "bodyType").asString(rt2);

                                                                    if(bodyType.utf8(rt2) == "text" && body.isString()) {
                                                                      res->end(body.asString(rt2).utf8(rt2));
                                                                    } else {
                                                                      res->end(R"({"message": "Internal server error"})");
                                                                    }
                                                                  }
                                                                  // ----- Write Response -----
                                                                }

                                                                return facebook::jsi::Value::undefined();
                                                              })); // cb.call
    }

  }); // asyncCallback.call
}

} // namespace

ReactNativeUwsModule::ReactNativeUwsModule(std::shared_ptr<CallInvoker> jsInvoker)
  : NativeReactNativeUwsCxxSpec(std::move(jsInvoker)) {}

void ReactNativeUwsModule::httpCreateServer(facebook::jsi::Runtime &rt,
                                             facebook::jsi::String serverID,
                                             facebook::jsi::Object options) {
  auto server = std::make_shared<react_native_echo::Server>(serverID.utf8(rt),
                                          react_native_echo::ServerOptions::fromJsiObject(rt, std::move(options)));

  servers.emplace_back(std::move(server));
}

void ReactNativeUwsModule::httpServerListen(facebook::jsi::Runtime &rt,
                                             facebook::jsi::String serverID,
                                             double port,
                                             facebook::jsi::Function onListener,
                                             facebook::jsi::Function onListenerFailure) {
  std::string strServerID = serverID.utf8(rt);
  auto intPort = static_cast<int>(port);
  auto serverPtr = getServerByID(strServerID);

  auto onListenerAsyncCallback = AsyncCallback(rt, std::move(onListener), this->jsInvoker_);
  auto onListenerFailureAsyncCallback = AsyncCallback(rt, std::move(onListenerFailure), this->jsInvoker_);

  if(serverPtr) {
    serverPtr->listen(intPort, [onListenerAsyncCallback_ = std::move(onListenerAsyncCallback)]() {
      onListenerAsyncCallback_.call();
    }, [onListenerFailureAsyncCallback_ = std::move(onListenerFailureAsyncCallback)]() {
      onListenerFailureAsyncCallback_.call();
    });
  }
}

void ReactNativeUwsModule::httpServerClose(facebook::jsi::Runtime &rt,
                                            facebook::jsi::String serverID) {
  std::string strServerID = serverID.utf8(rt);
  auto serverPtr = getServerByID(strServerID);

  if(serverPtr) {
    serverPtr->close([&strServerID]() {
      auto it = std::find_if(servers.begin(), servers.end(), [strServerID](auto &item) {
        return strServerID == item->id;
      });
      if(it != servers.end()) {
        servers.erase(it);
      }
    });
  }
}

void ReactNativeUwsModule::httpServerRouteAny(facebook::jsi::Runtime &rt,
                                               facebook::jsi::String serverID,
                                               facebook::jsi::String path,
                                               facebook::jsi::Function callback) {
  std::string strServerID = serverID.utf8(rt);
  auto serverPtr = getServerByID(strServerID);

  auto asyncCallback = AsyncCallback(rt, std::move(callback), this->jsInvoker_);

  if(serverPtr) {
    serverPtr->routeAny(path.utf8(rt), [asyncCallback_ = std::move(asyncCallback)](uWS::HttpResponse<false> *res, uWS::HttpRequest *req) {
      commonRouteHandler(asyncCallback_,
                         res,
                         req);
    });
  }
}

void ReactNativeUwsModule::httpServerRouteGet(facebook::jsi::Runtime &rt,
                                               facebook::jsi::String serverID,
                                               facebook::jsi::String path,
                                               facebook::jsi::Function callback) {
  // TODO
}

void ReactNativeUwsModule::httpServerRoutePost(facebook::jsi::Runtime &rt,
                                                facebook::jsi::String serverID,
                                                facebook::jsi::String path,
                                                facebook::jsi::Function callback) {
  // TODO
}

void ReactNativeUwsModule::httpServerRoutePut(facebook::jsi::Runtime &rt,
                                               facebook::jsi::String serverID,
                                               facebook::jsi::String path,
                                               facebook::jsi::Function callback) {
  // TODO
}

void ReactNativeUwsModule::httpServerRouteDelete(facebook::jsi::Runtime &rt,
                                                  facebook::jsi::String serverID,
                                                  facebook::jsi::String path,
                                                  facebook::jsi::Function callback) {
  // TODO
}

}