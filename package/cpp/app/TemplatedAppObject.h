#pragma once

#include <android/log.h>
#include <jsi/jsi.h>
#include <utility>
#include <uWebSockets/App.h>
#include "AppRunner.h"

namespace react_native_uws {

class TemplatedAppObject : public facebook::jsi::Object {

public:
  TemplatedAppObject(std::shared_ptr<AppRunner> &appRunner, facebook::jsi::Runtime &rt) : facebook::jsi::Object(rt) {

    this->setProperty(rt, "counter", facebook::jsi::Value(rt, 2));

    this->setProperty(rt, "listen", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                    facebook::jsi::PropNameID::forUtf8(rt, "listen"),
                                                                                    1,
                                                                                    [appRunner](facebook::jsi::Runtime &rt_1, const facebook::jsi::Value &thisValue, const facebook::jsi::Value *arguments, size_t count) mutable -> facebook::jsi::Value {
      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "get", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "get"),
                                                                                 1,
                                                                                 [appRunner](facebook::jsi::Runtime &rt_1, const facebook::jsi::Value &thisValue, const facebook::jsi::Value *arguments, size_t count) mutable -> facebook::jsi::Value {
      appRunner->app.get("/hola/:name", [](auto *res, auto *req) {
        res->end("HOLA WORLD");
      });

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "close", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                    facebook::jsi::PropNameID::forUtf8(rt, "close"),
                                                                                    0,
                                                                                    [appRunner](facebook::jsi::Runtime &rt_1, const facebook::jsi::Value &thisValue, const facebook::jsi::Value *arguments, size_t count) mutable -> facebook::jsi::Value {
      return {rt_1, thisValue};
    }));
  };

};

}
