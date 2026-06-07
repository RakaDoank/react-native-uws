#include "RequestHostObject.h"
#include <algorithm>
#include <jsi/jsi.h>
#include <react/bridging/Promise.h>
#include <string_view>
#include <vector>
#include "uWebSockets/HttpResponse.h"

namespace react_native_uws {

// +++++ PRIVATES +++++
facebook::jsi::Object RequestHostObject::headers(facebook::jsi::Runtime &rt) {
  auto headers = facebook::jsi::Object(rt);
  for(auto [key, val] : *(this->httpRequest)) {
    headers.setProperty(rt,
                        std::string(key).c_str(),
                        std::string(val));
  }
  return std::move(headers);
}

facebook::jsi::String RequestHostObject::method(facebook::jsi::Runtime &rt) {
  auto method = facebook::jsi::String::createFromUtf8(rt, std::string(this->httpRequest->getCaseSensitiveMethod()));
  return method;
}

facebook::jsi::Object RequestHostObject::url(facebook::jsi::Runtime &rt) {
  auto url = facebook::jsi::Object(rt);
  url.setProperty(rt,
                  "pathname",
                  std::string(this->httpRequest->getUrl()));
  // TODO search
  // url.setProperty(rt, "search", ...);
  return url;
}

facebook::jsi::Function RequestHostObject::text(facebook::jsi::Runtime &rt) {
  return facebook::jsi::Function::createFromHostFunction(rt,
                                                         facebook::jsi::PropNameID::forUtf8(rt, "text"),
                                                         1,
                                                         [this](facebook::jsi::Runtime &rt_1,
                                                                 const facebook::jsi::Value &thisValue,
                                                                 const facebook::jsi::Value *arguments,
                                                                 size_t count) -> facebook::jsi::Value {
    facebook::jsi::Function callback = arguments[0].getObject(rt_1).asFunction(rt_1);

    this->httpResponse->onDataV2([&rt_1, callback_ = std::move(callback), buffer = std::unique_ptr<std::string>()](std::string_view chunk, uint64_t maxRemainingBodyLength) mutable {
      if(maxRemainingBodyLength == 0) { // finish
        if(buffer) {
          callback_.call(rt_1, facebook::jsi::String::createFromUtf8(rt_1, *buffer));
        } else {
          callback_.call(rt_1, facebook::jsi::String::createFromUtf8(rt_1, std::string(chunk)));
        }
      } else {
        if(!buffer) {
          buffer = std::make_unique<std::string>(chunk);
        } else {
          buffer->append(chunk);
        }
      }
    });

    return facebook::jsi::Value::undefined();
  });
}
// ----- PRIVATES -----

facebook::jsi::Value RequestHostObject::get(facebook::jsi::Runtime &rt,
                                            const facebook::jsi::PropNameID &propNameID) {
  auto name = std::find_if(this->keys.begin(), this->keys.end(), [&rt, &propNameID](std::string &it) {
    return it == propNameID.utf8(rt);
  });

  if(name == this->keys.end()) {
    return facebook::jsi::Value::undefined();
  }

  if(*name == "headers") {
    return this->headers(rt);
  }

  if(*name == "method") {
    return this->method(rt);
  }

  if(*name == "url") {
    return this->url(rt);
  }

  return facebook::jsi::Value::undefined();
}

void RequestHostObject::set(facebook::jsi::Runtime &rt,
         const facebook::jsi::PropNameID &propNameID,
         const facebook::jsi::Value &value) {
  throw std::runtime_error("Request object is read-only");
}

std::vector<facebook::jsi::PropNameID> RequestHostObject::getPropertyNames(
        facebook::jsi::Runtime &rt) {
  std::vector<facebook::jsi::PropNameID> names = {};

  std::for_each(this->keys.begin(), this->keys.end(), [&names, &rt](auto &it) {
    names.emplace_back(facebook::jsi::PropNameID::forUtf8(rt, it));
  });

  return names;
}

}