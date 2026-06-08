#pragma once

#include <algorithm>
#include <ReactCommon/CallInvoker.h>
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
      auto asyncCallback = facebook::react::AsyncCallback(rt, std::move(callback), jsInvoker);

      std::function<void (uWS::HttpResponse<false> *res, uWS::HttpRequest *req)> uwsRouteHandler = [asyncCallback_ = std::move(asyncCallback), &jsInvoker](auto *res, auto *req) {
        auto aborted = std::make_shared<bool>(false);
        /**
         * I don't know why without this,
         * uWebSockets can't wait the route handler to be finished.
         * I thought `onAborted` is just a callback or event listener.
         */
        res->onAborted([aborted]() {
          *aborted = true;
        });

        asyncCallback_.call([&res, &req, &jsInvoker](facebook::jsi::Runtime &rt_1, facebook::jsi::Function &cb) {
          auto httpResponseObject = std::make_shared<HttpResponseObject>(rt_1, res);
          auto httpRequestObject = std::make_shared<HttpRequestObject>(rt_1, req, jsInvoker);

          cb.call(rt_1,
                  *httpResponseObject,
                  *httpRequestObject);
        });
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
                     std::shared_ptr<facebook::react::CallInvoker> &jsInvoker) : facebook::jsi::Object(rt) {

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
      double port = -1; // it could be unsigned, but JSI provide it as double
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

    this->setProperty(rt,
                      "close",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "close"),
                                                                      0,
                                                                      [](facebook::jsi::Runtime &rt_1,
                                                                              const facebook::jsi::Value &thisValue,
                                                                              const facebook::jsi::Value *arguments,
                                                                              size_t count) -> facebook::jsi::Value {
      return {rt_1, thisValue};
    }));
  };

};

}
