#include "httpserver.h"

#include <time.h>

#include <cJSON.h>

#include <esp_http_server.h>

#include "property.h"

namespace httpserver {

extern const char *INDEX_HTML;
extern const char *STYLE_CSS;
extern const char *SCRIPT_JSX;

namespace {

const char CONTENT_TYPE_TEXT[] = "text/plain";
const char CONTENT_TYPE_JSON[] = "application/json";
const char CONTENT_TYPE_CSS[] = "text/css";
const char CONTENT_TYPE_STYLE[] = "text/css";
const char CONTENT_TYPE_JSX[] = "text/jsx";

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

const char *propertyName(httpd_req_t *req) {
  std::size_t lastSlash = 0;
  for (std::size_t i = 0; req->uri[i] != '\0'; i++) {
    if (req->uri[i] == '/') {
      lastSlash = i;
    }
  }

  return req->uri + lastSlash + 1;
}

esp_err_t handlePropertiesList(httpd_req_t *req) {
  const auto root = cJSON_CreateObject();
  for (const auto &key : property::keys()) {
    const auto value = property::get(key.c_str());
    cJSON_AddItemToObject(root, key.c_str(), cJSON_CreateString(value.c_str()));
  }
  httpd_resp_set_type(req, CONTENT_TYPE_JSON);
  set_allow_origin_header(req);
  char *json = cJSON_PrintUnformatted(root);
  httpd_resp_sendstr(req, json);
  cJSON_free(json);
  cJSON_Delete(root);
  return ESP_OK;
}

esp_err_t handlePropertiesGet(httpd_req_t *req) {
  httpd_resp_set_type(req, CONTENT_TYPE_TEXT);
  set_allow_origin_header(req);
  const auto value = property::get(propertyName(req));
  httpd_resp_sendstr(req, value.c_str());
  return ESP_OK;
}

esp_err_t handlePropertiesPut(httpd_req_t *req) {
  char valueBuffer[256 + 1];
  const auto len = httpd_req_recv(req, valueBuffer, 256);
  valueBuffer[len] = '\0';
  property::set(propertyName(req), valueBuffer);
  set_allow_origin_header(req);
  httpd_resp_sendstr(req, "");
  return ESP_OK;
}

esp_err_t handleIndex(httpd_req_t *req) {
  set_allow_origin_header(req);
  httpd_resp_sendstr(req, INDEX_HTML);
  return ESP_OK;
}

esp_err_t handleStyle(httpd_req_t *req) {
  httpd_resp_set_type(req, CONTENT_TYPE_CSS);
  set_allow_origin_header(req);
  httpd_resp_sendstr(req, STYLE_CSS);
  return ESP_OK;
}

esp_err_t handleScript(httpd_req_t *req) {
  httpd_resp_set_type(req, CONTENT_TYPE_JSX);
  set_allow_origin_header(req);
  httpd_resp_sendstr(req, SCRIPT_JSX);
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
    {.uri = "/properties", .method = HTTP_GET, .handler = handlePropertiesList, .user_ctx = NULL},
    {.uri = "/properties/*", .method = HTTP_GET, .handler = handlePropertiesGet, .user_ctx = NULL},
    {.uri = "/properties/*", .method = HTTP_PUT, .handler = handlePropertiesPut, .user_ctx = NULL},
    {.uri = "/", .method = HTTP_GET, .handler = handleIndex, .user_ctx = NULL},
    {.uri = "/index.html", .method = HTTP_GET, .handler = handleIndex, .user_ctx = NULL},
    {.uri = "/style.css", .method = HTTP_GET, .handler = handleStyle, .user_ctx = NULL},
    {.uri = "/script.jsx", .method = HTTP_GET, .handler = handleScript, .user_ctx = NULL},
    {.uri = "/*", .method = HTTP_OPTIONS, .handler = handleCors, .user_ctx = NULL},
};

} // namespace

void init() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;
  config.max_uri_handlers = 16;
  ESP_ERROR_CHECK(httpd_start(&g_httpd, &config));

  for (std::size_t i = 0; i < sizeof(URIS) / sizeof(httpd_uri_t);i++) {
    ESP_ERROR_CHECK(httpd_register_uri_handler(g_httpd, &URIS[i]));
  }
}

} // namespace httpserver
