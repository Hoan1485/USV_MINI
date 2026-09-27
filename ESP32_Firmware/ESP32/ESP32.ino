#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ============================================================
// WIFI CONFIG
// ============================================================

const char* AP_SSID = "USV_MINI";
const char* AP_PASSWORD = "12345678";

WebServer server(80);


// ============================================================
// UART CONFIG
// ============================================================

HardwareSerial STM32Serial(2);

#define STM32_RX 16
#define STM32_TX 17

#define UART_BAUD 115200

String rxBuffer;


// ============================================================
// TELEMETRY DATA
// ============================================================

String latitude  = "0.000000";
String longitude = "0.000000";
String heading   = "0.0";
String speed     = "0.00";
String battery   = "0.00";
String current   = "0.00";
String temp      = "0.0";
String feed      = "0";
String mode      = "MANUAL";
String gps       = "0";

String lastError = "";


// ============================================================
// HTML DASHBOARD
// ============================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html lang="en">

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width, initial-scale=1.0">

<meta name="theme-color"
      content="#f2f4f7">

<title>USV MINI</title>


<style>

/* =========================================================
   GLOBAL
   ========================================================= */

* {
    box-sizing: border-box;
}

body {
    font-family: Arial, Helvetica, sans-serif;
    background: #f2f4f7;
    margin: 0;
    padding: 15px;
    color: #111;
}

.container {
    width: 100%;
    max-width: 700px;
    margin: auto;
}

h1 {
    text-align: center;
    margin: 10px 0 20px 0;
    font-size: 32px;
}

h2 {
    margin-top: 0;
    margin-bottom: 20px;
}


/* =========================================================
   CARD
   ========================================================= */

.card {
    background: white;
    border-radius: 14px;
    padding: 20px;
    margin-bottom: 15px;

    box-shadow:
        0 2px 10px rgba(0,0,0,0.08);
}


/* =========================================================
   STATUS
   ========================================================= */

.status-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 18px 25px;
}

.label {
    color: #777;
    font-size: 14px;
    margin-bottom: 4px;
}

.value {
    font-size: 21px;
    font-weight: bold;
    word-break: break-word;
}


/* =========================================================
   BUTTON
   ========================================================= */

button {
    border: none;
    border-radius: 10px;

    padding: 14px 18px;

    margin: 4px;

    font-size: 16px;

    cursor: pointer;

    background: #e5e5e5;

    color: #111;

    transition: 0.1s;
}

button:active {
    transform: scale(0.97);
}


/* =========================================================
   MODE
   ========================================================= */

.mode-buttons {
    display: flex;
    gap: 10px;
}

.mode-buttons button {
    flex: 1;
    margin: 0;
}

.manual {
    background: #dddddd;
}

.auto {
    background: #dddddd;
}

.active {
    background: #337ab7;
    color: white;
}


/* =========================================================
   MOTOR
   ========================================================= */

.motor-row {
    display: grid;

    grid-template-columns:
        1fr
        60px
        70px
        60px;

    align-items: center;

    gap: 5px;

    margin-bottom: 15px;
}

.motor-name {
    font-size: 17px;
    font-weight: bold;
}

.motor-value {
    text-align: center;

    font-size: 20px;

    font-weight: bold;
}

.motor-button {
    width: 55px;
    height: 50px;

    padding: 0;

    font-size: 24px;
}


/* =========================================================
   STOP
   ========================================================= */

.stop {
    width: 100%;

    background: #d9534f;

    color: white;

    font-size: 22px;

    margin: 10px 0 0 0;

    padding: 16px;
}


/* =========================================================
   AUTO
   ========================================================= */

.auto-button {
    width: 100%;

    margin: 5px 0;

    padding: 15px;

    font-size: 17px;
}


/* =========================================================
   FEED
   ========================================================= */

.feed {
    width: 100%;

    background: #f0ad4e;

    color: white;

    font-size: 18px;

    margin: 0;

    padding: 16px;
}


/* =========================================================
   ERROR
   ========================================================= */

.system-ok {
    color: #198754;

    font-weight: bold;
}

.system-error {
    color: #d9534f;

    font-weight: bold;
}


/* =========================================================
   RESPONSIVE
   ========================================================= */

@media (max-width: 500px)
{
    body {
        padding: 10px;
    }

    h1 {
        font-size: 28px;
    }

    .card {
        padding: 16px;
    }

    .status-grid {
        gap: 15px 10px;
    }

    .value {
        font-size: 18px;
    }

    .motor-row {
        grid-template-columns:
            1fr
            55px
            60px
            55px;
    }
}

</style>

</head>


<body>


<div class="container">


<!-- ======================================================
     TITLE
     ====================================================== -->

<h1>USV MINI</h1>


<!-- ======================================================
     STATUS
     ====================================================== -->

<div class="card">

<h2>Status</h2>


<div class="status-grid">


<div>
<div class="label">Mode</div>
<div id="mode" class="value">---</div>
</div>


<div>
<div class="label">GPS</div>
<div id="gps" class="value">---</div>
</div>


<div>
<div class="label">Latitude</div>
<div id="lat" class="value">---</div>
</div>


<div>
<div class="label">Longitude</div>
<div id="lon" class="value">---</div>
</div>


<div>
<div class="label">Heading</div>
<div id="heading" class="value">---</div>
</div>


<div>
<div class="label">Speed</div>
<div id="speed" class="value">---</div>
</div>


<div>
<div class="label">Battery</div>
<div id="battery" class="value">---</div>
</div>


<div>
<div class="label">Current</div>
<div id="current" class="value">---</div>
</div>


<div>
<div class="label">Water Temp</div>
<div id="temp" class="value">---</div>
</div>


<div>
<div class="label">Feed</div>
<div id="feed" class="value">---</div>
</div>


</div>

</div>


<!-- ======================================================
     MODE
     ====================================================== -->

<div class="card">

<h2>Mode</h2>

<div class="mode-buttons">

<button
    id="manualBtn"
    class="manual"
    onclick="setManual()">

MANUAL

</button>


<button
    id="autoBtn"
    class="auto"
    onclick="setAuto()">

AUTO

</button>

</div>

</div>


<!-- ======================================================
     MOTOR CONTROL
     ====================================================== -->

<div class="card">

<h2>Motor Control</h2>


<div class="motor-row">

<div class="motor-name">
LEFT
</div>


<button
    class="motor-button"
    onclick="changeLeft(-10)">

-

</button>


<div
    id="leftValue"
    class="motor-value">

0

</div>


<button
    class="motor-button"
    onclick="changeLeft(10)">

+

</button>

</div>


<div class="motor-row">

<div class="motor-name">
RIGHT
</div>


<button
    class="motor-button"
    onclick="changeRight(-10)">

-

</button>


<div
    id="rightValue"
    class="motor-value">

0

</div>


<button
    class="motor-button"
    onclick="changeRight(10)">

+

</button>

</div>


<button
    class="stop"
    onclick="stopMotor()">

STOP

</button>

</div>


<!-- ======================================================
     AUTO NAVIGATION
     ====================================================== -->

<div class="card">

<h2>Auto Navigation</h2>


<button
    class="auto-button"
    onclick="autoStart()">

AUTO START

</button>


<button
    class="auto-button"
    onclick="autoStop()">

AUTO STOP

</button>

</div>


<!-- ======================================================
     FEED
     ====================================================== -->

<div class="card">

<h2>Feed</h2>


<button
    class="feed"
    onclick="feed()">

FEED 500 ms

</button>

</div>


<!-- ======================================================
     SYSTEM
     ====================================================== -->

<div class="card">

<h2>System</h2>


<div
    id="error"
    class="system-ok">

System OK

</div>

</div>


</div>


<!-- ======================================================
     JAVASCRIPT
     ====================================================== -->

<script>


// ==========================================================
// MOTOR STATE
// ==========================================================

let leftMotor = 0;

let rightMotor = 0;


// ==========================================================
// SEND COMMAND
// ==========================================================

function command(cmd)
{
    fetch(
        "/command?cmd=" +
        encodeURIComponent(cmd)
    )
    .catch(function(error)
    {
        console.log(error);
    });
}


// ==========================================================
// LEFT MOTOR
// ==========================================================

function changeLeft(value)
{
    leftMotor += value;

    if (leftMotor > 100)
        leftMotor = 100;

    if (leftMotor < -100)
        leftMotor = -100;


    document.getElementById(
        "leftValue"
    ).innerText = leftMotor;


    sendMotor();
}


// ==========================================================
// RIGHT MOTOR
// ==========================================================

function changeRight(value)
{
    rightMotor += value;

    if (rightMotor > 100)
        rightMotor = 100;

    if (rightMotor < -100)
        rightMotor = -100;


    document.getElementById(
        "rightValue"
    ).innerText = rightMotor;


    sendMotor();
}


// ==========================================================
// SEND MOTOR COMMAND
// ==========================================================

function sendMotor()
{
    command(
        "MOTOR|" +
        leftMotor +
        "|" +
        rightMotor
    );
}


// ==========================================================
// STOP
// ==========================================================

function stopMotor()
{
    leftMotor = 0;

    rightMotor = 0;


    document.getElementById(
        "leftValue"
    ).innerText = "0";


    document.getElementById(
        "rightValue"
    ).innerText = "0";


    command("STOP");
}


// ==========================================================
// MANUAL
// ==========================================================

function setManual()
{
    command("MODE|MANUAL");
}


// ==========================================================
// AUTO
// ==========================================================

function setAuto()
{
    command("MODE|AUTO");
}


// ==========================================================
// AUTO START
// ==========================================================

function autoStart()
{
    command("AUTO_START");
}


// ==========================================================
// AUTO STOP
// ==========================================================

function autoStop()
{
    command("AUTO_STOP");
}


// ==========================================================
// FEED
// ==========================================================

function feed()
{
    command("FEED|500");
}


// ==========================================================
// UPDATE STATUS
// ==========================================================

function updateStatus()
{
    fetch("/status")

    .then(function(response)
    {
        return response.json();
    })

    .then(function(data)
    {

        // ----------------------------------------------
        // MODE
        // ----------------------------------------------

        document.getElementById(
            "mode"
        ).innerText = data.mode;


        // ----------------------------------------------
        // GPS
        // ----------------------------------------------

        let gpsElement =
            document.getElementById("gps");


        if (data.gps === "1")
        {
            gpsElement.innerText = "VALID";

            gpsElement.className =
                "value system-ok";
        }
        else
        {
            gpsElement.innerText = "INVALID";

            gpsElement.className =
                "value system-error";
        }


        // ----------------------------------------------
        // POSITION
        // ----------------------------------------------

        document.getElementById(
            "lat"
        ).innerText = data.lat;


        document.getElementById(
            "lon"
        ).innerText = data.lon;


        // ----------------------------------------------
        // HEADING
        // ----------------------------------------------

        document.getElementById(
            "heading"
        ).innerHTML =
            data.heading + "&deg;";


        // ----------------------------------------------
        // SPEED
        // ----------------------------------------------

        document.getElementById(
            "speed"
        ).innerText =
            data.speed + " m/s";


        // ----------------------------------------------
        // BATTERY
        // ----------------------------------------------

        document.getElementById(
            "battery"
        ).innerText =
            data.battery + " V";


        // ----------------------------------------------
        // CURRENT
        // ----------------------------------------------

        document.getElementById(
            "current"
        ).innerText =
            data.current + " A";


        // ----------------------------------------------
        // TEMPERATURE
        // ----------------------------------------------

        document.getElementById(
            "temp"
        ).innerHTML =
            data.temp + "&deg;C";


        // ----------------------------------------------
        // FEED
        // ----------------------------------------------

        document.getElementById(
            "feed"
        ).innerText =
            data.feed + " %";


        // ----------------------------------------------
        // ERROR
        // ----------------------------------------------

        let errorElement =
            document.getElementById("error");


        if (data.error &&
            data.error.length > 0)
        {
            errorElement.innerText =
                data.error;

            errorElement.className =
                "system-error";
        }
        else
        {
            errorElement.innerText =
                "System OK";

            errorElement.className =
                "system-ok";
        }


        // ----------------------------------------------
        // MODE BUTTON
        // ----------------------------------------------

        if (data.mode === "MANUAL")
        {
            document.getElementById(
                "manualBtn"
            ).className =
                "manual active";


            document.getElementById(
                "autoBtn"
            ).className =
                "auto";
        }
        else
        {
            document.getElementById(
                "manualBtn"
            ).className =
                "manual";


            document.getElementById(
                "autoBtn"
            ).className =
                "auto active";
        }

    })

    .catch(function(error)
    {

        document.getElementById(
            "error"
        ).innerText =
            "Connection lost";

        document.getElementById(
            "error"
        ).className =
            "system-error";

    });
}


// ==========================================================
// POLLING
// ==========================================================

setInterval(
    updateStatus,
    500
);


// Initial update

updateStatus();


</script>


</body>

</html>

)rawliteral";


// ============================================================
// SEND COMMAND TO STM32
// ============================================================

void sendCommand(const String& command)
{
    STM32Serial.print(command);
    STM32Serial.print('\n');

    Serial.print("[TX] ");
    Serial.println(command);
}


// ============================================================
// RECEIVE UART
// ============================================================

void receiveSTM32()
{
    while (STM32Serial.available())
    {
        char c = STM32Serial.read();

        // ----------------------------------------------
        // End of frame
        // ----------------------------------------------

        if (c == '\n')
        {
            rxBuffer.trim();

            if (rxBuffer.length() > 0)
            {
                parseSTM32Frame(rxBuffer);
            }

            rxBuffer = "";
        }

        // ----------------------------------------------
        // Normal character
        // ----------------------------------------------

        else
        {
            if (rxBuffer.length() < 200)
            {
                rxBuffer += c;
            }

            else
            {
                rxBuffer = "";

                Serial.println(
                    "[ERROR] RX buffer overflow"
                );
            }
        }
    }
}


// ============================================================
// PARSE STM32 FRAME
// ============================================================

void parseSTM32Frame(String frame)
{
    Serial.print("[RX] ");
    Serial.println(frame);


    if (frame.startsWith("STATUS|"))
    {
        parseStatus(frame);
    }

    else if (frame.startsWith("ERROR|"))
    {
        parseError(frame);
    }

    else
    {
        Serial.println(
            "[ERROR] Unknown frame type"
        );
    }
}


// ============================================================
// PARSE STATUS
// ============================================================

void parseStatus(String frame)
{
    String fields[11];

    int count =
        splitFrame(
            frame,
            fields,
            11
        );


    if (count != 11)
    {
        Serial.print(
            "[STATUS] Invalid field count: "
        );

        Serial.println(count);

        return;
    }


    latitude =
        fields[1];

    longitude =
        fields[2];

    heading =
        fields[3];

    speed =
        fields[4];

    battery =
        fields[5];

    current =
        fields[6];

    temp =
        fields[7];

    feed =
        fields[8];

    mode =
        fields[9];

    gps =
        fields[10];


    // New valid status clears old error
    lastError = "";
}


// ============================================================
// PARSE ERROR
// ============================================================

void parseError(String frame)
{
    int separator =
        frame.indexOf('|');


    if (separator < 0)
    {
        Serial.println(
            "[ERROR] Invalid ERROR frame"
        );

        return;
    }


    lastError =
        frame.substring(
            separator + 1
        );


    Serial.print(
        "[STM32 ERROR] "
    );

    Serial.println(
        lastError
    );
}


// ============================================================
// SPLIT FRAME
// ============================================================

int splitFrame(
    String frame,
    String fields[],
    int maxFields
)
{
    int count = 0;

    int start = 0;


    while (count < maxFields)
    {
        int separator =
            frame.indexOf(
                '|',
                start
            );


        // ----------------------------------------------
        // Last field
        // ----------------------------------------------

        if (separator < 0)
        {
            fields[count++] =
                frame.substring(start);

            break;
        }


        // ----------------------------------------------
        // Normal field
        // ----------------------------------------------

        fields[count++] =
            frame.substring(
                start,
                separator
            );


        start =
            separator + 1;
    }


    return count;
}


// ============================================================
// HTTP ROOT
// ============================================================

void handleRoot()
{
    server.send_P(
        200,
        "text/html; charset=UTF-8",
        INDEX_HTML
    );
}


// ============================================================
// HTTP COMMAND
// ============================================================

void handleCommand()
{
    if (!server.hasArg("cmd"))
    {
        server.send(
            400,
            "text/plain",
            "Missing cmd"
        );

        return;
    }


    String command =
        server.arg("cmd");


    command.trim();


    if (command.length() == 0)
    {
        server.send(
            400,
            "text/plain",
            "Empty command"
        );

        return;
    }


    // ----------------------------------------------
    // Basic command length protection
    // ----------------------------------------------

    if (command.length() > 100)
    {
        server.send(
            400,
            "text/plain",
            "Command too long"
        );

        return;
    }


    sendCommand(command);


    server.send(
        200,
        "text/plain",
        "OK"
    );
}


// ============================================================
// HTTP STATUS
// ============================================================

void handleStatus()
{
    String json = "{";


    json +=
        "\"lat\":\"" +
        latitude +
        "\",";


    json +=
        "\"lon\":\"" +
        longitude +
        "\",";


    json +=
        "\"heading\":\"" +
        heading +
        "\",";


    json +=
        "\"speed\":\"" +
        speed +
        "\",";


    json +=
        "\"battery\":\"" +
        battery +
        "\",";


    json +=
        "\"current\":\"" +
        current +
        "\",";


    json +=
        "\"temp\":\"" +
        temp +
        "\",";


    json +=
        "\"feed\":\"" +
        feed +
        "\",";


    json +=
        "\"mode\":\"" +
        mode +
        "\",";


    json +=
        "\"gps\":\"" +
        gps +
        "\",";


    json +=
        "\"error\":\"" +
        lastError +
        "\"";


    json += "}";


    server.send(
        200,
        "application/json",
        json
    );
}


// ============================================================
// SETUP
// ============================================================

void setup()
{
    // --------------------------------------------------------
    // Serial Monitor
    // --------------------------------------------------------

    Serial.begin(115200);


    // --------------------------------------------------------
    // UART to STM32 / Simulator
    // --------------------------------------------------------

    STM32Serial.begin(
        UART_BAUD,
        SERIAL_8N1,
        STM32_RX,
        STM32_TX
    );


    // --------------------------------------------------------
    // Wi-Fi Access Point
    // --------------------------------------------------------

    WiFi.mode(WIFI_AP);


    WiFi.softAP(
        AP_SSID,
        AP_PASSWORD
    );


    Serial.println();

    Serial.println(
        "================================"
    );

    Serial.println(
        "       USV MINI DASHBOARD"
    );

    Serial.println(
        "================================"
    );


    Serial.print(
        "WiFi SSID: "
    );

    Serial.println(
        AP_SSID
    );


    Serial.print(
        "WiFi IP: "
    );

    Serial.println(
        WiFi.softAPIP()
    );


    // --------------------------------------------------------
    // HTTP routes
    // --------------------------------------------------------

    server.on(
        "/",
        HTTP_GET,
        handleRoot
    );


    server.on(
        "/command",
        HTTP_GET,
        handleCommand
    );


    server.on(
        "/status",
        HTTP_GET,
        handleStatus
    );


    // --------------------------------------------------------
    // Start web server
    // --------------------------------------------------------

    server.begin();


    Serial.println(
        "HTTP server started"
    );


    // --------------------------------------------------------
    // Safe startup
    // --------------------------------------------------------

    delay(500);


    sendCommand(
        "MODE|MANUAL"
    );


    sendCommand(
        "STOP"
    );
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    // Receive STATUS / ERROR
    receiveSTM32();


    // Process HTTP requests
    server.handleClient();
}