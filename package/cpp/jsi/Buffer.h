#pragma once

#include <jsi/jsi.h>

namespace facebook::jsi {

class StringMutableBuffer : public MutableBuffer {

private:
  std::string s;

public:
  StringMutableBuffer(std::string &&s) {
    this->s = s;
  }

  size_t size() const override {
    return this->s.size();
  }

  uint8_t *data() override {
    return reinterpret_cast<uint8_t *>(this->s.data());
  }

}; // StringMutableBuffer

class StringViewBuffer : public Buffer {

private:
  std::string_view sv;

public:
  StringViewBuffer(std::string_view s) : sv(s) {}

  size_t size() const override {
    return this->sv.size();
  }

  const uint8_t *data() const override {
    return reinterpret_cast<const uint8_t *>(this->sv.data());
  }

}; // StringViewBuffer

} // namespace facebook::jsi
