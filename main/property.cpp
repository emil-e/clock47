#include "property.h"

#include <mutex>
#include <string>
#include <unordered_map>

namespace property {
namespace {

std::mutex g_mutex;
std::unordered_map<std::string, Property *> g_properties;

} // namespace

void add(const std::string &key, Property *proeprty) {
  const auto lock = std::lock_guard(g_mutex);
  g_properties[key] = proeprty;
}

void set(const std::string &key, const std::string &value) {
  const auto lock = std::lock_guard(g_mutex);
  const auto it = g_properties.find(key);
  if (it != g_properties.end()) {
    it->second->set(value);
  }
}

std::string get(const std::string &key) {
  const auto lock = std::lock_guard(g_mutex);
  const auto it = g_properties.find(key);
  if (it == g_properties.end()) {
    return std::string();
  }

  return it->second->get();
}

std::vector<std::string> keys() {
  std::vector<std::string> keyStrings;
  keyStrings.reserve(g_properties.size());
  for (const auto &p : g_properties) {
    keyStrings.push_back(p.first);
  }

  return keyStrings;
}

} // namespace property
