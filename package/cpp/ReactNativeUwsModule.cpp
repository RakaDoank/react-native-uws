#include "ReactNativeUwsModule.h"
#include <memory>
#include <vector>
#include <jsi/jsi.h>
#include "app/AppHost.h"
#include "app/TemplatedAppObject.h"
#include "jsi/Buffer.h"
#include "uWebSockets/HttpContextData.h"
#include "uWebSockets/HttpResponse.h"
#include "uWebSockets/Multipart.h"

namespace facebook::react {

namespace {

std::vector<std::shared_ptr<react_native_uws::AppHost>> appHosts;

} // namespace

ReactNativeUwsModule::ReactNativeUwsModule(std::shared_ptr<CallInvoker> jsInvoker)
  : NativeReactNativeUwsCxxSpec(std::move(jsInvoker)) {}

react_native_uws::TemplatedAppObject ReactNativeUwsModule::App(facebook::jsi::Runtime &rt,
                                                               std::optional<facebook::jsi::Object> appOptions) {
  auto assignedIndex = appHosts.size();

  auto appHost = std::make_shared<react_native_uws::AppHost>([assignedIndex]() {
    if(assignedIndex < appHosts.size()) {
      appHosts.erase(appHosts.begin() + assignedIndex);
    }
  });
  appHosts.emplace_back(appHost);

  /// Be aware!
  /// We pass increased number of the assignedIndex to the appHost->getTemplatedAppObject().
  /// When users use the listen method (app.listen)
  /// they got 1..n number, not zero at first.

  /// This is intentional to get similar behaviour from uWebSockets.js.
  /// And this is also for future usage kind of thing.
  /// When we actually start to pass the `us_socket_t` or `us_listen_socket_t` to the listen method in JS,
  /// we don't break users code space and they don't need to do anything at all.

  /// User often uses the app.listen like this below

  /// app.listen(5000, token => {
  ///   if(token) {
  ///     console.log("Listening to port 5000");
  ///   } else {
  ///     console.log("Failed to listen to port 5000");
  ///   }
  /// });
  return appHost->getTemplatedAppObject(rt, this->jsInvoker_, assignedIndex + 1);
}

/// Ported of uWebSockets.js `getParts`
/// See https://github.com/uNetworking/uWebSockets.js/blob/master/src/addon.cpp#L45
facebook::jsi::Object ReactNativeUwsModule::getParts(facebook::jsi::Runtime &rt,
                                                     facebook::jsi::Object body,
                                                     facebook::jsi::String contentType) {
  /// Sucks
  /// Codegen maps `unknown` type to JSI Object.
  /// We can't return Array | Undefined
  /// If an empty object, it indicates undefined.
  /// Empty object needs to be remapped to undefined.
  /// See /react-native-uws/package/src/modules/getParts.ts

  if(!body.isArrayBuffer(rt)) {
    /// undefined
    return facebook::jsi::Object(rt);
  }

  auto arrayBuffer = body.getArrayBuffer(rt);

  auto mp = uWS::MultipartParser(contentType.utf8(rt));
  if(mp.isValid()) {
    mp.setBody(std::string_view(reinterpret_cast<char *>(arrayBuffer.data(rt)),
                                arrayBuffer.size(rt)));

    std::pair<std::string_view, std::string_view> headers[10];

    auto parts = std::vector<facebook::jsi::Object>();

    while(true) {
      std::optional<std::string_view> optionalPart = mp.getNextPart(headers);
      if(!optionalPart.has_value()) {
        break;
      }

      std::string_view part = optionalPart.value();
      auto partObj = facebook::jsi::Object(rt);

      partObj.setProperty(rt,
                          "data",
                          facebook::jsi::ArrayBuffer(rt, std::make_shared<react_native_uws::StringViewMutableBuffer>(part)));

      for(int i = 0; headers[i].first.length(); i++) {
        if(headers[i].first == "content-type") {
          partObj.setProperty(rt,
                              "type",
                              std::string(headers[i].second));
        } else if(headers[i].first == "content-disposition") {
          /* Parse the parameters */
          uWS::ParameterParser pp(headers[i].second);
          while(true) {
            auto [key, value] = pp.getKeyValue();
            if(!key.length()) {
              break;
            }

            if(key == "name" || key == "filename") {
              partObj.setProperty(rt,
                                  facebook::jsi::PropNameID::forUtf8(rt, std::string(key)),
                                  std::string(value));
            }
          }
        }
      }

      parts.emplace_back(std::move(partObj));
    }

    auto multipartField = facebook::jsi::Array(rt, parts.size());

    std::for_each(parts.begin(), parts.end(), [&rt, &multipartField, i = 0](auto &part) mutable {
      multipartField.setValueAtIndex(rt, i, part);
      i++;
    });

    return multipartField;
  }

  /// undefined
  return facebook::jsi::Object(rt);
}

/// It's better to have different name than this.
/// It doesn't use us_listen_socket_close from uSockets directly in this function
void ReactNativeUwsModule::_us_listen_socket_close(facebook::jsi::Runtime &rt,
                                                  double id
                                                  /*facebook::jsi::Object listenSocket*/) {
  if(id < 1) {
    return;
  }

  /// Decreased one number. See notes above at ReactNativeUwsModule::App
  auto assignedIndex = id - 1;

  auto appHost = appHosts.at(assignedIndex);

  if(!appHost) {
    return;
  }

  appHost->appRunner.close([assignedIndex]() {
    if(assignedIndex < appHosts.size()) {
      appHosts.erase(appHosts.begin() + assignedIndex);
    }
  });
}

/// It's better to have different name than this.
/// JS side doesn't pass the actual us_socket_t or us_listen_socket_t to this function.
double ReactNativeUwsModule::_us_socket_local_port(facebook::jsi::Runtime &rt,
                                                double id
                                                /*facebook::jsi::Object socket*/) {
  if(id < 1) {
    return -1;
  }

  /// Decreased one number. See notes above at ReactNativeUwsModule::App
  auto assignedIndex = id - 1;

  auto appHost = appHosts.at(assignedIndex);

  if(!appHost) {
    return -1;
  }

  auto *listenSocket = appHost->appRunner.listenSocket;

  if(!listenSocket) {
    return -1;
  }

  return us_socket_local_port(0, (struct us_socket_t *)listenSocket);
}

} // namespace facebook::react
