/*
  Trac interpreter

 "Железное" исполнение интерпретатора Trac

 Железо: модуль ESP8266 (ESP-12x)с минимальной обвязкой.
 Например NodeMCU 0.9, 1.0, WeMos D1, "Бутерброд" и другие


 создано 25 Июня 2016
 последнее изменение 1 Июля 2016
 Юрий Ботов

 Код - public domain.

 */

#include <FS.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "trac.h"
#include "lib.h"


// Таймер используемый для сброса неактивных сессий
Ticker sec;
uint32_t seccounter = 0;

void newsec() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  seccounter++;
}

// Запуск WiFi
// раскомментируйте ssid и password и заполните своими данными, а #include "d:\wifipass.h" - закомментируйте или удалите
//const char* ssid = "********";
//const char* password = "********";
#include "d:\wifipass.h" // мои ssid и password

WiFiServer TelnetServer(23);
WiFiClient Telnet;

// Trac интерпретатор
extern const char NEXTPARAM;
extern const char ACTIVEFUNC;
extern const char NEYTRALFUNC;
extern const char ENDFUNC;
extern char* idle;
extern char meta;
extern bool z;
extern bool runned;
extern RingBuf& Ifile;
extern RingBuf& A;
extern RingBuf& N;
extern RingBuf& Ofile;
extern RingBuf& F;
extern bool isready;
extern struct form forms[255];
extern uint8_t formlength;
extern struct user users[MAX_SRV_CLIENTS];
extern uint8_t curuser;

// system startup
void setup() {
  // init Serial
  Serial.begin(115200);  
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
    delay(300);
  }
  Serial.setTimeout(60000);
  delay(300);
  Serial.println("\nESP8266 Trac interpreter");
  Serial.println("\nInit Serial as 115200 baud");
  // init ticker
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  sec.attach(1.,newsec);
  Serial.println("\nInit sec counter (see led)");
  // init FS
  Serial.print("Initializing SPIFFS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  // при первом старте раскомментируйте, пусть отформатируется, потом опять закомментируйте
  //if(SPIFFS.format()) { Serial.println("SPIFFS formatted"); } else { Serial.println("SPIFFS format error"); return;}
  struct FSInfo finfo;
  SPIFFS.info(finfo);
  Serial.println("");  Serial.print(finfo.totalBytes);  Serial.println(": total bytes in SPIFFS");
  Serial.print(finfo.usedBytes); Serial.println(": used bytes in SPIFFS");
  // init WiFi telnet
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting WIFI to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) { Serial.print("."); delay(500);}
  if(i == 21){
    Serial.print("Could not connect to"); Serial.println(ssid);
    while(1) delay(500);
  }
  TelnetServer.begin();
  TelnetServer.setNoDelay(true);
  Serial.print("Connected. Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(":23' to connect");
  // системный пользователь tracsys
  users[0].name = "tracsys";
  users[0].status = 'a';
  users[0].imode = 't';
  users[0].omode = 'n';
  users[0].ifile = NULL;
  users[0].ofile = NULL;
  users[0].lastuse = seccounter;
  users[0].I = new RingBuf(512);
  users[0].O = NULL;
  Serial.println("Create system user: tracsys");
  // локальный пользователь local
  users[1].name = "local";
  users[1].status = 'a';
  users[1].imode = 't';
  users[1].omode = 't';
  users[1].ifile = NULL;
  users[1].ofile = NULL;
  users[1].lastuse = seccounter;
  users[1].I = new RingBuf(1024);
  users[1].O = new RingBuf(1024);
  Serial.println("Create local user: local");
  // сетевой пользователь net
  users[2].name = "net";
  users[2].status = 'a';
  users[2].imode = 't';
  users[2].omode = 't';
  users[2].ifile = NULL;
  users[2].ofile = NULL;
  users[2].lastuse = seccounter;
  users[2].I = new RingBuf(1024);
  users[2].O = new RingBuf(1024);
  Serial.println("Create net user: net");
  A.push(idle);
  z = false;
  // поехали!
  Serial.print("\nReady to connecting...\n\n");
  Serial.print("\nlocal:"); Serial.print(ESP.getFreeHeap());Serial.print(":>");
}

// обработчики прерываний
/*
если пришло прерывание - положить что-то в users[0].I->push("что-то");
*/

bool ifmeta() {
  for(int i = 0; i < MAX_SRV_CLIENTS; i++) {
    if(users[i].status == 'a' && users[i].I->indexof(meta) != -1) { return true;}
  }
  return false;
}

// main arduino loop
void loop() {
  char* ptr;
  // обнаруживаем новичков в сети
  if (TelnetServer.hasClient()){
    if (!Telnet || !Telnet.connected()){
      if(Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
      if (Telnet && Telnet.connected()){Telnet.print("Trac interpreter\n\r"); Telnet.print("\n\rnet:"); Telnet.print(ESP.getFreeHeap());Telnet.print(":>");}
    } else {
      TelnetServer.available().stop();
    }
  }
  // если что то есть на устройстве ввода - поместить это во входной буфер
  for(int i = 1; i < MAX_SRV_CLIENTS; i++) {
    if(users[i].status == 'a') {
      if(i == 1) {
        while (Serial.available()) {
          users[i].I->push((char)Serial.read());   
        }
      } else {
        while(Telnet.available()) {
          users[i].I->push(Telnet.read());
        }
      }
    }
  }
  // проверяем состояние интерпретатора: работает или ожидает входных данных
  if(runned) {
    // если работает
    // освободить выходной буфер выдав его содержимое на устройство вывода
    for(int i = 1; i < MAX_SRV_CLIENTS; i++) {
      if(users[i].status == 'a') {
        if(users[i].O->length() > 0) {
          ptr = users[i].O->asstring();
          if( i == 1) {
            Serial.print(ptr);
          } else {
            Telnet.print(ptr);
          }
          if(ptr != NULL) delete ptr;
        }
      }
    }
    if(A.length() == 0 && users[curuser].I->indexof(meta) == -1) {
      if(curuser == 1) {
        Serial.print("\nlocal:"); Serial.print(ESP.getFreeHeap());Serial.print(":>");
      } else {
        if (Telnet && Telnet.connected()){Telnet.print("\n\rnet:"); Telnet.print(ESP.getFreeHeap());Telnet.print(":>");}
      }
    }
    // если в активной цепочке ничего нет, а во входном буфере нет заключительной литеры команды (meta)
    if(A.length() == 0 && !ifmeta()) {
      // переходим в режим ожидания
      runned = false;
    } else {
      // иначе интерпретируем дальше
      interpret();
    }
  } else {  
    // в режиме ожидания
    // если во входном буфере появился символ meta - переходим в рабочий режим
    for(int i = 0; i < MAX_SRV_CLIENTS; i++) {
      if(users[i].status == 'a') {
        if(users[i].I->indexof(meta) >= 0) {
          N.clear();
          curuser = i;
          A.push(idle);
          z = false;
          runned = true;
          if(curuser == 1) {
            Serial.print("\n");
          } else {
  //          if (Telnet && Telnet.connected()){ Telnet.print("\n\r");}
          }
        }
      }
    }  
  }
}

void Error(char* s){
  Serial.print("Error: "); Serial.println(s);
} 


