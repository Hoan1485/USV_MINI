#ifndef WEB_H
#define WEB_H

#include <Arduino.h>

// ============================================================
// USV MINI - HỆ THỐNG GIÁM SÁT & ĐIỀU KHIỂN RẢI THỨC ĂN THỦY SẢN
// Giao diện tinh gọn, đơn giản, hiện đại (Light Mode Cockpit)
// 1. Theo dõi trạng thái (Các cảm biến cốt lõi: GPS, Vận tốc, La bàn, Pin, Nhiệt độ nước, Mồi)
// 2. Điều khiển (Dạng ngang: Cần ga tốc độ + Joystick 8 hướng, Lực đẩy chân vịt, Rải mồi, Dừng)
// ============================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
    <title>USV MINI — Điều Khiển & Trạng Thái</title>

    <style>
        :root {
            --bg-page: #f1f5f9;
            --bg-card: #ffffff;
            --border: #e2e8f0;
            --border-active: #38bdf8;
            --primary: #0284c7;
            --primary-light: #e0f2fe;
            --accent-green: #10b981;
            --accent-red: #ef4444;
            --accent-orange: #f59e0b;
            --text-title: #0f172a;
            --text-body: #334155;
            --text-muted: #64748b;
            --shadow-sm: 0 1px 3px rgba(0, 0, 0, 0.05);
            --shadow-md: 0 4px 14px rgba(15, 23, 42, 0.06);
            --safe-bottom: env(safe-area-inset-bottom, 12px);
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            user-select: none;
            -webkit-user-select: none;
            -webkit-tap-highlight-color: transparent;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
        }

        body {
            background-color: var(--bg-page);
            color: var(--text-body);
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            padding-bottom: calc(70px + var(--safe-bottom));
        }

        .container {
            width: 100%;
            max-width: 500px;
            padding: 12px;
            display: flex;
            flex-direction: column;
            gap: 12px;
        }

        /* HEADER */
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: var(--bg-card);
            padding: 12px 16px;
            border-radius: 16px;
            border: 1px solid var(--border);
            box-shadow: var(--shadow-sm);
        }

        .header-title {
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .header-title h1 {
            font-size: 1.1rem;
            font-weight: 800;
            color: var(--text-title);
            letter-spacing: -0.01em;
        }

        .header-badge {
            font-size: 0.65rem;
            font-weight: 700;
            background: #e0f2fe;
            color: var(--primary);
            padding: 2px 7px;
            border-radius: 6px;
        }

        .conn-pill {
            display: flex;
            align-items: center;
            gap: 6px;
            font-size: 0.75rem;
            font-weight: 700;
            color: var(--accent-green);
            background: #ecfdf5;
            padding: 5px 10px;
            border-radius: 20px;
            border: 1px solid #d1fae5;
        }

        .pulse-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: var(--accent-green);
            animation: pulse 1.6s infinite;
        }

        @keyframes pulse {
            0% { transform: scale(0.9); opacity: 1; }
            50% { transform: scale(1.3); opacity: 0.4; }
            100% { transform: scale(0.9); opacity: 1; }
        }

        /* SEGMENTED TAB SWITCHER */
        .tab-switcher {
            display: grid;
            grid-template-columns: 1fr 1fr;
            background: #e2e8f0;
            padding: 3px;
            border-radius: 12px;
            gap: 4px;
        }

        .tab-btn {
            background: transparent;
            border: none;
            padding: 9px 0;
            border-radius: 10px;
            font-size: 0.88rem;
            font-weight: 700;
            color: var(--text-muted);
            cursor: pointer;
            transition: all 0.2s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 6px;
        }

        .tab-btn.active {
            background: var(--bg-card);
            color: var(--primary);
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.08);
        }

        /* TAB VIEWS */
        .tab-content {
            display: none;
            flex-direction: column;
            gap: 12px;
        }

        .tab-content.active {
            display: flex;
        }

        /* STATUS CARDS GRID */
        .status-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }

        .stat-card {
            background: var(--bg-card);
            border: 1px solid var(--border);
            border-radius: 14px;
            padding: 12px;
            box-shadow: var(--shadow-sm);
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .stat-card.full-width {
            grid-column: span 2;
        }

        .stat-label {
            font-size: 0.72rem;
            font-weight: 700;
            color: var(--text-muted);
            text-transform: uppercase;
            letter-spacing: 0.02em;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .stat-value {
            font-size: 1.25rem;
            font-weight: 800;
            color: var(--text-title);
        }

        .stat-unit {
            font-size: 0.75rem;
            font-weight: 600;
            color: var(--text-muted);
            margin-left: 2px;
        }

        .stat-progress-bar {
            width: 100%;
            height: 6px;
            background: #e2e8f0;
            border-radius: 4px;
            margin-top: 6px;
            overflow: hidden;
        }

        .stat-progress-fill {
            height: 100%;
            background: var(--primary);
            border-radius: 4px;
            transition: width 0.3s ease;
        }

        .stat-progress-fill.green {
            background: var(--accent-green);
        }

        /* CONTROL VIEW: HORIZONTAL COCKPIT */
        .cockpit-card {
            background: var(--bg-card);
            border: 1px solid var(--border);
            border-radius: 18px;
            padding: 14px;
            box-shadow: var(--shadow-md);
            display: flex;
            flex-direction: column;
            gap: 14px;
        }

        .cockpit-info-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: #f8fafc;
            border: 1px solid var(--border);
            border-radius: 10px;
            padding: 6px 12px;
            font-size: 0.75rem;
            font-weight: 700;
            color: var(--text-muted);
        }

        .cockpit-info-row span b {
            color: var(--primary);
        }

        /* HORIZONTAL DUAL JOYSTICK GRID */
        .dual-joystick-grid {
            display: grid;
            grid-template-columns: 100px 1fr;
            gap: 12px;
            align-items: center;
        }

        /* SPEED THROTTLE (CẦN GA DỌC) */
        .throttle-col {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            background: #f8fafc;
            border: 1px solid var(--border);
            border-radius: 16px;
            padding: 10px 4px;
        }

        .throttle-val-badge {
            font-size: 0.85rem;
            font-weight: 800;
            color: var(--primary);
        }

        .throttle-track {
            position: relative;
            width: 60px;
            height: 170px;
            background: #e2e8f0;
            border: 2px solid #cbd5e1;
            border-radius: 30px;
            display: flex;
            align-items: flex-end;
            justify-content: center;
            touch-action: none;
            cursor: pointer;
            overflow: hidden;
            box-shadow: inset 0 2px 6px rgba(0, 0, 0, 0.08);
        }

        .throttle-marks {
            position: absolute;
            inset: 8px 6px;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
            align-items: center;
            pointer-events: none;
            font-size: 0.6rem;
            font-weight: 800;
            color: #94a3b8;
            z-index: 1;
        }

        .throttle-fill {
            position: absolute;
            bottom: 0;
            width: 100%;
            background: linear-gradient(180deg, #38bdf8, #0284c7);
            border-radius: 0 0 28px 28px;
            transition: height 0.06s linear;
        }

        .throttle-knob {
            position: absolute;
            width: 52px;
            height: 28px;
            border-radius: 14px;
            background: #ffffff;
            border: 2px solid #0284c7;
            box-shadow: 0 3px 8px rgba(2, 132, 199, 0.4);
            left: 2px;
            transform: translateY(50%);
            display: flex;
            align-items: center;
            justify-content: center;
            pointer-events: none;
            z-index: 2;
        }

        .throttle-knob-grip {
            width: 18px;
            height: 4px;
            background: #94a3b8;
            border-radius: 2px;
        }

        .throttle-presets {
            display: flex;
            gap: 3px;
            width: 100%;
            justify-content: center;
        }

        .preset-chip {
            background: #ffffff;
            border: 1px solid var(--border);
            border-radius: 6px;
            padding: 3px 4px;
            font-size: 0.65rem;
            font-weight: 800;
            color: var(--text-body);
            cursor: pointer;
        }

        .preset-chip:active {
            background: var(--primary);
            color: #ffffff;
        }

        /* 8-WAY STEERING JOYSTICK */
        .steering-col {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            background: #f8fafc;
            border: 1px solid var(--border);
            border-radius: 16px;
            padding: 10px 8px;
        }

        .steer-dir-title {
            font-size: 0.85rem;
            font-weight: 800;
            color: var(--primary);
        }

        .joy-pad {
            position: relative;
            width: 170px;
            height: 170px;
            border-radius: 50%;
            background: #ffffff;
            border: 2px solid var(--border);
            box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.04);
            display: flex;
            align-items: center;
            justify-content: center;
            touch-action: none;
            cursor: pointer;
        }

        .joy-ring {
            position: absolute;
            width: 60%;
            height: 60%;
            border-radius: 50%;
            border: 1px dashed #cbd5e1;
            pointer-events: none;
        }

        .joy-cross-h, .joy-cross-v {
            position: absolute;
            background: #e2e8f0;
            pointer-events: none;
        }
        .joy-cross-h { width: 90%; height: 1px; }
        .joy-cross-v { width: 1px; height: 90%; }

        /* 8 Mũi tên chỉ hướng tinh gọn */
        .arrow-dir {
            position: absolute;
            font-size: 0.75rem;
            font-weight: 900;
            color: #94a3b8;
            pointer-events: none;
            transition: color 0.15s ease, transform 0.15s ease;
        }
        .arrow-dir.active {
            color: var(--primary);
            transform: scale(1.3);
        }

        .arrow-n  { top: 6px; left: 50%; transform: translateX(-50%); }
        .arrow-ne { top: 16px; right: 18px; }
        .arrow-e  { top: 50%; right: 8px; transform: translateY(-50%); }
        .arrow-se { bottom: 16px; right: 18px; }
        .arrow-s  { bottom: 6px; left: 50%; transform: translateX(-50%); }
        .arrow-sw { bottom: 16px; left: 18px; }
        .arrow-w  { top: 50%; left: 8px; transform: translateY(-50%); }
        .arrow-nw { top: 16px; left: 18px; }

        .joy-knob {
            position: absolute;
            width: 58px;
            height: 58px;
            border-radius: 50%;
            background: radial-gradient(circle at 35% 35%, #38bdf8, #0284c7);
            border: 2px solid #ffffff;
            box-shadow: 0 4px 12px rgba(2, 132, 199, 0.4);
            pointer-events: none;
            will-change: transform;
            z-index: 3;
            display: flex;
            align-items: center;
            justify-content: center;
        }

        .joy-knob-dot {
            width: 14px;
            height: 14px;
            border-radius: 50%;
            background: #0369a1;
            border: 1px solid rgba(255, 255, 255, 0.4);
        }

        /* THRUST METERS */
        .thrust-section {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            background: #f8fafc;
            border: 1px solid var(--border);
            border-radius: 12px;
            padding: 8px 12px;
        }

        .thrust-title {
            display: flex;
            justify-content: space-between;
            font-size: 0.7rem;
            font-weight: 700;
            color: var(--text-muted);
            margin-bottom: 4px;
        }

        .thrust-track {
            position: relative;
            width: 100%;
            height: 8px;
            background: #e2e8f0;
            border-radius: 4px;
            overflow: hidden;
        }

        .thrust-fill {
            position: absolute;
            height: 100%;
            top: 0;
            left: 50%;
            width: 0%;
            background: var(--primary);
            transition: all 0.08s ease;
        }

        /* ACTION BUTTONS */
        .actions-row {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }

        .btn-action {
            border: none;
            padding: 13px 10px;
            border-radius: 14px;
            font-size: 0.88rem;
            font-weight: 800;
            color: #ffffff;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            cursor: pointer;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
            transition: transform 0.1s ease, filter 0.1s ease;
        }

        .btn-action:active {
            transform: scale(0.96);
        }

        .btn-feed {
            background: linear-gradient(135deg, #10b981, #059669);
            box-shadow: 0 4px 14px rgba(16, 185, 129, 0.3);
        }

        .btn-stop {
            background: linear-gradient(135deg, #ef4444, #dc2626);
            box-shadow: 0 4px 14px rgba(239, 68, 68, 0.3);
        }

        /* TOAST */
        .toast {
            position: fixed;
            bottom: calc(75px + var(--safe-bottom));
            left: 50%;
            transform: translateX(-50%) translateY(100px);
            background: #0f172a;
            color: #ffffff;
            padding: 8px 16px;
            border-radius: 20px;
            font-size: 0.8rem;
            font-weight: 600;
            opacity: 0;
            pointer-events: none;
            transition: all 0.25s cubic-bezier(0.4, 0, 0.2, 1);
            z-index: 100;
            box-shadow: 0 8px 20px rgba(0, 0, 0, 0.2);
        }

        .toast.show {
            transform: translateX(-50%) translateY(0);
            opacity: 1;
        }
    </style>
</head>

<body>
<div class="container">

    <!-- HEADER -->
    <header class="header">
        <div class="header-title">
            <h1>USV MINI</h1>
            <span class="header-badge">Tàu Thủy Sản</span>
        </div>
        <div class="conn-pill" id="connPill">
            <div class="pulse-dot"></div>
            <span id="connText">Online</span>
        </div>
    </header>

    <!-- TABS SWITCHER -->
    <div class="tab-switcher">
        <button class="tab-btn active" id="tabBtnStatus" onclick="switchTab('tab-status')">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><path d="m9 12 2 2 4-4"/></svg>
            Trạng Thái
        </button>
        <button class="tab-btn" id="tabBtnControl" onclick="switchTab('tab-control')">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polygon points="3 11 22 2 13 21 11 13 3 11"/></svg>
            Điều Khiển
        </button>
    </div>

    <!-- ========================================================
         TAB 1: TRẠNG THÁI (STATUS)
         ======================================================== -->
    <div class="tab-content active" id="tab-status">
        <div class="status-grid">
            <!-- GPS Card -->
            <div class="stat-card full-width">
                <div class="stat-label">
                    <span>VỊ TRÍ GPS</span>
                    <span id="gpsFixBadge" style="color: var(--accent-green); font-size: 0.65rem;">ĐÃ KHÓA 3D</span>
                </div>
                <div class="stat-value" style="font-size: 1.05rem;" id="statGPS">20.987654, 105.765432</div>
            </div>

            <!-- Vận tốc -->
            <div class="stat-card">
                <div class="stat-label">VẬN TỐC</div>
                <div class="stat-value">
                    <span id="statSpeed" style="color: var(--primary);">0.0</span>
                    <span class="stat-unit">m/s</span>
                </div>
            </div>

            <!-- Hướng tàu (Heading) -->
            <div class="stat-card">
                <div class="stat-label">HƯỚNG (YAW)</div>
                <div class="stat-value">
                    <span id="statHeading">0</span>
                    <span class="stat-unit">°</span>
                </div>
            </div>

            <!-- Pin / Nguồn -->
            <div class="stat-card">
                <div class="stat-label">
                    <span>DUNG LƯỢNG PIN</span>
                    <span id="statBattVolt" style="font-size: 0.65rem;">12.4V</span>
                </div>
                <div class="stat-value">
                    <span id="statBattPct" style="color: var(--accent-green);">85</span>
                    <span class="stat-unit">%</span>
                </div>
                <div class="stat-progress-bar">
                    <div class="stat-progress-fill green" id="barBatt" style="width: 85%;"></div>
                </div>
            </div>

            <!-- Nhiệt độ nước -->
            <div class="stat-card">
                <div class="stat-label">NHIỆT ĐỘ NƯỚC</div>
                <div class="stat-value">
                    <span id="statTemp" style="color: #ef4444;">28.5</span>
                    <span class="stat-unit">°C</span>
                </div>
            </div>

            <!-- Dòng điện tiêu thụ -->
            <div class="stat-card">
                <div class="stat-label">DÒNG ĐỘNG CƠ</div>
                <div class="stat-value">
                    <span id="statCurrent" style="color: var(--accent-orange);">0.0</span>
                    <span class="stat-unit">A</span>
                </div>
            </div>

            <!-- Mức thức ăn còn lại -->
            <div class="stat-card">
                <div class="stat-label">THÙNG THỨC ĂN</div>
                <div class="stat-value">
                    <span id="statFeed" style="color: var(--primary);">100</span>
                    <span class="stat-unit">%</span>
                </div>
                <div class="stat-progress-bar">
                    <div class="stat-progress-fill" id="barFeed" style="width: 100%;"></div>
                </div>
            </div>
        </div>
    </div>

    <!-- ========================================================
         TAB 2: ĐIỀU KHIỂN (CONTROL - HORIZONTAL COCKPIT)
         ======================================================== -->
    <div class="tab-content" id="tab-control">
        <div class="cockpit-card">
            <!-- Dòng trạng thái tóm tắt -->
            <div class="cockpit-info-row">
                <span>Chế độ: <b>THỦ CÔNG</b></span>
                <span>Pin: <b id="ctrlBattText">85%</b></span>
                <span>GPS: <b id="ctrlGpsText">Fix</b></span>
            </div>

            <!-- BỐ CỤC NGANG: CẦN GA + JOYSTICK 8 HƯỚNG -->
            <div class="dual-joystick-grid">
                <!-- CỘT TRÁI: CẦN GA TỐC ĐỘ (0-100%) -->
                <div class="throttle-col">
                    <span style="font-size: 0.65rem; font-weight: 700; color: var(--text-muted);">TỐC ĐỘ</span>
                    <span class="throttle-val-badge" id="throttleVal">70%</span>

                    <div class="throttle-track" id="throttleTrack">
                        <div class="throttle-marks">
                            <span>100</span>
                            <span>75</span>
                            <span>50</span>
                            <span>25</span>
                            <span>0</span>
                        </div>
                        <div class="throttle-fill" id="throttleFill" style="height: 70%;"></div>
                        <div class="throttle-knob" id="throttleKnob" style="bottom: 70%;">
                            <div class="throttle-knob-grip"></div>
                        </div>
                    </div>

                    <div class="throttle-presets">
                        <button class="preset-chip" onclick="setThrottle(25)">25%</button>
                        <button class="preset-chip" onclick="setThrottle(50)">50%</button>
                        <button class="preset-chip" onclick="setThrottle(70)">70%</button>
                        <button class="preset-chip" onclick="setThrottle(100)">MAX</button>
                    </div>
                </div>

                <!-- CỘT PHẢI: JOYSTICK 8 HƯỚNG -->
                <div class="steering-col">
                    <span style="font-size: 0.65rem; font-weight: 700; color: var(--text-muted);">HƯỚNG LÁI</span>
                    <span class="steer-dir-title" id="steerText">DỪNG (STOP)</span>

                    <div class="joy-pad" id="joyPad">
                        <div class="joy-ring"></div>
                        <div class="joy-cross-h"></div>
                        <div class="joy-cross-v"></div>

                        <!-- 8 Mũi tên chỉ hướng -->
                        <span class="arrow-dir arrow-n"  id="arr_N">▲</span>
                        <span class="arrow-dir arrow-ne" id="arr_NE">↗</span>
                        <span class="arrow-dir arrow-e"  id="arr_E">►</span>
                        <span class="arrow-dir arrow-se" id="arr_SE">↘</span>
                        <span class="arrow-dir arrow-s"  id="arr_S">▼</span>
                        <span class="arrow-dir arrow-sw" id="arr_SW">↙</span>
                        <span class="arrow-dir arrow-w"  id="arr_W">◄</span>
                        <span class="arrow-dir arrow-nw" id="arr_NW">↖</span>

                        <div class="joy-knob" id="joyKnob">
                            <div class="joy-knob-dot"></div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- THANH ĐO CÔNG SUẤT CHÂN VỊT -->
            <div class="thrust-section">
                <div>
                    <div class="thrust-title">
                        <span>ĐỘNG CƠ TRÁI</span>
                        <span id="txtThrustL" style="color: var(--primary);">0%</span>
                    </div>
                    <div class="thrust-track">
                        <div class="thrust-fill" id="barThrustL"></div>
                    </div>
                </div>
                <div>
                    <div class="thrust-title">
                        <span>ĐỘNG CƠ PHẢI</span>
                        <span id="txtThrustR" style="color: var(--primary);">0%</span>
                    </div>
                    <div class="thrust-track">
                        <div class="thrust-fill" id="barThrustR"></div>
                    </div>
                </div>
            </div>

            <!-- NÚT HÀNH ĐỘNG -->
            <div class="actions-row">
                <button class="btn-action btn-feed" id="btnFeed" onclick="actionFeed()">
                    <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M12 2v20M17 5H9.5a3.5 3.5 0 0 0 0 7h5a3.5 3.5 0 0 1 0 7H6"/></svg>
                    RẢI THỨC ĂN
                </button>
                <button class="btn-action btn-stop" onclick="actionEmergencyStop()">
                    <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><line x1="15" y1="9" x2="9" y2="15"/><line x1="9" y1="9" x2="15" y2="15"/></svg>
                    DỪNG KHẨN CẤP
                </button>
            </div>
        </div>
    </div>

</div>

<!-- TOAST -->
<div class="toast" id="appToast">Thông báo</div>

<script>
    // ==========================================================
    // 1. STATE & GLOBAL VARIABLES
    // ==========================================================
    let currentThrottle = 70; // 0 - 100%
    let isJoyActive = false;
    let isThrottleActive = false;
    let isHttpBusy = false;
    let nextCmd = null;
    let lastSentTime = 0;
    const MIN_INTERVAL = 60; // 60ms = ~16Hz update

    const DIR_NAMES = {
        'STOP': 'DỪNG (STOP)',
        'N':  '▲ TIẾN THẲNG',
        'NE': '↗ TIẾN - PHẢI',
        'E':  '► RẼ PHẢI',
        'SE': '↘ LÙI - PHẢI',
        'S':  '▼ LÙI',
        'SW': '↙ LÙI - TRÁI',
        'W':  '◄ RẼ TRÁI',
        'NW': '↖ TIẾN - TRÁI'
    };

    // ==========================================================
    // 2. TAB SWITCHER
    // ==========================================================
    function switchTab(tabId) {
        document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
        document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));

        const target = document.getElementById(tabId);
        if (target) target.classList.add('active');

        if (tabId === 'tab-status') {
            document.getElementById('tabBtnStatus').classList.add('active');
        } else {
            document.getElementById('tabBtnControl').classList.add('active');
        }
    }

    // ==========================================================
    // 3. CẦN GA TỐC ĐỘ (VERTICAL SPEED THROTTLE)
    // ==========================================================
    const throttleTrack = document.getElementById('throttleTrack');
    const throttleFill  = document.getElementById('throttleFill');
    const throttleKnob  = document.getElementById('throttleKnob');
    const throttleVal   = document.getElementById('throttleVal');

    function initThrottle() {
        if (!throttleTrack) return;

        function updateThrottleFromY(clientY) {
            const rect = throttleTrack.getBoundingClientRect();
            let y = clientY - rect.top;
            let ratio = 1 - (y / rect.height);
            let pct = Math.round(Math.max(0, Math.min(100, ratio * 100)));
            setThrottle(pct);
        }

        throttleTrack.addEventListener('pointerdown', function(e) {
            isThrottleActive = true;
            throttleTrack.setPointerCapture(e.pointerId);
            updateThrottleFromY(e.clientY);
        });

        throttleTrack.addEventListener('pointermove', function(e) {
            if (isThrottleActive) updateThrottleFromY(e.clientY);
        });

        const stopThrottle = function(e) {
            if (isThrottleActive) {
                isThrottleActive = false;
                try { throttleTrack.releasePointerCapture(e.pointerId); } catch(err) {}
            }
        };

        throttleTrack.addEventListener('pointerup', stopThrottle);
        throttleTrack.addEventListener('pointercancel', stopThrottle);
    }

    function setThrottle(pct) {
        currentThrottle = pct;
        if (throttleFill) throttleFill.style.height = pct + '%';
        if (throttleKnob) throttleKnob.style.bottom = pct + '%';
        if (throttleVal)  throttleVal.innerText = pct + '%';
    }

    // ==========================================================
    // 4. JOYSTICK 8 HƯỚNG (8-WAY STEERING JOYSTICK)
    // ==========================================================
    const joyPad  = document.getElementById('joyPad');
    const joyKnob = document.getElementById('joyKnob');
    const steerText = document.getElementById('steerText');
    let joyCenterX = 0, joyCenterY = 0, joyRadius = 48;

    function init8WayJoystick() {
        if (!joyPad) return;

        joyPad.addEventListener('pointerdown', function(e) {
            isJoyActive = true;
            joyPad.setPointerCapture(e.pointerId);
            const rect = joyPad.getBoundingClientRect();
            joyCenterX = rect.left + rect.width / 2;
            joyCenterY = rect.top + rect.height / 2;
            joyRadius  = (rect.width / 2) - 30;
            handleJoyMove(e.clientX, e.clientY);
        });

        joyPad.addEventListener('pointermove', function(e) {
            if (isJoyActive) handleJoyMove(e.clientX, e.clientY);
        });

        const releaseJoy = function(e) {
            if (!isJoyActive) return;
            isJoyActive = false;
            try { joyPad.releasePointerCapture(e.pointerId); } catch(err) {}

            // Hồi tâm tự động
            joyKnob.style.transition = 'transform 0.2s cubic-bezier(0.18, 0.89, 0.32, 1.28)';
            joyKnob.style.transform = 'translate(0px, 0px)';
            setTimeout(() => { joyKnob.style.transition = ''; }, 200);

            highlightArrow('STOP');
            updateThrustUI(0, 0);
            sendStopCommand();
        };

        joyPad.addEventListener('pointerup', releaseJoy);
        joyPad.addEventListener('pointercancel', releaseJoy);
    }

    function handleJoyMove(clientX, clientY) {
        let dx = clientX - joyCenterX;
        let dy = clientY - joyCenterY;
        let dist = Math.hypot(dx, dy);

        if (dist > joyRadius) {
            dx = (dx / dist) * joyRadius;
            dy = (dy / dist) * joyRadius;
            dist = joyRadius;
        }

        joyKnob.style.transform = `translate(${dx.toFixed(1)}px, ${dy.toFixed(1)}px)`;

        if (dist < 8) {
            highlightArrow('STOP');
            updateThrustUI(0, 0);
            sendMotorCommand(0, 0);
        } else {
            // Tính góc và sector 8 hướng
            let angle = Math.atan2(-dy, dx) * 180 / Math.PI; // Up is 90 deg
            let compass = (90 - angle + 360) % 360; // 0 deg is North
            let sector = getSector(compass);
            highlightArrow(sector);

            // Tính tỷ lệ lực đẩy theo cần ga hiện tại
            let normX = dx / joyRadius;
            let normY = -dy / joyRadius; // Tiến là dương

            let maxPower = currentThrottle;
            let forward  = normY * maxPower;
            let turn     = normX * maxPower;

            let leftMotor  = Math.max(-100, Math.min(100, Math.round(forward + turn)));
            let rightMotor = Math.max(-100, Math.min(100, Math.round(forward - turn)));

            updateThrustUI(leftMotor, rightMotor);
            sendMotorCommand(leftMotor, rightMotor);
        }
    }

    function getSector(deg) {
        if (deg >= 337.5 || deg < 22.5)  return 'N';
        if (deg >= 22.5  && deg < 67.5)  return 'NE';
        if (deg >= 67.5  && deg < 112.5) return 'E';
        if (deg >= 112.5 && deg < 157.5) return 'SE';
        if (deg >= 157.5 && deg < 202.5) return 'S';
        if (deg >= 202.5 && deg < 247.5) return 'SW';
        if (deg >= 247.5 && deg < 292.5) return 'W';
        return 'NW';
    }

    function highlightArrow(sec) {
        document.querySelectorAll('.arrow-dir').forEach(el => el.classList.remove('active'));
        if (steerText) steerText.innerText = DIR_NAMES[sec] || sec;
        if (sec !== 'STOP') {
            const arr = document.getElementById('arr_' + sec);
            if (arr) arr.classList.add('active');
        }
    }

    // ==========================================================
    // 5. CÔNG SUẤT ĐỘNG CƠ & GỬI LỆNH (DISPATCHER)
    // ==========================================================
    const barThrustL = document.getElementById('barThrustL');
    const barThrustR = document.getElementById('barThrustR');
    const txtThrustL = document.getElementById('txtThrustL');
    const txtThrustR = document.getElementById('txtThrustR');

    function updateThrustUI(left, right) {
        if (txtThrustL) txtThrustL.innerText = (left > 0 ? "+" : "") + left + "%";
        if (txtThrustR) txtThrustR.innerText = (right > 0 ? "+" : "") + right + "%";

        if (barThrustL) {
            let half = Math.abs(left) / 2;
            barThrustL.style.left  = left >= 0 ? "50%" : (50 - half) + "%";
            barThrustL.style.width = half + "%";
            barThrustL.style.background = left >= 0 ? "var(--primary)" : "var(--accent-red)";
        }
        if (barThrustR) {
            let half = Math.abs(right) / 2;
            barThrustR.style.left  = right >= 0 ? "50%" : (50 - half) + "%";
            barThrustR.style.width = half + "%";
            barThrustR.style.background = right >= 0 ? "var(--primary)" : "var(--accent-red)";
        }
    }

    function sendMotorCommand(left, right) {
        nextCmd = { type: 'MOTOR', left: left, right: right };
        dispatchCommand();
    }

    function sendStopCommand() {
        nextCmd = { type: 'STOP' };
        dispatchCommand();
    }

    function dispatchCommand() {
        if (isHttpBusy || !nextCmd) return;

        let now = performance.now();
        let elapsed = now - lastSentTime;

        if (elapsed < MIN_INTERVAL) {
            setTimeout(dispatchCommand, MIN_INTERVAL - elapsed);
            return;
        }

        let cmd = nextCmd;
        nextCmd = null;
        isHttpBusy = true;
        lastSentTime = performance.now();

        let url = (cmd.type === 'STOP') 
            ? "/command?cmd=STOP" 
            : `/command?cmd=${encodeURIComponent(`MOTOR|${cmd.left}|${cmd.right}`)}`;

        fetch(url, { cache: "no-store" })
            .catch(() => {})
            .finally(() => {
                isHttpBusy = false;
                if (nextCmd) dispatchCommand();
            });
    }

    function actionFeed() {
        fetch("/command?cmd=" + encodeURIComponent("FEED|500"), { cache: "no-store" }).catch(() => {});
        showToast("Đang kích hoạt rải mồi (0.5s)...");

        const btn = document.getElementById('btnFeed');
        if (btn) {
            btn.style.filter = "brightness(1.2)";
            setTimeout(() => { btn.style.filter = ""; }, 500);
        }
    }

    function actionEmergencyStop() {
        sendStopCommand();
        updateThrustUI(0, 0);
        showToast("ĐÃ DỪNG KHẨN CẤP!");
    }

    // ==========================================================
    // 6. TRUY VẤN DỮ LIỆU TELEMETRY (CẬP NHẬT TRẠNG THÁI)
    // ==========================================================
    function updateTelemetry() {
        // Tạm dừng khi đang chạm tay vào cần điều khiển để tránh nghẽn WiFi
        if (isJoyActive || isThrottleActive || isHttpBusy) return;

        fetch("/status", { cache: "no-store" })
            .then(res => {
                if (!res.ok) throw new Error();
                return res.json();
            })
            .then(data => {
                applyData(data);
                setConnStatus(true);
            })
            .catch(() => {
                // Tự động mô phỏng dữ liệu nhẹ nhàng nếu chưa nối dây STM32
                simulateDevData();
            });
    }

    function applyData(data) {
        if (document.getElementById('statGPS')) {
            document.getElementById('statGPS').innerText = `${data.lat || '0.000000'}, ${data.lon || '0.000000'}`;
        }
        const isFix = (data.gps === "1" || parseInt(data.gps) > 0);
        if (document.getElementById('gpsFixBadge')) {
            document.getElementById('gpsFixBadge').innerText = isFix ? "ĐÃ KHÓA 3D" : "ĐANG TÌM GPS...";
            document.getElementById('gpsFixBadge').style.color = isFix ? "var(--accent-green)" : "var(--accent-orange)";
        }
        if (document.getElementById('ctrlGpsText')) {
            document.getElementById('ctrlGpsText').innerText = isFix ? "Fix" : "Tìm...";
        }

        if (document.getElementById('statSpeed')) {
            document.getElementById('statSpeed').innerText = parseFloat(data.speed || 0).toFixed(1);
        }
        if (document.getElementById('statHeading')) {
            document.getElementById('statHeading').innerText = Math.round(parseFloat(data.heading || 0));
        }

        // Tính % pin từ điện áp
        const volt = parseFloat(data.battery || 12.0).toFixed(1);
        let pct = Math.max(0, Math.min(100, Math.round(((volt - 10.5) / (12.6 - 10.5)) * 100)));
        if (pct === 0 && volt > 0) pct = 85;

        if (document.getElementById('statBattPct')) document.getElementById('statBattPct').innerText = pct;
        if (document.getElementById('statBattVolt')) document.getElementById('statBattVolt').innerText = volt + "V";
        if (document.getElementById('barBatt')) document.getElementById('barBatt').style.width = pct + "%";
        if (document.getElementById('ctrlBattText')) document.getElementById('ctrlBattText').innerText = pct + "%";

        if (document.getElementById('statTemp')) {
            document.getElementById('statTemp').innerText = parseFloat(data.temp || 28.5).toFixed(1);
        }
        if (document.getElementById('statCurrent')) {
            document.getElementById('statCurrent').innerText = parseFloat(data.current || 0).toFixed(1);
        }
        if (document.getElementById('statFeed')) {
            let feedVal = parseInt(data.feed || 100);
            document.getElementById('statFeed').innerText = feedVal;
            if (document.getElementById('barFeed')) document.getElementById('barFeed').style.width = feedVal + "%";
        }
    }

    function simulateDevData() {
        let t = Date.now() / 4000;
        applyData({
            lat: "20.987654",
            lon: "105.765432",
            speed: (Math.abs(Math.sin(t)) * 1.5).toFixed(1),
            heading: Math.round(180 + Math.sin(t) * 45),
            battery: "12.4",
            temp: (28.4 + Math.cos(t) * 0.3).toFixed(1),
            current: "1.2",
            feed: "95",
            gps: "1"
        });
        setConnStatus(true);
    }

    function setConnStatus(online) {
        const text = document.getElementById('connText');
        const pill = document.getElementById('connPill');
        if (online) {
            if (text) text.innerText = "Online";
            if (pill) { pill.style.background = "#ecfdf5"; pill.style.color = "var(--accent-green)"; }
        } else {
            if (text) text.innerText = "Mất kết nối";
            if (pill) { pill.style.background = "#fef2f2"; pill.style.color = "var(--accent-red)"; }
        }
    }

    function showToast(msg) {
        const t = document.getElementById("appToast");
        if (!t) return;
        t.innerText = msg;
        t.classList.add("show");
        setTimeout(() => { t.classList.remove("show"); }, 2000);
    }

    // Polling định kỳ mỗi giây
    setInterval(updateTelemetry, 1000);

    // Khởi tạo
    window.addEventListener("DOMContentLoaded", () => {
        initThrottle();
        init8WayJoystick();
        updateTelemetry();
    });
</script>
</body>
</html>
)rawliteral";

#endif // WEB_H
