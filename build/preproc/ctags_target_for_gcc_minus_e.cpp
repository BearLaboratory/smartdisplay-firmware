# 1 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
# 2 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 3 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 4 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 5 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 6 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
#define SHT31_ADDRESS 0x44
# 8 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 9 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 10 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
// FreeFonts from Adafruit_GFX
# 12 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 13 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 14 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 15 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
GxIO_SPI io(SPI, SS, 17, 16);
GxGDEH029A1 display(io, 16, 4);
SHT31 sht;
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP 60
__attribute__((section(".rtc.data" "." "16"))) bool inited = false;
__attribute__((section(".rtc.data" "." "17"))) int partialTimes = 0;

# 24 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 25 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 26 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2

const char *ssid = "PandoraBox";
const char *password = "dengyi1991";

// 蓝牙ble
# 32 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 33 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 34 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2

#define BTNAME "BLabSmartDisplay"
#define SERVICE_UUID "71db3107-6be5-4c0b-ad8e-731df7069e62"
#define READ_CHARACTERISTIC_UUID "8b3aed0e-a5a0-4e7f-9965-d0f3d26a1b36"
#define WRITE_CONFIG_CHARACTERISTIC_UUID "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a8"
#define WRITE_REBOOT_SYSTEM_CHARACTERISTIC_UUID "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a9"

String downConfigJson;
String readConfigJson;
# 44 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2
# 45 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino" 2

//系统是否配置标识
boolean systemConfigured = false;
JsonObject configJsonObject;
std::string version = "1.0.0";
std::string signal = "100";
std::string power = "100";

DynamicJsonDocument doc(1024);
/**

 * 软件重启方法

 */
# 57 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void rebootSystem()
{
  ESP.restart();
}
/**

 * 清空删除配置文件

 */
# 64 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void clearProperties()
{
  SPIFFS.remove("/config.json");
}
/**

 * 清空配置文件并重启系统

 * 注意：这个方法必须要在文件系统挂载以后才能调用

 */
# 72 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
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
# 83 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
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
    File file = SPIFFS.open("/config.json", "r");
    data = file.readString();
  }
  return data;
}
/**

 * 将数据保存至配置文件中

 *

 * @param jsonstring

 */
# 111 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
boolean writeProperties(String jsonstring)
{
  File file = SPIFFS.open("/config.json", "w");
  boolean status = false;
  if (file && file.print(jsonstring))
  {
    status = true;
  }
  file.close();
  return status;
}

// 3. 定义写数据回调函数
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      for (int i = 0; i < value.length(); ++i)
      {
        downConfigJson += value[i];
        if (value[i] == 125)
        {

          //保存配置文件
          // writeProperties(downConfigJson);
          //清空下发数据
          downConfigJson = "";
          rebootSystem();
        }
      }
    }
  }
};

/**

 * 蓝牙重启系统回调方法

 */
# 151 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
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
    display.setRotation(0);
    display.drawBitmap(full_image_in_config, 0, 0, 128, 296, 0xFFFF);
    display.update();
  };

  //连接断开回调
  void onDisconnect(BLEServer *pServer)
  {
    //重启系统
    rebootSystem();
  }
};
/**

 * 初始化蓝牙方法

 *

 */
# 196 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void initBLE()
{
  // 1. 蓝牙广播名字
  BLEDevice::init("BLabSmartDisplay");
  // 2. 创建蓝牙服务
  BLEServer *pServer = BLEDevice::createServer();

  // 3. 创建蓝牙主服务
  BLEService *pService = pServer->createService("71db3107-6be5-4c0b-ad8e-731df7069e62");
  // 4. 创建读取特征
  BLECharacteristic *readCharacteristic = pService->createCharacteristic(
      "8b3aed0e-a5a0-4e7f-9965-d0f3d26a1b36", BLECharacteristic::PROPERTY_READ);
  // 5. 创建配置特征
  BLECharacteristic *writeCharacteristic = pService->createCharacteristic(
      "2bbd7b92-5bc8-441c-a69a-5764d4f3d1a8", BLECharacteristic::PROPERTY_WRITE);
  // 6. 重启或格式化系统特征
  BLECharacteristic *writeRebootSystemCharacteristic =
      pService->createCharacteristic("2bbd7b92-5bc8-441c-a69a-5764d4f3d1a9",
                                     BLECharacteristic::PROPERTY_WRITE);

  //获取基本信息并返回
  DynamicJsonDocument jsonDocument(1024);
  jsonDocument["version"] = "1.0.0";
  jsonDocument["signal"] = 100;
  jsonDocument["power"] = 100;
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
  pAdvertising->addServiceUUID("71db3107-6be5-4c0b-ad8e-731df7069e62");
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

/**

 * 画天气预报页面

 * 

 */
# 244 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawTianqiFrame()
{
  //读取配置
  String configJsonString = readProperties2();
  DynamicJsonDocument djd(1024);
  deserializeJson(djd, configJsonString);
  JsonObject configJsonObject = djd.as<JsonObject>();
  String slogan = jsonObject["slogan"];
  String ssid = jsonObject["ssid"];
  String password = jsonObject["password"];
  String appId = jsonObject["appId"];
  String secret = jsonObject["secret"];

  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  HTTPClient http;
  // String serverPath =
  //     "http://tianqiapi.com/"
  //     "api?unescape=1&version=v6&appid=19631362&appsecret=GyUkqDo3";
  String serverPath =
      "http://tianqiapi.com/"
      "api?unescape=1&version=v6&appid="+appId+"&appsecret="+secret;
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
  display.fillScreen(0xFFFF);
  Wire.begin();
  sht.begin(0x44);
  Wire.setClock(100000);
  sht.read();

  //左上横线
  display.drawLine(0, 16, 50, 16, 0x0000);
  display.setCursor(0, 11);
  display.setFont(u8g2_font_wqy12_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.print("室外");
  String tem = jsonObject["tem"];
  display.print(tem);
  display.print("°C");

  display.setCursor(95, 16);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.print(slogan);

  display.drawLine(246, 16, 296, 16, 0x0000);
  display.setCursor(250, 11);
  display.setFont(u8g2_font_wqy12_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.print("室外");
  String humidity = jsonObject["humidity"];
  display.print(humidity);

  // 温度
  display.setCursor(0, 70);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(0x0000);
  display.print(sht.getTemperature(), 0);
  display.setCursor(43, 75);
  display.setFont(u8g2_font_helvR08_te);
  display.print("°C");

  display.setCursor(10, 92);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0xFFFF);
  display.setForegroundColor(0x0000);
  display.println("温度");

  // 时间
  display.setCursor(52, 89);
  display.setFont(&Dialog_plain_65);
  display.setTextColor(0x0000);
  String timestring = timeJsonObject["sysTime1"];
  display.print(timestring.substring(8, 10));
  display.print(":");
  display.print(timestring.substring(10, 12));

  // 湿度
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
  display.println("湿度");

  //左下线
  display.drawLine(5, 105, 85, 105, 0x0000);
  display.drawBitmap(icon_battery_1616, 5, 110, 16, 16, 0xFFFF);
  display.setCursor(21, 125);
  display.setFont(&FreeMonoBold9pt7b);
  display.print("100%");

  display.fillRoundRect(98, 105, 100, 22, 10, 0x0000);
  display.setCursor(130, 122);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0x0000);
  display.setForegroundColor(0xFFFF);
  String wea = jsonObject["wea"];
  display.print(wea);

  display.drawLine(211, 105, 291, 105, 0x0000);
  display.drawBitmap(icon_battery_wifi_100_1616, 230, 110, 16, 16, 0xFFFF);
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
      display.updateWindow(0, 0, 128, 296, false);
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
# 401 "c:\\Users\\BLab\\Desktop\\smartdisplay\\smartdisplay.ino"
void drawFanscountFrame()
{
  display.setRotation(1);
  display.fillScreen(0xFFFF);

  display.drawBitmap(icon_flower_128128, 0, 0, 128, 128, 0xFFFF);

  display.fillRect(128, 0, 168, 40, 0x0000);
  display.setCursor(133, 26);
  display.setFont(u8g2_font_wqy16_t_gb2312);
  display.setBackgroundColor(0x0000);
  display.setForegroundColor(0xFFFF);
  display.println("BLab大熊实验室");

  display.drawBitmap(icon_bilibili_4040, 133, 40, 40, 40, 0xFFFF);

  display.setCursor(180, 66);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(0x0000);
  display.print(5362);

  display.drawLine(128, 84, 296, 84, 0x0000);
  display.drawBitmap(icon_baijia_4040, 133, 88, 40, 40, 0xFFFF);

  display.setCursor(180, 120);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(0x0000);
  display.print(9527);
  display.update();
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
void setup()
{
  Serial.begin(115200);
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
  String configJsonString = readProperties2();
  if (configJsonString != "")
  {
    //解析配置
    DynamicJsonDocument djd(1024);
    deserializeJson(djd, configJsonString);
    JsonObject jsonObject = djd.as<JsonObject>();
    int type = jsonObject["type"];
    if (type == 1)
    {
      drawPicFrame();
    }
    else if (type == 2)
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
  }
}

void loop() {}