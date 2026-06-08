#pragma once

#include <jsi/jsi.h>
#include <utility>
#include <uWebSockets/HttpResponse.h>

namespace react_native_uws {

class HttpResponseObject : public facebook::jsi::Object {

public:
  HttpResponseObject(facebook::jsi::Runtime &rt,
                     uWS::HttpResponse<false> *res) : facebook::jsi::Object(rt) {
    this->setProperty(rt, "end", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "end"),
                                                                                 1,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                         const facebook::jsi::Value &thisValue,
                                                                                         const facebook::jsi::Value *arguments,
                                                                                         size_t count) -> facebook::jsi::Value {
      auto body = arguments[0].asString(rt_1).utf8(rt_1);
      res->end(std::move(body));

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "writeHeader", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                         facebook::jsi::PropNameID::forUtf8(rt, "writeHeader"),
                                                                                         2,
                                                                                         [res](facebook::jsi::Runtime &rt_1,
                                                                                                const facebook::jsi::Value &thisValue,
                                                                                                const facebook::jsi::Value *arguments,
                                                                                                size_t count) -> facebook::jsi::Value {
      auto headerKey = arguments[0].asString(rt_1).utf8(rt_1);
      auto headerVal = arguments[1].asString(rt_1).utf8(rt_1);
      res->writeHeader(std::move(headerKey), std::move(headerVal));

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "writeStatus", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "writeStatus"),
                                                                                 1,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                        const facebook::jsi::Value &thisValue,
                                                                                        const facebook::jsi::Value *arguments,
                                                                                        size_t count) -> facebook::jsi::Value {
      auto status = arguments[0].asString(rt_1).utf8(rt_1);
      res->writeStatus(std::move(status));

      return {rt_1, thisValue};
    }));
  } // HttpResponseObject

};

}