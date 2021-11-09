#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <painlessMesh.h>

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// IO pins
int fsrPin = 33;
int ledPin = 2;

// num LEDs in neo_pix strip
const int LED_COUNT = 16;

// const for the breathe effect
const float SPEED_FACTOR = 0.0025;
const int STEP_DELAY = 5;

// const for rainbow effect
unsigned long speedInterval = 100;
unsigned long lastUpdate = 0;
unsigned long intervals[] = {20, 20, 50, 100, 30};
int fadeStep = 0;
int numberOfCases = 3;

// values for updating state pattern
int pattern = 0;
int newPattern = 0;
int count = 0;

#define RXD2 16
#define TXD2 17
#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

Adafruit_NeoPixel strip(LED_COUNT, ledPin, NEO_GRB + NEO_KHZ800);

void updateFSR();

void stateBreathe(float speedFactor)
{

    float intensity = 255 / 2.0 * (1.0 + sin(speedFactor * count));
    strip.setBrightness(intensity);
    for (int i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, 4, 90, 32);
    }

    strip.show();
}

void wipe()
{ // clear all LEDsh
    for (int i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}

// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);
void sendMessage()
{
    mesh.sendBroadcast(String(pattern));
    taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}
// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
    Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}
void newConnectionCallback(uint32_t nodeId)
{
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
void changedConnectionCallback()
{
    Serial.printf("Changed connections\n");
}
void nodeTimeAdjustedCallback(int32_t offset)
{
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup()
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    Serial.begin(115200);
    strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    wipe();
    strip.show();             // Turn OFF all pixels ASAP
    strip.setBrightness(250); // Set BRIGHTNESS to about 1/5 (max = 255)
    // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    userScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(1000);
    Serial.println("Started up...");
}
void loop()
{
    updateFSR();
    if (pattern == 2)
    {
        stateBreathe(SPEED_FACTOR);
    }
    else
    {
        // TODO write code for cup lifted.
        wipe();
    }
    mesh.update();
    count++;
}

void updateFSR()
{
    int analogReading = analogRead(fsrPin);
    // Serial.println(analogReading);
    if (analogReading < 300)
    {
        // Serial.println(analogReading);
        pattern = 0;
        fadeStep = 0;
    }
    // else if (analogReading < 3000)
    // {
    //     // Serial.println(analogReading);
    //     pattern = 1;
    //     fadeStep = 0;
    // }
    else
    {
        // Serial.println(analogReading);
        pattern = 2;
        fadeStep = 0;
    }
    if (pattern != newPattern)
    {
        Serial.println(pattern);
    }
    newPattern = pattern;
}
