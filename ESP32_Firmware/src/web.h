#ifndef WEB_H
#define WEB_H

#include <Arduino.h>

// ============================================================
// USV MINI - HỆ THỐNG GIÁM SÁT & ĐIỀU KHIỂN RẢI THỨC ĂN THỦY SẢN
// Giao diện người dùng Mobile-First chuẩn Light Mode cao cấp (Marine Cockpit)
// Thứ tự phân hệ:
// 1. Theo dõi trạng thái (8 thông số cảm biến thời gian thực & Biểu đồ biến thiên)
// 2. Điều khiển thủ công (Dạng ngang: Cần ga tốc độ + Joystick 8 hướng, Rải mồi, Dừng khẩn cấp)
// 3. Lịch sử & Báo cáo (Hải trình ao nuôi, Nhật ký rải thức ăn, Xuất file CSV)
// ============================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
    <title>USV MINI — Trạm Giám Sát & Điều Khiển</title>

    <style>
        /* ============================================================
           CSS RESET & DESIGN TOKENS (PREMIUM LIGHT MODE THEME)
           ============================================================ */
        :root {
            --bg-page: #f4f6fa;
            --bg-surface: #ffffff;
            --bg-card: #ffffff;
            --bg-card-sub: #f8fafc;
            --bg-card-active: #e0f2fe;

            --border-subtle: #e2e8f0;
            --border-light: #cbd5e1;
            --border-highlight: #38bdf8;
            --border-focus: #0284c7;

            --primary: #0284c7;
            --primary-light: #e0f2fe;
            --accent-cyan: #0284c7;
            --accent-blue: #2563eb;
            --accent-emerald: #10b981;
            --accent-amber: #d97706;
            --accent-rose: #ef4444;
            --accent-purple: #7c3aed;

            --text-title: #0f172a;
            --text-body: #334155;
            --text-muted: #64748b;
            --text-dim: #94a3b8;
            --text-white: #ffffff;

            --shadow-card: 0 4px 18px -2px rgba(15, 23, 42, 0.05), 0 2px 6px -1px rgba(15, 23, 42, 0.03);
            --shadow-elevated: 0 10px 28px -4px rgba(15, 23, 42, 0.08), 0 4px 10px -2px rgba(15, 23, 42, 0.04);
            --shadow-joy-knob: 0 6px 18px rgba(2, 132, 199, 0.45);

            --safe-bottom: env(safe-area-inset-bottom, 12px);
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            user-select: none;
            -webkit-user-select: none;
            -webkit-tap-highlight-color: transparent;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
        }

        body {
            background-color: var(--bg-page);
            background-image: 
                radial-gradient(circle at 10% 8%, rgba(2, 132, 199, 0.04) 0%, transparent 45%),
                radial-gradient(circle at 90% 92%, rgba(16, 185, 129, 0.04) 0%, transparent 45%),
                linear-gradient(to right, rgba(0, 0, 0, 0.02) 1px, transparent 1px),
                linear-gradient(to bottom, rgba(0, 0, 0, 0.02) 1px, transparent 1px);
            background-size: 100% 100%, 100% 100%, 28px 28px, 28px 28px;
            color: var(--text-body);
            min-height: 100vh;
            padding-bottom: calc(75px + var(--safe-bottom));
            display: flex;
            flex-direction: column;
            align-items: center;
            overflow-x: hidden;
        }

        .app-shell {
            width: 100%;
            max-width: 520px;
            display: flex;
            flex-direction: column;
            gap: 14px;
            padding: 12px 14px;
        }

        @media (min-width: 1024px) {
            .app-shell {
                max-width: 980px;
            }
        }

        /* ============================================================
           COMMON HEADER BAR
           ============================================================ */
        .top-navbar {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: rgba(255, 255, 255, 0.94);
            border: 1px solid var(--border-subtle);
            border-radius: 18px;
            padding: 12px 18px;
            box-shadow: var(--shadow-card);
            backdrop-filter: blur(16px);
            -webkit-backdrop-filter: blur(16px);
            position: sticky;
            top: 10px;
            z-index: 40;
        }

        .nav-left {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .nav-title {
            display: flex;
            flex-direction: column;
        }

        .nav-title h1 {
            font-size: 1.05rem;
            font-weight: 800;
            letter-spacing: 0.01em;
            color: var(--text-title);
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .nav-title span.subtitle {
            font-size: 0.72rem;
            color: var(--text-muted);
            margin-top: 1px;
        }

        .nav-badges {
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .online-chip {
            display: flex;
            align-items: center;
            gap: 6px;
            background: #dcfce7;
            border: 1px solid #bbf7d0;
            border-radius: 20px;
            padding: 4px 10px;
            font-size: 0.75rem;
            font-weight: 700;
            color: #15803d;
        }

        .pulse-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background-color: #16a34a;
            box-shadow: 0 0 8px rgba(22, 163, 74, 0.6);
            animation: pulse-ring 1.8s infinite;
        }

        @keyframes pulse-ring {
            0% { transform: scale(0.95); opacity: 0.8; }
            50% { transform: scale(1.15); opacity: 1; filter: drop-shadow(0 0 4px #16a34a); }
            100% { transform: scale(0.95); opacity: 0.8; }
        }

        .btn-icon-nav {
            width: 36px;
            height: 36px;
            border-radius: 10px;
            background: #f1f5f9;
            border: 1px solid var(--border-subtle);
            color: var(--text-body);
            display: flex;
            align-items: center;
            justify-content: center;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .btn-icon-nav:active {
            background: var(--primary-light);
            color: var(--primary);
        }

        /* ============================================================
           TAB VIEWS CONTAINER
           ============================================================ */
        .tab-view {
            display: none;
            flex-direction: column;
            gap: 14px;
            animation: fadeInView 0.25s cubic-bezier(0.16, 1, 0.3, 1) forwards;
        }

        .tab-view.active {
            display: flex;
        }

        @keyframes fadeInView {
            from {
                opacity: 0;
                transform: translateY(8px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }

        .ui-card {
            background: var(--bg-card);
            border: 1px solid var(--border-subtle);
            border-radius: 18px;
            padding: 16px;
            box-shadow: var(--shadow-card);
            display: flex;
            flex-direction: column;
            gap: 12px;
            position: relative;
            overflow: hidden;
        }

        /* ============================================================
           1. THEO DÕI TRẠNG THÁI (TAB 1 - DEFAULT STARTING TAB)
           ============================================================ */
        .status-telemetry-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }

        .telemetry-card {
            background: #ffffff;
            border: 1px solid var(--border-subtle);
            border-radius: 14px;
            padding: 12px 14px;
            display: flex;
            flex-direction: column;
            gap: 6px;
            box-shadow: var(--shadow-card);
        }

        .telemetry-card.span-2 {
            grid-column: span 2;
        }

        .telemetry-card-head {
            display: flex;
            align-items: center;
            justify-content: space-between;
            font-size: 0.72rem;
            color: var(--text-muted);
            font-weight: 600;
        }

        .telemetry-card-head svg {
            color: var(--primary);
        }

        .telemetry-card-val {
            font-size: 1.25rem;
            font-weight: 800;
            color: var(--text-title);
            display: flex;
            align-items: baseline;
            gap: 4px;
        }

        .telemetry-unit {
            font-size: 0.75rem;
            font-weight: 600;
            color: var(--text-muted);
        }

        .progress-bar-wrap {
            width: 100%;
            height: 6px;
            background: #e2e8f0;
            border-radius: 4px;
            overflow: hidden;
            margin-top: 4px;
        }

        .progress-bar-fill {
            height: 100%;
            background: var(--primary);
            border-radius: 4px;
            transition: width 0.3s ease;
        }

        .progress-bar-fill.emerald { background: #16a34a; }
        .progress-bar-fill.amber { background: #d97706; }

        .gps-coords-display {
            font-family: ui-monospace, SFMono-Regular, Menlo, monospace;
            font-size: 0.95rem;
            font-weight: 700;
            color: var(--primary);
            letter-spacing: 0.02em;
        }

        .gps-actions-row {
            display: flex;
            gap: 8px;
            margin-top: 4px;
        }

        .btn-gps-mini {
            background: #f1f5f9;
            border: 1px solid var(--border-subtle);
            border-radius: 8px;
            padding: 5px 10px;
            font-size: 0.7rem;
            font-weight: 600;
            color: var(--text-body);
            cursor: pointer;
            display: inline-flex;
            align-items: center;
            gap: 4px;
        }

        .btn-gps-mini:hover {
            border-color: var(--primary);
            color: var(--primary);
        }

        .compass-dial-wrap {
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .compass-rose-mini {
            position: relative;
            width: 44px;
            height: 44px;
            border-radius: 50%;
            background: #f1f5f9;
            border: 1px solid #cbd5e1;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-shrink: 0;
        }

        .compass-needle-mini {
            width: 24px;
            height: 24px;
            transition: transform 0.3s ease;
        }

        /* Real-Time Sensor Chart */
        .chart-section-card {
            background: #ffffff;
            display: flex;
            flex-direction: column;
            gap: 12px;
            box-shadow: var(--shadow-card);
        }

        .chart-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .chart-title-wrap h3 {
            font-size: 0.9rem;
            font-weight: 700;
            color: var(--text-title);
        }

        .chart-current-stat {
            font-size: 1.2rem;
            font-weight: 800;
            color: var(--primary);
        }

        .chart-tabs-bar {
            display: flex;
            gap: 6px;
            background: #f1f5f9;
            padding: 3px;
            border-radius: 10px;
            border: 1px solid var(--border-subtle);
        }

        .chart-tab-btn {
            flex: 1;
            padding: 5px 0;
            border-radius: 8px;
            border: none;
            background: transparent;
            font-size: 0.72rem;
            font-weight: 700;
            color: var(--text-muted);
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .chart-tab-btn.active {
            background: #ffffff;
            color: var(--primary);
            box-shadow: 0 1px 4px rgba(0, 0, 0, 0.08);
        }

        .canvas-chart-holder {
            width: 100%;
            height: 140px;
            position: relative;
        }

        canvas.telemetry-chart {
            width: 100% !important;
            height: 100% !important;
            display: block;
        }

        /* ============================================================
           2. ĐIỀU KHIỂN THỦ CÔNG (TAB 2 - DẠNG NGANG DUAL JOYSTICK)
           ============================================================ */
        .comms-bar {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 6px;
            background: #ffffff;
            border: 1px solid var(--border-subtle);
            border-radius: 14px;
            padding: 10px 12px;
            font-size: 0.72rem;
            color: var(--text-body);
            box-shadow: var(--shadow-card);
            align-items: center;
            text-align: center;
        }

        .comms-item {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 5px;
            font-weight: 700;
        }


        /* DẠNG NGANG: COCKPIT DUAL JOYSTICK GRID */
        .horizontal-cockpit-card {
            padding: 14px 12px;
            display: flex;
            flex-direction: column;
            gap: 12px;
            background: #ffffff;
        }

        .cockpit-horizontal-grid {
            display: grid;
            grid-template-columns: 110px 1fr;
            gap: 12px;
            align-items: center;
            width: 100%;
        }

        /* CỘT TRÁI: JOYSTICK LÊN - XUỐNG ĐIỀU KHIỂN TỐC ĐỘ */
        .throttle-stick-col {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            background: #f8fafc;
            border: 1px solid var(--border-subtle);
            border-radius: 16px;
            padding: 10px 6px;
        }

        .stick-label-badge {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
            gap: 2px;
        }

        .badge-title {
            font-size: 0.68rem;
            font-weight: 700;
            color: var(--text-muted);
            letter-spacing: 0.02em;
        }

        .badge-val {
            font-size: 0.85rem;
            font-weight: 800;
            color: var(--primary);
        }

        .throttle-slot-track {
            position: relative;
            width: 68px;
            height: 180px;
            background: #e2e8f0;
            border: 2px solid #cbd5e1;
            border-radius: 34px;
            display: flex;
            align-items: flex-end;
            justify-content: center;
            box-shadow: inset 0 2px 10px rgba(0, 0, 0, 0.08);
            touch-action: none;
            cursor: pointer;
            overflow: hidden;
        }

        .throttle-scale-marks {
            position: absolute;
            inset: 8px 6px;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
            align-items: flex-start;
            pointer-events: none;
            z-index: 1;
        }

        .throttle-scale-marks span.mark {
            font-size: 0.6rem;
            font-weight: 800;
            color: #64748b;
            padding-left: 2px;
        }

        .throttle-fill-level {
            position: absolute;
            bottom: 0;
            left: 0;
            right: 0;
            background: linear-gradient(180deg, #38bdf8 0%, #0284c7 100%);
            border-radius: 0 0 32px 32px;
            transition: height 0.06s ease;
            pointer-events: none;
        }

        .throttle-handle-knob {
            position: absolute;
            left: 50%;
            transform: translate(-50%, 50%);
            width: 60px;
            height: 38px;
            border-radius: 18px;
            background: radial-gradient(circle at 35% 35%, #ffffff, #e2e8f0);
            border: 2px solid #0284c7;
            box-shadow: 0 4px 12px rgba(2, 132, 199, 0.4), inset 0 1px 2px rgba(255, 255, 255, 0.8);
            display: flex;
            align-items: center;
            justify-content: center;
            z-index: 3;
            pointer-events: none;
            transition: bottom 0.06s ease;
        }

        .handle-grip-lines {
            width: 24px;
            height: 8px;
            background: repeating-linear-gradient(90deg, #0284c7, #0284c7 2px, transparent 2px, transparent 5px);
            border-radius: 2px;
        }

        /* CỘT PHẢI: JOYSTICK 8 HƯỚNG */
        .steering-stick-col {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            background: #f8fafc;
            border: 1px solid var(--border-subtle);
            border-radius: 16px;
            padding: 10px 6px;
        }

        .joystick-zone-8way {
            position: relative;
            width: 195px;
            height: 195px;
            border-radius: 50%;
            background: radial-gradient(circle at center, #ffffff 0%, #f8fafc 70%, #edf2f7 100%);
            border: 2px solid #cbd5e1;
            box-shadow: inset 0 2px 10px rgba(0, 0, 0, 0.06), 0 6px 18px rgba(0, 0, 0, 0.05);
            display: flex;
            align-items: center;
            justify-content: center;
            touch-action: none;
            cursor: grab;
        }

        .joystick-zone-8way:active {
            cursor: grabbing;
        }

        /* 8 Direction Labels */
        .dir-8-label {
            position: absolute;
            font-size: 0.65rem;
            font-weight: 800;
            color: #64748b;
            pointer-events: none;
            letter-spacing: 0.02em;
            transition: color 0.15s ease, transform 0.15s ease;
        }

        .dir-8-label.active {
            color: var(--primary);
            font-weight: 900;
            transform: scale(1.15);
        }

        .dir-n  { top: 6px; }
        .dir-ne { top: 22px; right: 24px; }
        .dir-e  { right: 6px; }
        .dir-se { bottom: 22px; right: 24px; }
        .dir-s  { bottom: 6px; }
        .dir-sw { bottom: 22px; left: 24px; }
        .dir-w  { left: 6px; }
        .dir-nw { top: 22px; left: 24px; }

        .joy-ring {
            position: absolute;
            border-radius: 50%;
            border: 1px dashed rgba(2, 132, 199, 0.3);
            pointer-events: none;
        }
        .joy-ring.ring-inner { width: 85px; height: 85px; }
        .joy-ring.ring-outer { width: 145px; height: 145px; }

        .joy-cross-h {
            position: absolute;
            width: 100%;
            height: 1px;
            background: linear-gradient(90deg, transparent, rgba(2, 132, 199, 0.25), transparent);
            pointer-events: none;
        }

        .joy-cross-v {
            position: absolute;
            height: 100%;
            width: 1px;
            background: linear-gradient(180deg, transparent, rgba(2, 132, 199, 0.25), transparent);
            pointer-events: none;
        }

        .joy-cross-diag1 {
            position: absolute;
            width: 100%;
            height: 1px;
            background: linear-gradient(90deg, transparent, rgba(2, 132, 199, 0.15), transparent);
            transform: rotate(45deg);
            pointer-events: none;
        }

        .joy-cross-diag2 {
            position: absolute;
            width: 100%;
            height: 1px;
            background: linear-gradient(90deg, transparent, rgba(2, 132, 199, 0.15), transparent);
            transform: rotate(-45deg);
            pointer-events: none;
        }

        .joystick-knob-8way {
            position: absolute;
            width: 66px;
            height: 66px;
            border-radius: 50%;
            background: radial-gradient(circle at 35% 35%, #38bdf8, #0284c7);
            border: 2px solid #ffffff;
            box-shadow: var(--shadow-joy-knob), inset 0 2px 4px rgba(255, 255, 255, 0.6);
            display: flex;
            align-items: center;
            justify-content: center;
            pointer-events: none;
            will-change: transform;
            touch-action: none;
            z-index: 2;
        }

        .joystick-knob-8way::after {
            content: '';
            width: 24px;
            height: 24px;
            border-radius: 50%;
            background: #0369a1;
            border: 1px solid rgba(255, 255, 255, 0.4);
            box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.4);
        }

        /* DUAL ENGINE THRUST BALANCE METERS */
        .thrust-meters-deck {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            background: #f8fafc;
            border: 1px solid var(--border-subtle);
            border-radius: 12px;
            padding: 8px 12px;
            width: 100%;
        }

        .thrust-box {
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .thrust-title {
            display: flex;
            justify-content: space-between;
            font-size: 0.7rem;
            color: var(--text-muted);
            font-weight: 700;
        }

        .thrust-bar-bg {
            position: relative;
            width: 100%;
            height: 9px;
            background: #e2e8f0;
            border-radius: 5px;
            overflow: hidden;
        }

        .thrust-bar-active {
            position: absolute;
            height: 100%;
            top: 0;
            left: 50%;
            width: 0%;
            background: linear-gradient(90deg, #0284c7, #10b981);
            transition: width 0.08s ease, left 0.08s ease;
        }

        /* Action Buttons */
        .controls-action-row {
            display: grid;
            grid-template-columns: 1.2fr 1fr;
            gap: 10px;
        }

        .btn-feed-action {
            background: linear-gradient(145deg, #10b981, #059669);
            border: none;
            color: #ffffff;
            padding: 13px 14px;
            border-radius: 14px;
            font-size: 0.88rem;
            font-weight: 800;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            cursor: pointer;
            box-shadow: 0 4px 14px rgba(16, 185, 129, 0.3);
            transition: all 0.15s ease;
        }

        .btn-feed-action:active {
            transform: scale(0.96);
            filter: brightness(1.1);
        }

        .btn-stop-action {
            background: linear-gradient(145deg, #ef4444, #dc2626);
            border: none;
            color: #ffffff;
            padding: 13px 14px;
            border-radius: 14px;
            font-size: 0.88rem;
            font-weight: 800;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            cursor: pointer;
            box-shadow: 0 4px 14px rgba(239, 68, 68, 0.3);
            transition: all 0.15s ease;
        }

        .btn-stop-action:active {
            transform: scale(0.96);
            filter: brightness(1.15);
        }

        /* Speed Presets Row */
        .preset-row {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 8px;
        }

        .btn-preset {
            background: #ffffff;
            border: 1px solid var(--border-subtle);
            border-radius: 12px;
            padding: 8px 4px;
            font-size: 0.75rem;
            font-weight: 700;
            color: var(--text-body);
            text-align: center;
            cursor: pointer;
            box-shadow: var(--shadow-card);
            transition: all 0.2s ease;
        }

        .btn-preset span.pct {
            display: block;
            font-size: 0.65rem;
            color: var(--text-muted);
            margin-top: 1px;
        }

        .btn-preset.active {
            background: #e0f2fe;
            border-color: var(--primary);
            color: var(--primary);
            box-shadow: 0 2px 8px rgba(2, 132, 199, 0.2);
        }

        .btn-preset.active span.pct {
            color: var(--primary);
        }

        /* ============================================================
           3. LỊCH SỬ & BÁO CÁO (TAB 3 - HISTORY & REPORT VIEW)
           ============================================================ */
        .history-date-bar {
            display: flex;
            justify-content: space-between;
            align-items: center;
            background: #ffffff;
            border: 1px solid var(--border-subtle);
            border-radius: 14px;
            padding: 10px 14px;
            box-shadow: var(--shadow-card);
        }

        .history-date-select {
            background: #f8fafc;
            border: 1px solid var(--border-light);
            border-radius: 8px;
            color: var(--text-title);
            padding: 5px 10px;
            font-size: 0.8rem;
            font-weight: 600;
            outline: none;
        }

        .history-metrics-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
        }

        .history-metric-box {
            background: #ffffff;
            border: 1px solid var(--border-subtle);
            border-radius: 14px;
            padding: 12px 8px;
            text-align: center;
            display: flex;
            flex-direction: column;
            gap: 4px;
            box-shadow: var(--shadow-card);
        }

        .history-metric-label {
            font-size: 0.68rem;
            color: var(--text-muted);
            font-weight: 600;
        }

        .history-metric-val {
            font-size: 1.1rem;
            font-weight: 800;
            color: var(--text-title);
        }

        .route-map-holder {
            position: relative;
            width: 100%;
            height: 200px;
            background: #f0fdf4;
            border: 1px solid #bbf7d0;
            border-radius: 16px;
            overflow: hidden;
            display: flex;
            align-items: center;
            justify-content: center;
        }

        .lake-svg-map {
            width: 100%;
            height: 100%;
        }

        .feed-log-table {
            width: 100%;
            border-collapse: collapse;
            font-size: 0.75rem;
            text-align: left;
        }

        .feed-log-table th {
            padding: 8px 10px;
            color: var(--text-muted);
            font-weight: 700;
            border-bottom: 1px solid var(--border-subtle);
            background: #f8fafc;
        }

        .feed-log-table td {
            padding: 10px 10px;
            border-bottom: 1px solid #f1f5f9;
            color: var(--text-body);
        }

        .feed-log-table tr:last-child td {
            border-bottom: none;
        }

        .badge-status-done {
            background: #dcfce7;
            color: #15803d;
            padding: 2px 7px;
            border-radius: 6px;
            font-weight: 700;
            font-size: 0.68rem;
            display: inline-block;
        }

        .history-actions-row {
            display: flex;
            justify-content: flex-end;
            gap: 8px;
            margin-top: 6px;
        }

        .btn-history-export {
            background: #e0f2fe;
            border: 1px solid #bae6fd;
            color: var(--primary);
            border-radius: 10px;
            padding: 8px 14px;
            font-size: 0.78rem;
            font-weight: 700;
            display: flex;
            align-items: center;
            gap: 6px;
            cursor: pointer;
            box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
        }

        .btn-history-export:active {
            background: var(--primary);
            color: #ffffff;
        }

        /* ============================================================
           BOTTOM NAVIGATION BAR (TRẠNG THÁI -> ĐIỀU KHIỂN -> LỊCH SỬ)
           ============================================================ */
        .bottom-nav-bar {
            position: fixed;
            bottom: 0;
            left: 0;
            right: 0;
            height: calc(62px + var(--safe-bottom));
            background: rgba(255, 255, 255, 0.95);
            border-top: 1px solid var(--border-subtle);
            backdrop-filter: blur(20px);
            -webkit-backdrop-filter: blur(20px);
            display: flex;
            justify-content: space-around;
            align-items: flex-start;
            padding-top: 8px;
            z-index: 50;
            box-shadow: 0 -4px 16px rgba(0, 0, 0, 0.04);
        }

        .nav-item {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 3px;
            background: none;
            border: none;
            color: var(--text-muted);
            cursor: pointer;
            padding: 4px 16px;
            border-radius: 12px;
            transition: all 0.2s cubic-bezier(0.4, 0, 0.2, 1);
            position: relative;
        }

        .nav-item svg {
            width: 22px;
            height: 22px;
            stroke-width: 2;
            transition: transform 0.2s ease, stroke 0.2s ease;
        }

        .nav-item span {
            font-size: 0.7rem;
            font-weight: 700;
            letter-spacing: 0.01em;
        }

        .nav-item.active {
            color: var(--primary);
        }

        .nav-item.active svg {
            transform: translateY(-2px);
            filter: drop-shadow(0 2px 4px rgba(2, 132, 199, 0.3));
        }

        /* ============================================================
           SETTINGS MODAL
           ============================================================ */
        .modal-backdrop {
            position: fixed;
            inset: 0;
            background: rgba(15, 23, 42, 0.45);
            backdrop-filter: blur(6px);
            display: none;
            align-items: center;
            justify-content: center;
            padding: 16px;
            z-index: 100;
        }

        .modal-backdrop.open {
            display: flex;
        }

        .modal-box {
            background: #ffffff;
            border: 1px solid var(--border-subtle);
            border-radius: 20px;
            width: 100%;
            max-width: 440px;
            padding: 20px;
            box-shadow: var(--shadow-elevated);
            display: flex;
            flex-direction: column;
            gap: 14px;
            animation: zoomModal 0.2s ease-out;
        }

        @keyframes zoomModal {
            from { transform: scale(0.92); opacity: 0; }
            to { transform: scale(1); opacity: 1; }
        }

        .modal-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid var(--border-subtle);
            padding-bottom: 10px;
        }

        .modal-header h3 {
            font-size: 1.05rem;
            color: var(--text-title);
            font-weight: 800;
        }

        .btn-modal-close {
            background: none;
            border: none;
            color: var(--text-muted);
            cursor: pointer;
        }

        .setting-group {
            display: flex;
            flex-direction: column;
            gap: 6px;
        }

        .setting-group label {
            font-size: 0.75rem;
            color: var(--text-muted);
            font-weight: 600;
        }

        .setting-input {
            background: #f8fafc;
            border: 1px solid var(--border-subtle);
            border-radius: 10px;
            padding: 10px 12px;
            color: var(--text-title);
            font-size: 0.85rem;
            outline: none;
        }

        .setting-input:focus {
            border-color: var(--primary);
            background: #ffffff;
        }

        .toast {
            position: fixed;
            bottom: calc(75px + var(--safe-bottom));
            left: 50%;
            transform: translateX(-50%) translateY(100px);
            background: #0f172a;
            border: 1px solid #334155;
            color: #ffffff;
            padding: 9px 18px;
            border-radius: 30px;
            font-size: 0.82rem;
            font-weight: 600;
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.25);
            opacity: 0;
            pointer-events: none;
            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
            z-index: 90;
        }

        .toast.show {
            transform: translateX(-50%) translateY(0);
            opacity: 1;
        }
    </style>
</head>

<body>
<div class="app-shell">

    <!-- COMMON TOP NAVBAR -->
    <header class="top-navbar">
        <div class="nav-left">
            <div class="nav-title">
                <h1 id="headerMainTitle">USV — Trạng Thái Tàu</h1>
                <span class="subtitle" id="headerSubtitle">Dữ liệu cảm biến thời gian thực</span>
            </div>
        </div>

        <div class="nav-badges">
            <div class="online-chip" id="connChip">
                <div class="pulse-dot"></div>
                <span id="connText">Online</span>
            </div>
            <button class="btn-icon-nav" onclick="openSettingsModal()" title="Cài đặt">
                <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <circle cx="12" cy="12" r="3"></circle>
                    <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path>
                </svg>
            </button>
        </div>
    </header>

    <!-- ============================================================
         PHÂN HỆ 1: THEO DÕI TRẠNG THÁI (DEFAULT FIRST TAB)
         ============================================================ -->
    <section class="tab-view active" id="tab-status">
        <!-- Telemetry Cards Grid -->
        <div class="status-telemetry-grid">
            <!-- GPS Location Card -->
            <div class="telemetry-card span-2">
                <div class="telemetry-card-head">
                    <span>VỊ TRÍ GPS</span>
                    <span id="statGPSFix" class="badge-status-done">ĐÃ KHÓA 3D</span>
                </div>
                <div class="gps-coords-display" id="statGPSCoords">20.987654, 105.765432</div>
                <div class="gps-actions-row">
                    <button class="btn-gps-mini" onclick="openGoogleMaps()">
                        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6"/><polyline points="15 3 21 3 21 9"/><line x1="10" y1="14" x2="21" y2="3"/></svg>
                        Mở bản đồ
                    </button>
                    <button class="btn-gps-mini" onclick="copyGPS()">
                        <svg width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="9" y="9" width="13" height="13" rx="2"/><path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"/></svg>
                        Sao chép
                    </button>
                </div>
            </div>

            <!-- Satellites -->
            <div class="telemetry-card">
                <div class="telemetry-card-head">
                    <span>SỐ VỆ TINH</span>
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 2a14.5 14.5 0 0 0 0 20 14.5 14.5 0 0 0 0-20"/><path d="M2 12h20"/></svg>
                </div>
                <div class="telemetry-card-val">
                    <span id="statSats">12</span>
                    <span class="telemetry-unit">vệ tinh</span>
                </div>
            </div>

            <!-- Speed -->
            <div class="telemetry-card">
                <div class="telemetry-card-head">
                    <span>TỐC ĐỘ</span>
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/></svg>
                </div>
                <div class="telemetry-card-val" style="color: var(--primary);">
                    <span id="statSpeed">1.2</span>
                    <span class="telemetry-unit">m/s</span>
                </div>
            </div>

            <!-- Heading / Yaw with Compass Dial -->
            <div class="telemetry-card">
                <div class="telemetry-card-head">
                    <span>HƯỚNG (YAW)</span>
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><polygon points="16.24 7.76 14.12 14.12 7.76 16.24 9.88 9.88 16.24 7.76"/></svg>
                </div>
                <div class="compass-dial-wrap">
                    <div class="compass-rose-mini">
                        <svg class="compass-needle-mini" id="compassNeedle" viewBox="0 0 24 24" fill="currentColor">
                            <polygon points="12,2 8,22 12,18 16,22" fill="#ef4444"/>
                            <polygon points="12,18 8,22 12,2 12,18" fill="#0284c7"/>
                        </svg>
                    </div>
                    <div class="telemetry-card-val">
                        <span id="statHeading">245</span>
                        <span class="telemetry-unit">°</span>
                    </div>
                </div>
            </div>

            <!-- Water Temperature -->
            <div class="telemetry-card">
                <div class="telemetry-card-head">
                    <span>NHIỆT ĐỘ NƯỚC</span>
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#ef4444" stroke-width="2"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"/></svg>
                </div>
                <div class="telemetry-card-val" style="color: #ef4444;">
                    <span id="statTemp">28.4</span>
                    <span class="telemetry-unit">°C</span>
                </div>
            </div>

            <!-- pH Level -->
            <div class="telemetry-card">
                <div class="telemetry-card-head">
                    <span>ĐỘ PH NƯỚC</span>
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#0284c7" stroke-width="2"><path d="M10 2v7.31L4.2 18.1A2 2 0 0 0 5.88 21h12.24a2 2 0 0 0 1.68-2.9L14 9.31V2"/></svg>
                </div>
                <div class="telemetry-card-val" style="color: #0284c7;">
                    <span id="statPH">7.5</span>
                    <span class="telemetry-unit">pH</span>
                </div>
            </div>

            <!-- Feed Capacity Level -->
            <div class="telemetry-card">
                <div class="telemetry-card-head">
                    <span>LƯỢNG THỨC ĂN</span>
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#d97706" stroke-width="2"><rect x="2" y="7" width="20" height="14" rx="2"/><path d="M16 21V5a2 2 0 0 0-2-2h-4a2 2 0 0 0-2 2v16"/></svg>
                </div>
                <div class="telemetry-card-val" style="color: #d97706;">
                    <span id="statFeedLevel">60</span>
                    <span class="telemetry-unit">%</span>
                </div>
                <div class="progress-bar-wrap">
                    <div class="progress-bar-fill amber" id="barFeedProg" style="width: 60%;"></div>
                </div>
            </div>

            <!-- Battery Voltage -->
            <div class="telemetry-card">
                <div class="telemetry-card-head">
                    <span>PIN HỆ THỐNG</span>
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#16a34a" stroke-width="2"><rect x="2" y="7" width="16" height="10" rx="2"/><line x1="22" y1="11" x2="22" y2="13"/></svg>
                </div>
                <div class="telemetry-card-val" style="color: #16a34a;">
                    <span id="statBattPct">85</span>
                    <span class="telemetry-unit">% (<span id="statBattVolt">12.4</span>V)</span>
                </div>
                <div class="progress-bar-wrap">
                    <div class="progress-bar-fill emerald" id="barBattProg" style="width: 85%;"></div>
                </div>
            </div>
        </div>

        <!-- Real-Time Interactive Sensor Chart -->
        <div class="ui-card chart-section-card">
            <div class="chart-header">
                <div class="chart-title-wrap">
                    <h3 id="chartLabelTitle">Biểu đồ nhiệt độ nước</h3>
                    <span style="font-size: 0.7rem; color: var(--text-muted);">Cập nhật mỗi giây</span>
                </div>
                <div class="chart-current-stat" id="chartLiveStat">28.4 °C</div>
            </div>

            <!-- Metric Filter Tabs -->
            <div class="chart-tabs-bar">
                <button class="chart-tab-btn active" onclick="setChartMetric('temp')">Nhiệt độ</button>
                <button class="chart-tab-btn" onclick="setChartMetric('ph')">Độ pH</button>
                <button class="chart-tab-btn" onclick="setChartMetric('speed')">Tốc độ</button>
                <button class="chart-tab-btn" onclick="setChartMetric('current')">Dòng điện</button>
            </div>

            <!-- Live Chart Canvas Holder -->
            <div class="canvas-chart-holder">
                <canvas class="telemetry-chart" id="liveChartCanvas"></canvas>
            </div>
        </div>
    </section>

    <!-- ============================================================
         PHÂN HỆ 2: ĐIỀU KHIỂN THỦ CÔNG (DẠNG NGANG DUAL JOYSTICK)
         ============================================================ -->
    <section class="tab-view" id="tab-control">
        <!-- Telemetry & Comms Bar with Live Battery and GPS -->
        <div class="comms-bar">
            <div class="comms-item">
                <div class="pulse-dot"></div>
                <span id="ctrlOnline">Online</span>
            </div>
            <div class="comms-item">
                <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M5 12.55a11 11 0 0 1 14.08 0"/><path d="M1.42 9a16 16 0 0 1 21.16 0"/><path d="M8.53 16.11a6 6 0 0 1 6.95 0"/><line x1="12" y1="20" x2="12.01" y2="20"/></svg>
                <span id="ctrlRSSI">-57 dBm</span>
            </div>
            <div class="comms-item" style="color: #16a34a;">
                <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="7" width="16" height="10" rx="2"/><line x1="22" y1="11" x2="22" y2="13"/></svg>
                <span id="ctrlBattPill">Pin: 85%</span>
            </div>
            <div class="comms-item" style="color: var(--primary);">
                <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="10"/><path d="M12 2a14.5 14.5 0 0 0 0 20 14.5 14.5 0 0 0 0-20"/><path d="M2 12h20"/></svg>
                <span id="ctrlGpsPill">GPS: Fix</span>
            </div>
        </div>


        <!-- CỤM ĐIỀU KHIỂN DẠNG NGANG (SPEED STICK + JOYSTICK 8 HƯỚNG) -->
        <div class="ui-card horizontal-cockpit-card">
            <div class="cockpit-horizontal-grid">
                <!-- CỘT TRÁI: CẦN GA LÊN - XUỐNG ĐIỀU KHIỂN TỐC ĐỘ -->
                <div class="throttle-stick-col">
                    <div class="stick-label-badge">
                        <span class="badge-title">CẦN GA TỐC ĐỘ</span>
                        <span class="badge-val" id="throttleStickVal">70%</span>
                    </div>

                    <div class="throttle-slot-track" id="speedJoyTrack">
                        <div class="throttle-scale-marks">
                            <span class="mark">100</span>
                            <span class="mark">75</span>
                            <span class="mark">50</span>
                            <span class="mark">25</span>
                            <span class="mark">0</span>
                        </div>
                        <div class="throttle-fill-level" id="speedFillLevel" style="height: 70%;"></div>
                        <div class="throttle-handle-knob" id="speedKnob" style="bottom: 70%;">
                            <div class="handle-grip-lines"></div>
                        </div>
                    </div>
                </div>

                <!-- CỘT PHẢI: CẦN LÁI 8 HƯỚNG (8-WAY STEERING JOYSTICK) -->
                <div class="steering-stick-col">
                    <div class="stick-label-badge">
                        <span class="badge-title">HƯỚNG LÁI (8 HƯỚNG)</span>
                        <span class="badge-val" id="steerDirVal">DỪNG (STOP)</span>
                    </div>

                    <div class="joystick-zone-8way" id="joyZone8">
                        <span class="dir-8-label dir-n" id="dir_N">▲ TIẾN</span>
                        <span class="dir-8-label dir-ne" id="dir_NE">↗</span>
                        <span class="dir-8-label dir-e" id="dir_E">PHẢI ►</span>
                        <span class="dir-8-label dir-se" id="dir_SE">↘</span>
                        <span class="dir-8-label dir-s" id="dir_S">▼ LÙI</span>
                        <span class="dir-8-label dir-sw" id="dir_SW">↙</span>
                        <span class="dir-8-label dir-w" id="dir_W">◄ TRÁI</span>
                        <span class="dir-8-label dir-nw" id="dir_NW">↖</span>

                        <div class="joy-ring ring-outer"></div>
                        <div class="joy-ring ring-inner"></div>
                        <div class="joy-cross-h"></div>
                        <div class="joy-cross-v"></div>
                        <div class="joy-cross-diag1"></div>
                        <div class="joy-cross-diag2"></div>

                        <div class="joystick-knob-8way" id="joyKnob8"></div>
                    </div>
                </div>
            </div>

            <!-- Dual Engine Thrust Balance Bars -->
            <div class="thrust-meters-deck">
                <div class="thrust-box">
                    <div class="thrust-title">
                        <span>ĐỘNG CƠ TRÁI (L)</span>
                        <span id="joyLeftStat" style="color: var(--primary); font-weight:700;">0%</span>
                    </div>
                    <div class="thrust-bar-bg">
                        <div class="thrust-bar-active" id="barLeft"></div>
                    </div>
                </div>

                <div class="thrust-box">
                    <div class="thrust-title">
                        <span>ĐỘNG CƠ PHẢI (R)</span>
                        <span id="joyRightStat" style="color: var(--primary); font-weight:700;">0%</span>
                    </div>
                    <div class="thrust-bar-bg">
                        <div class="thrust-bar-active" id="barRight"></div>
                    </div>
                </div>
            </div>
        </div>

        <!-- Big Action Buttons: Rải thức ăn & Dừng khẩn cấp -->
        <div class="controls-action-row">
            <button class="btn-feed-action" id="btnFeedAction" onclick="sendFeedAction()">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"/><polyline points="3.27 6.96 12 12.01 20.73 6.96"/><line x1="12" y1="22.08" x2="12" y2="12"/></svg>
                <span>Rải thức ăn</span>
            </button>

            <button class="btn-stop-action" onclick="emergencyStop()">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><rect x="9" y="9" width="6" height="6" fill="currentColor"/></svg>
                <span>Dừng khẩn cấp</span>
            </button>
        </div>

        <!-- Speed Preset Chips -->
        <div class="preset-row">
            <button class="btn-preset" onclick="setSpeedPreset(25)">
                Chậm
                <span class="pct">25%</span>
            </button>
            <button class="btn-preset" onclick="setSpeedPreset(50)">
                Vừa
                <span class="pct">50%</span>
            </button>
            <button class="btn-preset active" onclick="setSpeedPreset(70)">
                Nhanh
                <span class="pct">70%</span>
            </button>
            <button class="btn-preset" onclick="setSpeedPreset(100)">
                Tối đa
                <span class="pct">100%</span>
            </button>
        </div>
    </section>

    <!-- ============================================================
         PHÂN HỆ 3: LỊCH SỬ & BÁO CÁO (TAB 3 - HISTORY & REPORT VIEW)
         ============================================================ -->
    <section class="tab-view" id="tab-history">
        <!-- Date Selector Header -->
        <div class="history-date-bar">
            <div style="display: flex; align-items: center; gap: 8px;">
                <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="3" y="4" width="18" height="18" rx="2"/><line x1="16" y1="2" x2="16" y2="6"/><line x1="8" y1="2" x2="8" y2="6"/><line x1="3" y1="10" x2="21" y2="10"/></svg>
                <span style="font-size: 0.85rem; font-weight: 700; color: var(--text-title);">Lịch sử hoạt động</span>
            </div>
            <select class="history-date-select" id="historyDateSelect">
                <option value="today">Hôm nay (30/09/2026)</option>
                <option value="yesterday">Hôm qua (29/09/2026)</option>
                <option value="past">15/09/2026</option>
            </select>
        </div>

        <!-- 3 Metrics Summary Cards -->
        <div class="history-metrics-grid">
            <div class="history-metric-box">
                <span class="history-metric-label">Tổng quãng đường</span>
                <span class="history-metric-val" style="color: var(--primary);">2.3 km</span>
            </div>
            <div class="history-metric-box">
                <span class="history-metric-label">Tổng thức ăn</span>
                <span class="history-metric-val" style="color: #d97706;">8.5 kg</span>
            </div>
            <div class="history-metric-box">
                <span class="history-metric-label">Thời gian</span>
                <span class="history-metric-val" style="color: #16a34a;">45 phút</span>
            </div>
        </div>

        <!-- Interactive Route Replay Vector Map -->
        <div class="ui-card">
            <div style="display:flex; justify-content:space-between; align-items:center;">
                <span style="font-size: 0.82rem; font-weight:700; color:var(--text-title);">Bản đồ hành trình & Điểm rải</span>
                <span class="badge-status-done">ĐÃ HOÀN THÀNH</span>
            </div>
            <div class="route-map-holder">
                <svg class="lake-svg-map" viewBox="0 0 340 180" xmlns="http://www.w3.org/2000/svg">
                    <path d="M30 90 C25 40, 80 15, 170 20 C260 25, 315 50, 310 100 C305 150, 240 165, 160 160 C80 155, 35 140, 30 90 Z" fill="#dcfce7" stroke="#16a34a" stroke-width="2"/>
                    <path d="M70 120 C60 70, 100 45, 170 45 C240 45, 275 75, 260 115 C245 145, 190 140, 140 135 C100 130, 80 145, 70 120" 
                          fill="none" stroke="#0284c7" stroke-width="2.5" stroke-dasharray="6,4"/>
                    <circle cx="95" cy="65" r="7" fill="#16a34a" stroke="#fff" stroke-width="1.5"/>
                    <text x="95" y="68" fill="#fff" font-size="8" font-weight="bold" text-anchor="middle">1</text>

                    <circle cx="170" cy="45" r="7" fill="#16a34a" stroke="#fff" stroke-width="1.5"/>
                    <text x="170" y="48" fill="#fff" font-size="8" font-weight="bold" text-anchor="middle">2</text>

                    <circle cx="260" cy="115" r="7" fill="#16a34a" stroke="#fff" stroke-width="1.5"/>
                    <text x="260" y="118" fill="#fff" font-size="8" font-weight="bold" text-anchor="middle">3</text>

                    <circle cx="140" cy="135" r="7" fill="#ef4444" stroke="#fff" stroke-width="1.5"/>
                    <text x="140" y="138" fill="#fff" font-size="8" font-weight="bold" text-anchor="middle">4</text>

                    <g transform="translate(70,120) rotate(-35)">
                        <polygon points="0,-8 6,8 0,5 -6,8" fill="#d97706" stroke="#fff" stroke-width="1.2"/>
                    </g>
                </svg>
            </div>
        </div>

        <!-- Feed Events Detail Table -->
        <div class="ui-card">
            <span style="font-size: 0.82rem; font-weight:700; color:var(--text-title);">Chi tiết điểm rải mồi</span>
            <table class="feed-log-table">
                <thead>
                    <tr>
                        <th>#</th>
                        <th>Thời gian</th>
                        <th>Vị trí</th>
                        <th>Lượng (kg)</th>
                        <th>Trạng thái</th>
                    </tr>
                </thead>
                <tbody id="feedTableBody">
                    <tr>
                        <td>1</td>
                        <td>09:05:12</td>
                        <td>20.9871, 105.7651</td>
                        <td>2.0 kg</td>
                        <td><span class="badge-status-done">Đã rải</span></td>
                    </tr>
                    <tr>
                        <td>2</td>
                        <td>09:12:40</td>
                        <td>20.9875, 105.7658</td>
                        <td>1.5 kg</td>
                        <td><span class="badge-status-done">Đã rải</span></td>
                    </tr>
                    <tr>
                        <td>3</td>
                        <td>09:20:15</td>
                        <td>20.9882, 105.7663</td>
                        <td>2.0 kg</td>
                        <td><span class="badge-status-done">Đã rải</span></td>
                    </tr>
                    <tr>
                        <td>4</td>
                        <td>09:28:50</td>
                        <td>20.9868, 105.7659</td>
                        <td>1.5 kg</td>
                        <td><span class="badge-status-done">Đã rải</span></td>
                    </tr>
                </tbody>
            </table>

            <div class="history-actions-row">
                <button class="btn-history-export" onclick="exportCSVReport()">
                    <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><polyline points="7 10 12 15 17 10"/><line x1="12" y1="15" x2="12" y2="3"/></svg>
                    Xuất báo cáo CSV
                </button>
            </div>
        </div>
    </section>

</div> <!-- End of app-shell -->

<!-- ============================================================
     BOTTOM NAVIGATION BAR (TRẠNG THÁI -> ĐIỀU KHIỂN -> LỊCH SỬ)
     ============================================================ -->
<nav class="bottom-nav-bar">
    <button class="nav-item active" id="navStatus" onclick="switchTab('tab-status')">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor"><line x1="18" y1="20" x2="18" y2="10"/><line x1="12" y1="20" x2="12" y2="4"/><line x1="6" y1="20" x2="6" y2="14"/></svg>
        <span>Trạng thái</span>
    </button>

    <button class="nav-item" id="navControl" onclick="switchTab('tab-control')">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor"><circle cx="12" cy="12" r="10"/><circle cx="12" cy="12" r="4"/><line x1="4.93" y1="4.93" x2="9.17" y2="9.17"/><line x1="14.83" y1="14.83" x2="19.07" y2="19.07"/><line x1="14.83" y1="9.17" x2="19.07" y2="4.93"/><line x1="4.93" y1="19.07" x2="9.17" y2="14.83"/></svg>
        <span>Điều khiển</span>
    </button>

    <button class="nav-item" id="navHistory" onclick="switchTab('tab-history')">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor"><circle cx="12" cy="12" r="10"/><polyline points="12 6 12 12 16 14"/></svg>
        <span>Lịch sử</span>
    </button>

    <button class="nav-item" id="navSettings" onclick="openSettingsModal()">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>
        <span>Cài đặt</span>
    </button>
</nav>

<!-- ============================================================
     SETTINGS MODAL
     ============================================================ -->
<div class="modal-backdrop" id="settingsModal" onclick="if(event.target===this)closeSettingsModal()">
    <div class="modal-box">
        <div class="modal-header">
            <h3>Cài Đặt Hệ Thống USV</h3>
            <button class="btn-modal-close" onclick="closeSettingsModal()">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><line x1="18" y1="6" x2="6" y2="18"/><line x1="6" y1="6" x2="18" y2="18"/></svg>
            </button>
        </div>

        <div class="setting-group">
            <label>Địa chỉ IP & Cổng Web Server</label>
            <input type="text" class="setting-input" value="192.168.4.1:80" readonly>
        </div>


        <div class="setting-group">
            <label>Thời lượng kích hoạt xả mồi (ms)</label>
            <input type="number" class="setting-input" id="feedDurationSetting" value="500" min="200" max="5000" step="100">
        </div>

        <div class="setting-group">
            <label>Thông tin Trạm Phát WiFi (AP Mode)</label>
            <input type="text" class="setting-input" value="SSID: USV_MINIdemo | Pass: 12345678" readonly>
        </div>

        <div style="display: flex; justify-content: flex-end; gap: 8px; margin-top: 6px;">
            <button class="btn-gps-mini" onclick="saveSettings()">Lưu cài đặt</button>
        </div>
    </div>
</div>

<!-- TOAST ALERT -->
<div class="toast" id="appToast">Đã sao chép!</div>

<!-- ============================================================
     JAVASCRIPT: CLIENT ENGINE, DUAL HORIZONTAL JOYSTICKS & CHARTS
     ============================================================ -->
<script>
    // ==========================================================
    // 1. GLOBAL STATE & CONFIG
    // ==========================================================
    let currentTab = 'tab-status'; // Tab Trạng thái mở đầu tiên!
    let currentMode = 'MANUAL';
    let currentThrottle = 70; // 0 - 100%
    let currentLat = "20.987654";
    let currentLon = "105.765432";
    let activeMetric = 'temp';

    // Zero-lag low latency dispatcher
    let isJoyActive = false;
    let isSpeedDragActive = false;
    let isHttpBusy = false;
    let nextCommand = null;
    let dispatchTimer = null;
    let lastSentTime = 0;
    const MIN_SEND_INTERVAL = 55; // ms (~18 commands/sec max)

    // 8-Way Joystick Steering Physics
    let joyCenterX = 0;
    let joyCenterY = 0;
    let joyMaxRadius = 65;
    let targetLeft = 0;
    let targetRight = 0;
    let lastSentLeft = 0;
    let lastSentRight = 0;
    let joyZone8, joyKnob8, joyLeftStat, joyRightStat, barLeft, barRight;
    let rafId = null;

    // Speed Throttle Slot Physics
    let speedJoyTrack, speedKnob, speedFillLevel, throttleStickVal;
    let trackRect = null;

    // Realtime Chart Buffer
    const CHART_POINTS = 20;
    let chartHistory = {
        temp: Array(CHART_POINTS).fill(28.4),
        ph: Array(CHART_POINTS).fill(7.5),
        speed: Array(CHART_POINTS).fill(1.2),
        current: Array(CHART_POINTS).fill(1.8)
    };

    // ==========================================================
    // 2. TAB NAVIGATION SYSTEM (TRẠNG THÁI -> ĐIỀU KHIỂN -> LỊCH SỬ)
    // ==========================================================
    const tabHeaders = {
        'tab-status': { title: 'USV — Trạng Thái Tàu', sub: 'Dữ liệu cảm biến thời gian thực' },
        'tab-control': { title: 'USV — Điều Khiển Tàu', sub: 'Cần ga & Joystick 8 hướng dạng ngang' },
        'tab-history': { title: 'Lịch Sử Hoạt Động', sub: 'Xem lại hành trình và dữ liệu rải' }
    };

    function switchTab(tabId) {
        currentTab = tabId;
        
        document.querySelectorAll('.tab-view').forEach(function(el) {
            el.classList.remove('active');
        });
        
        const targetView = document.getElementById(tabId);
        if (targetView) targetView.classList.add('active');

        document.querySelectorAll('.nav-item').forEach(function(btn) {
            btn.classList.remove('active');
        });
        
        if (tabId === 'tab-status') {
            document.getElementById('navStatus').classList.add('active');
            renderLiveChart();
        } else if (tabId === 'tab-control') {
            document.getElementById('navControl').classList.add('active');
        } else if (tabId === 'tab-history') {
            document.getElementById('navHistory').classList.add('active');
        }

        const headerInfo = tabHeaders[tabId] || tabHeaders['tab-status'];
        document.getElementById('headerMainTitle').innerText = headerInfo.title;
        document.getElementById('headerSubtitle').innerText = headerInfo.sub;

        window.scrollTo({ top: 0, behavior: 'smooth' });
    }

    // ==========================================================
    // 3. JOYSTICK 8 HƯỚNG (8-WAY STEERING ENGINE)
    // ==========================================================
    const DIR_LABELS = {
        'N': 'TIẾN (FORWARD)',
        'NE': 'TIẾN - PHẢI',
        'E': 'RẼ PHẢI (STARBOARD)',
        'SE': 'LÙI - PHẢI',
        'S': 'LÙI (REVERSE)',
        'SW': 'LÙI - TRÁI',
        'W': 'RẼ TRÁI (PORT)',
        'NW': 'TIẾN - TRÁI',
        'STOP': 'DỪNG (STOP)'
    };

    function init8WayJoystick() {
        joyZone8 = document.getElementById("joyZone8");
        joyKnob8 = document.getElementById("joyKnob8");
        joyLeftStat = document.getElementById("joyLeftStat");
        joyRightStat = document.getElementById("joyRightStat");
        barLeft = document.getElementById("barLeft");
        barRight = document.getElementById("barRight");

        if (!joyZone8 || !joyKnob8) return;

        joyZone8.addEventListener("pointerdown", on8WayDown, { passive: false });
        joyZone8.addEventListener("pointermove", on8WayMove, { passive: false });
        joyZone8.addEventListener("pointerup", on8WayUp, { passive: false });
        joyZone8.addEventListener("pointercancel", on8WayUp, { passive: false });
    }

    function on8WayDown(e) {
        e.preventDefault();
        isJoyActive = true;
        joyZone8.setPointerCapture(e.pointerId);
        joyKnob8.style.transition = "none";

        const rect = joyZone8.getBoundingClientRect();
        joyCenterX = rect.left + rect.width / 2;
        joyCenterY = rect.top + rect.height / 2;
        joyMaxRadius = (rect.width / 2) - (joyKnob8.offsetWidth / 2) - 4;

        handle8WayMove(e.clientX, e.clientY);
    }

    function on8WayMove(e) {
        if (!isJoyActive) return;
        e.preventDefault();
        handle8WayMove(e.clientX, e.clientY);
    }

    function on8WayUp(e) {
        if (!isJoyActive) return;
        isJoyActive = false;

        if (rafId) {
            cancelAnimationFrame(rafId);
            rafId = null;
        }

        // Hồi tâm lò xo
        joyKnob8.style.transition = "transform 0.22s cubic-bezier(0.18, 0.89, 0.32, 1.28)";
        joyKnob8.style.transform = "translate(0px, 0px)";

        highlight8WaySector('STOP');
        targetLeft = 0;
        targetRight = 0;
        lastSentLeft = 0;
        lastSentRight = 0;

        updateThrustMeters(0, 0);
        queueStop();

        setTimeout(updateTelemetry, 300);
    }

    function handle8WayMove(clientX, clientY) {
        let dx = clientX - joyCenterX;
        let dy = clientY - joyCenterY;
        let distance = Math.hypot(dx, dy);

        if (distance > joyMaxRadius) {
            dx = (dx / distance) * joyMaxRadius;
            dy = (dy / distance) * joyMaxRadius;
            distance = joyMaxRadius;
        }

        if (rafId) cancelAnimationFrame(rafId);
        rafId = requestAnimationFrame(function() {
            joyKnob8.style.transform = "translate(" + dx.toFixed(1) + "px, " + dy.toFixed(1) + "px)";
        });

        if (distance < 6) {
            highlight8WaySector('STOP');
            targetLeft = 0;
            targetRight = 0;
        } else {
            // Xác định 1 trong 8 hướng chính
            let angle = Math.atan2(-dy, dx) * 180 / Math.PI; // -180 đến 180 (Lên là 90)
            let compass = (90 - angle + 360) % 360; // 0 là Bắc (Tiến), 90 là Đông (Phải)...
            let sector = get8WaySector(compass);
            highlight8WaySector(sector);

            let normX = dx / joyMaxRadius;
            let normY = -dy / joyMaxRadius; // Up is forward (+)

            // Tính toán lực vi sai theo cần ga tốc độ (Speed Throttle)
            let maxP = currentThrottle;
            let forward = normY * maxP;
            let turn = normX * maxP;

            let left = forward + turn;
            let right = forward - turn;

            targetLeft = Math.max(-100, Math.min(100, Math.round(left)));
            targetRight = Math.max(-100, Math.min(100, Math.round(right)));
        }

        updateThrustMeters(targetLeft, targetRight);
        queueMotor(targetLeft, targetRight);
    }

    function get8WaySector(deg) {
        if (deg >= 337.5 || deg < 22.5) return 'N';
        if (deg >= 22.5 && deg < 67.5) return 'NE';
        if (deg >= 67.5 && deg < 112.5) return 'E';
        if (deg >= 112.5 && deg < 157.5) return 'SE';
        if (deg >= 157.5 && deg < 202.5) return 'S';
        if (deg >= 202.5 && deg < 247.5) return 'SW';
        if (deg >= 247.5 && deg < 292.5) return 'W';
        return 'NW';
    }

    function highlight8WaySector(sec) {
        document.querySelectorAll('.dir-8-label').forEach(function(el) {
            el.classList.remove('active');
        });
        const badge = document.getElementById('steerDirVal');
        if (sec === 'STOP') {
            if (badge) badge.innerText = DIR_LABELS['STOP'];
            return;
        }
        const el = document.getElementById('dir_' + sec);
        if (el) el.classList.add('active');
        if (badge) badge.innerText = DIR_LABELS[sec] || sec;
    }

    // ==========================================================
    // 4. CẦN GA LÊN - XUỐNG ĐIỀU KHIỂN TỐC ĐỘ (SPEED THROTTLE)
    // ==========================================================
    function initSpeedThrottle() {
        speedJoyTrack = document.getElementById("speedJoyTrack");
        speedKnob = document.getElementById("speedKnob");
        speedFillLevel = document.getElementById("speedFillLevel");
        throttleStickVal = document.getElementById("throttleStickVal");

        if (!speedJoyTrack || !speedKnob) return;

        speedJoyTrack.addEventListener("pointerdown", onSpeedDown, { passive: false });
        window.addEventListener("pointermove", onSpeedMove, { passive: false });
        window.addEventListener("pointerup", onSpeedUp, { passive: false });
        window.addEventListener("pointercancel", onSpeedUp, { passive: false });
    }

    function onSpeedDown(e) {
        e.preventDefault();
        isSpeedDragActive = true;
        speedJoyTrack.setPointerCapture(e.pointerId);
        trackRect = speedJoyTrack.getBoundingClientRect();
        updateSpeedFromPointer(e.clientY);
    }

    function onSpeedMove(e) {
        if (!isSpeedDragActive) return;
        e.preventDefault();
        updateSpeedFromPointer(e.clientY);
    }

    function onSpeedUp(e) {
        if (!isSpeedDragActive) return;
        isSpeedDragActive = false;
    }

    function updateSpeedFromPointer(clientY) {
        if (!trackRect) trackRect = speedJoyTrack.getBoundingClientRect();
        let trackH = trackRect.height;
        let relativeY = clientY - trackRect.top;
        // relativeY = 0 ở đỉnh (100%), relativeY = trackH ở đáy (0%)
        let clampedY = Math.max(0, Math.min(trackH, relativeY));
        let pct = Math.round((1 - (clampedY / trackH)) * 100);
        // Tối thiểu 10%
        pct = Math.max(10, pct);
        setSpeedThrottleUI(pct);
    }

    function setSpeedThrottleUI(pct) {
        currentThrottle = pct;
        if (speedKnob) speedKnob.style.bottom = pct + '%';
        if (speedFillLevel) speedFillLevel.style.height = pct + '%';
        if (throttleStickVal) throttleStickVal.innerText = pct + '%';

        // Cập nhật chip preset
        document.querySelectorAll('.btn-preset').forEach(function(btn) {
            btn.classList.remove('active');
            if (btn.querySelector('.pct').innerText === pct + '%') {
                btn.classList.add('active');
            }
        });
    }

    function setSpeedPreset(pct) {
        setSpeedThrottleUI(pct);
        showToast("Tốc độ: " + pct + "%");
    }

    // ==========================================================
    // 5. CÔNG SUẤT ĐỘNG CƠ & GỬI LỆNH (DISPATCHER)
    // ==========================================================
    function updateThrustMeters(left, right) {
        if (joyLeftStat) joyLeftStat.innerText = (left > 0 ? "+" : "") + left + "%";
        if (joyRightStat) joyRightStat.innerText = (right > 0 ? "+" : "") + right + "%";

        if (barLeft) {
            let half = Math.abs(left) / 2;
            barLeft.style.left = left >= 0 ? "50%" : (50 - half) + "%";
            barLeft.style.width = half + "%";
            barLeft.style.background = left >= 0 ? "linear-gradient(90deg, #0284c7, #10b981)" : "linear-gradient(90deg, #ef4444, #f59e0b)";
        }

        if (barRight) {
            let half = Math.abs(right) / 2;
            barRight.style.left = right >= 0 ? "50%" : (50 - half) + "%";
            barRight.style.width = half + "%";
            barRight.style.background = right >= 0 ? "linear-gradient(90deg, #0284c7, #10b981)" : "linear-gradient(90deg, #ef4444, #f59e0b)";
        }
    }

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

        isHttpBusy = true;
        lastSentTime = performance.now();

        let url = (cmd.type === 'STOP') 
            ? "/command?cmd=STOP" 
            : "/command?cmd=" + encodeURIComponent("MOTOR|" + cmd.left + "|" + cmd.right);

        fetch(url, { cache: "no-store" })
            .catch(function() {})
            .finally(function() {
                isHttpBusy = false;
                if (nextCommand) dispatchNextCommand();
            });
    }

    function emergencyStop() {
        on8WayUp();
        queueStop();
        fetch("/command?cmd=STOP", { cache: "no-store" }).catch(function(){});
        showToast("ĐÃ DỪNG KHẨN CẤP TOÀN BỘ ĐỘNG CƠ!");
    }

    function sendFeedAction() {
        let dur = document.getElementById('feedDurationSetting') ? document.getElementById('feedDurationSetting').value : 500;
        fetch("/command?cmd=" + encodeURIComponent("FEED|" + dur), { cache: "no-store" }).catch(function(){});
        showToast("Đang kích hoạt rải mồi (" + dur + "ms)...");

        const btn = document.getElementById('btnFeedAction');
        if (btn) {
            btn.style.transform = "scale(0.96)";
            btn.style.filter = "brightness(1.15)";
            setTimeout(function() {
                btn.style.transform = "";
                btn.style.filter = "";
            }, 600);
        }
    }

    // ==========================================================
    // 6. TELEMETRY POLLING & STATS
    // ==========================================================
    function updateTelemetry() {
        if (isJoyActive || isHttpBusy) return;

        fetch("/status", { cache: "no-store" })
            .then(function(res) {
                if (!res.ok) throw new Error("Status error");
                return res.json();
            })
            .then(function(data) {
                applyTelemetry(data);
                setConnState(true);
            })
            .catch(function() {
                simulateTelemetryDev();
            });
    }

    function applyTelemetry(data) {
        currentLat = data.lat || currentLat;
        currentLon = data.lon || currentLon;
        const coordsText = currentLat + ", " + currentLon;
        
        const statGPSCoords = document.getElementById('statGPSCoords');
        if (statGPSCoords) statGPSCoords.innerText = coordsText;

        const isFix = (data.gps === "1" || parseInt(data.gps) > 3);
        const ctrlGpsPill = document.getElementById('ctrlGpsPill');
        if (ctrlGpsPill) ctrlGpsPill.innerText = isFix ? "GPS: " + (data.gps_sats || 12) + " SV" : "GPS: Tìm...";

        const spd = parseFloat(data.speed || 1.2).toFixed(1);
        if (document.getElementById('statSpeed')) document.getElementById('statSpeed').innerText = spd;

        const hdg = Math.round(parseFloat(data.heading || 245));
        if (document.getElementById('statHeading')) document.getElementById('statHeading').innerText = hdg;
        const needle = document.getElementById('compassNeedle');
        if (needle) needle.style.transform = "rotate(" + hdg + "deg)";

        const volt = parseFloat(data.battery || 12.4).toFixed(1);
        let pct = Math.max(0, Math.min(100, Math.round(((volt - 10.5) / (12.6 - 10.5)) * 100)));
        if (pct === 0 && volt > 0) pct = 85;
        
        const ctrlBattPill = document.getElementById('ctrlBattPill');
        if (ctrlBattPill) ctrlBattPill.innerText = "Pin: " + pct + "% (" + volt + "V)";

        if (document.getElementById('statBattPct')) document.getElementById('statBattPct').innerText = pct;
        if (document.getElementById('statBattVolt')) document.getElementById('statBattVolt').innerText = volt;
        if (document.getElementById('barBattProg')) document.getElementById('barBattProg').style.width = pct + "%";

        const tmp = parseFloat(data.temp || 28.4).toFixed(1);
        if (document.getElementById('statTemp')) document.getElementById('statTemp').innerText = tmp;

        const feedLvl = parseInt(data.feed || 60);
        if (document.getElementById('statFeedLevel')) document.getElementById('statFeedLevel').innerText = feedLvl;
        if (document.getElementById('barFeedProg')) document.getElementById('barFeedProg').style.width = feedLvl + "%";

        pushChartData(parseFloat(tmp), 7.5, parseFloat(spd), parseFloat(data.current || 1.8));
    }

    function simulateTelemetryDev() {
        let t = Date.now() / 3000;
        let simSpeed = (1.2 + Math.sin(t) * 0.15).toFixed(1);
        let simTemp = (28.4 + Math.cos(t) * 0.2).toFixed(1);
        let simHeading = Math.round(245 + Math.sin(t * 0.7) * 8);

        applyTelemetry({
            lat: "20.987654",
            lon: "105.765432",
            heading: simHeading.toString(),
            speed: simSpeed,
            battery: "12.4",
            current: "1.8",
            temp: simTemp,
            feed: "60",
            mode: currentMode,
            gps: "1",
            gps_sats: "12"
        });
        setConnState(true);
    }

    function setConnState(isOnline) {
        const txt = document.getElementById('connText');
        const ctrlOnline = document.getElementById('ctrlOnline');
        if (isOnline) {
            if (txt) txt.innerText = "Online";
            if (ctrlOnline) ctrlOnline.innerText = "Online";
        } else {
            if (txt) txt.innerText = "Mất kết nối";
            if (ctrlOnline) ctrlOnline.innerText = "Offline";
        }
    }

    setInterval(updateTelemetry, 1000);

    // ==========================================================
    // 7. REAL-TIME CANVAS CHART (LIGHT THEME)
    // ==========================================================
    function setChartMetric(metric) {
        activeMetric = metric;
        document.querySelectorAll('.chart-tab-btn').forEach(function(btn) {
            btn.classList.remove('active');
        });
        event.target.classList.add('active');

        const titleMap = {
            'temp': 'Biểu đồ nhiệt độ nước',
            'ph': 'Biểu đồ độ pH nước',
            'speed': 'Biểu đồ vận tốc USV',
            'current': 'Biểu đồ dòng tiêu thụ'
        };
        document.getElementById('chartLabelTitle').innerText = titleMap[metric];
        renderLiveChart();
    }

    function pushChartData(temp, ph, speed, current) {
        chartHistory.temp.shift();
        chartHistory.temp.push(temp);

        chartHistory.ph.shift();
        chartHistory.ph.push(ph);

        chartHistory.speed.shift();
        chartHistory.speed.push(speed);

        chartHistory.current.shift();
        chartHistory.current.push(current);

        if (currentTab === 'tab-status') {
            renderLiveChart();
        }
    }

    function renderLiveChart() {
        const canvas = document.getElementById('liveChartCanvas');
        if (!canvas) return;
        const ctx = canvas.getContext('2d');
        const width = canvas.offsetWidth;
        const height = canvas.offsetHeight;
        canvas.width = width * window.devicePixelRatio;
        canvas.height = height * window.devicePixelRatio;
        ctx.scale(window.devicePixelRatio, window.devicePixelRatio);

        const data = chartHistory[activeMetric] || chartHistory.temp;
        const currentVal = data[data.length - 1];

        const unitMap = { temp: ' °C', ph: ' pH', speed: ' m/s', current: ' A' };
        document.getElementById('chartLiveStat').innerText = currentVal.toFixed(1) + (unitMap[activeMetric] || '');

        let minVal = Math.min(...data);
        let maxVal = Math.max(...data);
        let range = (maxVal - minVal) || 1;
        minVal -= range * 0.15;
        maxVal += range * 0.15;
        range = maxVal - minVal;

        ctx.clearRect(0, 0, width, height);
        ctx.strokeStyle = "#e2e8f0";
        ctx.lineWidth = 1;
        for (let i = 1; i <= 3; i++) {
            let y = (height / 4) * i;
            ctx.beginPath();
            ctx.moveTo(0, y);
            ctx.lineTo(width, y);
            ctx.stroke();
        }

        let grad = ctx.createLinearGradient(0, 0, 0, height);
        if (activeMetric === 'temp') {
            grad.addColorStop(0, 'rgba(239, 68, 68, 0.25)');
            grad.addColorStop(1, 'rgba(239, 68, 68, 0.0)');
            ctx.strokeStyle = '#ef4444';
        } else if (activeMetric === 'ph') {
            grad.addColorStop(0, 'rgba(2, 132, 199, 0.25)');
            grad.addColorStop(1, 'rgba(2, 132, 199, 0.0)');
            ctx.strokeStyle = '#0284c7';
        } else if (activeMetric === 'speed') {
            grad.addColorStop(0, 'rgba(16, 185, 129, 0.25)');
            grad.addColorStop(1, 'rgba(16, 185, 129, 0.0)');
            ctx.strokeStyle = '#10b981';
        } else {
            grad.addColorStop(0, 'rgba(217, 119, 6, 0.25)');
            grad.addColorStop(1, 'rgba(217, 119, 6, 0.0)');
            ctx.strokeStyle = '#d97706';
        }

        ctx.lineWidth = 2.5;
        ctx.beginPath();
        const step = width / (data.length - 1);

        for (let i = 0; i < data.length; i++) {
            let x = i * step;
            let norm = (data[i] - minVal) / range;
            let y = height - (norm * (height - 20) + 10);
            if (i === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        }
        ctx.stroke();

        ctx.lineTo(width, height);
        ctx.lineTo(0, height);
        ctx.closePath();
        ctx.fillStyle = grad;
        ctx.fill();

        let lastX = width;
        let lastNorm = (currentVal - minVal) / range;
        let lastY = height - (lastNorm * (height - 20) + 10);
        ctx.beginPath();
        ctx.arc(lastX - 2, lastY, 4.5, 0, Math.PI * 2);
        ctx.fillStyle = ctx.strokeStyle;
        ctx.fill();
        ctx.strokeStyle = '#ffffff';
        ctx.lineWidth = 1.5;
        ctx.stroke();
    }


    // ==========================================================
    // 9. UTILS & EXPORT
    // ==========================================================
    function openGoogleMaps() {
        const url = "https://www.google.com/maps?q=" + currentLat + "," + currentLon;
        window.open(url, "_blank");
    }

    function copyGPS() {
        const text = currentLat + ", " + currentLon;
        if (navigator.clipboard) {
            navigator.clipboard.writeText(text).then(function() {
                showToast("Đã sao chép: " + text);
            });
        } else {
            showToast("Tọa độ: " + text);
        }
    }

    function exportCSVReport() {
        const rows = [
            ["ID", "Thoi_gian", "Vi_tri_GPS", "Luong_thuc_an_kg", "Trang_thai"],
            ["1", "09:05:12", "20.9871, 105.7651", "2.0", "Hoan thanh"],
            ["2", "09:12:40", "20.9875, 105.7658", "1.5", "Hoan thanh"],
            ["3", "09:20:15", "20.9882, 105.7663", "2.0", "Hoan thanh"],
            ["4", "09:28:50", "20.9868, 105.7659", "1.5", "Hoan thanh"]
        ];

        let csvContent = "data:text/csv;charset=utf-8," + rows.map(e => e.join(",")).join("\n");
        let encodedUri = encodeURI(csvContent);
        let link = document.createElement("a");
        link.setAttribute("href", encodedUri);
        link.setAttribute("download", "USV_Feeding_Report_" + new Date().toISOString().slice(0,10) + ".csv");
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        showToast("Đã xuất tệp báo cáo CSV!");
    }

    function openSettingsModal() {
        document.getElementById('settingsModal').classList.add('open');
    }

    function closeSettingsModal() {
        document.getElementById('settingsModal').classList.remove('open');
    }

    function saveSettings() {
        closeSettingsModal();
        showToast("Đã lưu cấu hình hệ thống!");
    }

    function showToast(msg) {
        const t = document.getElementById("appToast");
        if (!t) return;
        t.innerText = msg;
        t.classList.add("show");
        setTimeout(function() {
            t.classList.remove("show");
        }, 2200);
    }

    window.addEventListener("DOMContentLoaded", function() {
        init8WayJoystick();
        initSpeedThrottle();
        renderLiveChart();
    });
</script>
</body>
</html>
)rawliteral";

#endif // WEB_H
