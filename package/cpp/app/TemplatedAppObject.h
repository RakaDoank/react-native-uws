#pragma once

#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/SchedulerPriority.h>
#include <jsi/jsi.h>
#include "AppRunner.h"
#include "HttpRequestObject.h"
#include "HttpResponseObject.h"
#include "HttpResponseObjectProvider.h"
#include "RecognizedString.h"
#include "uWebSockets/App.h"

namespace uws_react_native {

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

  static void routeRegister(AppRunner &appRunner,
                            UwsRouteMethod &&method,
                            facebook::jsi::Runtime &rt,
                            std::shared_ptr<facebook::react::CallInvoker> &jsInvoker,
                            const facebook::jsi::Value *arguments) {
    auto pattern = RecognizedString(rt, arguments[0]).getString();
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

    /// See /uws-react-native/package/src/types/HttpRouterOptions.ts
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

    std::function<void (uWS::HttpResponse<false> *res, uWS::HttpRequest *req)> uwsRouteHandler = [&jsInvoker, disableBodyRead, maxBodySize, asyncCallback = facebook::react::AsyncCallback(rt, std::move(callback), jsInvoker)](uWS::HttpResponse<false> *res, uWS::HttpRequest *req) {
//      auto httpResponseObject = std::make_shared<HttpResponseObject>(rt,
//                                                                     res,
//                                                                     jsInvoker);
//
//      auto httpRequestObject = std::make_shared<HttpRequestObject>(rt, req);

      auto httpResponseObjectProvider = std::make_shared<HttpResponseObjectProvider>(res);
      auto sharedRequest = std::make_shared<uWS::HttpRequest>(*req);

      asyncCallback.callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority, [httpResponseObjectProvider, sharedRequest, &jsInvoker](facebook::jsi::Runtime &rt_1, facebook::jsi::Function &cb) {
        if(httpResponseObjectProvider && sharedRequest) {
          cb.call(rt_1,
                  HttpResponseObject(rt_1, httpResponseObjectProvider, jsInvoker),
                  HttpRequestObject(rt_1, sharedRequest));
        }
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
      res->onAborted([httpResponseObjectProvider]() {
        if(httpResponseObjectProvider->dataAbort.callback) {
          httpResponseObjectProvider->dataAbort.callback->call(facebook::jsi::Value::undefined());
        }
      });

      /// Sadly, we can't do late assignment to the onDataV2 and onData.
      /// uWebSockets will do nothing to our handler if we assign the lambda so late.
      /// So we have to predefined onDataV2 handler here, and save the chunk.
      if(!disableBodyRead) {
        res->onDataV2([httpResponseObjectProvider, maxBodySize](auto chunk, auto maxRemainingBodyLength) {
          if(httpResponseObjectProvider->dataBody.isStopCollecting) {
            return;
          }

          if(maxBodySize > 0) {
            auto chunkSize = chunk.size();
            auto currentChunkSize = httpResponseObjectProvider->dataBody.buffer->size();

            /// First and possibly only chunk
            if(currentChunkSize == 0 && chunkSize > maxBodySize) {
              httpResponseObjectProvider->dataBody.isStopCollecting = true;
              /// set the first chunk
              httpResponseObjectProvider->insertBodyBuffer(chunk, maxRemainingBodyLength);
              httpResponseObjectProvider->invokeBodyCallback();
              /// Don't worry,
              /// JS call may late
              /// it will invokes the handler once when user pass the handler.
              return;
            }

            /// subsequent chunks overflow
            if(currentChunkSize > 0 && chunkSize > maxBodySize - currentChunkSize) {
              /// tell to JS that we already stop collecting chunk
              /// and invoke the JSI onData / onDataText / onDataV2 / onFullData / onFullDataText handler immediately
              httpResponseObjectProvider->dataBody.isStopCollecting = true;
              httpResponseObjectProvider->invokeBodyCallback();
              /// Don't worry,
              /// JS call may late
              /// it will invokes the handler once when user pass the handler.
              return;
            }
          }

          httpResponseObjectProvider->insertBodyBuffer(chunk, maxRemainingBodyLength);
          httpResponseObjectProvider->invokeBodyCallback();
        });
      }
    };

    if(method == UwsRouteMethod::ANY) {
      appRunner.app.any(pattern, std::move(uwsRouteHandler));
    } else if(method == UwsRouteMethod::DEL) {
      appRunner.app.del(pattern, std::move(uwsRouteHandler));
    } else if(method == UwsRouteMethod::GET) {
      appRunner.app.get(pattern, std::move(uwsRouteHandler));
    } else if(method == UwsRouteMethod::HEAD) {
      appRunner.app.head(pattern, std::move(uwsRouteHandler));
    } else if(method == UwsRouteMethod::OPTIONS) {
      appRunner.app.options(pattern, std::move(uwsRouteHandler));
    } else if(method == UwsRouteMethod::PATCH) {
      appRunner.app.patch(pattern, std::move(uwsRouteHandler));
    } else if(method == UwsRouteMethod::POST) {
      appRunner.app.post(pattern, std::move(uwsRouteHandler));
    } else if(method == UwsRouteMethod::PUT) {
      appRunner.app.put(pattern, std::move(uwsRouteHandler));
    } else {
      appRunner.app.trace(pattern, std::move(uwsRouteHandler));
    }
  }

public:
  TemplatedAppObject(AppRunner &appRunner,

                     facebook::jsi::Runtime &rt,

                     std::shared_ptr<facebook::react::CallInvoker> &jsInvoker,

                     /**
                      * Important! Please pass a number larger than 0.
                      * 
                      * This `id` number will be passed to the "listen" method handler.
                      * We don't actually pass the us_listen_socket_context_t there. The actual reason is
                      * we need to remove the AppHost instance which stored inside of vector.
                      * So, when user use the us_listen_socket_close JS module with this `id` passed,
                      * we don't really use the us_listen_socket_close from uSockets directly,
                      * instead, we just call the `close` method from AppRunner which a member of AppHost.
                      * See ReactNativeUwsModule.cpp.
                      *
                      * It's also used by us_socket_local_port JS module to get assigned port from uWebSockets app.
                      * We use the us_socket_local_port from uSockets tho, but we need to retrieve us_listen_socket_t
                      * from the AppRunner which a member of AppHost.
                      */
                     unsigned short id,

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
      appRunner.app.domain(serverName);

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

      appRunner.app.filter([&jsInvoker, asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker)](auto *res, int count) {
        auto httpResponseObjectProvider = std::make_shared<HttpResponseObjectProvider>(res);

        asyncCallback.call([&jsInvoker, count, httpResponseObjectProvider](facebook::jsi::Runtime &rt_2, facebook::jsi::Function &cb) {
          cb.call(rt_2,
                  HttpResponseObject(rt_2, httpResponseObjectProvider, jsInvoker),
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
//      uWS::App *app = &(appRunner.app);
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
                                                                      [&appRunner, &jsInvoker, id](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      std::optional<std::string> host = std::nullopt;
      double port                     = -1; // it could be unsigned short, but JSI provide it as double
      std::optional<int> options      = std::nullopt;

      if(arguments[0].isString()) {
        host = RecognizedString(rt_1, arguments[0]).getString();
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

      appRunner.listen(host, static_cast<int>(port), options, [id, asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker)](us_listen_socket_t *listenedSocket) {
        asyncCallback.call([id, listenedSocket](facebook::jsi::Runtime &rt_2, facebook::jsi::Function &cb) {
          cb.call(rt_2,
                  listenedSocket ? static_cast<double>(id) : 0);
        });
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
      auto topic = RecognizedString(rt_1, arguments[0]).getStringView();
      return static_cast<int>(appRunner.app.numSubscribers(topic));
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
      auto topic = RecognizedString(rt_1, arguments[0]).getStringView();
      auto message = RecognizedString(rt_1, arguments[1]).getStringView();

      bool isBinary = false;
      if(arguments[2].isBool()) {
        isBinary = arguments[2].asBool();
      }

      bool compress = false;
      if(arguments[3].isBool()) {
        compress = arguments[3].asBool();
      }

      return appRunner.app.publish(topic,
                             message,
                             isBinary ? uWS::OpCode::BINARY : uWS::TEXT,
                             compress);
    }));

    // +++++ ROUTER +++++

    this->setProperty(rt,
                      "any",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "any"),
                                                                      3,
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
                                                                      3,
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
                                                                      3,
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
                                                                      3,
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
                                                                      3,
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
                                                                      3,
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
                                                                      3,
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
                                                                      3,
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
                                                                      3,
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
