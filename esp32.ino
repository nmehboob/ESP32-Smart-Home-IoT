#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

const char *ssid = "SSID-NAME";
const char *password = "YOUR-PASSWORD";

AsyncWebServer server(80);
AsyncEventSource events("/events");

// -------- DEVICE STRUCT --------
struct Device {
  int id;
  int pin;
  int btnPin;
  int prevBtnState;
  int status;
};

// -------- DEVICE ARRAY --------
#define NUM_DEVICES 4

Device devices[NUM_DEVICES] = {
  {1, 16, 12, 1, 0},
  {2, 17, 14, 1, 0},
  {3, 18, 26, 1, 0},
  {4, 19, 27, 1, 0}
};

// -------- HTML PROCESSOR --------
String processor(const String &var) {
  for (int i = 0; i < NUM_DEVICES; i++) {
    String btnTxt = "btn" + String(i + 1) + "txt";
    String btnCls = "btn" + String(i + 1) + "class";

    if (var == btnTxt) {
      return devices[i].status == 0 ? "ON" : "OFF";
    }
    if (var == btnCls) {
      return devices[i].status == 0 ? "button" : "button2";
    }
  }
  return "";
}

// -------- HTML --------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Smart Home</title>
<style>
.button {background-color: mediumseagreen; padding:10px 15px; font-size:24px;}
.button2 {background-color: gray; padding:10px 15px; font-size:24px;}
.button3 {background-color: crimson; padding:5px 10px; font-size:22px;}
</style>
</head>
<body>
<h1>ESP32 Smart Home</h1>

<p><a href="/set?button_id=1"><button id="btn1" class="%btn1class%">%btn1txt%</button></a></p>
<p><a href="/set?button_id=2"><button id="btn2" class="%btn2class%">%btn2txt%</button></a></p>
<p><a href="/set?button_id=3"><button id="btn3" class="%btn3class%">%btn3txt%</button></a></p>
<p><a href="/set?button_id=4"><button id="btn4" class="%btn4class%">%btn4txt%</button></a></p>

<p><a href="/reset"><button class="button3">Reset ALL</button></a></p>

<script>
if (!!window.EventSource) {
  var source = new EventSource('/events');
  source.addEventListener('toggleState', function(e) {
    let data = JSON.parse(e.data);
    let el = document.getElementById(data.id);

    if (data.status == 1) {
      el.innerHTML = 'OFF';
      el.className = "button2";
    } else {
      el.innerHTML = 'ON';
      el.className = "button";
    }
  });
}
</script>

</body>
</html>
)rawliteral";

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_DEVICES; i++) {
    pinMode(devices[i].pin, OUTPUT);
    pinMode(devices[i].btnPin, INPUT_PULLUP);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  MDNS.begin("esp32");
  MDNS.addService("http", "tcp", 80);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("button_id")) {
      int id = request->getParam("button_id")->value().toInt();
      toggleDevice(id);
    }
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    resetAll();
    request->send_P(200, "text/html", index_html, processor);
  });

  events.onConnect([](AsyncEventSourceClient *client) {
    client->send("connected", NULL, millis(), 10000);
  });

  server.addHandler(&events);
  server.begin();
}

// -------- LOOP --------
void loop() {
  for (int i = 0; i < NUM_DEVICES; i++) {
    int newState = digitalRead(devices[i].btnPin);

    if (devices[i].prevBtnState == 1 && newState == 0) {
      toggleDevice(devices[i].id);
    }

    devices[i].prevBtnState = newState;
  }
  delay(10);
}

// -------- TOGGLE --------
void toggleDevice(int id) {
  for (int i = 0; i < NUM_DEVICES; i++) {
    if (devices[i].id == id) {

      devices[i].status = !devices[i].status;
      digitalWrite(devices[i].pin, devices[i].status);

      char data[100];
      snprintf(data, sizeof(data),
               "{\"id\":\"btn%d\",\"status\":%d}",
               id, devices[i].status);

      events.send(data, "toggleState", millis());
    }
  }
}

// -------- RESET --------
void resetAll() {
  for (int i = 0; i < NUM_DEVICES; i++) {
    if (devices[i].status == 1) {
      toggleDevice(devices[i].id);
    }
  }
}  
