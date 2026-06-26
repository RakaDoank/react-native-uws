#pragma once

#include <jsi/jsi.h>
#include <memory>
#include <string_view>
#include "uWebSockets/HttpResponse.h"

namespace uws_react_native {

class HttpResponseObjectProvider {

public:
  HttpResponseObjectProvider(uWS::HttpResponse<false> *pRes) {
    this->res = pRes;
  }

  uWS::HttpResponse<false> *res;
  
  struct {
    std::shared_ptr<facebook::react::AsyncCallback<facebook::jsi::Value>> callback = nullptr;

    bool isAlreadyAborted = false;
  } dataAbort;
  
  struct {
    /**
     * These handlers are for
     * - onData
     * - onDataV2
     * - onFullData
     * but only allow one handler assignment at the time
     */
    std::unique_ptr<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>> callback = nullptr;

    /**
     * For passing string and it's only for onFullDataText
     */
    std::unique_ptr<facebook::react::AsyncCallback<facebook::jsi::Value, facebook::jsi::Value>> callbackStr = nullptr;

    std::shared_ptr<std::vector<char>> buffer = nullptr;

    unsigned long maxRemainingBodyLength = 0;

    bool isCallbackForFullChunk = false;

    /**
     * It's marked from TemplatedAppObject that predefined res->onDataV2 there
     * that it wants to stop collecting for certain condition.
     *
     * At the time, this boolean belong to the TemplatedAppObject only,
     * but if the JS handler assignment for the onData, onDataText, onDataV2, onFullData, and onFullDataText is late,
     * we invoke the handler immediately from the `this->setProperty`.
     */
    bool isStopCollecting = false;
  } dataBody;

  void insertBodyBuffer(std::string_view chunk,
                        unsigned long maxRemainingBodyLength) {
    if(!this->dataBody.buffer) {
      this->dataBody.buffer = std::make_shared<std::vector<char>>();
      this->dataBody.buffer->reserve(maxRemainingBodyLength + chunk.size()); // preallocate with hint
    }

    this->dataBody.buffer->insert(this->dataBody.buffer->end(), chunk.begin(), chunk.end());
    this->dataBody.maxRemainingBodyLength = maxRemainingBodyLength;
  }

  /**
   * Probably a bad name,
   * it's used either for "onData", "onDataV2", or "onFullData", and combined for "onFullDataText"
   */
  void invokeBodyCallback() const {
    if(
      !this->dataBody.isCallbackForFullChunk ||
      this->dataBody.maxRemainingBodyLength == 0
    ) {

      /// HELP me the better way to pass JSI ArrayBuffer here.
      /// with faster buffer or anything

      /// I have tested that,
      /// when we captured the buffer by reference into the lambda,
      /// in the middle of stream, it is often giving inaccuracy of
      /// JS ArrayBuffer.byteLength in the `onDataV2` argument
      /// when it's compared to the maxRemainingBodyLength differentiation that captured by value.
      /// It's still accurate when it's finished.

      /// While capturing the buffer by value is often accurate.
      /// but is it slower?

      if(this->dataBody.callback) {
        this->dataBody.callback
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [buffer = this->dataBody.buffer, maxRemainingBodyLength = this->dataBody.maxRemainingBodyLength](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          auto mutableBuffer = CharsMutableBuffer(buffer);

          cb.call(rt,
                  facebook::jsi::ArrayBuffer(rt, std::make_shared<CharsMutableBuffer>(std::move(mutableBuffer))),
                  facebook::jsi::BigInt::fromUint64(rt, maxRemainingBodyLength));
        });
      }

      if(this->dataBody.callbackStr) {
        this->dataBody.callbackStr
          ->callWithPriority(facebook::react::SchedulerPriority::ImmediatePriority,
                             [buffer = this->dataBody.buffer, maxRemainingBodyLength = this->dataBody.maxRemainingBodyLength](facebook::jsi::Runtime &rt, facebook::jsi::Function &cb) {
          cb.call(rt,
                  std::string(buffer->begin(), buffer->end()),
                  facebook::jsi::BigInt::fromUint64(rt, maxRemainingBodyLength));
        });
      }

    }
  }
  
}; // HttpResponseObjectProvider

} // uws_react_native
