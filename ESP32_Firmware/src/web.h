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
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&family=JetBrains+Mono:wght@400;700&display=swap" rel="stylesheet">

    <style>
        :root {
            --bg-color: #0b1120;
            --card-bg: rgba(30, 41, 59, 0.72);
            --card-border: rgba(255, 255, 255, 0.08);
            --primary: #38bdf8;
            --primary-glow: rgba(56, 189, 248, 0.4);
            --success: #34d399;
            --success-glow: rgba(52, 211, 153, 0.4);
            --danger: #fb7185;
            --danger-glow: rgba(251, 113, 133, 0.4);
            --warning: #fbbf24;
            --text-main: #f8fafc;
            --text-dim: #94a3b8;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Outfit', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            user-select: none;
            -webkit-user-select: none;
        }

        body {
            background-color: var(--bg-color);
            background-image: 
                radial-gradient(at 0% 0%, rgba(56, 189, 248, 0.12) 0px, transparent 50%),
                radial-gradient(at 100% 100%, rgba(52, 211, 153, 0.08) 0px, transparent 50%);
            background-attachment: fixed;
            color: var(--text-main);
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            padding: 16px;
        }

        .container {
            max-width: 1200px;
            width: 100%;
            margin: 0 auto;
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        /* HEADER */
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 16px 20px;
            background: var(--card-bg);
            backdrop-filter: blur(12px);
            -webkit-backdrop-filter: blur(12px);
            border: 1px solid var(--card-border);
            border-radius: 20px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
        }

        .header-title h1 {
            font-size: 1.6rem;
            font-weight: 800;
            background: linear-gradient(135deg, var(--primary), var(--success));
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            letter-spacing: 0.5px;
        }

        .header-title p {
            color: var(--text-dim);
            font-size: 0.85rem;
            margin-top: 3px;
        }

        .status-indicator {
            display: flex;
            align-items: center;
            gap: 10px;
            padding: 8px 16px;
            background: rgba(15, 23, 42, 0.6);
            border-radius: 50px;
            border: 1px solid var(--card-border);
            font-weight: 600;
            font-size: 0.85rem;
        }

        .dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background-color: var(--danger);
            box-shadow: 0 0 10px var(--danger);
            transition: all 0.3s ease;
        }

        .dot.connected {
            background-color: var(--success);
            box-shadow: 0 0 12px var(--success);
        }

        /* MAIN GRID */
        .main-grid {
            display: grid;
            grid-template-columns: 1.1fr 1fr;
            gap: 20px;
        }

        /* CARD CHUNG */
        .card {
            background: var(--card-bg);
            backdrop-filter: blur(14px);
            -webkit-backdrop-filter: blur(14px);
            border: 1px solid var(--card-border);
            border-radius: 20px;
            padding: 22px;
            box-shadow: 0 10px 30px -10px rgba(0, 0, 0, 0.5);
            display: flex;
            flex-direction: column;
            gap: 18px;
        }

        .card-header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            font-size: 1.15rem;
            font-weight: 700;
            color: var(--text-main);
            padding-bottom: 12px;
            border-bottom: 1px solid rgba(255, 255, 255, 0.06);
        }

        .card-header .title-group {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .card-header svg, .card-header i {
            color: var(--primary);
            width: 20px;
            height: 20px;
        }

        /* TELEMETRY GRID */
        .telemetry-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 12px;
        }

        .data-box {
            background: rgba(15, 23, 42, 0.55);
            border-radius: 14px;
            padding: 14px;
            display: flex;
            flex-direction: column;
            border: 1px solid rgba(255, 255, 255, 0.05);
            transition: border-color 0.2s;
        }

        .data-box:hover {
            border-color: rgba(56, 189, 248, 0.3);
        }

        .data-label {
            font-size: 0.75rem;
            color: var(--text-dim);
            text-transform: uppercase;
            letter-spacing: 0.8px;
            margin-bottom: 6px;
            display: flex;
            align-items: center;
            justify-content: space-between;
        }

        .data-value {
            font-family: 'JetBrains Mono', monospace;
            font-size: 1.4rem;
            font-weight: 700;
            color: var(--primary);
        }

        .data-value.gps-val {
            font-size: 0.95rem;
            word-break: break-all;
        }

        .status-badge {
            font-size: 0.7rem;
            padding: 2px 8px;
            border-radius: 6px;
            font-weight: bold;
            font-family: sans-serif;
        }
        .status-badge.ok {
            background: rgba(52, 211, 153, 0.15);
            color: var(--success);
            border: 1px solid rgba(52, 211, 153, 0.3);
        }
        .status-badge.err {
            background: rgba(251, 113, 133, 0.15);
            color: var(--danger);
            border: 1px solid rgba(251, 113, 133, 0.3);
        }

        /* CONTROLS AREA */
        .controls-container {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 18px;
        }

        /* MODE SWITCH */
        .mode-switch {
            display: flex;
            background: rgba(15, 23, 42, 0.7);
            border-radius: 50px;
            padding: 4px;
            width: 100%;
            border: 1px solid var(--card-border);
        }

        .mode-btn {
            flex: 1;
            padding: 10px 16px;
            border: none;
            background: transparent;
            color: var(--text-dim);
            border-radius: 50px;
            font-weight: 700;
            font-size: 0.85rem;
            cursor: pointer;
            transition: all 0.25s ease;
        }

        .mode-btn.active {
            background: linear-gradient(135deg, var(--primary), #0284c7);
            color: #0b1120;
            box-shadow: 0 0 15px var(--primary-glow);
        }

        /* JOYSTICK STYLES */
        .joystick-wrapper {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 14px;
            width: 100%;
        }

        .joystick-zone {
            position: relative;
            width: 220px;
            height: 220px;
            border-radius: 50%;
            background: radial-gradient(circle at center, rgba(30, 41, 59, 0.9) 0%, rgba(15, 23, 42, 0.98) 100%);
            border: 2px solid rgba(56, 189, 248, 0.35);
            box-shadow: inset 0 0 25px rgba(0, 0, 0, 0.85), 0 0 20px rgba(56, 189, 248, 0.15);
            display: flex;
            align-items: center;
            justify-content: center;
            touch-action: none;
            cursor: grab;
        }

        .joystick-zone:active {
            cursor: grabbing;
        }

        .joystick-ring {
            position: absolute;
            border-radius: 50%;
            border: 1px dashed rgba(56, 189, 248, 0.25);
            pointer-events: none;
        }
        .joystick-ring.ring-inner { width: 95px; height: 95px; }
        .joystick-ring.ring-outer { width: 160px; height: 160px; }

        .joystick-axis-x {
            position: absolute;
            width: 100%;
            height: 1px;
            background: linear-gradient(90deg, transparent, rgba(56, 189, 248, 0.25), transparent);
            pointer-events: none;
        }

        .joystick-axis-y {
            position: absolute;
            height: 100%;
            width: 1px;
            background: linear-gradient(180deg, transparent, rgba(56, 189, 248, 0.25), transparent);
            pointer-events: none;
        }

        .joystick-label-dir {
            position: absolute;
            font-size: 0.7rem;
            font-weight: 800;
            color: rgba(148, 163, 184, 0.6);
            pointer-events: none;
            letter-spacing: 1px;
        }
        .joystick-label-dir.up { top: 8px; }
        .joystick-label-dir.down { bottom: 8px; }
        .joystick-label-dir.left { left: 10px; }
        .joystick-label-dir.right { right: 10px; }

        .joystick-knob {
            position: absolute;
            width: 74px;
            height: 74px;
            border-radius: 50%;
            background: radial-gradient(circle at 35% 35%, #38bdf8, #0284c7);
            border: 2px solid rgba(255, 255, 255, 0.65);
            box-shadow: 0 4px 18px rgba(2, 132, 199, 0.6), inset 0 2px 4px rgba(255, 255, 255, 0.4);
            display: flex;
            align-items: center;
            justify-content: center;
            transition: transform 0.22s cubic-bezier(0.18, 0.89, 0.32, 1.28);
            pointer-events: none;
            will-change: transform;
            touch-action: none;
        }

        .joystick-knob::after {
            content: '';
            width: 26px;
            height: 26px;
            border-radius: 50%;
            background: rgba(15, 23, 42, 0.7);
            border: 1px solid rgba(255, 255, 255, 0.3);
            box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.6);
        }

        /* LIVE MOTOR READOUT */
        .joystick-stats {
            display: flex;
            gap: 12px;
            width: 100%;
            justify-content: center;
        }

        .stat-badge {
            background: rgba(15, 23, 42, 0.6);
            border: 1px solid rgba(255, 255, 255, 0.08);
            padding: 8px 14px;
            border-radius: 10px;
            font-size: 0.85rem;
            display: flex;
            align-items: center;
            gap: 8px;
            flex: 1;
            justify-content: center;
        }
        .stat-badge .stat-name {
            color: var(--text-dim);
            font-size: 0.8rem;
        }
        .stat-badge .stat-num {
            font-family: 'JetBrains Mono', monospace;
            font-weight: 700;
            color: var(--primary);
            font-size: 1rem;
        }

        /* BUTTONS */
        .btn-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            width: 100%;
        }

        .action-btn {
            padding: 13px 16px;
            border: none;
            border-radius: 12px;
            font-size: 0.95rem;
            font-weight: 700;
            cursor: pointer;
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 8px;
            transition: all 0.2s ease;
        }

        .action-btn:active {
            transform: scale(0.97);
        }

        .btn-stop {
            width: 100%;
            background: rgba(251, 113, 133, 0.15);
            border: 1px solid var(--danger);
            color: var(--danger);
            font-size: 1rem;
            padding: 13px;
        }
        .btn-stop:hover, .btn-stop:active {
            background: var(--danger);
            color: #0b1120;
            box-shadow: 0 0 16px var(--danger-glow);
        }

        .btn-feed {
            width: 100%;
            background: linear-gradient(135deg, var(--warning), #f59e0b);
            color: #0b1120;
            box-shadow: 0 4px 15px rgba(251, 191, 36, 0.25);
        }

        .btn-auto-start {
            background: rgba(56, 189, 248, 0.15);
            border: 1px solid var(--primary);
            color: var(--primary);
        }
        .btn-auto-start:hover, .btn-auto-start:active {
            background: var(--primary);
            color: #0b1120;
            box-shadow: 0 0 14px var(--primary-glow);
        }

        .btn-auto-stop {
            background: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(255, 255, 255, 0.15);
            color: var(--text-dim);
        }
        .btn-auto-stop:hover, .btn-auto-stop:active {
            background: rgba(255, 255, 255, 0.12);
            color: var(--text-main);
        }

        /* SYSTEM STATUS FOOTER */
        .system-banner {
            padding: 12px 16px;
            border-radius: 12px;
            font-size: 0.85rem;
            font-weight: 600;
            display: flex;
            align-items: center;
            gap: 8px;
            background: rgba(15, 23, 42, 0.5);
            border: 1px solid var(--card-border);
        }

        /* RESPONSIVE */
        @media (max-width: 800px) {
            .main-grid {
                grid-template-columns: 1fr;
            }
            .header {
                flex-direction: column;
                align-items: flex-start;
                gap: 12px;
            }
            .status-indicator {
                align-self: flex-start;
            }
        }

        @media (max-width: 480px) {
            body {
                padding: 10px;
            }
            .telemetry-grid {
                grid-template-columns: 1fr;
            }
            .joystick-zone {
                width: 200px;
                height: 200px;
            }
        }
    </style>
</head>

<body>
<div class="container">

    <!-- HEADER -->
    <div class="header">
        <div class="header-title">
            <h1>USV MINI DASHBOARD</h1>
            <p>Trạm Giám Sát & Điều Khiển Tàu Không Người Lái</p>
        </div>
        <div class="status-indicator">
            <div id="statusDot" class="dot"></div>
            <span id="statusText">Đang kết nối ESP32...</span>
        </div>
    </div>

    <!-- MAIN GRID -->
    <div class="main-grid">

        <!-- THẺ THÔNG SỐ HÀNH TRÌNH (TELEMETRY) -->
        <div class="card">
            <div class="card-header">
                <div class="title-group">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"/></svg>
                    <span>Dữ Liệu Hành Trình</span>
                </div>
                <span id="gpsBadge" class="status-badge err">GPS: TÌM VỆ TINH</span>
            </div>

            <div class="telemetry-grid">
                <!-- Vận tốc -->
                <div class="data-box">
                    <span class="data-label">Vận Tốc (m/s)</span>
                    <span class="data-value" id="valSpeed">0.00</span>
                </div>

                <!-- Hướng bàn -->
                <div class="data-box">
                    <span class="data-label">Hướng La Bàn</span>
                    <span class="data-value" id="valHeading">0.0°</span>
                </div>

                <!-- Tọa độ GPS Lat/Lon -->
                <div class="data-box" style="grid-column: span 2;">
                    <span class="data-label">Tọa Độ GPS (Vĩ độ / Kinh độ)</span>
                    <span class="data-value gps-val" id="valGPS">0.000000, 0.000000</span>
                </div>

                <!-- Điện áp Pin -->
                <div class="data-box">
                    <span class="data-label">Điện Áp Pin</span>
                    <span class="data-value" id="valBattery" style="color: var(--success);">0.00 V</span>
                </div>

                <!-- Dòng tiêu thụ -->
                <div class="data-box">
                    <span class="data-label">Dòng Tiêu Thụ</span>
                    <span class="data-value" id="valCurrent">0.00 A</span>
                </div>

                <!-- Nhiệt độ nước -->
                <div class="data-box">
                    <span class="data-label">Nhiệt Độ Nước</span>
                    <span class="data-value" id="valTemp" style="color: var(--danger);">0.0°C</span>
                </div>

                <!-- Rải mồi -->
                <div class="data-box">
                    <span class="data-label">Trạng Thái Mồi</span>
                    <span class="data-value" id="valFeed" style="color: var(--warning);">0 %</span>
                </div>
            </div>

            <!-- Banner trạng thái hệ thống -->
            <div class="system-banner" id="systemBanner">
                <span style="color: var(--text-dim);">Trạng thái phần cứng:</span>
                <span id="valError" style="color: var(--success); font-weight: 700;">Hệ thống sẵn sàng (System OK)</span>
            </div>
        </div>

        <!-- THẺ ĐIỀU KHIỂN ĐỘNG CƠ (JOYSTICK & CONTROLS) -->
        <div class="card">
            <div class="card-header">
                <div class="title-group">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"/><path d="m4.93 4.93 4.24 4.24M14.83 9.17l4.24-4.24M14.83 14.83l4.24 4.24M9.17 14.83l-4.24 4.24"/></svg>
                    <span>Cần Gạt Điều Khiển</span>
                </div>
                <span id="modeBadge" class="status-badge ok">MANUAL</span>
            </div>

            <div class="controls-container">
                <!-- Chuyển đổi chế độ lái -->
                <div class="mode-switch">
                    <button class="mode-btn active" id="btnManual" onclick="setMode('MANUAL')">BẰNG TAY (MANUAL)</button>
                    <button class="mode-btn" id="btnAuto" onclick="setMode('AUTO')">TỰ ĐỘNG (AUTO)</button>
                </div>

                <!-- Bảng Joystick ảo -->
                <div class="joystick-wrapper">
                    <div class="joystick-zone" id="joyZone">
                        <div class="joystick-ring ring-outer"></div>
                        <div class="joystick-ring ring-inner"></div>
                        <div class="joystick-axis-x"></div>
                        <div class="joystick-axis-y"></div>
                        <span class="joystick-label-dir up">TIẾN</span>
                        <span class="joystick-label-dir down">LÙI</span>
                        <span class="joystick-label-dir left">TRÁI</span>
                        <span class="joystick-label-dir right">PHẢI</span>
                        <div class="joystick-knob" id="joyKnob"></div>
                    </div>

                    <!-- Chỉ số công suất 2 động cơ L/R -->
                    <div class="joystick-stats">
                        <div class="stat-badge">
                            <span class="stat-name">Động cơ Trái (L):</span>
                            <span class="stat-num" id="joyLeftStat">0%</span>
                        </div>
                        <div class="stat-badge">
                            <span class="stat-name">Động cơ Phải (R):</span>
                            <span class="stat-num" id="joyRightStat">0%</span>
                        </div>
                    </div>

                    <!-- Nút dừng khẩn cấp -->
                    <button class="action-btn btn-stop" onclick="emergencyStop()">
                        <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><rect x="9" y="9" width="6" height="6"/></svg>
                        DỪNG KHẨN CẤP (STOP)
                    </button>
                </div>

                <!-- Nút Rải Mồi (Feed) -->
                <button class="action-btn btn-feed" onclick="sendFeed()">
                    <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 2v20M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6"/></svg>
                    RẢI MỒI CÂU (500 ms)
                </button>

                <!-- Điều hướng tự động -->
                <div class="btn-grid">
                    <button class="action-btn btn-auto-start" onclick="autoStart()">
                        <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor"><polygon points="5 3 19 12 5 21 5 3"/></svg>
                        AUTO START
                    </button>
                    <button class="action-btn btn-auto-stop" onclick="autoStop()">
                        <svg width="16" height="16" viewBox="0 0 24 24" fill="currentColor"><rect x="6" y="6" width="12" height="12"/></svg>
                        AUTO STOP
                    </button>
                </div>
            </div>
        </div>

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
        command("FEED|500");
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
