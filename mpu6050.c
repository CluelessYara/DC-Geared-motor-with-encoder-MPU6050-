#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // العنوان الافتراضي لشاشة LCD

// تعريف المنافذ
#define ENA 9         // Pin للتحكم في السرعة (PWM)
#define IN1 7         // Pin للتحكم باتجاه المحرك
#define IN2 6         // Pin للتحكم باتجاه المحرك
//#define JOYSTICK A0   // Pin لقراءة الجويستيك أو البوتنشيوميتر
#define ENCODER_A 3   // Pin لقناة A من الـ Encoder (Interrupt)
#define ENCODER_B 2   // Pin لقناة B من الـ Encoder

const int JOYSTICK = A0;  // محور X
volatile int position = 0;  // الموضع الحالي للمحرك
int speedValue = 0;         // سرعة المحرك
String direction = "STOP";  // اتجاه دوران المحرك

void setup() {
  // إعداد المنافذ
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(JOYSTICK, INPUT);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  // تفعيل Interrupt على Channel A
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), readEncoder, CHANGE);

  // تهيئة الاتصال التسلسلي
  Serial.begin(9600);

  // تهيئة شاشة LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
}

void loop() {
  // قراءة الجويستيك أو البوتنشيوميتر
  int joystickValue = analogRead(JOYSTICK);

  // تحويل القيمة إلى سرعة واتجاه
  if (joystickValue > 530) { // مع عقارب الساعة
    direction = "CW";
    speedValue = map(joystickValue, 530, 1023, 0, 255);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (joystickValue < 490)
  { // عكس عقارب الساعة
    direction = "CCW";
    speedValue = map(joystickValue, 490, 0, 0, 255);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else { // توقف
    direction = "STOP";
    speedValue = 0;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  // تعيين سرعة المحرك باستخدام PWM
  analogWrite(ENA, speedValue);

  // عرض البيانات على شاشة LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mot: ");
  lcd.print(direction);
  lcd.setCursor(0, 1);
  lcd.print("Pos: ");
  lcd.print(position);

  // طباعة البيانات إلى Serial Monitor
  Serial.print("Speed: ");
  Serial.print(speedValue);
  Serial.print(" Direction: ");
  Serial.print(direction);
  Serial.print(" Position: ");
  Serial.println(position);

  delay(200); // تحديث كل 200ms
}

// وظيفة Interrupt لقراءة بيانات الـ Encoder
void readEncoder() {
  // قراءة حالة Channel A و Channel B
  int stateA = digitalRead(ENCODER_A);
  int stateB = digitalRead(ENCODER_B);

  // تحديث الموضع بناءً على حالة Channel A و Channel B
  if (stateA == stateB) {
    position++;  // اتجاه مع عقارب الساعة (CW)
  } else {
    position--;  // اتجاه عكس عقارب الساعة (CCW)
  }
}
