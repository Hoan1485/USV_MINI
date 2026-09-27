#include <Arduino.h>

// =========================
// UART CONFIG
// =========================
HardwareSerial STM32Serial(2);

#define STM32_RX 16
#define STM32_TX 17

#define UART_BAUD 115200

// =========================
// SIMULATED STATE
// =========================
enum SystemMode
{
    MODE_MANUAL,
    MODE_AUTO
};

SystemMode currentMode = MODE_MANUAL;

int leftMotor = 0;
int rightMotor = 0;

double latitude = 21.028511;
double longitude = 105.804817;

float heading = 90.0f;
float speed = 0.0f;

float battery = 11.4f;
float current = 0.0f;
float waterTemp = 28.5f;

int feedPercent = 100;

bool gpsValid = true;
bool waypointValid = false;
bool autoRunning = false;

double waypointLat = 0.0;
double waypointLon = 0.0;

// =========================
// UART RX BUFFER
// =========================
String rxBuffer;

// =========================
// TIMING
// =========================
unsigned long lastStatusTime = 0;

const unsigned long STATUS_INTERVAL = 500;


// ============================================================
// SETUP
// ============================================================
void setup()
{
    Serial.begin(115200);

    STM32Serial.begin(
        UART_BAUD,
        SERIAL_8N1,
        STM32_RX,
        STM32_TX
    );

    Serial.println();
    Serial.println("================================");
    Serial.println(" STM32 SIMULATOR");
    Serial.println(" UART READY");
    Serial.println("================================");

    sendStatus();
}


// ============================================================
// LOOP
// ============================================================
void loop()
{
    receiveUART();

    simulateSystem();

    if (millis() - lastStatusTime >= STATUS_INTERVAL)
    {
        lastStatusTime = millis();
        sendStatus();
    }
}


// ============================================================
// UART RECEIVE
// ============================================================
void receiveUART()
{
    while (STM32Serial.available())
    {
        char c = STM32Serial.read();

        if (c == '\n')
        {
            rxBuffer.trim();

            if (rxBuffer.length() > 0)
            {
                processCommand(rxBuffer);
            }

            rxBuffer = "";
        }
        else
        {
            // Prevent buffer overflow
            if (rxBuffer.length() < 200)
            {
                rxBuffer += c;
            }
            else
            {
                rxBuffer = "";
                sendError("BUFFER_OVERFLOW");
            }
        }
    }
}


// ============================================================
// COMMAND PARSER
// ============================================================
void processCommand(String frame)
{
    Serial.print("[RX] ");
    Serial.println(frame);

    // -------------------------
    // STOP
    // -------------------------
    if (frame == "STOP")
    {
        stopMotors();

        Serial.println("[CMD] STOP");

        return;
    }


    // -------------------------
    // AUTO_START
    // -------------------------
    if (frame == "AUTO_START")
    {
        if (!gpsValid)
        {
            sendError("GPS_INVALID");
            return;
        }

        if (!waypointValid)
        {
            sendError("INVALID_STATE");
            return;
        }

        currentMode = MODE_AUTO;
        autoRunning = true;

        Serial.println("[CMD] AUTO_START");

        return;
    }


    // -------------------------
    // AUTO_STOP
    // -------------------------
    if (frame == "AUTO_STOP")
    {
        autoRunning = false;

        stopMotors();

        currentMode = MODE_MANUAL;

        Serial.println("[CMD] AUTO_STOP");

        return;
    }


    // -------------------------
    // MODE
    // -------------------------
    if (frame.startsWith("MODE|"))
    {
        handleMode(frame);
        return;
    }


    // -------------------------
    // MOTOR
    // -------------------------
    if (frame.startsWith("MOTOR|"))
    {
        handleMotor(frame);
        return;
    }


    // -------------------------
    // FEED
    // -------------------------
    if (frame.startsWith("FEED|"))
    {
        handleFeed(frame);
        return;
    }


    // -------------------------
    // WAYPOINT
    // -------------------------
    if (frame.startsWith("WAYPOINT|"))
    {
        handleWaypoint(frame);
        return;
    }


    // -------------------------
    // UNKNOWN
    // -------------------------
    sendError("UNKNOWN_COMMAND");
}


// ============================================================
// MODE HANDLER
// ============================================================
void handleMode(String frame)
{
    int separator = frame.indexOf('|');

    if (separator < 0)
    {
        sendError("INVALID_PARAMETER");
        return;
    }

    String mode = frame.substring(separator + 1);

    mode.trim();
    mode.toUpperCase();

    if (mode == "MANUAL")
    {
        currentMode = MODE_MANUAL;
        autoRunning = false;

        stopMotors();

        Serial.println("[MODE] MANUAL");
    }
    else if (mode == "AUTO")
    {
        currentMode = MODE_AUTO;

        Serial.println("[MODE] AUTO");
    }
    else
    {
        sendError("INVALID_PARAMETER");
    }
}


// ============================================================
// MOTOR HANDLER
// ============================================================
void handleMotor(String frame)
{
    if (currentMode != MODE_MANUAL)
    {
        sendError("INVALID_STATE");
        return;
    }

    int p1 = frame.indexOf('|');
    int p2 = frame.indexOf('|', p1 + 1);

    if (p1 < 0 || p2 < 0)
    {
        sendError("INVALID_PARAMETER");
        return;
    }

    String leftStr =
        frame.substring(p1 + 1, p2);

    String rightStr =
        frame.substring(p2 + 1);

    int left = leftStr.toInt();
    int right = rightStr.toInt();

    if (left < -100 || left > 100 ||
        right < -100 || right > 100)
    {
        sendError("INVALID_PARAMETER");
        return;
    }

    leftMotor = left;
    rightMotor = right;

    Serial.print("[MOTOR] L=");
    Serial.print(leftMotor);

    Serial.print(" R=");
    Serial.println(rightMotor);
}


// ============================================================
// FEED HANDLER
// ============================================================
void handleFeed(String frame)
{
    int separator = frame.indexOf('|');

    if (separator < 0)
    {
        sendError("INVALID_PARAMETER");
        return;
    }

    String timeStr =
        frame.substring(separator + 1);

    int timeMs = timeStr.toInt();

    if (timeMs <= 0 || timeMs > 10000)
    {
        sendError("INVALID_PARAMETER");
        return;
    }

    Serial.print("[FEED] ");
    Serial.print(timeMs);
    Serial.println(" ms");

    // Simulate feed consumption
    feedPercent -= 5;

    if (feedPercent < 0)
    {
        feedPercent = 0;
    }
}


// ============================================================
// WAYPOINT HANDLER
// ============================================================
void handleWaypoint(String frame)
{
    int p1 = frame.indexOf('|');
    int p2 = frame.indexOf('|', p1 + 1);

    if (p1 < 0 || p2 < 0)
    {
        sendError("INVALID_PARAMETER");
        return;
    }

    String latStr =
        frame.substring(p1 + 1, p2);

    String lonStr =
        frame.substring(p2 + 1);

    double lat = latStr.toDouble();
    double lon = lonStr.toDouble();

    if (lat < -90.0 || lat > 90.0 ||
        lon < -180.0 || lon > 180.0)
    {
        sendError("INVALID_PARAMETER");
        return;
    }

    waypointLat = lat;
    waypointLon = lon;

    waypointValid = true;

    Serial.print("[WAYPOINT] LAT=");
    Serial.print(waypointLat, 6);

    Serial.print(" LON=");
    Serial.println(waypointLon, 6);
}


// ============================================================
// STOP MOTORS
// ============================================================
void stopMotors()
{
    leftMotor = 0;
    rightMotor = 0;

    Serial.println("[MOTOR] STOP");
}


// ============================================================
// SIMULATE SYSTEM
// ============================================================
void simulateSystem()
{
    if (currentMode == MODE_AUTO &&
        autoRunning)
    {
        simulateAutoNavigation();
    }

    // Simulate speed from motor command
    float averageMotor =
        (abs(leftMotor) + abs(rightMotor)) / 2.0f;

    speed = averageMotor / 100.0f * 1.5f;

    // Simulate current
    current =
        averageMotor / 100.0f * 5.0f;
}


// ============================================================
// SIMULATED AUTO NAVIGATION
// ============================================================
void simulateAutoNavigation()
{
    if (!waypointValid)
    {
        stopMotors();
        return;
    }

    // Very simple simulator:
    // move heading gradually toward waypoint

    float targetHeading = heading;

    if (waypointLon > longitude)
    {
        targetHeading = 90.0f;
    }
    else if (waypointLon < longitude)
    {
        targetHeading = 270.0f;
    }

    float error = targetHeading - heading;

    while (error > 180.0f)
        error -= 360.0f;

    while (error < -180.0f)
        error += 360.0f;

    // Simple proportional controller
    int correction =
        constrain((int)(error * 1.0f), -30, 30);

    int baseThrottle = 40;

    leftMotor =
        constrain(baseThrottle - correction, -100, 100);

    rightMotor =
        constrain(baseThrottle + correction, -100, 100);

    // Simulate heading response
    heading += correction * 0.05f;

    if (heading >= 360.0f)
        heading -= 360.0f;

    if (heading < 0.0f)
        heading += 360.0f;
}


// ============================================================
// SEND STATUS
// ============================================================
void sendStatus()
{
    STM32Serial.print("STATUS|");

    STM32Serial.print(latitude, 6);
    STM32Serial.print("|");

    STM32Serial.print(longitude, 6);
    STM32Serial.print("|");

    STM32Serial.print(heading, 1);
    STM32Serial.print("|");

    STM32Serial.print(speed, 2);
    STM32Serial.print("|");

    STM32Serial.print(battery, 2);
    STM32Serial.print("|");

    STM32Serial.print(current, 2);
    STM32Serial.print("|");

    STM32Serial.print(waterTemp, 1);
    STM32Serial.print("|");

    STM32Serial.print(feedPercent);
    STM32Serial.print("|");

    if (currentMode == MODE_MANUAL)
        STM32Serial.print("MANUAL");
    else
        STM32Serial.print("AUTO");

    STM32Serial.print("|");

    STM32Serial.print(gpsValid ? 1 : 0);

    STM32Serial.print("\n");
}


// ============================================================
// SEND ERROR
// ============================================================
void sendError(const char *error)
{
    STM32Serial.print("ERROR|");
    STM32Serial.print(error);
    STM32Serial.print("\n");

    Serial.print("[ERROR] ");
    Serial.println(error);
}