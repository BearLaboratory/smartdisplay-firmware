#include <Arduino.h>
#line 1 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
#include "SHT31.h"
#include "Wire.h"
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.h>
#define SHT31_ADDRESS 0x44
#include "./Dialog_plain_65.h"
#include "./fullscreenpics.h"
#include "./icon.h"
// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <GxIO/GxIO.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
GxIO_Class io(SPI, SS, 17, 16);
GxEPD_Class display(io, 16, 4);
SHT31 sht;
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60
RTC_DATA_ATTR bool inited = false;
RTC_DATA_ATTR int partialTimes = 0;

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "driver/adc.h"

// 蓝牙ble
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define BTNAME "BLabSmartDisplay"
#define SERVICE_UUID "71db3107-6be5-4c0b-ad8e-731df7069e62"
#define READ_CHARACTERISTIC_UUID "8b3aed0e-a5a0-4e7f-9965-d0f3d26a1b36"
#define WRITE_CONFIG_CHARACTERISTIC_UUID "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a8"
#define WRITE_REBOOT_SYSTEM_CHARACTERISTIC_UUID \
  "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a9"
String downConfigJson = "";
#include "FS.h"
#include "SPIFFS.h"

//系统是否配置标识
boolean systemConfigured = false;
JsonObject configJsonObject;
std::string version = "1.0.0";

DynamicJsonDocument doc(1024);

boolean bleConnect = false;
/**
 * 软件重启方法
 */
#line 54 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void rebootSystem();
#line 62 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void clearProperties();
#line 70 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void clearSystemAndReboot();
#line 81 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void readProperties();
#line 92 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
String readProperties2();
#line 116 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
boolean writeProperties(String jsonstring);
#line 226 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void initBLE();
#line 281 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawTianqiFrame();
#line 436 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawFanscountFrame();
#line 508 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawPicFrame();
#line 516 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawMainFrame();
#line 523 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawNotConfig();
#line 530 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void disableWiFi();
#line 537 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void enableWiFi();
#line 543 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void setup();
#line 601 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void loop();
#line 54 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void rebootSystem()
{
  delay(500);
  ESP.restart();
}
/**
 * 清空删除配置文件
 */
void clearProperties()
{
  SPIFFS.remove("/config.json");
}
/**
 * 清空配置文件并重启系统
 * 注意：这个方法必须要在文件系统挂载以后才能调用
 */
void clearSystemAndReboot()
{
  clearProperties();
  rebootSystem();
}
/**
 * 读取配置文件,同时将配置文件解析出来赋值给全局变量
 *
 * 如果配置文件存在则解析出配置，如果配置文件不存在提示用户配置
 *
 */
void readProperties()
{
  //检查文件是否存在
  boolean exist = SPIFFS.exists("/config.json");
  File file = SPIFFS.open("/config.json", "r");
  String data = file.readString();
  deserializeJson(doc, data);
  configJsonObject = doc.as<JsonObject>();
  systemConfigured = true;
}

String readProperties2()
{
  String data = "";
  //检查文件是否存在
  boolean exist = SPIFFS.exists("/config.json");
  if (exist)
  {
    Serial.println("file exist");
    File file = SPIFFS.open("/config.json");
    String data2 = file.readString();
    return data2;
    file.close();
  }
  else
  {
    Serial.println("file not  exist");
  }
  return data;
}
/**
 * 将数据保存至配置文件中
 *
 * @param jsonstring
 */
boolean writeProperties(String jsonstring)
{
  File file = SPIFFS.open("/config.json", FILE_WRITE);
  if (!file)
  {
    Serial.println("openfile fail");
    return false;
  }
  if (file.print(jsonstring))
  {
    file.close();
    Serial.println("save file success");
    return true;
  }
  else
  {
    Serial.println("save file fail");
    return false;
  }

  // Serial.print("config ");
  // Serial.print(jsonstring);
  // File file = SPIFFS.open("/config.json", FILE_WRITE);
  // boolean status = false;
  // if (file && file.print(jsonstring))
  // {
  //   status = true;
  // }
  // file.close();
  // return status;
}

// 3. 定义写数据回调函数
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      for (int i = 0; i < value.length(); i++)
      {
        downConfigJson += value[i];
        Serial.print(downConfigJson);
        if (value[i] == 125)
        {

          //保存配置文件
          if (writeProperties(downConfigJson))
          {
            //清空下发数据
            downConfigJson = "";
            rebootSystem();
          }
        }
      }
    }
  }
};

/**
 * 蓝牙重启系统回调方法
 */
class RebootSystemCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      for (int i = 0; i < value.length(); ++i)
      {
        // 1重启系统
        if (value[i] == 0x01)
        {
          rebootSystem();
        }
        else if (value[i] == 0x02)
        {
          // 2 格式化系统
          clearSystemAndReboot();
        }
      }
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks
{
  //蓝牙连接上回调
  void onConnect(BLEServer *pServer)
  {
    bleConnect = true;
    display.setRotation(0);
    display.drawBitmap(full_image_in_config, 0, 0, 128, 296, GxEPD_WHITE);
    display.update();
  };

  //连接断开回调
  void onDisconnect(BLEServer *pServer)
  {
    bleConnect = false;
    //重启系统
    rebootSystem();
  }
};
/**
 * 初始化蓝牙方法
 *
 */
void initBLE()
{
  // 1. 蓝牙广播名字
  BLEDevice::init(BTNAME);
  // 2. 创建蓝牙服务
  BLEServer *pServer = BLEDevice::createServer();
  // 3. 创建蓝牙主服务
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // 4. 创建读取特征
  BLECharacteristic *readCharacteristic = pService->createCharacteristic(
      READ_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  // 5. 创建配置特征
  BLECharacteristic *writeCharacteristic = pService->createCharacteristic(
      WRITE_CONFIG_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  // 6. 重启或格式化系统特征
  BLECharacteristic *writeRebootSystemCharacteristic =
      pService->createCharacteristic(WRITE_REBOOT_SYSTEM_CHARACTERISTIC_UUID,
                                     BLECharacteristic::PROPERTY_WRITE);

  //获取基本信息并返回
  DynamicJsonDocument jsonDocument(100);
  jsonDocument["version"] = "1.0.0";
  jsonDocument["signal"] = 100;
  jsonDocument["power"] = 100;
  String configJsonString = readProperties2();
  DynamicJsonDocument djd(1024);
  deserializeJson(djd, configJsonString);
  JsonObject configJsonObject = djd.as<JsonObject>();
  int type = configJsonObject["type"];
  jsonDocument["type"] = type;

  String rawDataString;
  serializeJson(jsonDocument, rawDataString);
  std::string dataString = std::string(rawDataString.c_str());
  readCharacteristic->setValue(dataString);
  //设置写数据回调
  pServer->setCallbacks(new MyServerCallbacks());
  writeCharacteristic->setCallbacks(new MyCallbacks());
  writeRebootSystemCharacteristic->setCallbacks(new RebootSystemCallback());
  // 7. 启动服务
  pService->start();
  // 8. 设置广播信息
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();
  BLEDevice::startAdvertising();
}

/**
 * 画天气预报页面
 * 
 */
void drawTianqiFrame()
{
  //读取配置
  String configJsonString = readProperties2();
  DynamicJsonDocument djd(1024);
  deserializeJson(djd, configJsonString);
  JsonObject configJsonObject = djd.as<JsonObject>();
  String slogan = configJsonObject["slogan"];
  String ssid = configJsonObject["ssid"];
  String password = configJsonObject["password"];
  String appId = configJsonObject["appId"];
  String secret = configJsonObject["secret"];

  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
  }
  HTTPClient http;
  String serverPath =
      "http://tianqiapi.com/"
      "api?unescape=1&version=v6&appid=" +
      appId + "&appsecret=" + secret;
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  String payload = http.getString();
  DynamicJsonDocument doc1(1024);
  deserializeJson(doc1, payload);
  JsonObject jsonObject = doc1.as<JsonObject>();
  http.end();
  HTTPClient http2;
  String serverPath1 = "http://quan.suning.com/getSysTime.do";
  http2.begin(serverPath1.c_str());
  int httpResponseCode2 = http2.GET();
  String payload2 = http2.getString();
  DynamicJsonDocument doc2(1024);
  deserializeJson(doc2, payload2);
  JsonObject timeJsonObject = doc2.as<JsonObject>();
  http2.end();

  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  Wire.begin();
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);
  sht.read();

  //左上横线
  display.drawLine(0, 16, 50, 16, GxEPD_BLACK);
  display.setCursor(0, 11);
  display.setFont(u8g2_font_wqy12_t_gb2312);
  display.setBackgroundColor(GxEPD_WHITE);
  display.setForegroundColor(GxEPD_BLACK);
  display.print("室外");
  String tem = jsonObject["tem"];
  display.print(tem);
  display.print("°C");

  display.setCursor(95, 16);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(GxEPD_WHITE);
  display.setForegroundColor(GxEPD_BLACK);
  display.print(slogan);

  display.drawLine(246, 16, 296, 16, GxEPD_BLACK);
  display.setCursor(250, 11);
  display.setFont(u8g2_font_wqy12_t_gb2312);
  display.setBackgroundColor(GxEPD_WHITE);
  display.setForegroundColor(GxEPD_BLACK);
  display.print("室外");
  String humidity = jsonObject["humidity"];
  display.print(humidity);

  // 温度
  display.setCursor(0, 70);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.print(sht.getTemperature(), 0);
  display.setCursor(43, 75);
  display.setFont(u8g2_font_helvR08_te);
  display.print("°C");

  display.setCursor(10, 92);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(GxEPD_WHITE);
  display.setForegroundColor(GxEPD_BLACK);
  display.println("温度");

  // 时间
  display.setCursor(52, 89);
  display.setFont(&Dialog_plain_65);
  display.setTextColor(GxEPD_BLACK);
  String timestring = timeJsonObject["sysTime1"];
  display.print(timestring.substring(8, 10));
  display.print(":");
  display.print(timestring.substring(10, 12));

  // 湿度
  display.setCursor(240, 75);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.print(sht.getHumidity(), 0);
  display.setFont(&FreeMonoBold9pt7b);
  display.print("%");
  display.setCursor(252, 92);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(GxEPD_WHITE);
  display.setForegroundColor(GxEPD_BLACK);
  display.println("湿度");

  //左下线
  display.drawLine(5, 105, 85, 105, GxEPD_BLACK);
  display.drawBitmap(icon_battery_1616, 5, 110, 16, 16, GxEPD_WHITE);
  display.setCursor(21, 125);
  display.setFont(&FreeMonoBold9pt7b);
  display.print("100%");

  display.fillRoundRect(98, 105, 100, 22, 10, GxEPD_BLACK);
  display.setCursor(130, 122);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(GxEPD_BLACK);
  display.setForegroundColor(GxEPD_WHITE);
  String wea = jsonObject["wea"];
  display.print(wea);

  display.drawLine(211, 105, 291, 105, GxEPD_BLACK);
  display.drawBitmap(icon_battery_wifi_100_1616, 230, 110, 16, 16, GxEPD_WHITE);
  display.setCursor(250, 125);
  display.setFont(&FreeMonoBold9pt7b);
  display.print("100%");

  if (inited)
  {
    partialTimes++;

    if (partialTimes == 5)
    {
      partialTimes = 0;
      display.update();
    }
    else
    {
      display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
    }
  }
  else
  {
    display.update();
  }
}

/**
 * 画粉丝计数器页面
 * 
 */
void drawFanscountFrame()
{
  //读取配置
  String configJsonString = readProperties2();
  DynamicJsonDocument djd(1024);
  deserializeJson(djd, configJsonString);
  JsonObject configJsonObject = djd.as<JsonObject>();
  String ssid = configJsonObject["ssid"];
  String password = configJsonObject["password"];
  String vmId = configJsonObject["vmId"];
  String appId = configJsonObject["appId"];
  String appToken = configJsonObject["token"];
  //连接WiFi
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
  }

  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);

  display.drawBitmap(icon_flower_128128, 0, 0, 128, 128, GxEPD_WHITE);

  display.fillRect(128, 0, 168, 40, GxEPD_BLACK);
  display.setCursor(133, 26);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(GxEPD_BLACK);
  display.setForegroundColor(GxEPD_WHITE);
  display.println("BLab大熊实验室");

  display.drawBitmap(icon_bilibili_4040, 133, 40, 40, 40, GxEPD_WHITE);

  display.setCursor(180, 66);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  //请求粉丝数
  HTTPClient http1;
  String biliServerPath = "http://api.bilibili.com/x/relation/stat?vmid=" + vmId + "&jsonp=jsonp";
  http1.begin(biliServerPath.c_str());
  http1.GET();
  String payload1 = http1.getString();
  DynamicJsonDocument djd1(500);
  deserializeJson(djd1, payload1);
  JsonObject jsonObject1 = djd1.as<JsonObject>();
  int biliCount = jsonObject1["data"]["follower"];
  display.print(biliCount);

  display.drawLine(128, 84, 296, 84, GxEPD_BLACK);
  display.drawBitmap(icon_baijia_4040, 133, 88, 40, 40, GxEPD_WHITE);

  display.setCursor(180, 120);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  //百家号粉丝数
  HTTPClient http2;
  String baijiaServerPath = "http://baijiahao.baidu.com/builderinner/open/resource/query/fansListall?app_token=" + appToken + "&app_id=" + appId + "&page_no=1&page_size=1";
  http2.begin(baijiaServerPath.c_str());
  http2.GET();
  String payload2 = http2.getString();
  DynamicJsonDocument djd2(500);
  deserializeJson(djd2, payload2);
  JsonObject jsonObject2 = djd2.as<JsonObject>();
  int baijiaCount = jsonObject2["data"]["page"]["total_count"];

  display.print(baijiaCount);
  display.update();

  esp_sleep_enable_timer_wakeup(60 * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void drawPicFrame()
{
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(full_image_cat, 0, 0, 296, 128, GxEPD_WHITE);
  display.update();
}

void drawMainFrame()
{
  display.setRotation(0);
  display.drawBitmap(full_image_main, 0, 0, 128, 296, GxEPD_WHITE);
  display.update();
}

void drawNotConfig()
{
  display.setRotation(0);
  display.drawBitmap(full_image_not_config, 0, 0, 128, 296, GxEPD_WHITE);
  display.update();
}

void disableWiFi()
{
  adc_power_off();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void enableWiFi()
{
  adc_power_on();
  WiFi.disconnect(false); // Reconnect the network
  WiFi.mode(WIFI_STA);    // Switch WiFi off
}
void setup()
{
  Serial.begin(115200);
  //挂载文件系统
  if (SPIFFS.begin(true))
  {
    //初始化蓝牙
    initBLE();
    //初始化标记
    inited = true;
    //初始化屏幕
    display.init();
    //画主图
    drawMainFrame();
    //延迟1.5秒
    delay(1500);
    //读取配置文件
    String configJsonString = readProperties2();
    Serial.println(configJsonString);
    //判断是否配置，未配置则显示未配置
    if (configJsonString != "")
    {
      //解析配置
      DynamicJsonDocument djd(1024);
      deserializeJson(djd, configJsonString);
      JsonObject jsonObject = djd.as<JsonObject>();
      int type = jsonObject["type"];
      if (type == 0)
      {
        drawPicFrame();
      }
      else if (type == 1)
      {
        drawTianqiFrame();
      }
      else if (type == 2)
      {
        drawFanscountFrame();
      }
    }
    else
    {
      //显示未配置
      drawNotConfig();
      //disableWiFi();
      if (!bleConnect)
      {
        // esp_sleep_enable_timer_wakeup(600 * uS_TO_S_FACTOR);
        // esp_deep_sleep_start();
      }
    }
  }
  else
  {
    Serial.println("file system mount fail");
  }
}

void loop() {}

