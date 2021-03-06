# 1 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
# 2 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 3 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 4 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 5 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
#define SHT31_ADDRESS 0x44
# 7 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 8 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 9 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
// FreeFonts from Adafruit_GFX
# 11 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 12 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 13 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 14 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
GxIO_SPI io(SPI, SS, 17, 16);
GxGDEH029A1 display(io, 16, 4);
SHT31 sht;
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60
__attribute__((section(".rtc.data" "." "16"))) bool inited = false;
__attribute__((section(".rtc.data" "." "17"))) int partialTimes = 0;

__attribute__((section(".rtc.data" "." "18"))) char tempArray[4];
__attribute__((section(".rtc.data" "." "19"))) char humidityArray[4];
__attribute__((section(".rtc.data" "." "20"))) char weaArray[8];
__attribute__((section(".rtc.data" "." "21"))) char sloganArray[50];

# 28 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 29 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 30 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 31 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 32 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2

// ่็ble
# 35 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 36 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 37 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2

#define BTNAME "BLabSmartDisplay"
#define SERVICE_UUID "71db3107-6be5-4c0b-ad8e-731df7069e62"
#define READ_CHARACTERISTIC_UUID "8b3aed0e-a5a0-4e7f-9965-d0f3d26a1b36"
#define WRITE_CONFIG_CHARACTERISTIC_UUID "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a8"
#define WRITE_REBOOT_SYSTEM_CHARACTERISTIC_UUID "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a9"

String downConfigJson = "";
# 46 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 47 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 48 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2

//็ณป็ปๆฏๅฆ้็ฝฎๆ?่ฏ
boolean systemConfigured = false;
JsonObject configJsonObject;
std::string version = "1.0.0";

DynamicJsonDocument doc(1024);

boolean bleConnect = false;
/**

 * ่ฝฏไปถ้ๅฏๆนๆณ

 */
# 60 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void rebootSystem()
{
  delay(500);
  ESP.restart();
}
/**

 * ๆธ็ฉบๅ?้ค้็ฝฎๆไปถ

 */
# 68 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void clearProperties()
{
  SPIFFS.remove("/config.json");
}
/**

 * ๆธ็ฉบ้็ฝฎๆไปถๅนถ้ๅฏ็ณป็ป

 * ๆณจๆ๏ผ่ฟไธชๆนๆณๅฟ้กป่ฆๅจๆไปถ็ณป็ปๆ่ฝฝไปฅๅๆ่ฝ่ฐ็จ

 */
# 76 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void clearSystemAndReboot()
{
  clearProperties();
  rebootSystem();
}
/**

 * ่ฏปๅ้็ฝฎๆไปถ,ๅๆถๅฐ้็ฝฎๆไปถ่งฃๆๅบๆฅ่ตๅผ็ปๅจๅฑๅ้

 *

 * ๅฆๆ้็ฝฎๆไปถๅญๅจๅ่งฃๆๅบ้็ฝฎ๏ผๅฆๆ้็ฝฎๆไปถไธๅญๅจๆ็คบ็จๆท้็ฝฎ

 *

 */
# 87 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void readProperties()
{
  //ๆฃๆฅๆไปถๆฏๅฆๅญๅจ
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
  //ๆฃๆฅๆไปถๆฏๅฆๅญๅจ
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

 * ๅฐๆฐๆฎไฟๅญ่ณ้็ฝฎๆไปถไธญ

 *

 * @param jsonstring

 */
# 122 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
boolean writeProperties(String jsonstring)
{
  File file = SPIFFS.open("/config.json", "w");
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

// 3. ๅฎไนๅๆฐๆฎๅ่ฐๅฝๆฐ
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    Serial.print(value.c_str());
    if (value.length() > 0)
    {
      for (int i = 0; i < value.length(); i++)
      {
        downConfigJson += value[i];
        if (value[i] == 125)
        {

          //ไฟๅญ้็ฝฎๆไปถ
          if (writeProperties(downConfigJson))
          {
            //ๆธ็ฉบไธๅๆฐๆฎ
            downConfigJson = "";
            rebootSystem();
          }
        }
      }
    }
  }
};

/**

 * ่็้ๅฏ็ณป็ปๅ่ฐๆนๆณ

 */
# 185 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
class RebootSystemCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      for (int i = 0; i < value.length(); ++i)
      {
        // 1้ๅฏ็ณป็ป
        if (value[i] == 0x01)
        {
          rebootSystem();
        }
        else if (value[i] == 0x02)
        {
          // 2 ๆ?ผๅผๅ็ณป็ป
          clearSystemAndReboot();
        }
      }
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks
{
  //่็่ฟๆฅไธๅ่ฐ
  void onConnect(BLEServer *pServer)
  {
    bleConnect = true;
    display.setRotation(0);
    display.drawBitmap(full_image_in_config, 0, 0, 128, 296, 0xFFFF);
    display.update();
  };

  //่ฟๆฅๆญๅผๅ่ฐ
  void onDisconnect(BLEServer *pServer)
  {
    bleConnect = false;
    //้ๅฏ็ณป็ป
    rebootSystem();
  }
};
/**

 * ๅๅงๅ่็ๆนๆณ

 *

 */
# 232 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void initBLE()
{
  // 1. ่็ๅนฟๆญๅๅญ
  BLEDevice::init("BLabSmartDisplay");
  // 2. ๅๅปบ่็ๆๅก
  BLEServer *pServer = BLEDevice::createServer();
  // 3. ๅๅปบ่็ไธปๆๅก
  BLEService *pService = pServer->createService("71db3107-6be5-4c0b-ad8e-731df7069e62");
  // 4. ๅๅปบ่ฏปๅ็นๅพ
  BLECharacteristic *readCharacteristic = pService->createCharacteristic(
      "8b3aed0e-a5a0-4e7f-9965-d0f3d26a1b36", BLECharacteristic::PROPERTY_READ);
  // 5. ๅๅปบ้็ฝฎ็นๅพ
  BLECharacteristic *writeCharacteristic = pService->createCharacteristic(
      "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a8", BLECharacteristic::PROPERTY_WRITE);
  // 6. ้ๅฏๆๆ?ผๅผๅ็ณป็ป็นๅพ
  BLECharacteristic *writeRebootSystemCharacteristic =
      pService->createCharacteristic("2bbd7b92-5bc8-441c-a69a-5764d4f3d1a9",
                                     BLECharacteristic::PROPERTY_WRITE);

  //่ทๅๅบๆฌไฟกๆฏๅนถ่ฟๅ
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
  //่ฎพ็ฝฎๅๆฐๆฎๅ่ฐ
  pServer->setCallbacks(new MyServerCallbacks());
  writeCharacteristic->setCallbacks(new MyCallbacks());
  writeRebootSystemCharacteristic->setCallbacks(new RebootSystemCallback());
  // 7. ๅฏๅจๆๅก
  pService->start();
  // 8. ่ฎพ็ฝฎๅนฟๆญไฟกๆฏ
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("71db3107-6be5-4c0b-ad8e-731df7069e62");
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();
  BLEDevice::startAdvertising();
}

/**

 * ็ปๅคฉๆฐ้ขๆฅ้กต้ข

 * 

 */
# 287 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawTianqiFrame()
{

  //้่ฆ่ฏทๆฑ็ฝ็ป๏ผ้ซ่่ฝ
  if (partialTimes == 0)
  {
    adc_power_on();
    WiFi.disconnect(false);
    WiFi.mode(WIFI_MODE_STA);
    //่ฏปๅ้็ฝฎ
    String configJsonString = readProperties2();
    DynamicJsonDocument djd(1024);
    deserializeJson(djd, configJsonString);
    JsonObject configJsonObject = djd.as<JsonObject>();
    String configSlogan = configJsonObject["slogan"];
    configSlogan.toCharArray(sloganArray, 50);
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
    //ไธญๅฝๅจไธๅซๅบๆถ้ด่ฆๅ?8ๅฐๆถ
    configTime(8 * 60 * 60, 0, "pool.ntp.org");

    String rtem = jsonObject["tem"];
    rtem.toCharArray(tempArray, 4);
    String rhumidity = jsonObject["humidity"];
    rhumidity.toCharArray(humidityArray, 4);
    String rwea = jsonObject["wea"];
    rwea.toCharArray(weaArray, 8);
  }
  else
  {
    //ๅณ้ญWiFiๅ่ฝ็็ต
    adc_power_off();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_MODE_NULL);
  }

  display.setRotation(1);
  display.fillScreen(0xFFFF);
  Wire.begin();
  sht.begin(0x44);
  Wire.setClock(100000);
  sht.read();

  //ๅทฆไธๆจช็บฟ
  display.drawLine(0, 16, 50, 16, 0x0000);
  display.setCursor(0, 11);
  display.setFont(u8g2_font_wqy12_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.print("ๅฎคๅค");
  //่ทๅRTCไธญ็็ผๅญๆฐๆฎ
  display.print(String(tempArray));
  display.print("ยฐC");

  display.setCursor(95, 16);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.print(String(sloganArray));

  display.drawLine(246, 16, 296, 16, 0x0000);
  display.setCursor(250, 11);
  display.setFont(u8g2_font_wqy12_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.print("ๅฎคๅค");
  display.print(String(humidityArray));

  // ๆธฉๅบฆ
  display.setCursor(0, 70);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(0x0000);
  display.print(sht.getTemperature(), 0);
  display.setCursor(43, 75);
  display.setFont(u8g2_font_helvR08_te);
  display.print("ยฐC");

  display.setCursor(10, 92);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.println("ๆธฉๅบฆ");

  // ๆถ้ด
  display.setCursor(52, 89);
  display.setFont(&Dialog_plain_65);
  display.setTextColor(0x0000);
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  int hour = timeinfo.tm_hour;
  int minu = timeinfo.tm_min;
  display.print(hour);
  display.print(":");
  display.print(minu);

  // ๆนฟๅบฆ
  display.setCursor(240, 75);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(0x0000);
  display.print(sht.getHumidity(), 0);
  display.setFont(&FreeMonoBold9pt7b);
  display.print("%");
  display.setCursor(252, 92);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.println("ๆนฟๅบฆ");

  //ๅทฆไธ็บฟ
  display.drawLine(5, 105, 85, 105, 0x0000);
  display.drawBitmap(icon_battery_1616, 5, 110, 16, 16, 0xFFFF);
  display.setCursor(21, 125);
  display.setFont(&FreeMonoBold9pt7b);
  display.print("100%");

  display.fillRoundRect(98, 105, 100, 22, 10, 0x0000);

  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0x0000);
  display.setForegroundColor(0xFFFF);
  String realWeather = String(weaArray);
  Serial.print(realWeather.length());
  if (realWeather.length() == 3)
  {
    display.setCursor(140, 122);
  }
  else
  {
    display.setCursor(132, 122);
  }
  display.print(realWeather);

  display.drawLine(211, 105, 291, 105, 0x0000);
  display.drawBitmap(icon_battery_wifi_100_1616, 230, 110, 16, 16, 0xFFFF);
  display.setCursor(250, 125);
  display.setFont(&FreeMonoBold9pt7b);
  display.print("100%");

  //ไบๅ้ไธๆฌกๅจๅฑๅทๆฐ
  if (partialTimes == 0)
  {
    display.update();
  }
  else
  {
    display.updateWindow(0, 0, 128, 296, false);
  }
  //็ฌฌ5ๅ้็ฝฎไธบ0
  if (partialTimes == 4)
  {
    partialTimes = 0;
  }
  else
  {
    partialTimes++;
  }

  if (!bleConnect)
  {
    esp_sleep_enable_timer_wakeup(60 * 1000000ULL);
    esp_deep_sleep_start();
  }
}

/**

 * ็ป็ฒไธ่ฎกๆฐๅจ้กต้ข

 * 

 */
# 475 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawFanscountFrame()
{

  if (partialTimes == 0)
  {
    //่ฏปๅ้็ฝฎ
    String configJsonString = readProperties2();
    DynamicJsonDocument djd(1024);
    deserializeJson(djd, configJsonString);
    JsonObject configJsonObject = djd.as<JsonObject>();
    String brand = configJsonObject["brand"];
    String ssid = configJsonObject["ssid"];
    String password = configJsonObject["password"];
    String vmId = configJsonObject["vmId"];
    String appId = configJsonObject["appId"];
    String appToken = configJsonObject["token"];
    //่ฟๆฅWiFi
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(200);
    }

    display.setRotation(1);
    display.fillScreen(0xFFFF);

    display.drawBitmap(icon_flower_128128, 0, 0, 128, 128, 0xFFFF);

    display.fillRect(128, 0, 168, 40, 0x0000);
    display.setCursor(133, 26);
    display.setFont(u8g2_font_wqy16_t_gb2312);
    display.setBackgroundColor(0x0000);
    display.setForegroundColor(0xFFFF);
    display.print(brand);

    display.drawBitmap(icon_bilibili_4040, 133, 40, 40, 40, 0xFFFF);

    display.setCursor(180, 66);
    display.setFont(&FreeMonoBold18pt7b);
    display.setTextColor(0x0000);
    //่ฏทๆฑ็ฒไธๆฐ
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

    display.drawLine(128, 84, 296, 84, 0x0000);
    display.drawBitmap(icon_baijia_4040, 133, 88, 40, 40, 0xFFFF);

    display.setCursor(180, 120);
    display.setFont(&FreeMonoBold18pt7b);
    display.setTextColor(0x0000);
    //็พๅฎถๅท็ฒไธๆฐ
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
  }

  //็ฌฌ5ๅ้็ฝฎไธบ0
  if (partialTimes == 4)
  {
    partialTimes = 0;
  }
  else
  {
    partialTimes++;
  }

  if (!bleConnect)
  {
    esp_sleep_enable_timer_wakeup(600 * 1000000ULL);
    esp_deep_sleep_start();
  }
}

void drawPicFrame()
{
  display.setRotation(1);
  display.fillScreen(0xFFFF);
  display.drawBitmap(full_image_cat, 0, 0, 296, 128, 0xFFFF);
  display.update();
}

void drawMainFrame()
{
  display.setRotation(0);
  display.drawBitmap(full_image_main, 0, 0, 128, 296, 0xFFFF);
  display.update();
}

void drawNotConfig()
{
  display.setRotation(0);
  display.drawBitmap(full_image_not_config, 0, 0, 128, 296, 0xFFFF);
  display.update();
}

void disableWiFi()
{
  adc_power_off();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_MODE_NULL);
}

void enableWiFi()
{
  adc_power_on();
  WiFi.disconnect(false); // Reconnect the network
  WiFi.mode(WIFI_MODE_STA); // Switch WiFi off
}
void setup()
{
  Serial.begin(115200);
  //ๆ่ฝฝๆไปถ็ณป็ป
  if (SPIFFS.begin(true))
  {
    //ๅๅงๅ่็
    initBLE();
    //ๅๅงๅๅฑๅน
    display.init();
    if (!inited)
    {
      //็ปไธปๅพ
      drawMainFrame();
      //ๅปถ่ฟ1.5็ง
      delay(1500);
      //ๅๅงๅๆ?่ฎฐ
      inited = true;
    }

    //่ฏปๅ้็ฝฎๆไปถ
    String configJsonString = readProperties2();
    Serial.println(configJsonString);
    //ๅคๆญๆฏๅฆ้็ฝฎ๏ผๆช้็ฝฎๅๆพ็คบๆช้็ฝฎ
    if (configJsonString != "")
    {
      //่งฃๆ้็ฝฎ
      DynamicJsonDocument djd(2048);
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
      //ๆพ็คบๆช้็ฝฎ
      drawNotConfig();
      disableWiFi();
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
