#pragma once

#include <jsi/jsi.h>
#include <memory>
#include "AppRunner.h"
#include "TemplatedAppObject.h"

namespace react_native_uws {

class AppWrapper {

private:
  std::shared_ptr<AppRunner> appRunner;

public:
  AppWrapper() {
    // TODO Implement the uWS::App options
    this->appRunner = std::make_shared<AppRunner>();
  }

  TemplatedAppObject getTemplatedAppObject(facebook::jsi::Runtime &rt) {
    return {this->appRunner, rt};
  }

};

}