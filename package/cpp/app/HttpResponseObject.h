#pragma once

#include <jsi/Buffer.h>
#include <jsi/jsi.h>
#include <utility>
#include "RecognizedString.h"
#include "uWebSockets/HttpResponse.h"

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
    std::unique_ptr<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>> callback = nullptr;

    /**
     * For passing string, only for onFullDataText
     */
    std::unique_ptr<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>> callbackStr = nullptr;

    std::shared_ptr<std::vector<char>> buffer = nullptr;

    unsigned long maxRemainingBodyLength = 0;

    bool isCallbackForFullChunk = false;

    /**
     * It's marked from TemplatedAppObject that predefined res->onDataV2 there
     * want to stop collecting for certain condition.
     *
     * At the time, this boolean belong to the TemplatedAppObject only,
     * but if the JS handler assignment for the onData, onDataText, onDataV2, onFullData, and onFullDataText is late,
     * we invoke the handler immediately from the `this->setProperty`.
     */
    bool isStopCollecting = false;
  } OnDataV2Assignee;

//  void preEnd(facebook::jsi::Runtime &rt) const {
//    if(this->OnAbortedAssignee.alreadyAborted) {
//      /// Stated from uWebSockets
//      /// Every HttpResponse MUST have an attached abort handler.
//      /// If you do not respond to it immediately inside of the callback.
//      /// Returning from an Http request handler without attaching
//      /// (by calling onAborted) an abort handler is ill-use and will terminate.
//      /// When this event emits, the response has been aborted and may not be used.
//      throw facebook::jsi::JSError(rt, "Cannot send response to aborted request");
//    }
//  }

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
      /// Due to JS run at different thread
      /// The race condition event is not avoidable under stress test
      /// Our predefined `res->onAborted` call earlier than JS callback
      /// This below makes JS side can call the "res.end" without attaching `onAborted` handler at all.
      /// If we know how to make a sync call across the thread, please update this.
      if(this->OnAbortedAssignee.alreadyAborted) {
        return {rt_1, thisValue};
      }

      auto body = RecognizedString(rt_1, arguments[0]).getStringView();
      res->end(body);

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "endWithoutBody", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "endWithoutBody"),
                                                                                 2,
                                                                                 [this, res](facebook::jsi::Runtime &rt_1,
                                                                                             const facebook::jsi::Value &thisValue,
                                                                                             const facebook::jsi::Value *arguments,
                                                                                             size_t count) -> facebook::jsi::Value {
      /// Due to JS run at different thread
      /// The race condition event is not avoidable under stress test
      /// Our predefined `res->onAborted` call earlier than JS callback
      /// This below makes JS side can call the "res.end" without attaching `onAborted` handler at all.
      /// If we know how to make a sync call across the thread, please update this.
      if(this->OnAbortedAssignee.alreadyAborted) {
        return {rt_1, thisValue};
      }

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
      this->OnDataV2Assignee.callback = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This a late call
      if(
        this->OnDataV2Assignee.isStopCollecting ||
        this->OnDataV2Assignee.maxRemainingBodyLength == 0
      ) {
        this->OnDataV2Assignee.callback->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                                                          [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
//          auto stringMutableBuffer = facebook::jsi::StringMutableBuffer(&this->OnDataV2Assignee.chunk);
          auto mutableBuffer = CharsMutableBuffer(this->OnDataV2Assignee.buffer.get());
          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))),
                  this->OnDataV2Assignee.maxRemainingBodyLength == 0);
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
      this->OnDataV2Assignee.callback = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onDataV2 callback
      /// due to the onDataV2 predefined lambda has finished earlier
      /// or isStopCollecting is already marked
      if(
        this->OnDataV2Assignee.buffer &&
        (this->OnDataV2Assignee.isStopCollecting || this->OnDataV2Assignee.maxRemainingBodyLength == 0)
      ) {
        this->OnDataV2Assignee.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
//          auto stringMutableBuffer = facebook::jsi::StringMutableBuffer(&this->OnDataV2Assignee.chunk);
          auto mutableBuffer = CharsMutableBuffer(this->OnDataV2Assignee.buffer.get());
          cb.call(rt,
                 facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))),
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
      this->OnDataV2Assignee.callback = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onFullData callback
      /// due to the onDataV2 predefined lambda has finished earlier
      /// or isStopCollecting is already marked
      if(
        this->OnDataV2Assignee.buffer &&
        (this->OnDataV2Assignee.isStopCollecting || this->OnDataV2Assignee.maxRemainingBodyLength == 0)
      ) {
        this->OnDataV2Assignee.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
//          auto stringMutableBuffer = facebook::jsi::StringMutableBuffer(&this->OnDataV2Assignee.chunk);
          auto mutableBuffer = CharsMutableBuffer(this->OnDataV2Assignee.buffer.get());
          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))));
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
      if(this->OnDataV2Assignee.callbackStr) {
        throw facebook::jsi::JSError(rt_1, "Cannot reassign onFullDataText handler");
      }

      auto callback = arguments[0].asObject(rt_1).asFunction(rt_1);
      this->OnDataV2Assignee.isCallbackForFullChunk = true;
      this->OnDataV2Assignee.callbackStr = std::make_unique<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>>(rt_1, std::move(callback), jsInvoker);

      /// This is a late call to the onFullDataText callback
      /// due to the onDataV2 predefined lambda has finished earlier
      /// or isStopCollecting is already marked
      if(
        this->OnDataV2Assignee.buffer &&
        (this->OnDataV2Assignee.isStopCollecting || this->OnDataV2Assignee.maxRemainingBodyLength == 0)
      ) {
        this->OnDataV2Assignee.callbackStr
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [this](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          cb.call(rt,
                  std::string(this->OnDataV2Assignee.buffer->begin(), this->OnDataV2Assignee.buffer->end()));
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
      /// Due to JS run at different thread
      /// The race condition event is not avoidable under stress test
      /// Our predefined `res->onAborted` call earlier than JS callback
      /// This below makes JS side can call the "res.end" without attaching `onAborted` handler at all.
      /// If we know how to make a JS sync call across the thread, please update this.
      if(this->OnAbortedAssignee.alreadyAborted) {
        return {rt_1, thisValue};
      }

      auto fullBodyOrChunk = RecognizedString(rt_1, arguments[0]).getStringView();
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
      auto chunk = RecognizedString(rt_1, arguments[0]).getStringView();
      return res->write(std::string_view(chunk));
    }));

    this->setProperty(rt, "writeHeader", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                         facebook::jsi::PropNameID::forUtf8(rt, "writeHeader"),
                                                                                         2,
                                                                                         [res](facebook::jsi::Runtime &rt_1,
                                                                                               const facebook::jsi::Value &thisValue,
                                                                                               const facebook::jsi::Value *arguments,
                                                                                               size_t count) -> facebook::jsi::Value {
      auto headerKey = RecognizedString(rt_1, arguments[0]).getStringView();
      auto headerVal = RecognizedString(rt_1, arguments[1]).getStringView();
      res->writeHeader(headerKey, headerVal);

      return {rt_1, thisValue};
    }));

    this->setProperty(rt, "writeStatus", facebook::jsi::Function::createFromHostFunction(rt,
                                                                                 facebook::jsi::PropNameID::forUtf8(rt, "writeStatus"),
                                                                                 1,
                                                                                 [res](facebook::jsi::Runtime &rt_1,
                                                                                       const facebook::jsi::Value &thisValue,
                                                                                       const facebook::jsi::Value *arguments,
                                                                                       size_t count) -> facebook::jsi::Value {
      auto status = RecognizedString(rt_1, arguments[0]).getStringView();
      res->writeStatus(status);

      return {rt_1, thisValue};
    }));
  } // HttpResponseObject

  void jsCall_onAborted() {
    this->OnAbortedAssignee.alreadyAborted = true;

    if(this->OnAbortedAssignee.callback) {
      this->OnAbortedAssignee.callback->call(facebook::jsi::Value::undefined());
    }
  }

  bool isStopCollectingChunk() const {
    return this->OnDataV2Assignee.isStopCollecting;
  }

  void stopCollectingChunk() {
    this->OnDataV2Assignee.isStopCollecting = true;
  }

  void setChunk(std::string_view chunk,
               unsigned long maxRemainingBodyLength) {
    if(!this->OnDataV2Assignee.buffer) {
      this->OnDataV2Assignee.buffer = std::make_shared<std::vector<char>>();
      this->OnDataV2Assignee.buffer->reserve(maxRemainingBodyLength + chunk.size()); // preallocate with hint
    }
    this->OnDataV2Assignee.buffer->insert(this->OnDataV2Assignee.buffer->end(), chunk.begin(), chunk.end());
//    this->OnDataV2Assignee.chunk.append(chunk.data(), chunk.size());
    this->OnDataV2Assignee.maxRemainingBodyLength = maxRemainingBodyLength;
  }

  void invokeOnDataV2() {
    if(
      !this->OnDataV2Assignee.isCallbackForFullChunk ||
      this->OnDataV2Assignee.maxRemainingBodyLength == 0
    ) {
      /// HELP me the better way to pass JSI ArrayBuffer here.
      /// with faster buffer or anything

      /// I have tested that,
      /// when we captured the buffer by reference into the lambda,
      /// in the middle of stream, it is often giving inaccruacy of
      /// JS ArrayBuffer.byteLength in the `onDataV2` argument (due to async call)
      /// when it's compared to the maxRemainingBodyLength differentiation that captured by value.
      /// It's still accurate when it's finished.

      /// While capturing the buffer by value is often accurate.
      /// but is it slower because of heap allocation?

      if(this->OnDataV2Assignee.callback) {
        this->OnDataV2Assignee.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [buffer = this->OnDataV2Assignee.buffer, maxRemainingBodyLength = this->OnDataV2Assignee.maxRemainingBodyLength](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto mutableBuffer = CharsMutableBuffer(buffer);

          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))),
                  facebook::jsi::BigInt::fromUint64(rt, maxRemainingBodyLength));
        });
      }

      if(this->OnDataV2Assignee.callbackStr) {
        this->OnDataV2Assignee.callbackStr
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [buffer = this->OnDataV2Assignee.buffer, maxRemainingBodyLength = this->OnDataV2Assignee.maxRemainingBodyLength](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          cb.call(rt,
                  std::string(buffer->begin(), buffer->end()),
                  facebook::jsi::BigInt::fromUint64(rt, maxRemainingBodyLength));
        });
      }
    }
  }

  size_t getChunkSize() const {
//    return this->OnDataV2Assignee.chunk.size();
    return this->OnDataV2Assignee.buffer->size();
  }

};

}
