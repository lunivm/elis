#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_wifi.h"
#include "sys/param.h"

#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"

// Tag used for ESP serial console messages
static const char TAG[] = "http_server";

// Wifi connect status
//static http_server_wifi_connect_status g_wifi_connect_status = NONE;

// Firmware update status
//static int g_fw_update_status = OTA_UPDATE_PENDING;

// Local Time status
//static bool g_is_local_time_set = false;

// HTTP server task handle
static httpd_handle_t http_server_handle = NULL;

// HTTP server monitor task handle
//static TaskHandle_t task_http_server_monitor = NULL;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_queue_handle;

// Embedded files: JQuery, index.html, app.css, app.js and favicon.ico files
extern const uint8_t jquery_min_js_start[]	asm("_binary_jquery_min_js_start");
extern const uint8_t jquery_min_js_end[]		asm("_binary_jquery_min_js_end");
extern const uint8_t index_html_start[]				asm("_binary_index_html_start");
extern const uint8_t index_html_end[]				asm("_binary_index_html_end");
extern const uint8_t app_css_start[]				asm("_binary_app_css_start");
extern const uint8_t app_css_end[]					asm("_binary_app_css_end");
extern const uint8_t app_js_start[]					asm("_binary_app_js_start");
extern const uint8_t app_js_end[]					asm("_binary_app_js_end");
extern const uint8_t favicon_ico_start[]			asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[]				asm("_binary_favicon_ico_end");

/**
 * Jquery get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_jquery_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "Jquery requested");

	httpd_resp_set_type(req, "application/javascript");
	httpd_resp_send(req, (const char *)jquery_min_js_start, jquery_min_js_end - jquery_min_js_start);

	return ESP_OK;
}

/**
 * Sends the index.html page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "index.html requested");

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);

	return ESP_OK;
}

/**
 * app.css get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "app.css requested");

	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);

	return ESP_OK;
}

/**
 * app.js get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "app.js requested");

	httpd_resp_set_type(req, "application/javascript");
	httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);

	return ESP_OK;
}

/**
 * Sends the .ico (icon) file when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_favicon_ico_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "favicon.ico requested");

	httpd_resp_set_type(req, "image/x-icon");
	httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);

	return ESP_OK;
}

/**
 * Sets up the default httpd server configuration.
 * @return http server instance handle if successful, NULL otherwise.
 */
static httpd_handle_t http_server_configure(void)
{
	// Generate the default configuration
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	// Create the message queue
	http_server_monitor_queue_handle = xQueueCreate(3, sizeof(http_server_queue_message_t));

	// The core that the HTTP server will run on
	config.core_id = HTTP_SERVER_TASK_CORE_ID;

	// Adjust the default priority to 1 less than the wifi application task
	config.task_priority = HTTP_SERVER_TASK_PRIORITY;

	// Bump up the stack size (default is 4096)
	config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

	// Increase uri handlers
  config.max_uri_handlers = 20;

	// Increase the timeout limits
	config.recv_wait_timeout = 10;
	config.send_wait_timeout = 10;

	ESP_LOGI(TAG,
			"http_server_configure: Starting server on port: '%d' with task priority: '%d'",
			config.server_port,
			config.task_priority);

	esp_err_t startup_code = httpd_start(&http_server_handle, &config);
	if (startup_code != ESP_OK)
	{
		ESP_LOGI(TAG, "http_server_configure: Starting server error - %s", esp_err_to_name(startup_code));
		return NULL;
	}

	ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");

  // register query handler
  httpd_uri_t jquery_js = {
      .uri = "/jquery.min.js",
      .method = HTTP_GET,
      .handler = http_server_jquery_handler,
      .user_ctx = NULL
  };
  httpd_register_uri_handler(http_server_handle, &jquery_js);

  // register index.html handler
  httpd_uri_t index_html = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = http_server_index_html_handler,
      .user_ctx = NULL
  };
  httpd_register_uri_handler(http_server_handle, &index_html);

  // register app.css handler
  httpd_uri_t app_css = {
      .uri = "/app.css",
      .method = HTTP_GET,
      .handler = http_server_app_css_handler,
      .user_ctx = NULL
  };
  httpd_register_uri_handler(http_server_handle, &app_css);

  // register app.js handler
  httpd_uri_t app_js = {
      .uri = "/app.js",
      .method = HTTP_GET,
      .handler = http_server_app_js_handler,
      .user_ctx = NULL
  };
  httpd_register_uri_handler(http_server_handle, &app_js);

  // register favicon.ico handler
  httpd_uri_t favicon_ico = {
      .uri = "/favicon.ico",
      .method = HTTP_GET,
      .handler = http_server_favicon_ico_handler,
      .user_ctx = NULL
  };
  httpd_register_uri_handler(http_server_handle, &favicon_ico);

	return http_server_handle;
}

void http_server_start(void)
{
	if (http_server_handle == NULL)
	{
		http_server_handle = http_server_configure();
	}
}

void http_server_stop(void)
{
	if (http_server_handle)
	{
		httpd_stop(http_server_handle);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server");
		http_server_handle = NULL;
	}
//	if (task_http_server_monitor)
//	{
//		vTaskDelete(task_http_server_monitor);
//		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server monitor");
//		task_http_server_monitor = NULL;
//	}
}
