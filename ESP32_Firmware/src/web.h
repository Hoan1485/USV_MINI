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
    <link
        href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;800&family=JetBrains+Mono:wght@400;700&display=swap"
        rel="stylesheet">

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
            padding: 16px 16px calc(88px + env(safe-area-inset-bottom, 0px)) 16px;
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

        .badge,
        .status-badge {
            font-size: 0.75rem;
            font-weight: 700;
            padding: 4px 10px;
            border-radius: 6px;
        }

        .badge.err,
        .status-badge.err {
            background: var(--danger-light);
            color: var(--danger);
        }

        .badge.ok,
        .status-badge.ok {
            background: var(--success-light);
            color: var(--success);
        }

        .badge.primary,
        .status-badge.primary {
            background: var(--primary-light);
            color: var(--primary);
        }

        /* BOTTOM NAVIGATION BAR (SIDEBAR DƯỚI CÙNG TRANG WEB) */
        .bottom-nav {
            position: fixed;
            bottom: 0;
            left: 0;
            right: 0;
            display: flex;
            justify-content: center;
            padding: 10px 16px calc(10px + env(safe-area-inset-bottom, 0px)) 16px;
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            border-top: 1px solid var(--border);
            box-shadow: 0 -4px 20px rgba(15, 23, 42, 0.08);
            z-index: 1000;
        }

        .bottom-nav-inner {
            max-width: 600px;
            width: 100%;
            display: flex;
            gap: 12px;
        }

        .nav-tab {
            flex: 1;
            padding: 13px 18px;
            border: none;
            background: var(--bg);
            border-radius: var(--radius-md);
            font-weight: 700;
            font-size: 0.95rem;
            color: var(--text-dim);
            cursor: pointer;
            transition: all 0.25s ease;
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 10px;
        }

        .nav-tab i {
            font-size: 1.15rem;
        }

        .nav-tab.active {
            background: var(--primary);
            color: #ffffff;
            box-shadow: 0 4px 14px rgba(37, 99, 235, 0.3);
        }

        .nav-tab:hover:not(.active) {
            background: #E2E8F0;
            color: var(--text-main);
        }

        .tab-content {
            display: flex;
            flex-direction: column;
            gap: 20px;
        }

        .tab-content.hidden {
            display: none;
        }

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

        .data-value.gps-val {
            font-size: 0.9rem;
        }

        .highlight-blue {
            color: var(--primary);
        }

        .highlight-green {
            color: var(--success);
        }

        .highlight-orange {
            color: var(--warning);
        }

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

        /* COCKPIT CONTROLS (TỐI GIẢN & CÂN ĐỐI) */
        .cockpit-container {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            background: var(--bg);
            padding: 20px 16px;
            border-radius: var(--radius-lg);
            border: 1px solid var(--border);
            margin: 6px 0 16px 0;
            align-items: stretch;
        }

        .control-col {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: space-between;
            min-height: 236px;
            width: 100%;
        }

        .control-header {
            display: flex;
            align-items: center;
            justify-content: space-between;
            width: 100%;
            height: 28px;
            padding: 0 2px;
            box-sizing: border-box;
        }

        .control-title {
            font-size: 0.82rem;
            font-weight: 800;
            color: var(--text-dim);
            letter-spacing: 0.5px;
            white-space: nowrap;
            display: flex;
            align-items: center;
            gap: 6px;
        }

        .control-title i {
            color: var(--primary);
            font-size: 0.85rem;
        }

        .control-badge {
            font-family: 'JetBrains Mono', monospace;
            font-size: 0.82rem;
            font-weight: 800;
            padding: 2px 8px;
            border-radius: 6px;
            background: var(--surface);
            color: var(--primary);
            border: 1px solid var(--border);
            min-width: 52px;
            height: 26px;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            text-align: center;
            white-space: nowrap;
            box-sizing: border-box;
        }

        .control-badge.stop {
            color: var(--text-dim);
        }

        .control-hint {
            font-size: 0.72rem;
            color: var(--text-dim);
            font-weight: 500;
            height: 18px;
            line-height: 18px;
            text-align: center;
            white-space: nowrap;
        }

        /* SPEED SLIDER (CẦN TỐC ĐỘ) */
        .speed-box {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            flex: 1;
            padding: 6px 0;
            width: 100%;
        }

        .speed-track {
            width: 44px;
            height: 145px;
            background: var(--surface);
            border: 2px solid var(--border);
            border-radius: 22px;
            position: relative;
            box-shadow: inset 0 2px 4px rgba(15, 23, 42, 0.06);
            touch-action: none;
            cursor: pointer;
            overflow: hidden;
        }

        .speed-fill {
            position: absolute;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(180deg, #3B82F6, #2563EB);
            border-radius: 0 0 20px 20px;
            pointer-events: none;
            transition: none;
        }

        .speed-thumb {
            position: absolute;
            left: 3px;
            bottom: 3px;
            width: 34px;
            height: 34px;
            border-radius: 50%;
            background: #FFFFFF;
            border: 3px solid var(--primary);
            box-shadow: 0 3px 8px rgba(0, 0, 0, 0.16);
            pointer-events: none;
            z-index: 5;
        }

        /* STEERING SLIDER (THANH GẠT HƯỚNG LÁI) */
        .steer-box {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            flex: 1;
            gap: 8px;
            width: 100%;
            padding: 6px 0;
        }

        .steer-helm-wrap {
            width: 42px;
            height: 42px;
            border-radius: 50%;
            background: var(--surface);
            border: 2px solid var(--border);
            display: flex;
            align-items: center;
            justify-content: center;
            color: var(--primary);
            font-size: 1.35rem;
            box-shadow: var(--shadow-sm);
        }

        .steer-helm-wrap i {
            transition: transform 0.12s ease-out;
            will-change: transform;
        }

        .steer-labels {
            display: grid;
            grid-template-columns: 1fr auto 1fr;
            align-items: center;
            width: 100%;
            max-width: 180px;
            font-size: 0.72rem;
            font-weight: 700;
            color: var(--text-dim);
            letter-spacing: 0.3px;
        }

        .steer-labels .steer-left-label {
            text-align: left;
        }

        .steer-labels .steer-mid-label {
            text-align: center;
            color: var(--text-dim);
            font-weight: 700;
            padding: 0 4px;
        }

        .steer-labels .steer-right-label {
            text-align: right;
        }

        .steer-track {
            width: 100%;
            max-width: 180px;
            height: 44px;
            background: var(--surface);
            border: 2px solid var(--border);
            border-radius: 22px;
            position: relative;
            box-shadow: inset 0 2px 4px rgba(15, 23, 42, 0.06);
            touch-action: none;
            cursor: pointer;
            overflow: hidden;
        }

        .steer-center-line {
            position: absolute;
            left: 50%;
            top: 6px;
            bottom: 6px;
            width: 2px;
            background: var(--border);
            transform: translateX(-50%);
            pointer-events: none;
            z-index: 2;
        }

        .steer-fill-left {
            position: absolute;
            top: 0;
            bottom: 0;
            right: 50%;
            width: 0px;
            background: linear-gradient(270deg, #3B82F6, #2563EB);
            pointer-events: none;
            z-index: 1;
        }

        .steer-fill-right {
            position: absolute;
            top: 0;
            bottom: 0;
            left: 50%;
            width: 0px;
            background: linear-gradient(90deg, #3B82F6, #2563EB);
            pointer-events: none;
            z-index: 1;
        }

        .steer-thumb {
            position: absolute;
            top: 3px;
            left: calc(50% - 17px);
            width: 34px;
            height: 34px;
            border-radius: 50%;
            background: #FFFFFF;
            border: 3px solid var(--primary);
            box-shadow: 0 3px 8px rgba(0, 0, 0, 0.16);
            pointer-events: none;
            z-index: 5;
            display: flex;
            align-items: center;
            justify-content: center;
            color: var(--primary);
        }

        .control-stats-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 12px;
            width: 100%;
            margin-bottom: 6px;
        }

        .stat-badge {
            background: var(--bg);
            padding: 10px;
            border-radius: var(--radius-md);
            text-align: center;
            font-size: 0.85rem;
            font-weight: 600;
            color: var(--text-dim);
        }

        .stat-badge span {
            color: var(--primary);
            font-size: 1.1rem;
            display: block;
            font-family: monospace;
        }

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

        .btn:active {
            transform: scale(0.98);
        }

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



        <!-- PHẦN 1: TRANG CHỦ -->
        <div id="sectionHome" class="tab-content">
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
                        <span class="data-value highlight-blue" id="valSpeed">0.00 <span
                                style="font-size:0.8rem">m/s</span></span>
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

                <div
                    style="background: var(--bg); padding: 12px; border-radius: var(--radius-md); font-size: 0.85rem; font-weight: 600; display:flex; justify-content:space-between;">
                    <span style="color: var(--text-dim);">Trạng thái phần cứng:</span>
                    <span id="valError" class="highlight-green">Sẵn sàng (OK)</span>
                </div>
            </div>
        </div>

        <!-- PHẦN 2: ĐIỀU KHIỂN -->
        <div id="sectionControl" class="tab-content hidden">
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

                <!-- BỘ ĐIỀU KHIỂN: TỐC ĐỘ + HƯỚNG (TỐI GIẢN & ĐỒNG BỘ) -->
                <div class="cockpit-container">
                    <!-- CẦN TỐC ĐỘ -->
                    <div class="control-col">
                        <div class="control-header">
                            <span class="control-title"><i class="fa-solid fa-gauge"></i> TỐC ĐỘ</span>
                            <span id="speedBadge" class="control-badge stop">0%</span>
                        </div>
                        <div class="speed-box">
                            <div class="speed-track" id="speedTrack">
                                <div class="speed-fill" id="speedFill"></div>
                                <div class="speed-thumb" id="speedHandle"></div>
                            </div>
                        </div>
                        <div class="control-hint">Kéo cần để chỉnh ga</div>
                    </div>

                    <!-- THANH GẠT HƯỚNG LÁI (TRÁI - PHẢI) -->
                    <div class="control-col">
                        <div class="control-header">
                            <span class="control-title"><i class="fa-solid fa-compass"></i> HƯỚNG LÁI</span>
                            <span id="directionBadge" class="control-badge stop">DỪNG</span>
                        </div>
                        <div class="steer-box">
                            <div class="steer-helm-wrap">
                                <i class="fa-solid fa-dharmachakra" id="steerHelm"></i>
                            </div>
                            <div class="steer-labels">
                                <span class="steer-left-label"><i class="fa-solid fa-chevron-left"></i> TRÁI</span>
                                <span class="steer-mid-label">THẲNG</span>
                                <span class="steer-right-label">PHẢI <i class="fa-solid fa-chevron-right"></i></span>
                            </div>
                            <div class="steer-track" id="steerTrack">
                                <div class="steer-center-line"></div>
                                <div class="steer-fill-left" id="steerFillLeft"></div>
                                <div class="steer-fill-right" id="steerFillRight"></div>
                                <div class="steer-thumb" id="steerHandle">
                                    <i class="fa-solid fa-arrows-left-right" style="font-size: 0.7rem; opacity: 0.85;"></i>
                                </div>
                            </div>
                        </div>
                        <div class="control-hint">Thả tay tự hồi về giữa</div>
                    </div>
                </div>

                <!-- THỐNG KÊ CÔNG SUẤT MOTOR -->
                <div class="control-stats-grid">
                    <div class="stat-badge">Motor L (Trái)<span id="joyLeftStat">0%</span></div>
                    <div class="stat-badge">Motor R (Phải)<span id="joyRightStat">0%</span></div>
                </div>

                <button class="btn btn-feed" onclick="sendFeed()">
                    <i class="fa-solid fa-wheat-awn"></i> RẢI MỒI (1 VÒNG)
                </button>

                <button class="btn btn-stop" onclick="emergencyStop()">
                    <i class="fa-solid fa-hand"></i> DỪNG KHẨN CẤP
                </button>
            </div>
        </div>

    </div>

    <!-- BOTTOM NAVIGATION BAR (THANH ĐIỀU HƯỚNG DƯỚI CÙNG TRANG WEB) -->
    <nav class="bottom-nav">
        <div class="bottom-nav-inner">
            <button class="nav-tab active" id="tabBtnHome" onclick="switchTab('home')">
                <i class="fa-solid fa-house"></i>
                <span>TRANG CHỦ</span>
            </button>
            <button class="nav-tab" id="tabBtnControl" onclick="switchTab('control')">
                <i class="fa-solid fa-gamepad"></i>
                <span>ĐIỀU KHIỂN</span>
            </button>
        </div>
    </nav>

    <!-- JAVASCRIPT LOGIC CLIENT -->
    <script>
        // ==========================================================
        // PHẢN HỒI RUNG (HAPTIC FEEDBACK CHO ĐIỆN THOẠI)
        // ==========================================================
        function triggerHaptic(duration = 20) {
            if ("vibrate" in navigator) {
                try { navigator.vibrate(duration); } catch (e) { }
            }
        }

        // ==========================================================
        // CHUYỂN ĐỔI TAB: TRANG CHỦ <-> ĐIỀU KHIỂN
        // ==========================================================
        function switchTab(tab) {
            triggerHaptic(15);
            const secHome = document.getElementById("sectionHome");
            const secControl = document.getElementById("sectionControl");
            const tabBtnHome = document.getElementById("tabBtnHome");
            const tabBtnControl = document.getElementById("tabBtnControl");

            if (tab === 'home') {
                secHome.classList.remove("hidden");
                secControl.classList.add("hidden");
                tabBtnHome.classList.add("active");
                tabBtnControl.classList.remove("active");
                // Dừng an toàn nếu đang thao tác cần gạt mà chuyển tab
                if (isSteerActive) onSteerPointerUp();
                if (isSpeedActive) onSpeedPointerUp();
            } else {
                secHome.classList.add("hidden");
                secControl.classList.remove("hidden");
                tabBtnControl.classList.add("active");
                tabBtnHome.classList.remove("active");
                // Cập nhật lại vị trí hiển thị cần gạt sau khi hiển thị tab điều khiển
                requestAnimationFrame(function () {
                    updateSpeedVisual(masterSpeed, false);
                    updateSteerVisual(steerNormX, false);
                });
            }
        }

        // ==========================================================
        // CẤU HÌNH & TRẠNG THÁI: TỐC ĐỘ + LÁI TRÁI / PHẢI
        // ==========================================================
        let isSteerActive = false;
        let isSpeedActive = false;
        let isHttpBusy = false;
        let nextCommand = null;
        let dispatchTimer = null;
        let lastSentTime = 0;
        const MIN_SEND_INTERVAL = 55; // ms (tối đa ~18 lệnh/giây, rất mượt và không nghẽn TCP)

        // Tốc độ chung của động cơ (0% đến 100%)
        let masterSpeed = 0; // Mặc định 0% khi vào trang để an toàn tuyệt đối

        // Độ bẻ lái chuẩn hóa (-1.0 Trái đến +1.0 Phải)
        let steerNormX = 0;

        // Giá trị công suất motor (-100 đến 100)
        let targetLeft = 0;
        let targetRight = 0;
        let lastSentLeft = 0;
        let lastSentRight = 0;

        // DOM Elements & animation frames
        let speedTrack, speedHandle, speedFill, speedBadge;
        let steerTrack, steerHandle, steerFillLeft, steerFillRight, directionBadge, steerHelm;
        let joyLeftStat, joyRightStat;
        let steerRafId = null;
        let speedRafId = null;

        // ==========================================================
        // KHỞI TẠO CẦN GẠT TỐC ĐỘ CHUNG (0 - 100%)
        // ==========================================================
        function initSpeedSlider() {
            speedTrack = document.getElementById("speedTrack");
            speedHandle = document.getElementById("speedHandle");
            speedFill = document.getElementById("speedFill");
            speedBadge = document.getElementById("speedBadge");

            if (!speedTrack || !speedHandle) return;

            speedTrack.addEventListener("pointerdown", onSpeedPointerDown, { passive: false });
            speedTrack.addEventListener("pointermove", onSpeedPointerMove, { passive: false });
            speedTrack.addEventListener("pointerup", onSpeedPointerUp, { passive: false });
            speedTrack.addEventListener("pointercancel", onSpeedPointerUp, { passive: false });

            updateSpeedVisual(masterSpeed, false);
        }

        function onSpeedPointerDown(e) {
            e.preventDefault();
            isSpeedActive = true;
            speedTrack.setPointerCapture(e.pointerId);
            speedHandle.style.transition = "none";
            handleSpeedPointerMove(e.clientY);
        }

        function onSpeedPointerMove(e) {
            if (!isSpeedActive) return;
            e.preventDefault();
            handleSpeedPointerMove(e.clientY);
        }

        function onSpeedPointerUp(e) {
            if (!isSpeedActive) return;
            isSpeedActive = false;
            setTimeout(updateTelemetry, 300);
        }

        function handleSpeedPointerMove(clientY) {
            const rect = speedTrack.getBoundingClientRect();
            const handleH = speedHandle.offsetHeight || 34;
            const usableH = rect.height - handleH;

            let relY = clientY - (rect.top + handleH / 2);
            relY = Math.max(0, Math.min(usableH, relY));

            let ratio = usableH > 0 ? (1 - (relY / usableH)) : 0;
            let val = Math.round(ratio * 100);
            val = Math.max(0, Math.min(100, val));

            masterSpeed = val;
            updateSpeedVisual(masterSpeed, false);
            updateDirectionBadge(steerNormX);
            calculateAndDispatchMotors();
        }

        function updateSpeedVisual(val, animated) {
            if (!speedTrack || !speedHandle) return;

            const rect = speedTrack.getBoundingClientRect();
            const trackH = rect.height > 0 ? rect.height : 145;
            const handleH = speedHandle.offsetHeight || 34;
            const usableH = trackH - handleH;

            let ratio = val / 100;
            let topPx = (1 - ratio) * usableH;

            if (speedRafId) cancelAnimationFrame(speedRafId);
            speedRafId = requestAnimationFrame(function () {
                speedHandle.style.transition = animated ? "top 0.2s cubic-bezier(0.18, 0.89, 0.32, 1.28)" : "none";
                speedHandle.style.top = topPx.toFixed(1) + "px";

                if (speedFill) {
                    let fillH = (val / 100) * trackH;
                    speedFill.style.height = fillH.toFixed(1) + "px";
                }

                if (speedBadge) {
                    speedBadge.innerText = val + "%";
                    speedBadge.className = val > 0 ? "control-badge" : "control-badge stop";
                }
            });
        }

        function setMasterSpeed(val) {
            masterSpeed = Math.max(0, Math.min(100, val));
            updateSpeedVisual(masterSpeed, true);
            updateDirectionBadge(steerNormX);
            calculateAndDispatchMotors();
        }

        // ==========================================================
        // KHỞI TẠO CẦN LÁI TRÁI - PHẢI (THANH TRƯỢT NGANG)
        // ==========================================================
        function initSteering() {
            steerTrack = document.getElementById("steerTrack");
            steerHandle = document.getElementById("steerHandle");
            steerFillLeft = document.getElementById("steerFillLeft");
            steerFillRight = document.getElementById("steerFillRight");
            steerHelm = document.getElementById("steerHelm");
            directionBadge = document.getElementById("directionBadge");
            joyLeftStat = document.getElementById("joyLeftStat");
            joyRightStat = document.getElementById("joyRightStat");

            if (!steerTrack || !steerHandle) return;

            steerTrack.addEventListener("pointerdown", onSteerPointerDown, { passive: false });
            steerTrack.addEventListener("pointermove", onSteerPointerMove, { passive: false });
            steerTrack.addEventListener("pointerup", onSteerPointerUp, { passive: false });
            steerTrack.addEventListener("pointercancel", onSteerPointerUp, { passive: false });

            updateSteerVisual(0, false);
            updateDirectionBadge(0);
        }

        function onSteerPointerDown(e) {
            e.preventDefault();
            isSteerActive = true;
            steerTrack.setPointerCapture(e.pointerId);
            steerHandle.style.transition = "none";
            if (steerFillLeft) steerFillLeft.style.transition = "none";
            if (steerFillRight) steerFillRight.style.transition = "none";
            handleSteerPointerMove(e.clientX);
        }

        function onSteerPointerMove(e) {
            if (!isSteerActive) return;
            e.preventDefault();
            handleSteerPointerMove(e.clientX);
        }

        function onSteerPointerUp(e) {
            if (!isSteerActive) return;
            isSteerActive = false;

            // Lò xo tự hồi về giữa 0 (Chạy thẳng an toàn)
            steerNormX = 0;
            updateSteerVisual(0, true);
            updateDirectionBadge(0);
            calculateAndDispatchMotors();
            setTimeout(updateTelemetry, 300);
        }

        function handleSteerPointerMove(clientX) {
            const rect = steerTrack.getBoundingClientRect();
            const handleW = steerHandle.offsetWidth || 34;
            const trackW = rect.width > 0 ? rect.width : 180;
            const usableW = trackW - handleW;
            const centerPx = usableW / 2;

            let relX = clientX - (rect.left + handleW / 2);
            relX = Math.max(0, Math.min(usableW, relX));

            let deltaX = relX - centerPx; // -centerPx -> +centerPx
            let norm = centerPx > 0 ? (deltaX / centerPx) : 0;
            norm = Math.max(-1.0, Math.min(1.0, norm));

            // Vùng chết Deadzone 8% tại tâm
            if (Math.abs(norm) < 0.08) {
                steerNormX = 0;
            } else {
                steerNormX = norm;
            }

            updateSteerVisual(steerNormX, false);
            updateDirectionBadge(steerNormX);
            calculateAndDispatchMotors();
        }

        function updateSteerVisual(normX, animated) {
            if (!steerTrack || !steerHandle) return;

            const rect = steerTrack.getBoundingClientRect();
            const trackW = rect.width > 0 ? rect.width : 180;
            const handleW = steerHandle.offsetWidth || 34;
            const usableW = trackW - handleW;
            const centerPx = usableW / 2;

            let posLeft = centerPx + (normX * centerPx);

            if (steerRafId) cancelAnimationFrame(steerRafId);
            steerRafId = requestAnimationFrame(function () {
                const tr = animated ? "left 0.22s cubic-bezier(0.18, 0.89, 0.32, 1.28)" : "none";
                steerHandle.style.transition = tr;
                steerHandle.style.left = posLeft.toFixed(1) + "px";

                if (steerFillLeft && steerFillRight) {
                    steerFillLeft.style.transition = animated ? "width 0.22s ease-out" : "none";
                    steerFillRight.style.transition = animated ? "width 0.22s ease-out" : "none";

                    if (normX < 0) {
                        let fillW = Math.abs(normX) * (trackW / 2);
                        steerFillLeft.style.width = fillW.toFixed(1) + "px";
                        steerFillRight.style.width = "0px";
                    } else if (normX > 0) {
                        let fillW = normX * (trackW / 2);
                        steerFillRight.style.width = fillW.toFixed(1) + "px";
                        steerFillLeft.style.width = "0px";
                    } else {
                        steerFillLeft.style.width = "0px";
                        steerFillRight.style.width = "0px";
                    }
                }

                if (steerHelm) {
                    steerHelm.style.transform = "rotate(" + (normX * 60).toFixed(1) + "deg)";
                }
            });
        }

        function updateDirectionBadge(nx) {
            if (!directionBadge) return;

            if (masterSpeed === 0) {
                directionBadge.innerText = "DỪNG";
                directionBadge.className = "control-badge stop";
                return;
            }

            if (Math.abs(nx) <= 0.08) {
                directionBadge.innerText = "THẲNG";
                directionBadge.className = "control-badge";
            } else if (nx < 0) {
                let pct = Math.round(Math.abs(nx) * 100);
                directionBadge.innerText = "TRÁI " + pct + "%";
                directionBadge.className = "control-badge";
            } else {
                let pct = Math.round(nx * 100);
                directionBadge.innerText = "PHẢI " + pct + "%";
                directionBadge.className = "control-badge";
            }
        }

        // ==========================================================
        // TÍNH TOÁN CÔNG SUẤT ĐỘNG CƠ: TỐC ĐỘ + VI SAI LÁI TRÁI/PHẢI
        // ==========================================================
        function calculateAndDispatchMotors() {
            if (masterSpeed === 0) {
                targetLeft = 0;
                targetRight = 0;
            } else {
                // Vi sai bẻ lái: bẻ bên nào thì bên đó giảm, bên đối diện giữ/tăng
                let turn = steerNormX * Math.max(masterSpeed, 40);

                let left = masterSpeed + turn;
                let right = masterSpeed - turn;

                targetLeft = Math.max(-100, Math.min(100, Math.round(left)));
                targetRight = Math.max(-100, Math.min(100, Math.round(right)));
            }

            if (joyLeftStat) joyLeftStat.innerText = (targetLeft > 0 ? "+" : "") + targetLeft + "%";
            if (joyRightStat) joyRightStat.innerText = (targetRight > 0 ? "+" : "") + targetRight + "%";

            if (targetLeft === 0 && targetRight === 0) {
                queueStop();
            } else {
                queueMotor(targetLeft, targetRight);
            }
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
                    dispatchTimer = setTimeout(function () {
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
                .catch(function (err) {
                    console.warn("Lệnh không tới được ESP32:", err);
                })
                .finally(function () {
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
                .catch(function (err) { console.error("Lỗi gửi lệnh:", err); });
        }

        function emergencyStop() {
            isSteerActive = false;
            steerNormX = 0;
            masterSpeed = 0;
            updateSpeedVisual(0, true);
            updateSteerVisual(0, true);
            updateDirectionBadge(0);
            targetLeft = 0;
            targetRight = 0;
            if (joyLeftStat) joyLeftStat.innerText = "0%";
            if (joyRightStat) joyRightStat.innerText = "0%";
            command("STOP");
            triggerHaptic(60);
        }

        function setMode(mode) {
            triggerHaptic(20);
            if (mode === "AUTO") {
                command("MODE|AUTO");
                command("AUTO_START");
            } else {
                command("MODE|MANUAL");
                command("AUTO_STOP");
            }
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

        function sendFeed() {
            triggerHaptic(35);
            command("FEED|1");
        }

        // ==========================================================
        // ĐIỀU KHIỂN BẰNG BÀN PHÍM TRÊN MÁY TÍNH (PC / LAPTOP)
        // ==========================================================
        const keysPressed = {};

        window.addEventListener("keydown", function (e) {
            const secControl = document.getElementById("sectionControl");
            if (!secControl || secControl.classList.contains("hidden")) return;
            if (e.target.tagName === "INPUT" || e.target.tagName === "TEXTAREA") return;

            const k = e.key.toLowerCase();

            // Phím Space: Dừng khẩn cấp
            if (e.code === "Space") {
                e.preventDefault();
                emergencyStop();
                return;
            }

            // Phím 0 - 5: Đặt nhanh mức tốc độ
            if (e.key >= '0' && e.key <= '5') {
                const speeds = { '0': 0, '1': 20, '2': 40, '3': 60, '4': 80, '5': 100 };
                setMasterSpeed(speeds[e.key]);
                return;
            }

            // Phím W / Mũi tên lên: Tăng tốc độ (+5%)
            if (k === 'w' || k === 'arrowup') {
                e.preventDefault();
                setMasterSpeed(Math.min(100, masterSpeed + 5));
                return;
            }

            // Phím S / Mũi tên xuống: Giảm tốc độ (-5%)
            if (k === 's' || k === 'arrowdown') {
                e.preventDefault();
                setMasterSpeed(Math.max(0, masterSpeed - 5));
                return;
            }

            // Phím A / D hoặc Mũi tên trái / phải: Bẻ lái
            if (['a', 'd', 'arrowleft', 'arrowright'].includes(k)) {
                e.preventDefault();
                if (!keysPressed[k]) {
                    keysPressed[k] = true;
                    updateKeyboardSteer();
                }
            }
        });

        window.addEventListener("keyup", function (e) {
            const k = e.key.toLowerCase();
            if (keysPressed[k]) {
                delete keysPressed[k];
                updateKeyboardSteer();
            }
        });

        function updateKeyboardSteer() {
            let kx = 0;
            if (keysPressed['d'] || keysPressed['arrowright']) kx += 1.0;
            if (keysPressed['a'] || keysPressed['arrowleft']) kx -= 1.0;

            steerNormX = kx;

            if (!steerTrack || !steerHandle) return;

            if (kx === 0) {
                isSteerActive = false;
                updateSteerVisual(0, true);
                updateDirectionBadge(0);
                calculateAndDispatchMotors();
            } else {
                isSteerActive = true;
                updateSteerVisual(steerNormX, false);
                updateDirectionBadge(steerNormX);
                calculateAndDispatchMotors();
            }
        }

        // ==========================================================
        // TỰ ĐỘNG CẬP NHẬT KHI XOAY MÀN HÌNH HOẶC RESIZE CỬA SỔ
        // ==========================================================
        window.addEventListener("resize", function () {
            updateSpeedVisual(masterSpeed, false);
            updateSteerVisual(steerNormX, false);
        });

        // ==========================================================
        // LẤY DỮ LIỆU TELEMETRY ĐỊNH KỲ (THÔNG MINH, KHÔNG TRANH CHẤP KHI LÁI)
        // ==========================================================
        function updateTelemetry() {
            // QUAN TRỌNG: Nếu người dùng đang kéo cần tốc độ hoặc lái, tạm ngưng polling
            // để dành 100% băng thông Wi-Fi và CPU cho lệnh động cơ, loại bỏ hoàn toàn lag!
            if (isSteerActive || isSpeedActive || isHttpBusy) return;

            fetch("/status", { cache: "no-store" })
                .then(function (res) {
                    if (!res.ok) throw new Error("HTTP " + res.status);
                    return res.json();
                })
                .then(function (data) {
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
                .catch(function (err) {
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
        window.addEventListener("DOMContentLoaded", function () {
            initSpeedSlider();
            initSteering();
            updateTelemetry();
        });
    </script>
</body>

</html>
)rawliteral";

#endif // WEB_H
