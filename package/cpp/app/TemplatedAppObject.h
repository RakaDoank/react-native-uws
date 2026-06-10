#pragma once

#include <ReactCommon/CallInvoker.h>
#include <algorithm>
#include <jsi/jsi.h>
#include <utility>
#include <uWebSockets/App.h>
#include "AppRunner.h"
#include "HttpRequestObject.h"
#include "HttpResponseObject.h"

namespace react_native_uws {

class TemplatedAppObject : public facebook::jsi::Object {

private:
  enum UwsRouteMethod {
    ANY,
    DEL,
    GET,
    HEAD,
    OPTIONS,
    PATCH,
    POST,
    PUT,
    TRACE,
  };

  static void routeRegister(std::shared_ptr<AppRunner> &appRunner,
                            UwsRouteMethod &&method,
                            facebook::jsi::Runtime &rt,
                            std::shared_ptr<facebook::react::CallInvoker> &jsInvoker,
                            const facebook::jsi::Value *arguments) {
    if(appRunner) {
      auto pattern = arguments[0].asString(rt).utf8(rt);
      auto callback = arguments[1].asObject(rt).asFunction(rt);

      /// We can't make a sync call from arbitrary thread
      /// where the uWebSockets runner lives

//      std::function<void (uWS::HttpResponse<false> *res, uWS::HttpRequest *req)> uwsRouteHandler = [&rt, syncCallback = std::make_shared<facebook::react::SyncCallback<void (facebook::jsi::Value, facebook::jsi::Value)>>(rt, std::move(callback), jsInvoker)](auto *res, auto *req) {
//        auto httpResponseObject = std::make_shared<HttpResponseObject>(rt, res);
//        auto httpRequestObject = std::make_shared<HttpRequestObject>(rt, req);
//
//        syncCallback->call(httpResponseObject.get(), httpRequestObject.get());
//      };

      std::function<void (uWS::HttpResponse<false> *res, uWS::HttpRequest *req)> uwsRouteHandler = [&jsInvoker, asyncCallback = facebook::react::AsyncCallback(rt, std::move(callback), jsInvoker)](auto *res, auto *req) {
        asyncCallback.call([&jsInvoker, &res, &req](facebook::jsi::Runtime &rt_1, facebook::jsi::Function &cb) {
          auto httpResponseObject = std::make_shared<HttpResponseObject>(rt_1, res, jsInvoker);
          auto httpRequestObject = std::make_shared<HttpRequestObject>(rt_1, req);

          cb.call(rt_1,
                  *httpResponseObject,
                  *httpRequestObject);
        });

//        auto aborted = std::make_shared<bool>(false);
        /**
         * I don't know why without this,
         * uWebSockets can't wait the route handler to be finished.
         * I thought `onAborted` is just a callback or event listener.
         */
        res->onAborted([]() {});
      };

      if(method == UwsRouteMethod::ANY) {
        appRunner->app.any(pattern, std::move(uwsRouteHandler));
      } else if(method == UwsRouteMethod::DEL) {
        appRunner->app.del(pattern, std::move(uwsRouteHandler));
      } else if(method == UwsRouteMethod::GET) {
        appRunner->app.get(pattern, std::move(uwsRouteHandler));
      } else if(method == UwsRouteMethod::HEAD) {
        appRunner->app.head(pattern, std::move(uwsRouteHandler));
      } else if(method == UwsRouteMethod::OPTIONS) {
        appRunner->app.options(pattern, std::move(uwsRouteHandler));
      } else if(method == UwsRouteMethod::PATCH) {
        appRunner->app.patch(pattern, std::move(uwsRouteHandler));
      } else if(method == UwsRouteMethod::POST) {
        appRunner->app.post(pattern, std::move(uwsRouteHandler));
      } else if(method == UwsRouteMethod::PUT) {
        appRunner->app.put(pattern, std::move(uwsRouteHandler));
      } else {
        appRunner->app.trace(pattern, std::move(uwsRouteHandler));
      }
    }
  }

public:
  TemplatedAppObject(std::shared_ptr<AppRunner> &appRunner,
                     facebook::jsi::Runtime &rt,
                     std::shared_ptr<facebook::react::CallInvoker> &jsInvoker,
                     const std::function<void ()> &closeHandler) : facebook::jsi::Object(rt) {
    this->setProperty(rt,
                      "close",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "close"),
                                                                      0,
                                                                      [closeHandler](facebook::jsi::Runtime &rt_1,
                                                                                     const facebook::jsi::Value &thisValue,
                                                                                     const facebook::jsi::Value *arguments,
                                                                                     size_t count) -> facebook::jsi::Value {
      closeHandler();
      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "domain",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "domain"),
                                                                      0,
                                                                      [&appRunner](facebook::jsi::Runtime &rt_1,
                                                                                     const facebook::jsi::Value &thisValue,
                                                                                     const facebook::jsi::Value *arguments,
                                                                                     size_t count) -> facebook::jsi::Value {
      auto serverName = arguments[0].asString(rt_1).utf8(rt_1);
      appRunner->app.domain(serverName);

      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "filter",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "filter"),
                                                                      0,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                   const facebook::jsi::Value &thisValue,
                                                                                   const facebook::jsi::Value *arguments,
                                                                                   size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      appRunner->app.filter([&jsInvoker, asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker)](auto *res, int count) {
        asyncCallback.call([&jsInvoker, &res, count](facebook::jsi::Runtime &rt_2, facebook::jsi::Function &cb) {
          auto httpResponseObject = std::make_shared<react_native_uws::HttpResponseObject>(rt_2, res, jsInvoker);
          cb.call(rt_2,
                  *httpResponseObject,
                  count);
        });
      });

      return {rt_1, thisValue};
    }));

    /// UNTESTED
    /// It's copied and modified method from uWebSockets.js (for the Node.js).
    /// Yet, I don't even know what the method is for.
//    this->setProperty(rt,
//                      "getDescriptor",
//                      facebook::jsi::Function::createFromHostFunction(rt,
//                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getDescriptor"),
//                                                                      0,
//                                                                      [&appRunner](facebook::jsi::Runtime &rt_1,
//                                                                                   const facebook::jsi::Value &thisValue,
//                                                                                   const facebook::jsi::Value *arguments,
//                                                                                   size_t count) -> facebook::jsi::Value {
//      static_assert(sizeof(double) >= sizeof(appRunner));
//
//      uWS::App *app = &(appRunner->app);
//
//      double descriptor = 0;
//      memcpy(&descriptor, &app, sizeof(app));
//
//      return descriptor;
//    }));

    this->setProperty(rt,
                      "listen",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "listen"),
                                                                      4,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      std::optional<std::string> host = std::nullopt;
      double port = -1; // it could be unsigned short, but JSI provide it as double
      std::optional<int> options = std::nullopt;

      if(arguments[0].isString()) {
        host = arguments[0].asString(rt_1).utf8(rt_1);
        if(arguments[1].isNumber()) {
          port = arguments[1].asNumber();
        }
        if(arguments[2].isNumber()) {
          options = arguments[2].asNumber();
        }
      } else if(arguments[0].isNumber()) {
        port = arguments[0].asNumber();
        if(arguments[1].isNumber()) {
          options = arguments[1].asNumber();
        }
      }

      if(port < 0 || port > 65535) {
        throw facebook::jsi::JSError(rt_1, "Illegal port number expression");
      }

      auto callback = arguments[count - 1].asObject(rt_1).asFunction(rt_1);
      auto asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker);

      appRunner->listen(host, static_cast<int>(port), options, [asyncCallback_ = std::move(asyncCallback)](us_listen_socket_t *listenedSocket) {
        // TODO : pass the us_listen_socket_t here in facebook::jsi::Object
        asyncCallback_.call();
      });

      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "numSubscribers",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "numSubscribers"),
                                                                      1,
                                                                      [&appRunner](facebook::jsi::Runtime &rt_1,
                                                                                   const facebook::jsi::Value &thisValue,
                                                                                   const facebook::jsi::Value *arguments,
                                                                                   size_t count) -> facebook::jsi::Value {
      auto topic = arguments[0].asString(rt_1).utf8(rt_1);
      return static_cast<int>(appRunner->app.numSubscribers(std::string_view(topic)));
    }));

    this->setProperty(rt,
                      "publish",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "publish"),
                                                                      4,
                                                                      [&appRunner](facebook::jsi::Runtime &rt_1,
                                                                                   const facebook::jsi::Value &thisValue,
                                                                                   const facebook::jsi::Value *arguments,
                                                                                   size_t count) -> facebook::jsi::Value {
      if(!arguments ||
         !arguments[0].isString() ||
         !arguments[1].isString()) {
        return false;
      }

      auto topic = arguments[0].asString(rt_1).utf8(rt_1);
      auto message = arguments[1].asString(rt_1).utf8(rt_1);

      bool isBinary = false;
      if(arguments[2].isBool()) {
        isBinary = arguments[2].asBool();
      }

      bool compress = false;
      if(arguments[3].isBool()) {
        compress = arguments[3].asBool();
      }

      return appRunner->app.publish(std::string_view(topic),
                             std::string_view(message),
                             isBinary ? uWS::OpCode::BINARY : uWS::TEXT,
                             compress);
    }));

    // +++++ ROUTER +++++

    this->setProperty(rt,
                      "any",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "any"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::ANY,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "del",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "del"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::DEL,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "get",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "get"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::GET,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "head",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "head"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::HEAD,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "options",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "options"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::OPTIONS,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "patch",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "patch"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::PATCH,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "post",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "post"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::POST,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "put",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "put"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::PUT,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "trace",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "trace"),
                                                                      1,
                                                                      [&appRunner, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      TemplatedAppObject::routeRegister(appRunner,
                                        UwsRouteMethod::TRACE,
                                        rt_1,
                                        jsInvoker,
                                        arguments);
      return {rt_1, thisValue};
    }));

    // ----- ROUTER -----
  };

};

}
