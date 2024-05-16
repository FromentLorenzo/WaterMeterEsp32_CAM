/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant/
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_http_server.h"
#include <HTTPClient.h>


//Replace with your network credentials
const char* ssid = "Khu S";
const char* password = "khu@s2022";

typedef struct {
        httpd_req_t *req;
        size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"

// This project was tested with the AI Thinker Model, M5STACK PSRAM Model and M5STACK WITHOUT PSRAM
#define CAMERA_MODEL_AI_THINKER
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM

// Not tested with this model
//#define CAMERA_MODEL_WROVER_KIT

#if defined(CAMERA_MODEL_WROVER_KIT)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
  #define LED_GPIO_NUM       4
#else
  #error "Camera model not selected"
#endif

esp_timer_handle_t photo_timer;

unsigned long lastCaptureTime = 0;

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

#define ROI_X_MIN 0
#define ROI_Y_MIN 0
#define ROI_X_MAX 1600
#define ROI_Y_MAX 1200

#define LED_CHANNEL     0
#define LED_RESOLUTION  8 // Nombre de bits pour la résolution de PWM (de 0 à 255)

// Fonction pour initialiser le flash avec PWM
void setupFlashPWM() {
    ledcAttachPin(LED_GPIO_NUM, LED_CHANNEL);
    ledcSetup(LED_CHANNEL, 5000, LED_RESOLUTION); // Fréquence de PWM: 5000 Hz
}

// Fonction pour régler l'intensité du flash
void setFlashIntensity(int intensity) {
    ledcWrite(LED_CHANNEL, intensity);
}

camera_fb_t* crop_image(camera_fb_t* src_fb, int x_min, int y_min, int x_max, int y_max) {
  int src_width = src_fb->width;
  int src_height = src_fb->height;

  // Assurez-vous que les valeurs de recadrage ne dépassent pas les dimensions de l'image capturée
  if (x_min < 0) x_min = 0;
  if (y_min < 0) y_min = 0;
  if (x_max > src_width) x_max = src_width;
  if (y_max > src_height) y_max = src_height;

  int crop_width = x_max - x_min;
  Serial.print("crop width:");

  Serial.print(crop_width);
  int crop_height = y_max - y_min;
  Serial.print("crop height:");

  Serial.print(crop_height);

  uint8_t* src_data = src_fb->buf;
  camera_fb_t* cropped_fb = esp_camera_fb_get();
  if (!cropped_fb) {
    Serial.println("Failed to allocate memory for cropped image");
    return NULL;
  }
  cropped_fb->format = PIXFORMAT_GRAYSCALE;
  cropped_fb->width = crop_width;
  cropped_fb->height = crop_height;
  cropped_fb->len = crop_width * crop_height;
  
  uint8_t* cropped_data = cropped_fb->buf;
  for (int y = y_min; y < y_max; y++) {
    for (int x = x_min; x < x_max; x++) {
      int src_index = y * src_width + x;
      int cropped_index = (y - y_min) * crop_width + (x - x_min);
      cropped_data[cropped_index] = src_data[src_index];
    }
  }

  return cropped_fb;
}


const int defaultFlashIntensity = 20;


static esp_err_t capture_handler(httpd_req_t *req){
  setFlashIntensity(defaultFlashIntensity);
  delay(50);
    // Toggle the LED to indicate photo capture

    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;

    // Capture a photo
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        
        // Turn off the LED if capture failed
        digitalWrite(LED_GPIO_NUM, LOW);
        
        return ESP_FAIL;
    }

    // Crop the captured image
    camera_fb_t* cropped_fb = crop_image(fb, ROI_X_MIN, ROI_Y_MIN, ROI_X_MAX, ROI_Y_MAX);
    if (!cropped_fb) {
        Serial.println("Failed to crop image");
        httpd_resp_send_500(req);
        esp_camera_fb_return(fb);
        
        // Turn off the LED if cropping failed
        digitalWrite(LED_GPIO_NUM, LOW);
        
        return ESP_FAIL;
    }

    // Set the content type and headers
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    // Send the cropped image data
    res = httpd_resp_send(req, (const char *)cropped_fb->buf, cropped_fb->len);

    // Free up the memory used for the photo capture and cropped image
    esp_camera_fb_return(fb);
    esp_camera_fb_return(cropped_fb);
    delay(50);


    // Turn off the LED after photo capture process is complete
    setFlashIntensity(0);

    return res;
}






void startCameraServer() {

    // Configure HTTP server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80; // You can change this port if needed

    // Start the stream server
    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
      

        // Register URI handler for the stream server
        httpd_uri_t capture_uri = {
            .uri       = "/capture", // Endpoint to capture a photo
            .method    = HTTP_GET,
            .handler   = capture_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(stream_httpd, &capture_uri);
    } else {
        Serial.println("Error starting stream server");
    }

}


void setup() {
    // Disable brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
    // Initialize serial communication
    Serial.begin(115200);
    Serial.setDebugOutput(false);
    pinMode(LED_GPIO_NUM, OUTPUT);
    setupFlashPWM(); // Initialiser le flash avec PWM
    setFlashIntensity(defaultFlashIntensity); 
    
    
    // Camera configuration
    camera_config_t config;
    // Configure camera pins
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; 
    
    if(psramFound()){
        config.frame_size = FRAMESIZE_UXGA; //UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    
    // Initialize the camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }
    
    // Wi-Fi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    
    // Print the IP address of the ESP32-CAM
    Serial.print("Camera Stream Ready! Go to: http://");
    Serial.print(WiFi.localIP());
    
    // Start streaming web server
    startCameraServer();
    lastCaptureTime = millis();

}


const unsigned long captureInterval = 100000;


#include <HTTPClient.h>
void loop() {
    // Attendre jusqu'à ce qu'il soit temps de capturer une nouvelle image
    unsigned long currentTime = millis();
    if (currentTime - lastCaptureTime < captureInterval) {
        delay(100); // Attente courte avant de vérifier à nouveau
        return;
    }
    // Définir l'URL de l'endpoint de capture d'image
    String captureUrl = "http://";
    captureUrl += WiFi.localIP().toString(); // Adresse IP locale de l'ESP32
    captureUrl += "/capture"; // Endpoint de capture d'image

    // Créer un objet HTTPClient
    HTTPClient http;

    // Commencer une nouvelle requête HTTP
    http.begin(captureUrl);

    // Envoyer une requête GET à l'endpoint de capture d'image
    int httpCode = http.GET();

    // Vérifier le code de réponse HTTP
    if(httpCode == HTTP_CODE_OK) {
        Serial.println("Capture d'image réussie");
    } else {
        Serial.print("Échec de la capture d'image, code HTTP: ");
        Serial.println(httpCode);
    }

    // Libérer les ressources de l'objet HTTPClient
    http.end();


    // Mettre à jour le temps de la dernière capture
    lastCaptureTime = currentTime;

    // Attendre un certain temps avant de capturer une autre image
    delay(100); // 10 secondes
}




