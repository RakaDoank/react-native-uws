#pragma once

#include <jsi/jsi.h>
#include <utility>
#include <uWebSockets/HttpResponse.h>

namespace react_native_uws {

class HttpResponseObject : public facebook::jsi::Object {

public:
  HttpResponseObject(facebook::jsi::Runtime &rt,
                     uWS::HttpResponse<false> *res) : facebook::jsi::Object(rt) {
    this->setProperty(rt, "close", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "close"),
                                                                                 1,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      res->close();
      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "cork", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "cork"),
                                                                                 1,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      res->cork([&rt_1, callback_ = std::move(callback)]() {
        callback_.call(rt_1);
      });

      return {rt_1, thisValue};
    }));

    /// TODO
    /// It is a custom method from uWebSockets.js.
    /// We need to figure it out how to implement this method in JSI way.
//     this->setProperty(rt, "collectBody", facebook::jsi::Function::createFromHostFunction(rt,
//                                                                                  facebook::jsi::PropNameID::forUtf8(rt, "collectBody"),
//                                                                                  1,
//                                                                                  [res](facebook::jsi::Runtime &rt_1,
//                                                                                                    const facebook::jsi::Value &thisValue,
//                                                                                                    const facebook::jsi::Value *arguments,
//                                                                                                    size_t count) -> facebook::jsi::Value {
//       auto maxSize = arguments[0].asNumber();
//       auto handler = arguments[1].asObject(rt_1).asFunction(rt_1);
//
//       // res->onDataV2
//
//       return {rt_1, thisValue};
//     }));

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

    this->setProperty(rt, "endWithoutBody", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "endWithoutBody"),
                                                                                 2,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      if(!arguments) {
        res->endWithoutBody();
        return {rt_1, thisValue};
      }

      std::optional<double> reportedContentLength = std::nullopt;
      bool closeConnection = false;

      if(arguments[0].isNumber()) {
        reportedContentLength = arguments[0].asNumber();
      }

      if(arguments[1].isBool()) {
        closeConnection = arguments[1].asBool();
      }

      res->endWithoutBody(reportedContentLength, closeConnection);

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "getRemoteAddressAsText", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "getRemoteAddressAsText"),
                                                                                 0,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      auto remoteAddress = res->getRemoteAddressAsText();
      return facebook::jsi::String::createFromUtf8(rt_1, std::string(remoteAddress));
    }));
    
    this->setProperty(rt, "getRemotePort", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "getRemotePort"),
                                                                                 0,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      return facebook::jsi::BigInt::fromUint64(rt_1, res->getRemotePort());
    }));

    this->setProperty(rt, "getWriteOffset", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "getWriteOffset"),
                                                                                 0,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      return facebook::jsi::BigInt::fromUint64(rt_1, res->getWriteOffset());
    }));

    this->setProperty(rt, "onAborted", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                            facebook::jsi::PropNameID::forUtf8(rt, "onAborted"),
                                                                                            1,
                                                                                            [res](facebook::jsi::Runtime &rt_1,
                                                                                                  const facebook::jsi::Value &thisValue,
                                                                                                  const facebook::jsi::Value *arguments,
                                                                                                  size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      res->onAborted([&rt_1, callback_ = std::move(callback)]() {
        callback_.call(rt_1);
      });
      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "onData", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "onData"),
                                                                                 1,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      res->onData([&rt_1, callback_ = std::move(callback)](auto chunk, auto isLast) {
        callback_.call(rt_1,
                       std::string(chunk),
                       isLast);
      });

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "onDataV2", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onDataV2"),
                                                                                      1,
                                                                                      [res](facebook::jsi::Runtime &rt_1,
                                                                                            const facebook::jsi::Value &thisValue,
                                                                                            const facebook::jsi::Value *arguments,
                                                                                            size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      res->onDataV2([&rt_1, callback_ = std::move(callback)](auto chunk, auto maxRemainingBodyLength) {
        callback_.call(rt_1,
                       std::string(chunk),
                       facebook::jsi::BigInt::fromUint64(rt_1, maxRemainingBodyLength));
      });

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "pause", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                         facebook::jsi::PropNameID::forUtf8(rt, "pause"),
                                                                                         0,
                                                                                         [res](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      res->pause();
      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt, "resume", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                         facebook::jsi::PropNameID::forUtf8(rt, "resume"),
                                                                                         0,
                                                                                         [res](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      res->resume();
      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt, "tryEnd", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                         facebook::jsi::PropNameID::forUtf8(rt, "tryEnd"),
                                                                                         2,
                                                                                         [res](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      auto fullBodyOrChunk = arguments[0].asString(rt_1).utf8(rt_1);
      auto totalSize = arguments[1].asNumber();

      auto tryEndResult = res->tryEnd(fullBodyOrChunk, static_cast<uintmax_t>(totalSize));

      return facebook::jsi::Array::createWithElements(rt_1, {tryEndResult.first, tryEndResult.second});
    }));

    /// TODO
    /// Implement this method later
    /// It looks like we need to persist/save the us_socket_context_t from the ws.
//    this->setProperty(rt, "upgrade", facebook::jsi::Function::createFromHostFunction(rt,
//                                                                                         facebook::jsi::PropNameID::forUtf8(rt, "upgrade"),
//                                                                                         5,
//                                                                                         [res](facebook::jsi::Runtime &rt_1,
//                                                                                               const facebook::jsi::Value &thisValue,
//                                                                                               const facebook::jsi::Value *arguments,
//                                                                                               size_t count) -> facebook::jsi::Value {
//      if(!arguments ||
//         count != 5 ||
//         !arguments[0].isString() ||
//         !arguments[1].isString() ||
//         !arguments[2].isString() ||
//         !arguments[3].isString() ||
//         !arguments[4].isNumber()) {
//        return facebook::jsi::Value::undefined();
//      }
//
//      auto userData = arguments[0].asString(rt_1).utf8(rt_1);
//      auto secWebSocketKey = arguments[1].asString(rt_1).utf8(rt_1);
//      auto secWebSocketProtocol = arguments[2].asString(rt_1).utf8(rt_1);
//      auto secWebSocketExtensions = arguments[3].asString(rt_1).utf8(rt_1);
//      auto context = arguments[4].asNumber();
//
//      // TODO give user a warning if this method has been called outside of cork handler
//      // assumeCorked()
//
//      auto *usSocketContext = reinterpret_cast<us_socket_context_t *>(static_cast<u_long>(context));
//
//      res->upgrade(std::string_view(userData),
//                   std::string_view(secWebSocketKey),
//                   std::string_view(secWebSocketProtocol),
//                   std::string_view(secWebSocketExtensions),
//                   usSocketContext);
//
//      return facebook::jsi::Value::undefined();
//    }));

    this->setProperty(rt, "write", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                         facebook::jsi::PropNameID::forUtf8(rt, "write"),
                                                                                         2,
                                                                                         [res](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      auto chunk = arguments[0].asString(rt_1).utf8(rt_1);
      return res->write(std::string_view(std::move(chunk)));
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