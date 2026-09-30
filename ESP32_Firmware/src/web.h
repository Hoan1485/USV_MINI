#ifndef WEB_H
#define WEB_H

#include <Arduino.h>

// ============================================================
// USV MINI - TRẠM ĐIỀU KHIỂN & GIÁM SÁT HÀNH TRÌNH
// Hợp nhất giao diện Glassmorphism hiện đại + Backend ESP32 WebServer
// ============================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>USV MINI - Trạm Điều Khiển</title>

    <!-- Font Awesome (Hỗ trợ hiển thị online) -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <!-- Google Fonts -->
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;800&family=JetBrains+Mono:wght@400;700&display=swap" rel="stylesheet">

    <style>
        :root {
            --bg: #F3F6F9;
            --surface: #FFFFFF;
            --text-main: #1E293B;
            --text-dim: #64748B;
            --primary: #2563EB;
            --primary-light: #EFF6FF;
            --success: #10B981;
            --success-light: #ECFDF5;
            --danger: #EF4444;
            --danger-light: #FEF2F2;
            --warning: #F59E0B;
            --warning-light: #FFFBEB;
            --border: #E2E8F0;
            --radius-lg: 20px;
            --radius-md: 12px;
            --radius-sm: 8px;
            --shadow-sm: 0 2px 4px rgba(15, 23, 42, 0.04);
            --shadow-md: 0 4px 12px rgba(15, 23, 42, 0.06);
            --shadow-lg: 0 10px 25px rgba(15, 23, 42, 0.08);
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            user-select: none;
            -webkit-user-select: none;
        }

        body {
            background-color: var(--bg);
            color: var(--text-main);
            min-height: 100vh;
            padding: 16px;
            display: flex;
            justify-content: center;
        }

        .container {
            max-width: 600px;
            width: 100%;
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        /* HEADER */
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: var(--surface);
            padding: 16px 20px;
            border-radius: var(--radius-lg);
            box-shadow: var(--shadow-sm);
        }
        
        .header-title h1 {
            font-size: 1.3rem;
            font-weight: 800;
            color: var(--primary);
            letter-spacing: -0.5px;
        }

        .header-title p {
            font-size: 0.8rem;
            color: var(--text-dim);
            margin-top: 2px;
        }

        .status-indicator {
            display: flex;
            align-items: center;
            gap: 8px;
            background: var(--success-light);
            color: var(--success);
            padding: 6px 12px;
            border-radius: 50px;
            font-size: 0.8rem;
            font-weight: 600;
        }

        .dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background-color: var(--danger);
            transition: all 0.3s;
        }
        .dot.connected {
            background-color: var(--success);
        }

        /* CARDS */
        .card {
            background: var(--surface);
            border-radius: var(--radius-lg);
            padding: 24px;
            box-shadow: var(--shadow-md);
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid var(--border);
            padding-bottom: 12px;
        }

        .card-header .title {
            font-size: 1.1rem;
            font-weight: 700;
            color: var(--text-main);
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .card-header .title i {
            color: var(--primary);
        }

        .badge {
            font-size: 0.75rem;
            font-weight: 700;
            padding: 4px 10px;
            border-radius: 6px;
        }
        .badge.err { background: var(--danger-light); color: var(--danger); }
        .badge.ok { background: var(--success-light); color: var(--success); }
        .badge.primary { background: var(--primary-light); color: var(--primary); }

        /* TELEMETRY GRID */
        .telemetry-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 16px;
        }

        .data-box {
            background: var(--bg);
            padding: 16px;
            border-radius: var(--radius-md);
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .data-label {
            font-size: 0.8rem;
            color: var(--text-dim);
            font-weight: 600;
            display: flex;
            align-items: center;
            gap: 6px;
        }

        .data-value {
            font-size: 1.3rem;
            font-weight: 800;
            color: var(--text-main);
            font-family: 'JetBrains Mono', monospace;
        }

        .data-value.gps-val { font-size: 0.9rem; }

        .highlight-blue { color: var(--primary); }
        .highlight-green { color: var(--success); }
        .highlight-orange { color: var(--warning); }

        /* CONTROLS */
        .mode-switch {
            display: flex;
            background: var(--bg);
            border-radius: 50px;
            padding: 4px;
        }

        .mode-btn {
            flex: 1;
            padding: 12px;
            border: none;
            background: transparent;
            border-radius: 50px;
            font-weight: 700;
            color: var(--text-dim);
            cursor: pointer;
            transition: 0.3s;
        }

        .mode-btn.active {
            background: var(--surface);
            color: var(--primary);
            box-shadow: var(--shadow-sm);
        }

        /* JOYSTICK */
        .joystick-container {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 20px;
            margin: 10px 0;
        }

        .joystick-zone {
            width: 220px;
            height: 220px;
            background: var(--bg);
            border-radius: 50%;
            position: relative;
            box-shadow: inset var(--shadow-sm);
            border: 2px solid var(--border);
            display: flex;
            justify-content: center;
            align-items: center;
            touch-action: none;
        }

        .joystick-axis {
            position: absolute;
            background: var(--border);
            pointer-events: none;
        }
        .axis-x { width: 100%; height: 2px; }
        .axis-y { width: 2px; height: 100%; }

        .joystick-knob {
            width: 70px;
            height: 70px;
            background: var(--primary);
            border-radius: 50%;
            position: absolute;
            box-shadow: var(--shadow-md), inset 0 2px 4px rgba(255,255,255,0.4);
            border: 4px solid var(--surface);
            transition: transform 0.2s cubic-bezier(0.18, 0.89, 0.32, 1.28);
            pointer-events: none;
        }

        .joystick-stats {
            display: flex;
            gap: 12px;
            width: 100%;
        }

        .stat-badge {
            flex: 1;
            background: var(--bg);
            padding: 10px;
            border-radius: var(--radius-md);
            text-align: center;
            font-size: 0.85rem;
            font-weight: 600;
            color: var(--text-dim);
        }
        .stat-badge span { color: var(--primary); font-size: 1.1rem; display: block; font-family: monospace;}

        /* BUTTONS */
        .btn {
            padding: 16px;
            border-radius: var(--radius-md);
            border: none;
            font-weight: 700;
            font-size: 1rem;
            cursor: pointer;
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 8px;
            transition: 0.2s;
        }
        .btn:active { transform: scale(0.98); }

        .btn-stop {
            background: var(--danger);
            color: white;
            width: 100%;
            box-shadow: 0 4px 15px rgba(239, 68, 68, 0.3);
        }

        .btn-feed {
            background: var(--success);
            color: white;
            width: 100%;
            box-shadow: 0 4px 15px rgba(16, 185, 129, 0.3);
        }

        .grid-2 {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 12px;
        }

        .btn-outline {
            background: var(--surface);
            border: 2px solid var(--primary);
            color: var(--primary);
        }

        .btn-fill {
            background: var(--primary);
            color: white;
        }

    </style>
</head>
<body>

<div class="container">
    
    <!-- HEADER -->
    <div class="header">
        <div class="header-title">
            <h1>USV MINI</h1>
            <p>Trạm Giám Sát Hành Trình</p>
        </div>
        <div class="status-indicator" id="statusBadge">
            <div id="statusDot" class="dot"></div>
            <span id="statusText">Kết nối...</span>
        </div>
    </div>

    <!-- CARDS -->
    <div class="card">
        <div class="card-header">
            <div class="title">
                <i class="fa-solid fa-chart-pie"></i> Dữ Liệu Hành Trình
            </div>
            <span id="gpsBadge" class="badge err">MẤT GPS</span>
        </div>
        
        <div class="telemetry-grid">
            <div class="data-box">
                <span class="data-label"><i class="fa-solid fa-gauge-high"></i> Vận Tốc</span>
                <span class="data-value highlight-blue" id="valSpeed">0.00 <span style="font-size:0.8rem">m/s</span></span>
            </div>
            <div class="data-box">
                <span class="data-label"><i class="fa-regular fa-compass"></i> Góc Hướng</span>
                <span class="data-value highlight-blue" id="valHeading">0.0°</span>
            </div>
            
            <div class="data-box" style="grid-column: span 2;">
                <span class="data-label"><i class="fa-solid fa-location-dot"></i> Tọa độ GPS</span>
                <span class="data-value gps-val" id="valGPS">---, ---</span>
            </div>

            <div class="data-box">
                <span class="data-label"><i class="fa-solid fa-battery-half"></i> Pin (V)</span>
                <span class="data-value highlight-green" id="valBattery">0.00</span>
            </div>
            <div class="data-box">
                <span class="data-label"><i class="fa-solid fa-bolt"></i> Dòng (A)</span>
                <span class="data-value" id="valCurrent">0.00</span>
            </div>
            
            <div class="data-box">
                <span class="data-label"><i class="fa-solid fa-temperature-half"></i> Nhiệt Độ</span>
                <span class="data-value highlight-orange" id="valTemp">0.0°C</span>
            </div>
            <div class="data-box">
                <span class="data-label"><i class="fa-solid fa-fish"></i> Mồi</span>
                <span class="data-value" id="valFeed">0 %</span>
            </div>
        </div>

        <div style="background: var(--bg); padding: 12px; border-radius: var(--radius-md); font-size: 0.85rem; font-weight: 600; display:flex; justify-content:space-between;">
            <span style="color: var(--text-dim);">Trạng thái phần cứng:</span>
            <span id="valError" class="highlight-green">Sẵn sàng (OK)</span>
        </div>
    </div>

    <!-- CONTROLS -->
    <div class="card">
        <div class="card-header">
            <div class="title">
                <i class="fa-solid fa-gamepad"></i> Điều Khiển Tàu
            </div>
            <span id="modeBadge" class="badge primary">MANUAL</span>
        </div>

        <div class="mode-switch">
            <button class="mode-btn active" id="btnManual" onclick="setMode('MANUAL')">THỦ CÔNG</button>
            <button class="mode-btn" id="btnAuto" onclick="setMode('AUTO')">TỰ ĐỘNG</button>
        </div>

        <div class="joystick-container">
            <div class="joystick-zone" id="joyZone">
                <div class="joystick-axis axis-x"></div>
                <div class="joystick-axis axis-y"></div>
                <div class="joystick-knob" id="joyKnob"></div>
            </div>

            <div class="joystick-stats">
                <div class="stat-badge">Motor L (Trái)<span id="joyLeftStat">0%</span></div>
                <div class="stat-badge">Motor R (Phải)<span id="joyRightStat">0%</span></div>
            </div>
        </div>

        <button class="btn btn-feed" onclick="sendFeed()">
            <i class="fa-solid fa-wheat-awn"></i> RẢI MỒI (1 VÒNG)
        </button>

        <div class="grid-2">
            <button class="btn btn-outline" onclick="autoStart()">BẮT ĐẦU AUTO</button>
            <button class="btn btn-outline" onclick="autoStop()">DỪNG AUTO</button>
        </div>

        <button class="btn btn-stop" onclick="emergencyStop()">
            <i class="fa-solid fa-hand"></i> DỪNG KHẨN CẤP
        </button>
    </div>

</div>

<!-- JAVASCRIPT LOGIC CLIENT -->
<script>
    // ==========================================================
    // CẤU HÌNH & TRẠNG THÁI JOYSTICK CỰC KỲ MƯỢT (LOW-LATENCY ENGINE)
    // ==========================================================
    let isJoyActive = false;
    let isHttpBusy = false;
    let nextCommand = null;
    let dispatchTimer = null;
    let lastSentTime = 0;
    const MIN_SEND_INTERVAL = 55; // ms (tối đa ~18 lệnh/giây, rất mượt và không nghẽn TCP)

    let joyCenterX = 0;
    let joyCenterY = 0;
    let joyMaxRadius = 65;
    let targetLeft = 0;
    let targetRight = 0;
    let lastSentLeft = 0;
    let lastSentRight = 0;

    let joyZone, joyKnob, joyLeftStat, joyRightStat;
    let rafId = null;

    // ==========================================================
    // KHỞI TẠO JOYSTICK (TOUCH + MOUSE POINTER CAPTURE)
    // ==========================================================
    function initJoystick() {
        joyZone = document.getElementById("joyZone");
        joyKnob = document.getElementById("joyKnob");
        joyLeftStat = document.getElementById("joyLeftStat");
        joyRightStat = document.getElementById("joyRightStat");

        if (!joyZone || !joyKnob) return;

        joyZone.addEventListener("pointerdown", onJoyPointerDown, { passive: false });
        joyZone.addEventListener("pointermove", onJoyPointerMove, { passive: false });
        joyZone.addEventListener("pointerup", onJoyPointerUp, { passive: false });
        joyZone.addEventListener("pointercancel", onJoyPointerUp, { passive: false });
    }

    function onJoyPointerDown(e) {
        e.preventDefault();
        isJoyActive = true;
        joyZone.setPointerCapture(e.pointerId);
        joyKnob.style.transition = "none";

        const rect = joyZone.getBoundingClientRect();
        joyCenterX = rect.left + rect.width / 2;
        joyCenterY = rect.top + rect.height / 2;
        joyMaxRadius = (rect.width / 2) - (joyKnob.offsetWidth / 2) - 4;

        handleJoyPointerMove(e.clientX, e.clientY);
    }

    function onJoyPointerMove(e) {
        if (!isJoyActive) return;
        e.preventDefault();
        handleJoyPointerMove(e.clientX, e.clientY);
    }

    function onJoyPointerUp(e) {
        if (!isJoyActive) return;
        isJoyActive = false;

        if (rafId) {
            cancelAnimationFrame(rafId);
            rafId = null;
        }
        if (dispatchTimer) {
            clearTimeout(dispatchTimer);
            dispatchTimer = null;
        }

        // Hiệu ứng hồi tâm lò xo mượt mà
        joyKnob.style.transition = "transform 0.22s cubic-bezier(0.18, 0.89, 0.32, 1.28)";
        joyKnob.style.transform = "translate(0px, 0px)";

        targetLeft = 0;
        targetRight = 0;
        lastSentLeft = 0;
        lastSentRight = 0;

        if (joyLeftStat) joyLeftStat.innerText = "0%";
        if (joyRightStat) joyRightStat.innerText = "0%";

        // Gửi lệnh dừng STOP với ưu tiên cao nhất
        queueStop();

        // Kích hoạt cập nhật telemetry lại ngay sau 300ms
        setTimeout(updateTelemetry, 300);
    }

    function handleJoyPointerMove(clientX, clientY) {
        let dx = clientX - joyCenterX;
        let dy = clientY - joyCenterY;
        let distance = Math.hypot(dx, dy);

        if (distance > joyMaxRadius) {
            dx = (dx / distance) * joyMaxRadius;
            dy = (dy / distance) * joyMaxRadius;
            distance = joyMaxRadius;
        }

        // Tối ưu render 60fps/120fps bằng requestAnimationFrame
        if (rafId) cancelAnimationFrame(rafId);
        rafId = requestAnimationFrame(function() {
            joyKnob.style.transform = "translate(" + dx.toFixed(1) + "px, " + dy.toFixed(1) + "px)";
        });

        // Vùng chết trung tâm (Deadzone 5%): nếu ngón tay ở rất gần tâm, giữ 0 để không bị rung
        if (distance < 5) {
            targetLeft = 0;
            targetRight = 0;
        } else {
            let normX = dx / joyMaxRadius;
            let normY = -dy / joyMaxRadius; // Trục Y hướng lên là Tiến (+)

            // Pha trộn vi sai động cơ (Differential Skid-Steer)
            let forward = normY * 100;
            let turn = normX * 100;

            let left = forward + turn;
            let right = forward - turn;

            targetLeft = Math.max(-100, Math.min(100, Math.round(left)));
            targetRight = Math.max(-100, Math.min(100, Math.round(right)));
        }

        if (joyLeftStat) joyLeftStat.innerText = (targetLeft > 0 ? "+" : "") + targetLeft + "%";
        if (joyRightStat) joyRightStat.innerText = (targetRight > 0 ? "+" : "") + targetRight + "%";

        // Đưa vào hàng đợi gửi lệnh với cơ chế tự điều tiết không nghẽn mạng
        queueMotor(targetLeft, targetRight);
    }

    // ==========================================================
    // CƠ CHẾ GỬI LỆNH KHÔNG NGHẼN (PIPELINED DISPATCHER)
    // ==========================================================
    function queueMotor(left, right) {
        nextCommand = { left: left, right: right, type: 'MOTOR' };
        dispatchNextCommand();
    }

    function queueStop() {
        nextCommand = { left: 0, right: 0, type: 'STOP' };
        dispatchNextCommand();
    }

    function dispatchNextCommand() {
        if (isHttpBusy || !nextCommand) return;

        let now = performance.now();
        let elapsed = now - lastSentTime;

        // Nếu lệnh trước vừa gửi xong quá nhanh, hẹn giờ gửi lệnh mới nhất sau
        if (elapsed < MIN_SEND_INTERVAL) {
            if (!dispatchTimer) {
                dispatchTimer = setTimeout(function() {
                    dispatchTimer = null;
                    dispatchNextCommand();
                }, MIN_SEND_INTERVAL - elapsed);
            }
            return;
        }

        let cmd = nextCommand;
        nextCommand = null;

        // Bỏ qua các vi dịch chuyển nhỏ (< 3%) nếu không phải là lệnh STOP
        if (cmd.type !== 'STOP') {
            if (Math.abs(cmd.left - lastSentLeft) < 3 && Math.abs(cmd.right - lastSentRight) < 3) {
                return;
            }
        }

        isHttpBusy = true;
        lastSentTime = performance.now();
        lastSentLeft = cmd.left;
        lastSentRight = cmd.right;

        let url = (cmd.type === 'STOP') 
            ? "/command?cmd=STOP" 
            : "/command?cmd=" + encodeURIComponent("MOTOR|" + cmd.left + "|" + cmd.right);

        fetch(url, { cache: "no-store" })
            .catch(function(err) {
                console.warn("Lệnh không tới được ESP32:", err);
            })
            .finally(function() {
                isHttpBusy = false;
                // Nếu trong lúc HTTP đang truyền mà ngón tay đã di chuyển đến góc mới:
                if (nextCommand) {
                    dispatchNextCommand();
                }
            });
    }

    // ==========================================================
    // HÀM GỬI LỆNH ĐỘC LẬP (NÚT BẤM)
    // ==========================================================
    function command(cmd) {
        fetch("/command?cmd=" + encodeURIComponent(cmd), { cache: "no-store" })
            .catch(function(err) { console.error("Lỗi gửi lệnh:", err); });
    }

    function emergencyStop() {
        onJoyPointerUp();
        command("STOP");
    }

    function setMode(mode) {
        command("MODE|" + mode);
        updateModeUI(mode);
    }

    function updateModeUI(mode) {
        const btnManual = document.getElementById("btnManual");
        const btnAuto = document.getElementById("btnAuto");
        const modeBadge = document.getElementById("modeBadge");

        if (mode === "MANUAL") {
            btnManual.classList.add("active");
            btnAuto.classList.remove("active");
            modeBadge.innerText = "MANUAL";
            modeBadge.className = "status-badge ok";
        } else {
            btnAuto.classList.add("active");
            btnManual.classList.remove("active");
            modeBadge.innerText = "AUTO";
            modeBadge.className = "status-badge err";
        }
    }

    function autoStart() {
        command("AUTO_START");
    }

    function autoStop() {
        command("AUTO_STOP");
    }

    function sendFeed() {
        command("FEED|1");
    }

    // ==========================================================
    // LẤY DỮ LIỆU TELEMETRY ĐỊNH KỲ (THÔNG MINH, KHÔNG TRANH CHẤP KHI LÁI)
    // ==========================================================
    function updateTelemetry() {
        // QUAN TRỌNG: Nếu người dùng đang điều khiển Joystick, tạm ngưng polling
        // để dành 100% băng thông Wi-Fi và CPU cho lệnh động cơ, loại bỏ hoàn toàn lag!
        if (isJoyActive || isHttpBusy) return;

        fetch("/status", { cache: "no-store" })
            .then(function(res) {
                if (!res.ok) throw new Error("HTTP " + res.status);
                return res.json();
            })
            .then(function(data) {
                const dot = document.getElementById("statusDot");
                const txt = document.getElementById("statusText");
                dot.classList.add("connected");
                txt.innerText = "Đã kết nối trực tuyến";

                document.getElementById("valSpeed").innerText = data.speed;
                document.getElementById("valHeading").innerText = data.heading + "°";
                document.getElementById("valBattery").innerText = data.battery + " V";
                document.getElementById("valCurrent").innerText = data.current + " A";
                document.getElementById("valTemp").innerText = data.temp + "°C";
                document.getElementById("valFeed").innerText = data.feed + " %";

                const gpsBadge = document.getElementById("gpsBadge");
                const valGPS = document.getElementById("valGPS");
                if (data.gps === "1") {
                    gpsBadge.innerText = "GPS: ĐÃ KHÓA";
                    gpsBadge.className = "status-badge ok";
                    valGPS.innerText = data.lat + ", " + data.lon;
                } else {
                    gpsBadge.innerText = "GPS: TÌM VỆ TINH";
                    gpsBadge.className = "status-badge err";
                    valGPS.innerText = (data.lat === "0.000000" && data.lon === "0.000000") ? "Đang tìm vệ tinh..." : (data.lat + ", " + data.lon);
                }

                const valError = document.getElementById("valError");
                if (data.error && data.error.length > 0) {
                    valError.innerText = data.error;
                    valError.style.color = "var(--danger)";
                } else {
                    valError.innerText = "Hệ thống sẵn sàng (System OK)";
                    valError.style.color = "var(--success)";
                }

                updateModeUI(data.mode);
            })
            .catch(function(err) {
                const dot = document.getElementById("statusDot");
                const txt = document.getElementById("statusText");
                dot.classList.remove("connected");
                txt.innerText = "Mất kết nối ESP32...";

                const valError = document.getElementById("valError");
                valError.innerText = "Lỗi kết nối HTTP WebServer";
                valError.style.color = "var(--danger)";
            });
    }

    // Polling mỗi 1000ms khi rảnh rỗi (không điều khiển cần gạt)
    setInterval(updateTelemetry, 1000);

    // Khởi tạo ngay khi tải xong trang
    window.addEventListener("DOMContentLoaded", function() {
        initJoystick();
        updateTelemetry();
    });
</script>
</body>
</html>
)rawliteral";

#endif // WEB_H
