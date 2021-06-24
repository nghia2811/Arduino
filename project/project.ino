#include <SocketIoClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <stdio.h>
//#include <stdlib.h>   //for exit(0), system("cls")
//#include <conio.h>    //For kbhit, getch()
//#include <time.h>     //For clock(),clock_t

SocketIoClient client;
const char* ssid = "Sườn xào chua ngọt";          //Tên mạng Wifi mà Socket server của bạn đang kết nối
const char* password = "28112404";

char host[] = "first-server-nghia.herokuapp.com";  //Địa chỉ IP dịch vụ, hãy thay đổi nó theo địa chỉ IP Socket server của bạn.
//char host[] = "192.168.43.30";
int port = 3000;
char path[] = "/socket.io/?transport=websocket"; // Socket.IO Base Path
bool useSSL = false; // Use SSL Authentication
const char * sslFingerprint = "";  // SSL Certificate Fingerprint

int Speed = 1500;
int motor1_d = D1;
int motor1_a = D2;
int motor2_d = D7;
int motor2_a = D8;

void socket_Connected(const char * payload, size_t length) {
  Serial.println("Socket.IO Connected!");
}

void trailui() {
  analogWrite(motor2_a, Speed);
  digitalWrite(motor2_d, LOW);// chân này không có PWM
}

void phailui() {
  analogWrite(motor1_a, Speed);
  digitalWrite(motor1_d, LOW);// chân này không có PWM
}

void traitien() {
  analogWrite(motor2_a,  1023 - Speed);
  digitalWrite(motor2_d, HIGH);// chân này không có PWM;
}

void phaitien() {
  analogWrite(motor1_a, 1023 - Speed);
  digitalWrite(motor1_d, HIGH);// chân này không có PWM
}

void forward(const char * payload, size_t length) {
  Serial.println("Go ahead");
  traitien();
  phaitien();
}

void backward(const char * payload, size_t length) {
  Serial.println("Go back");
  trailui();
  phailui();
}

void Stop(const char * payload, size_t length) {
  Serial.println("stop");
  digitalWrite(motor1_d, 0);
  digitalWrite(motor1_a, 0);
  digitalWrite(motor2_d, 0);
  digitalWrite(motor2_a, 0);
}

void right(const char * payload, size_t length) {
  Serial.println("right");
  traitien();
  phailui();
}

void left(const char * payload, size_t length) {
  Serial.println("left");
  trailui();
  phaitien();
}
//
//void delay(int ms)  //delay function
//{
//    clock_t timeDelay = ms + clock();    //Step up the difference from clock delay
//    while (timeDelay > clock());         //stop when the clock is higher than time delay
//}
//
//int counter(int value){
//    while(!kbhit() && second <= value){     //keep looping while the user didn't hit any key and flag is 0
//            forward();           //print out the new data, delay for 1000 millisecond and increase 1 second.
//            delay(1000);
//            second += 1;
//        }
//}

void getJson(const char * payload, size_t length) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload);

  // Test if parsing succeeds.
  if (error) {
    Serial.println(F("deserializeJson() failed!"));
    return;
  }

  // Fetch values.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do root["time"].as<long>();
  int huongdi = doc["DirectionCode"];
  int khoangcach = doc["Distance"];
  //  const char* nguoigui = doc["CreatedBy"];

  // Print values.
  Serial.print("Hướng đi: ");
  Serial.println(huongdi);
  Serial.print("Khoảng cách: ");
  Serial.println(khoangcach);
  //  Serial.println(nguoigui);

  //    while(1){             //keep the program running and never end
  //    counter(3);
  //    }
}

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(50);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }

  Serial.println("Connected");
  Serial.println(F("Dia chi IP cua ESP8266 (Socket Client ESP8266): "));
  Serial.println(WiFi.localIP());


  // Setup Connection
  if (useSSL) {
    client.beginSSL(host, port, path, sslFingerprint);
  } else {
//    client.begin(host, port, path);  //kết nối server cục bộ, thay host = 192.168.x.x tùy theo địa chỉ ip
          client.begin(host);              //kết nối server heroku
    Serial.println("Ket noi thanh cong");
  }

  delay(1000);
  client.emit("connection", "\"ESP8266 connected !!!!\"");
  client.emit("get-commands", "\"Get commands from Esp8266\"");
  client.on("welcome", socket_Connected);
  client.on("go-ahead", forward);
  client.on("go-back", backward);
  client.on("left", left);
  client.on("right", right);
  client.on("stop", Stop);
  client.on("commands-from-user", getJson);
  client.on("send-commands-from-server", getJson);
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
}
