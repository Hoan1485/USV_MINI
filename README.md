# UART Communication Structures between ESP32 (rssp32) and ESP32‑Giả lập (STM32)

## 1. Frame Overview
| Direction | Frame Type | Example (ASCII) | Terminator |
|-----------|------------|-----------------|------------|
| **rssp32 → ESP32‑Giả lập** | **Command** | `MOTOR|60|-30\n` | `\n` |
|  | **STOP** | `STOP\n` | `\n` |
|  | **AUTO_START** | `AUTO_START\n` | `\n` |
|  | **AUTO_STOP** | `AUTO_STOP\n` | `\n` |
|  | **MODE** | `MODE|MANUAL\n` or `MODE|AUTO\n` | `\n` |
|  | **WAYPOINT** | `WAYPOINT|21.028511|105.804817\n` | `\n` |
|  | **FEED** | `FEED|3000\n` (ms) | `\n` |
| **ESP32‑Giả lập → rssp32** | **Telemetry (STATUS)** | `STATUS|21.028511|105.804817|90.0|1.20|11.40|2.50|28.5|95|MANUAL|1\n` | `\n` |
|  | **Error (ERROR)** | `ERROR|INVALID_PARAMETER\n` | `\n` |

## 2. Detailed Field Description
### Command Frames (sent from rssp32)
- **STOP** – Stop all motors immediately.
- **AUTO_START** – Begin autonomous navigation (requires valid GPS and waypoint).
- **AUTO_STOP** – End autonomous navigation and switch to manual mode.
- **MODE|MANUAL** or **MODE|AUTO** – Force mode change.
- **MOTOR|<left>|<right>** – Set left/right motor throttle (range -100 … 100).
- **WAYPOINT|<lat>|<lon>** – Set navigation target (lat: -90…90, lon: -180…180).
- **FEED|<timeMs>** – Activate feeder for the given time (0‑10000 ms).

### Telemetry (STATUS) Frame (sent from ESP32‑Giả lập)
`STATUS|lat|lon|heading|speed|battery|current|temp|feed|mode|gps\n`
| Field | Type | Description |
|-------|------|-------------|
| **lat** | float (6 d.p.) | Latitude ° |
| **lon** | float (6 d.p.) | Longitude ° |
| **heading** | float | Heading angle ° |
| **speed** | float | Speed (m/s) |
| **battery** | float | Battery voltage V |
| **current** | float | Current consumption A |
| **temp** | float | Water temperature °C |
| **feed** | int | Remaining feed percent |
| **mode** | string | `MANUAL` or `AUTO` |
| **gps** | int (0/1) | GPS fix flag (1 = valid) |

### Error (ERROR) Frame (sent from ESP32‑Giả lập)
`ERROR|<CODE>\n` – `<CODE>` can be one of:
- `BUFFER_OVERFLOW`
- `GPS_INVALID`
- `INVALID_STATE`
- `INVALID_PARAMETER`
- `UNKNOWN_COMMAND`

## 3. Typical Communication Flow
1. **rssp32** sends a command string terminated by `\n`.
2. **ESP32‑Giả lập** receives bytes, builds a line buffer, and calls `processCommand()`.
3. If the command is valid it updates internal state and optionally replies with `ERROR`.
4. Independently, every **500 ms** the simulator calls `sendStatus()` which builds a STATUS line and writes it to the UART.
5. **rssp32** continuously reads incoming lines, dispatching them to `parseStatus()` or `parseError()`.

## 4. Possible Extensions
- **Add CRC16** at the end of each frame (`|CRC\n`).
- **Switch to binary frames** with a 2‑byte header (`0xAA55`), length byte, payload and CRC for higher reliability.
- **Use FreeRTOS UART task + queue** to avoid blocking the web server.

---
*This README documents the current ASCII‑based UART protocol used in the `USV_MINI` project.*
