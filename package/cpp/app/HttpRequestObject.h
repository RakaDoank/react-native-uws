#pragma once

#include <ReactCommon/CallInvoker.h>
#include <algorithm>
#include <cmath>
#include <jsi/jsi.h>
#include <uWebSockets/HttpContextData.h>

namespace react_native_uws {

class HttpRequestObject : public facebook::jsi::Object {

private:
  /**
   * This is just a hacky way to fix the "req->getParameter(0)" that doesn't work from JS call.
   * Consider this solution is temporary until we know how to fix `req->getParameter()` properly
   * inside of the JSI object.
   */
  std::vector<std::pair<std::string/*key*/, std::string_view>> parameters;

public:
  HttpRequestObject(facebook::jsi::Runtime &rt,
                    uWS::HttpRequest *req) : facebook::jsi::Object(rt) {

    this->setProperty(rt,
                      "forEach",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "forEach"),
                                                                      1,
                                                                      [req](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      std::for_each(req->begin(),
                    req->end(),
                    [&rt_1, callback_ = std::move(callback)](const auto &item) {
                      callback_.call(rt_1,
                                     facebook::jsi::String::createFromUtf8(rt_1, std::string(item.first)),
                                     facebook::jsi::String::createFromUtf8(rt_1, std::string(item.second)));
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
      auto lowerCasedHeader = arguments[0].asString(rt_1).utf8(rt_1);
      auto result = req->getHeader(std::string_view(lowerCasedHeader));
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
                                                                      [this](facebook::jsi::Runtime &rt_1,
                                                                            const facebook::jsi::Value &thisValue,
                                                                            const facebook::jsi::Value *arguments,
                                                                            size_t count) -> facebook::jsi::Value {
      if(!arguments) {
        return facebook::jsi::Value::undefined();
      }

      /// Doesn't work. I don't know why.
      /// req->getParameter() always returned string_view null data
//      std::string_view parameter;
//
//      if(arguments[0].isNumber()) {
//        // get by index
//        auto index = arguments[0].asNumber();
//        parameter = req->getParameter(static_cast<int>(std::floor(index)));
//      } else {
//        // get by name
//        auto paramNamed = arguments[0].asString(rt_1).utf8(rt_1);
//        parameter = req->getParameter(std::string_view(paramNamed));
//      }
//
//      if(parameter.data() == nullptr) {
//        return facebook::jsi::Value::undefined();
//      }

      /// This is temporary solution

      if(this->parameters.empty()) {
        return facebook::jsi::Value::undefined();
      }

      std::string_view it;

      if(arguments[0].isNumber()) {
        /// get by index
        /// this is much faster

        auto index = arguments[0].asNumber();
        if(index < this->parameters.size()) {
          it = this->parameters.at(static_cast<int>(index)).second;
        }
      } else {
        /// get by name
        /// Do we really need std::unordered_map for the faster lookup here?

        auto _it = std::find_if(this->parameters.begin(), this->parameters.end(), [key = arguments[0].asString(rt_1).utf8(rt_1)](auto &item) -> bool {
          return item.first == key;
        });
        if(_it != this->parameters.end()) {
          it = _it->second;
        }
      }

      if(it.data() == nullptr) {
        return facebook::jsi::Value::undefined();
      }

      return facebook::jsi::String::createFromAscii(rt_1,
                                                    std::string(it));
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

  } // HttpRequestObject

  void addParameter(std::string &&key,
                    std::string_view value) {
    this->parameters.emplace_back(key, value);
  }

};

}
