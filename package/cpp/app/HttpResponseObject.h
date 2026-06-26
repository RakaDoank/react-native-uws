#pragma once

#include <ReactCommon/CallInvoker.h>
#include <jsi/Buffer.h>
#include <jsi/jsi.h>
#include <utility>
#include "RecognizedString.h"
#include "HttpResponseObjectProvider.h"
#include "uWebSockets/App.h"

namespace uws_react_native {

/// Only use this in the JavaScript thread
class HttpResponseObject : public facebook::jsi::Object {

public:
  HttpResponseObject(facebook::jsi::Runtime &rt,
                     const std::shared_ptr<HttpResponseObjectProvider> &provider,
                     std::shared_ptr<facebook::react::CallInvoker> &jsInvoker) : facebook::jsi::Object(rt) {

    this->setProperty(rt,
                      "close",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "close"),
                                                                      1,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      provider->res->close();
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "cork",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "cork"),
                                                                      1,
                                                                      [provider, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      provider->res->cork([asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker)]() {
        asyncCallback.call();
      });

      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "end", facebook::jsi::Function::createFromHostFunction(rt,
                                                                             facebook::jsi::PropNameID::forUtf8(rt, "end"),
                                                                             1,
                                                                             [provider](facebook::jsi::Runtime &rt_1,
                                                                                        const facebook::jsi::Value &thisValue,
                                                                                        const facebook::jsi::Value *arguments,
                                                                                        size_t count) -> facebook::jsi::Value {
      /// Due to JS run at different thread
      /// The race condition event is not avoidable under stress test
      /// Our predefined `res->onAborted` call earlier than JS callback
      /// This below makes JS side can call the "res.end" without attaching `onAborted` handler at all.
      /// If we know how to make a sync call across the thread, please update this.
      if(provider->dataAbort.isAlreadyAborted) {
        return {rt_1, thisValue};
      }

      auto body = RecognizedString(rt_1, arguments[0]).getStringView();
      provider->res->end(body);
      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "endWithoutBody",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "endWithoutBody"),
                                                                      2,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      /// Due to JS run at different thread
      /// The race condition event is not avoidable under stress test
      /// Our predefined `res->onAborted` call earlier than JS callback
      /// This below makes JS side can call the "res.end" without attaching `onAborted` handler at all.
      /// If we know how to make a sync call across the thread, please update this.
      if(provider->dataAbort.isAlreadyAborted) {
        return {rt_1, thisValue};
      }

      if(!arguments) {
        provider->res->endWithoutBody();
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

      provider->res->endWithoutBody(reportedContentLength, closeConnection);

      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "getRemoteAddressAsText",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getRemoteAddressAsText"),
                                                                      0,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      auto remoteAddress = provider->res->getRemoteAddressAsText();
      return facebook::jsi::String::createFromUtf8(rt_1, std::string(remoteAddress));
    }));

    this->setProperty(rt,
                      "getRemotePort",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getRemotePort"),
                                                                      0,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      return facebook::jsi::BigInt::fromUint64(rt_1, provider->res->getRemotePort());
    }));

    this->setProperty(rt,
                      "getWriteOffset",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "getWriteOffset"),
                                                                      0,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      return facebook::jsi::BigInt::fromUint64(rt_1, provider->res->getWriteOffset());
    }));

    this->setProperty(rt,
                      "onAborted",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onAborted"),
                                                                      1,
                                                                      [provider, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      if(provider->dataAbort.isAlreadyAborted) {
        facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker).call();
      } else {
        provider->dataAbort.callback = std::make_shared<facebook::react::AsyncCallback<facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);
      }

      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "onData",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onData"),
                                                                      1,
                                                                      [provider, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      if(provider->dataBody.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onData or assign it with existing onDataV2 and/or onFullData handler");
      }

      /// Same usage as the onDataV2
      /// except the second parameter to the JS handler is the boolean `isLast`
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      provider->dataBody.callback = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This a late call
      if(
        provider->dataBody.isStopCollecting ||
        provider->dataBody.maxRemainingBodyLength == 0
      ) {
        provider->dataBody.callback->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                                                      [provider](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto mutableBuffer = CharsMutableBuffer(provider->dataBody.buffer.get());
          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))),
                  provider->dataBody.maxRemainingBodyLength == 0);
        });
      }

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "onDataV2",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onDataV2"),
                                                                      1,
                                                                      [provider, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      if(provider->dataBody.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onDataV2 or assign it with existing onData and/or onFullData handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      provider->dataBody.callback = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onDataV2 callback
      /// due to the onDataV2 predefined lambda has finished earlier
      /// or isStopCollecting is already marked
      if(
        provider->dataBody.buffer &&
        (provider->dataBody.isStopCollecting || provider->dataBody.maxRemainingBodyLength == 0)
      ) {
        provider->dataBody.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [provider](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto mutableBuffer = CharsMutableBuffer(provider->dataBody.buffer.get());
          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))),
                  facebook::jsi::BigInt::fromUint64(rt, provider->dataBody.maxRemainingBodyLength));
        });
      }

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "onFullData",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onFullData"),
                                                                      1,
                                                                      [provider, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      if(provider->dataBody.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onFullData or assign it with existing onData and/or onDataV2 handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      provider->dataBody.isCallbackForFullChunk = true;
      provider->dataBody.callback = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onFullData callback
      /// due to the onDataV2 predefined lambda has finished earlier
      /// or isStopCollecting is already marked
      if(
        provider->dataBody.buffer &&
        (provider->dataBody.isStopCollecting || provider->dataBody.maxRemainingBodyLength == 0)
      ) {
        provider->dataBody.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [provider](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto mutableBuffer = CharsMutableBuffer(provider->dataBody.buffer.get());
          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))));
        });
      }

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "onFullDataText",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onFullDataText"),
                                                                      1,
                                                                      [provider, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      if(provider->dataBody.callbackStr) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onFullDataText handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      provider->dataBody.isCallbackForFullChunk = true;
      provider->dataBody.callbackStr = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onFullDataText callback
      /// due to the onDataV2 predefined lambda has finished earlier
      /// or isStopCollecting is already marked
      if(
        provider->dataBody.buffer &&
        (provider->dataBody.isStopCollecting || provider->dataBody.maxRemainingBodyLength == 0)
      ) {
        provider->dataBody.callbackStr
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [provider](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          cb.call(rt,
                  std::string(provider->dataBody.buffer->begin(), provider->dataBody.buffer->end()));
        });
      }

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "pause",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "pause"),
                                                                      0,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      provider->res->pause();
      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "resume",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "resume"),
                                                                      0,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      provider->res->resume();
      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt,
                      "tryEnd",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "tryEnd"),
                                                                      2,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      /// Due to JS run at different thread
      /// The race condition event is not avoidable under stress test
      /// Our predefined `res->onAborted` call earlier than JS callback
      /// This below makes JS side can call the "res.end" without attaching `onAborted` handler at all.
      /// If we know how to make a JS sync call across the thread, please update this.
      if(provider->dataAbort.isAlreadyAborted) {
        return {rt_1, thisValue};
      }

      auto fullBodyOrChunk = RecognizedString(rt_1, arguments[0]).getStringView();
      auto totalSize = arguments[1].asNumber();

      auto tryEndResult = provider->res->tryEnd(fullBodyOrChunk, static_cast<uintmax_t>(totalSize));

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

    this->setProperty(rt,
                      "write",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "write"),
                                                                      2,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      auto chunk = RecognizedString(rt_1, arguments[0]).getStringView();
      return provider->res->write(chunk);
    }));

    this->setProperty(rt,
                      "writeHeader",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "writeHeader"),
                                                                      2,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      auto headerKey = RecognizedString(rt_1, arguments[0]).getStringView();
      auto headerVal = RecognizedString(rt_1, arguments[1]).getStringView();
      provider->res->writeHeader(headerKey, headerVal);

      return {rt_1, thisValue};
    }));

    this->setProperty(rt,
                      "writeStatus",
                      facebook::jsi::Function::createFromHostFunction(rt,
                                                                      facebook::jsi::PropNameID::forUtf8(rt, "writeStatus"),
                                                                      1,
                                                                      [provider](facebook::jsi::Runtime &rt_1,
                                                                                 const facebook::jsi::Value &thisValue,
                                                                                 const facebook::jsi::Value *arguments,
                                                                                 size_t count) -> facebook::jsi::Value {
      auto status = RecognizedString(rt_1, arguments[0]).getStringView();
      provider->res->writeStatus(status);

      return {rt_1, thisValue};
    }));

  } // HttpResponseObject

}; // HttpResponseObject

} // namespace uws_react_native
