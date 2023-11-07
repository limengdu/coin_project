#define COIN_PIN D3                  // 投币机连接引脚
#define PHOTO_BTN_PIN D4             // 拍照按钮连接引脚
#define PRINT_BTN_PIN D4             // 打印按钮连接引脚
#define RES_PIN D0                   // 复位引脚

#define DEBUG  1                     // 调试开关

int countcoin = 0;                   // 投币计数

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(COIN_PIN, INPUT_PULLUP);
  pinMode(PHOTO_BTN_PIN, INPUT_PULLUP);
  pinMode(PRINT_BTN_PIN, INPUT_PULLUP);
  pinMode(RES_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RES_PIN), resetDevice, FALLING);  // 按键中断
}

void ICACHE_RAM_ATTR resetDevice() {
  ESP.restart();
}

void loop() {
  // put your main code here, to run repeatedly:
  int coin_vol = digitalRead(COIN_PIN);
  if(coin_vol != 0){
    countcoin++;
    if(DEBUG)Serial.println(String(countcoin));
    if(countcoin == 3){
      Serial.println("start");
      while(1){
//        Serial.println(digitalRead(PHOTO_BTN_PIN));
        if(digitalRead(PHOTO_BTN_PIN) == 0){             // 拍照按键消抖+按下判断
          delay(50);
          if(digitalRead(PHOTO_BTN_PIN) == 0){
            delay(50);
            if(digitalRead(PHOTO_BTN_PIN) == 0){
              Serial.println("photo");
              while(1){
                if(digitalRead(PRINT_BTN_PIN) == 0){             // 打印按键消抖+按下判断
                  delay(50);
                  if(digitalRead(PRINT_BTN_PIN) == 0){
                    delay(50);
                    if(digitalRead(PRINT_BTN_PIN) == 0){
                      Serial.println("print");
                      break;
                    }
                  }
                }
              }
              break;
            }
          }
        }
      }
      countcoin = 0;                                     // 归位，重新投币计算
    }
  }
  delay(20);                                             // 最短消抖时间，勿调
}
