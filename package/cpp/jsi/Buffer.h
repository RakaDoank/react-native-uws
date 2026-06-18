#pragma once

#include <jsi/jsi.h>
#include <utility>

namespace react_native_uws {

// Probably bad name and usage.
// I can't figure it how create JS ArrayBuffer with faster buffer

class StringMutableBuffer : public facebook::jsi::MutableBuffer {

private:
  std::string *s;

public:
  StringMutableBuffer(std::string *s) {
    this->s = s;
  };

  size_t size() const override {
    return this->s->size();
  }

  uint8_t *data() override {
    return (uint8_t *)this->s->data();
  }

}; // StringMutableBuffer

class CharsMutableBuffer : public facebook::jsi::MutableBuffer {

private:
  std::vector<char> *c;

public:
  CharsMutableBuffer(std::vector<char> *c) {
    this->c = c;
  }

  CharsMutableBuffer(const std::shared_ptr<std::vector<char>> &c) {
    this->c = c.get();
  }

  size_t size() const override {
    return this->c->size();
  }

  uint8_t *data() override {
    return reinterpret_cast<uint8_t *>(this->c->data());
  }

};

class StringViewBuffer : public facebook::jsi::Buffer {

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

class StringViewMutableBuffer : public facebook::jsi::MutableBuffer {

private:
  std::string sv;

public:
  StringViewMutableBuffer(std::string_view sv) : sv(sv) {}

  size_t size() const override {
    return this->sv.size();
  }

  uint8_t *data() override {
    return reinterpret_cast<uint8_t *>(this->sv.data());
  }

};

} // namespace facebook::jsi
