#pragma once

#include <android/log.h>
#include <jsi/Buffer.h>
#include <jsi/jsi.h>
#include <utility>
#include <uWebSockets/HttpResponse.h>

namespace react_native_uws {

class HttpResponseObject : public facebook::jsi::Object {

private:
  struct {
    std::shared_ptr<facebook::react::AsyncCallback<facebook::jsi::Value>> callback = nullptr;
    bool alreadyAborted = false;
  } OnAbortedAssignee;

  struct {
    /**
     * For passing ArrayBuffer
     * - onData
     * - onDataV2
     * - onFullData
     */
    std::shared_ptr<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>> callback = nullptr;

    /**
     * For passing string
     * - onDataText
     * - onFullDataText
     */
    std::shared_ptr<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>> callbackStr = nullptr;

    std::string chunk = {};
    unsigned long maxRemainingBodyLength = 0;
    bool isCallbackForFullChunk = false;
  } OnDataV2Assignee;

  void preEnd(facebook::jsi::Runtime &rt) const {
    if(this->OnAbortedAssignee.alreadyAborted) {
      /// Stated from uWebSockets
      /// Every HttpResponse MUST have an attached abort handler.
      /// If you do not respond to it immediately inside of the callback.
      /// Returning from an Http request handler without attaching
      /// (by calling onAborted) an abort handler is ill-use and will terminate.
      /// When this event emits, the response has been aborted and may not be used.
      throw facebook::jsi::JSError(rt, "Cannot send response to aborted request");
    }
  }

public:
  HttpResponseObject(facebook::jsi::Runtime &rt,
                     uWS::HttpResponse<false> *res,
                     std::shared_ptr<facebook::react::CallInvoker> &jsInvoker) : facebook::jsi::Object(rt) {
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
                                                                                 [res, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                                   const facebook::jsi::Value &thisValue,
                                                                                                   const facebook::jsi::Value *arguments,
                                                                                                   size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      res->cork([asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker)]() {
        asyncCallback.call();
      });

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "end", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "end"),
                                                                                 1,
                                                                                 [this, res](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      this->preEnd(rt_1);

      auto body = arguments[0].asString(rt_1).utf8(rt_1);
      res->end(std::move(body));

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "endWithoutBody", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "endWithoutBody"),
                                                                                 2,
                                                                                 [this, res](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      this->preEnd(rt_1);

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
                                                                                            [this, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                                  const facebook::jsi::Value &thisValue,
                                                                                                  const facebook::jsi::Value *arguments,
                                                                                                  size_t count) -> facebook::jsi::Value {
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);

      if(this->OnAbortedAssignee.alreadyAborted) {
        facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker).call();
      } else {
        this->OnAbortedAssignee.callback = std::make_shared<facebook::react::AsyncCallback<facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);
      }

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "onData", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "onData"),
                                                                                 1,
                                                                                 [this, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                                    const facebook::jsi::Value &thisValue,
                                                                                                    const facebook::jsi::Value *arguments,
                                                                                                    size_t count) -> facebook::jsi::Value {
      if(this->OnDataV2Assignee.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onData or assign it with existing onDataV2 and/or onFullData handler");
      }

      /// Same usage as the onDataV2
      /// except the second parameter to the JS handler is the boolean `isLast`
      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      this->OnDataV2Assignee.callback = std::make_shared<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This a late call
      if(this->OnDataV2Assignee.maxRemainingBodyLength == 0) {
        this->OnDataV2Assignee.callback->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                                                          [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto stringMutableBuffer = facebook::jsi::StringMutableBuffer(&this->OnDataV2Assignee.chunk);

          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<facebook::jsi::StringMutableBuffer>(stringMutableBuffer)),
                  this->OnDataV2Assignee.maxRemainingBodyLength == 0);
        });
      }

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt, "onDataText", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onDataText"),
                                                                                      1,
                                                                                      [this, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                                         const facebook::jsi::Value &thisValue,
                                                                                                         const facebook::jsi::Value *arguments,
                                                                                                         size_t count) -> facebook::jsi::Value {
      if(this->OnDataV2Assignee.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onDataText or assign it with existing onFullDataText handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      this->OnDataV2Assignee.callbackStr = std::make_shared<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onDataText callback
      /// due to the onDataV2 predefined lambda has finished earlier.
      if(this->OnDataV2Assignee.maxRemainingBodyLength == 0) {
        this->OnDataV2Assignee.callbackStr
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          cb.call(rt,
                  this->OnDataV2Assignee.chunk,
                  facebook::jsi::BigInt::fromUint64(rt, this->OnDataV2Assignee.maxRemainingBodyLength));
        });
      }

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt, "onDataV2", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onDataV2"),
                                                                                      1,
                                                                                      [this, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                                         const facebook::jsi::Value &thisValue,
                                                                                                         const facebook::jsi::Value *arguments,
                                                                                                         size_t count) -> facebook::jsi::Value {
      if(this->OnDataV2Assignee.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onDataV2 or assign it with existing onData and/or onFullData handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      this->OnDataV2Assignee.callback = std::make_shared<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onDataV2 callback
      /// due to the onDataV2 predefined lambda has finished earlier.
      if(this->OnDataV2Assignee.maxRemainingBodyLength == 0) {
        this->OnDataV2Assignee.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto stringMutableBuffer = facebook::jsi::StringMutableBuffer(&this->OnDataV2Assignee.chunk);

          cb.call(rt,
                 facebook::jsi::ArrayBuffer(rt, std::make_shared<facebook::jsi::StringMutableBuffer>(std::move(stringMutableBuffer))),
                 facebook::jsi::BigInt::fromUint64(rt, this->OnDataV2Assignee.maxRemainingBodyLength));
        });
      }

      /// Sadly, we can't assign onDataV2 so late
//      res->onDataV2([asyncCallback = facebook::react::AsyncCallback(rt_1, std::move(callback), jsInvoker)](std::string_view chunk, auto maxRemainingBodyLength) {
//        asyncCallback.call([chunk, maxRemainingBodyLength](facebook::jsi::Runtime &rt_2, facebook::jsi::Function &cb) {
//          auto stringMutableBuffer = std::make_shared<facebook::jsi::StringMutableBuffer>(std::string(chunk));
//          cb.call(rt_2,
//                  facebook::jsi::ArrayBuffer(rt_2, stringMutableBuffer),
//                  facebook::jsi::BigInt::fromUint64(rt_2, maxRemainingBodyLength));
//        });
//      });

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt, "onFullData", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                      facebook::jsi::PropNameID::forUtf8(rt, "onFullData"),
                                                                                      1,
                                                                                      [this, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                                         const facebook::jsi::Value &thisValue,
                                                                                                         const facebook::jsi::Value *arguments,
                                                                                                         size_t count) -> facebook::jsi::Value {
      if(this->OnDataV2Assignee.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onFullData or assign it with existing onData and/or onDataV2 handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      this->OnDataV2Assignee.isCallbackForFullChunk = true;
      this->OnDataV2Assignee.callback = std::make_shared<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onFullData callback
      /// due to the onDataV2 predefined lambda has finished earlier.
      if(this->OnDataV2Assignee.maxRemainingBodyLength == 0 && this->OnDataV2Assignee.callback) {
        this->OnDataV2Assignee.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto stringMutableBuffer = facebook::jsi::StringMutableBuffer(&this->OnDataV2Assignee.chunk);
          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<facebook::jsi::StringMutableBuffer>(std::move(stringMutableBuffer))));
        });
      }

      return facebook::jsi::Value::undefined();
    }));

    this->setProperty(rt, "onFullDataText", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                        facebook::jsi::PropNameID::forUtf8(rt, "onFullDataText"),
                                                                                        1,
                                                                                        [this, &jsInvoker](facebook::jsi::Runtime &rt_1,
                                                                                                           const facebook::jsi::Value &thisValue,
                                                                                                           const facebook::jsi::Value *arguments,
                                                                                                           size_t count) -> facebook::jsi::Value {
      if(this->OnDataV2Assignee.callback) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onFullDataText or assign it with existing onDataText handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      this->OnDataV2Assignee.isCallbackForFullChunk = true;
      this->OnDataV2Assignee.callbackStr = std::make_shared<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onFullDataText callback
      /// due to the onDataV2 predefined lambda has finished earlier.
      if(this->OnDataV2Assignee.maxRemainingBodyLength == 0 && this->OnDataV2Assignee.callbackStr) {
        this->OnDataV2Assignee.callbackStr
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          cb.call(rt,
                  this->OnDataV2Assignee.chunk);
        });
      }

      return facebook::jsi::Value::undefined();
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
                                                                                         [this, res](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      this->preEnd(rt_1);

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

  void jsCall_onAborted() {
    this->OnAbortedAssignee.alreadyAborted = true;

    if(this->OnAbortedAssignee.callback) {
      this->OnAbortedAssignee.callback->call(facebook::jsi::Value::undefined());
    }
  }

  void jsCall_onDataV2(std::string_view chunk,
                       unsigned long maxRemainingBodyLength) {
    this->OnDataV2Assignee.chunk.append(chunk.data(), chunk.size());
    this->OnDataV2Assignee.maxRemainingBodyLength = maxRemainingBodyLength;

    if(
        !this->OnDataV2Assignee.isCallbackForFullChunk ||
        maxRemainingBodyLength == 0
    ) {
      if(this->OnDataV2Assignee.callback) {
        this->OnDataV2Assignee.callback->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                                                 [this, maxRemainingBodyLength](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto stringMutableBuffer = facebook::jsi::StringMutableBuffer(&this->OnDataV2Assignee.chunk);

          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<facebook::jsi::StringMutableBuffer>(std::move(stringMutableBuffer))),
                  facebook::jsi::BigInt::fromUint64(rt, maxRemainingBodyLength));
        });
      } else if(this->OnDataV2Assignee.callbackStr) {
        this->OnDataV2Assignee.callbackStr
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this, maxRemainingBodyLength](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
            cb.call(rt,
                    this->OnDataV2Assignee.chunk,
                    facebook::jsi::BigInt::fromUint64(rt, maxRemainingBodyLength));
          });
      }
    }
  }

};

}