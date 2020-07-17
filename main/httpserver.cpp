#include "httpserver.h"

#include <time.h>

#include <esp_http_server.h>

#include "message.h"
#include "property.h"

namespace httpserver {

extern const char *WEBPAGE;

namespace {

const char CONTENT_TYPE_TEXT[] = "text/plain";

httpd_handle_t g_httpd;

void set_allow_origin_header(httpd_req_t *req) {
  ESP_ERROR_CHECK(httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*"));
}

esp_err_t handlePing(httpd_req_t *req) {
  const char response[] = "PONG";
  httpd_resp_set_type(req, CONTENT_TYPE_TEXT);
  set_allow_origin_header(req);
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
  set_allow_origin_header(req);
  httpd_resp_send(req, buffer, len);
  return ESP_OK;
}

ui::Color parseColor(httpd_req_t *req) {
  char queryBuffer[20];
  if (httpd_req_get_url_query_str(req, queryBuffer, sizeof(queryBuffer)) != ESP_OK) {
    return ui::color::WHITE;
  }
  char colorBuffer[2 * 3 + 1];
  if (httpd_query_key_value(queryBuffer, "color", colorBuffer, sizeof(colorBuffer)) != ESP_OK) {
    return ui::color::WHITE;
  }

  return ui::Color(std::strtoull(colorBuffer, nullptr, 16));
}

esp_err_t handleMessagePost(httpd_req_t *req) {
  char msgBuffer[message::MAX_LENGTH + 1];
  const auto len = httpd_req_recv(req, msgBuffer, message::MAX_LENGTH);
  msgBuffer[len] = '\0';

  message::display(msgBuffer, parseColor(req));
  set_allow_origin_header(req);
  httpd_resp_sendstr(req, "");
  return ESP_OK;
}

esp_err_t handleMessageDelete(httpd_req_t *req) {
  message::clear();
  set_allow_origin_header(req);
  httpd_resp_sendstr(req, "");
  return ESP_OK;
}

const char *propertyName(httpd_req_t *req) {
  std::size_t lastSlash = 0;
  for (std::size_t i = 0; req->uri[i] != '\0'; i++) {
    if (req->uri[i] == '/') {
      lastSlash = i;
    }
  }

  return req->uri + lastSlash + 1;
}

esp_err_t handlePropertyGet(httpd_req_t *req) {
  httpd_resp_set_type(req, CONTENT_TYPE_TEXT);
  set_allow_origin_header(req);
  const auto value = property::get(propertyName(req));
  httpd_resp_sendstr(req, value.c_str());
  return ESP_OK;
}

esp_err_t handlePropertyPut(httpd_req_t *req) {
  char valueBuffer[256 + 1];
  const auto len = httpd_req_recv(req, valueBuffer, 256);
  valueBuffer[len] = '\0';
  property::set(propertyName(req), valueBuffer);
  set_allow_origin_header(req);
  httpd_resp_sendstr(req, "");
  return ESP_OK;
}

esp_err_t handleRoot(httpd_req_t *req) {
  message::clear();
  httpd_resp_sendstr(req, WEBPAGE);
  return ESP_OK;
}

esp_err_t handleCors(httpd_req_t *req) {
  set_allow_origin_header(req);
  ESP_ERROR_CHECK(
      httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, PUT, POST, DELETE"));
  httpd_resp_sendstr(req, "");
  return ESP_OK;
}

httpd_uri_t URIS[] = {
    {.uri = "/ping", .method = HTTP_GET, .handler = handlePing, .user_ctx = NULL},
    {.uri = "/time", .method = HTTP_GET, .handler = handleTime, .user_ctx = NULL},
    {.uri = "/message", .method = HTTP_PUT, .handler = handleMessagePost, .user_ctx = NULL},
    {.uri = "/message", .method = HTTP_DELETE, .handler = handleMessageDelete, .user_ctx = NULL},
    {.uri = "/property/*", .method = HTTP_GET, .handler = handlePropertyGet, .user_ctx = NULL},
    {.uri = "/property/*", .method = HTTP_PUT, .handler = handlePropertyPut, .user_ctx = NULL},
    {.uri = "/", .method = HTTP_GET, .handler = handleRoot, .user_ctx = NULL},
    {.uri = "/*", .method = HTTP_OPTIONS, .handler = handleCors, .user_ctx = NULL},
};

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
