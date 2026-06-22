#pragma once

#include <ReactCommon/CallInvoker.h>
#include <algorithm>
#include <cmath>
#include <jsi/jsi.h>
#include "RecognizedString.h"
#include "uWebSockets/HttpContextData.h"

namespace uws_react_native {

class HttpRequestObject : public facebook::jsi::Object {

public:
  HttpRequestObject(facebook::jsi::Runtime &rt,
                    uWS::HttpRequest *pReq,
                    std::shared_ptr<facebook::react::CallInvoker> &jsInvoker) : facebook::jsi::Object(rt) {

    /// Without shared pointer
    /// uWS::HttpRequest returns weird behaviour for some methods like
    /// getHeader, getUrl, and getParameter
    auto req = std::make_shared<uWS::HttpRequest>(*pReq);

    this->setProperty(rt,
                      "forEach",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "forEach"),
                                                                      1,
                                                                      [req, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      std::for_each(req->begin(),
                    req->end(),
                    [asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker)](const auto &item) {
        asyncCallback.call([item_ = std::move(item)](facebook::jsi::Runtime &rt_2, facebook::jsi::Function &cb) {
          cb.call(rt_2,
                  facebook::jsi::String::createFromUtf8(rt_2, std::string(item_.first)),
                  facebook::jsi::String::createFromUtf8(rt_2, std::string(item_.second)));
        });
      });
      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "getCaseSensitiveMethod",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getCaseSensitiveMethod"),
                                                                      0,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      return facebook::jsi::String::createFromUtf8(rt_1,
                                                   std::string(req->getCaseSensitiveMethod()));
    }));

    this->setProperty(rt,
                      "getHeader",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getHeader"),
                                                                      1,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      auto lowerCasedHeader = RecognizedString(rt_1, arguments[0]).getStringView();
      auto result = req->getHeader(lowerCasedHeader);
      return facebook::jsi::String::createFromUtf8(rt_1,
                                                   std::string(result));
    }));

    this->setProperty(rt,
                      "getMethod",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getMethod"),
                                                                      0,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      return facebook::jsi::String::createFromUtf8(rt_1,
                                                   std::string(req->getMethod()));
    }));

    this->setProperty(rt,
                      "getParameter",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getParameter"),
                                                                      1,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      if(!arguments) {
        return facebook::jsi::Value::undefined();
      }

      std::string_view parameter;

      if(arguments[0].isNumber()) {
        // get by index
        auto index = arguments[0].asNumber();
        parameter = req->getParameter(static_cast<int>(std::floor(index)));
      } else {
        // get by name
        auto paramNamed = arguments[0].asString(rt_1).utf8(rt_1);
        parameter = req->getParameter(std::string_view(paramNamed));
      }

      if(parameter.data() == nullptr) {
        return facebook::jsi::Value::undefined();
      }
      return facebook::jsi::String::createFromAscii(rt_1,
                                                    std::string(parameter));
    }));

    this->setProperty(rt,
                      "getQuery",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getQuery"),
                                                                      1,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      std::string_view query;

      if(!arguments || arguments[0].isUndefined()) {
        query = req->getQuery();
      } else if(arguments[0].isString()) {
        query = req->getQuery(arguments[0].asString(rt_1).utf8(rt_1));
      }

      if(query.data() == nullptr) {
        return facebook::jsi::Value::undefined();
      }

      return facebook::jsi::String::createFromAscii(rt_1,
                                                   std::string(query));
    }));

    this->setProperty(rt,
                      "getUrl",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getUrl"),
                                                                      0,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      return facebook::jsi::String::createFromUtf8(rt_1,
                                                   std::string(req->getUrl()));
    }));

    this->setProperty(rt,
                      "setYield",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "setYield"),
                                                                      1,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      req->setYield(arguments[0].asBool());
      return {rt_1, thisValue};
    }));

    req.reset();

  } // HttpRequestObject

};

}
