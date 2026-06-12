#pragma once

#include <android/log.h>
#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/SchedulerPriority.h>
#include <algorithm>
#include <jsi/jsi.h>
#include <mutex>
#include <sstream>
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

      /// This is not working if the route handler is
      /// doing long operation or async function.
      /// It may throw an Error
      /// `terminating due to uncaught exception of type facebook::jsi::JSError: Exception in HostFunction: Unable to retrieve jni environment. Is the thread attached?`
//      std::function<void (uWS::HttpResponse<false> *res, uWS::HttpRequest *req)> uwsRouteHandler = [&rt, &jsInvoker, cb = std::make_shared<facebook::jsi::Function>(std::move(callback))](uWS::HttpResponse<false> *res, uWS::HttpRequest *req) {
//        auto httpResponseObject = std::make_shared<HttpResponseObject>(rt, res, jsInvoker);
//        auto httpRequestObject = std::make_shared<HttpRequestObject>(rt, req);
//
//        cb->call(rt,
//                 *httpResponseObject,
//                 *httpRequestObject);
//      };

      /// Same, it also doesn't work with SyncCallback.
      /// We can't make a sync call to JS
      /// from arbitrary thread where the uWebSockets runner lives,
      /// even the route handler is not doing anything long opt
//      std::function<void (uWS::HttpResponse<false> *res, uWS::HttpRequest *req)> uwsRouteHandler = [&rt, syncCallback = std::make_shared<facebook::react::SyncCallback<void (facebook::jsi::Value, facebook::jsi::Value)>>(rt, std::move(callback), jsInvoker)](auto *res, auto *req) {
//        auto httpResponseObject = std::make_shared<HttpResponseObject>(rt, res);
//        auto httpRequestObject = std::make_shared<HttpRequestObject>(rt, req);
//
//        syncCallback->call(httpResponseObject.get(), httpRequestObject.get());
//      };

      bool disableBodyRead = false;
      unsigned long maxBodySize = 0;

      /// See /react-native-uws/package/src/types/HttpRouterOptions.ts
      if(arguments[2].isObject()) {
        auto httpRouterOptions = arguments[2].asObject(rt);

        {
          std::string key = "disableBodyRead";
          if(httpRouterOptions.getProperty(rt, key.c_str()).isBool()) {
            disableBodyRead = httpRouterOptions.getProperty(rt, key.c_str()).asBool();
          }
        }

        {
          std::string key = "maxBodySize";
          if(httpRouterOptions.getProperty(rt, key.c_str()).isNumber()) {
            auto _maxBodySize = httpRouterOptions.getProperty(rt, key.c_str()).asNumber();
            if(_maxBodySize < 0) {
              throw facebook::jsi::JSError(rt, "Illegal maxBodySize number expression");
            }
            maxBodySize = static_cast<unsigned long>(_maxBodySize);
          }
        }
      }

      std::function<void (uWS::HttpResponse<false> *res, uWS::HttpRequest *req)> uwsRouteHandler = [pattern, disableBodyRead, maxBodySize, &rt, &jsInvoker, asyncCallback = facebook::react::AsyncCallback(rt, std::move(callback), jsInvoker)](uWS::HttpResponse<false> *res, uWS::HttpRequest *req) {
        auto httpResponseObject = std::make_shared<HttpResponseObject>(rt, res, jsInvoker);
        auto httpRequestObject = std::make_shared<HttpRequestObject>(rt, req);

        asyncCallback.callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority, [httpResponseObject, httpRequestObject](facebook::jsi::Runtime &rt_1, facebook::jsi::Function &cb) {
          cb.call(rt_1,
                  *httpResponseObject,
                  *httpRequestObject);
        });

        /// We have to make JS call asynchronously because the uWebSockets app run at different thread.
        /// See the `react_native_uws::AppRunner`, and `facebook::react::AsyncCallback`.
        /// So this predefined `onAborted` assignment below is to tell that
        /// uWebSockets has to wait until JS call finished with the `res.end() or res.tryEnd()`.
        ///
        /// Stated from uWebSockets
        /// `Returning from a request handler without responding or attaching an onAborted handler is ill-use`
        ///
        /// I thought `onAborted` is just a callback or event listener.
        res->onAborted([httpResponseObject]() {
          httpResponseObject->jsCall_onAborted();
        });

        /// Sadly, we can't do late assignment to the onDataV2 and onData.
        /// uWebSockets will do nothing to our handler if we assign the lambda so late.
        /// So we have to predefined onDataV2 handler here, and save the chunk.
        if(!disableBodyRead) {
          res->onDataV2([httpResponseObject, maxBodySize](auto chunk, auto maxRemainingBodyLength) {
            if(maxBodySize > 0 && chunk.size() > maxBodySize) {
              return;
            }
            httpResponseObject->jsCall_onDataV2(chunk, maxRemainingBodyLength);
          });
        }

        /// This is a hacky way and probably temporary.
        /// The "req->getParameter" does not working from JS call.
        /// It always returned null data of string_view both by index and named.
        {
          std::stringstream ss = std::stringstream(pattern);
          std::string token;

          while(std::getline(ss, token, '/')) {
            if(token[0] == ':') {
              auto key = token.substr(1);
              auto value = req->getParameter(key);
              if(value.data() != nullptr) {
                httpRequestObject->addParameter(std::move(key), value);
              }
            }
          }
        }
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
      double port                     = -1; // it could be unsigned short, but JSI provide it as double
      std::optional<int> options      = std::nullopt;

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
