#pragma once

#include <string>
#include <vector>

namespace property {

class Property {
public:
  virtual void set(const std::string &value) {}
  virtual std::string get() const = 0;

  virtual ~Property() = default;
};

void add(const std::string &key, Property *property);
void set(const std::string &key, const std::string &value);
std::string get(const std::string &key);
std::vector<std::string> keys();

} // namespace property
