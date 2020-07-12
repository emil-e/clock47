#include "httpserver.h"

#include <time.h>

#include <esp_http_server.h>

namespace httpserver {
namespace {

const char CONTENT_TYPE_TEXT[] = "text/plain";

httpd_handle_t g_httpd;

esp_err_t handlePing(httpd_req_t *req) {
  const char response[] = "PONG";
  httpd_resp_set_type(req, CONTENT_TYPE_TEXT);
  httpd_resp_send(req, response, sizeof(response) - 1);
  return ESP_OK;
}

esp_err_t handleTime(httpd_req_t *req) {
  time_t now;
  time(&now);

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  char buffer[64];
  const auto len = strftime(buffer, sizeof(buffer), "%c", &timeinfo);

  httpd_resp_set_type(req, CONTENT_TYPE_TEXT);
  httpd_resp_send(req, buffer, len);
  return ESP_OK;
}

httpd_uri_t URIS[] = {
    {.uri = "/ping", .method = HTTP_GET, .handler = handlePing, .user_ctx = NULL},
    {.uri = "/time", .method = HTTP_GET, .handler = handleTime, .user_ctx = NULL}};

} // namespace

void init() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;
  ESP_ERROR_CHECK(httpd_start(&g_httpd, &config));

  for (std::size_t i = 0; i < sizeof(URIS) / sizeof(httpd_uri_t);i++) {
    ESP_ERROR_CHECK(httpd_register_uri_handler(g_httpd, &URIS[i]));
  }
}

} // namespace httpserver
