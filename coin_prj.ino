#define COIN_PIN D3                  // 投币机连接引脚
#define PHOTO_BTN_PIN D4             // 拍照按钮连接引脚
#define PRINT_BTN_PIN D4             // 打印按钮连接引脚
#define RES_PIN D0                   // 复位引脚

#define DEBUG  1                     // 调试开关

int countcoin = 0;                   // 投币计数
const int DEBOUNCE_TIME = 50;        // 消抖时间

TaskHandle_t ListenTask;             // FreeRTOS 任务，监听串口消息
bool resetFlag = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(COIN_PIN, INPUT_PULLUP);
  pinMode(PHOTO_BTN_PIN, INPUT_PULLUP);
  pinMode(PRINT_BTN_PIN, INPUT_PULLUP);
  pinMode(RES_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RES_PIN), resetDevice, FALLING);  // 按键中断

  // 创建一个新任务来监听串口
  xTaskCreate(
    listenForFinish,    // 任务函数
    "ListenForFinish",  // 任务名称
    10000,              // 栈大小
    NULL,               // 任务参数
    1,                  // 任务优先级
    &ListenTask         // 任务句柄
  );
}

void ICACHE_RAM_ATTR resetDevice() {
  ESP.restart();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(COIN_PIN) != 0) {
    countcoin++;
    resetFlag = false;
    if (DEBUG) {
      Serial.println(String(countcoin));
    }
    if (countcoin == 3) {
      Serial.println("start");
      if (waitForButton(PHOTO_BTN_PIN)) {
        Serial.println("photo");
        if (waitForButton(PRINT_BTN_PIN)) {
          Serial.println("print");
          countcoin = 0;                          // 归位，重新投币计算
        }
      }
    }
  }
  delay(20);   // 最短消抖时间，勿调
}

// 任务函数，用于监听串口
void listenForFinish(void* parameter) {
  while (1) {
    if (Serial.available()) {
      String input = Serial.readString();
      input.trim();  // 去除可能的前后空格
      if (input == "finish") {
        Serial.println("finish");
        countcoin = 0;
        resetFlag = true;
      }
    }
    vTaskDelay(100);  // 让出 CPU，使得其他任务可以运行
  }
}

bool waitForButton(int pin) {
  while (!resetFlag) {
    if (digitalRead(pin) == 0) {
      delay(DEBOUNCE_TIME);                              // 按键消抖+按下判断
      if (digitalRead(pin) == 0) {
        delay(DEBOUNCE_TIME);
        if (digitalRead(pin) == 0) {
          return true;
        }
      }
    }
  }
  return false;
}
