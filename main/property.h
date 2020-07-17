#pragma once

#include <string>

namespace property {

class Property {
public:
  virtual void set(const std::string &value) = 0;
  virtual std::string get() const = 0;

  virtual ~Property() = default;
};

void add(const std::string &key, Property *proeprty);
void set(const std::string &key, const std::string &value);
std::string get(const std::string &key);

} // namespace property
