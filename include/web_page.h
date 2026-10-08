#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>HAWA — Project CLAW Controller & Web Flasher</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=DotGothic16&family=Inter:wght@400;500;600;700;800&family=Space+Mono:ital,wght@0,400;0,700;1,400&display=swap" rel="stylesheet">
  <style>
    :root {
      --canvas-bg: #ece7de;
      --grid-dot: #b8b3a5;
      --grid-dot-size: 20px;
      --surface-card: #f4efe7;
      --surface-header: #eae5dc;
      --surface-elevated: #ffffff;
      --border-main: #d3cebf;
      --border-subtle: #dfdad0;
      --border-dark: #121212;
      --text-main: #141414;
      --text-sub: #524f48;
      --text-dim: #78746c;
      --btn-black-bg: #0e0e0e;
      --btn-black-text: #ffffff;
      --btn-outline-bg: #f5f1ea;
      --btn-outline-border: #bcb7aa;
      --pixel-red: #e11d48;
      --pixel-amber: #f59e0b;
      --pixel-green: #10b981;
      --pixel-blue: #2563eb;
      --font-dot: 'DotGothic16', monospace;
      --font-mono: 'Space Mono', monospace;
      --font-sans: 'Inter', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    }

    [data-theme="dark"] {
      --canvas-bg: #0f1318;
      --grid-dot: #222933;
      --surface-card: #181d24;
      --surface-header: #14181f;
      --surface-elevated: #202732;
      --border-main: #2b3543;
      --border-subtle: #1e2632;
      --border-dark: #f1f5f9;
      --text-main: #f1f5f9;
      --text-sub: #94a3b8;
      --text-dim: #64748b;
      --btn-black-bg: #38bdf8;
      --btn-black-text: #090d14;
      --btn-outline-bg: #1c222c;
      --btn-outline-border: #334155;
    }

    * { box-sizing: border-box; margin: 0; padding: 0; -webkit-font-smoothing: antialiased; }
    html, body { min-height: 100vh; width: 100%; }
    body {
      background-color: var(--canvas-bg);
      background-image: radial-gradient(circle, var(--grid-dot) 1.2px, transparent 1.2px);
      background-size: var(--grid-dot-size) var(--grid-dot-size);
      color: var(--text-main);
      font-family: var(--font-sans);
      line-height: 1.5;
      overflow-x: hidden;
      transition: background-color 0.25s ease;
    }

    .navbar {
      width: 100%;
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 16px 28px;
      max-width: 1200px;
      margin: 0 auto;
    }

    .nav-brand {
      display: flex;
      align-items: center;
      gap: 10px;
      text-decoration: none;
      color: var(--text-main);
      cursor: pointer;
    }

    .brand-icon-box {
      width: 26px;
      height: 26px;
      background: var(--text-main);
      color: var(--canvas-bg);
      display: flex;
      align-items: center;
      justify-content: center;
      border-radius: 2px;
    }

    .brand-icon-box svg { width: 16px; height: 16px; stroke: currentColor; }
    .brand-title { font-family: var(--font-mono); font-size: 15px; font-weight: 700; letter-spacing: 2px; text-transform: uppercase; }

    .nav-links { display: flex; align-items: center; gap: 24px; }
    .nav-link {
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 1px;
      text-transform: uppercase;
      color: var(--text-sub);
      text-decoration: none;
      cursor: pointer;
      transition: color 0.15s ease;
    }
    .nav-link:hover, .nav-link.active { color: var(--text-main); }

    .nav-actions { display: flex; align-items: center; gap: 12px; }
    .btn-theme-toggle {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      background: transparent;
      border: 1px solid var(--border-main);
      padding: 6px 12px;
      border-radius: 4px;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      color: var(--text-sub);
      cursor: pointer;
    }
    .btn-theme-toggle:hover { border-color: var(--text-main); color: var(--text-main); }

    .btn-command-center {
      background: transparent;
      border: 1px solid var(--border-dark);
      padding: 7px 14px;
      border-radius: 2px;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      color: var(--text-main);
      cursor: pointer;
    }

    .btn-connect-device {
      background: var(--btn-black-bg);
      border: 1px solid var(--btn-black-bg);
      color: var(--btn-black-text);
      padding: 7px 16px;
      border-radius: 2px;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }

    .hero-container {
      display: flex;
      flex-direction: column;
      align-items: center;
      text-align: center;
      padding: 36px 20px 24px;
      max-width: 900px;
      margin: 0 auto;
    }

    .hero-tag-badge {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      background: rgba(255, 255, 255, 0.45);
      border: 1px solid var(--border-main);
      padding: 5px 14px;
      border-radius: 4px;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 1.5px;
      text-transform: uppercase;
      color: var(--text-sub);
      margin-bottom: 22px;
    }

    .hero-title {
      font-family: var(--font-dot);
      font-size: clamp(32px, 5.2vw, 56px);
      font-weight: 700;
      letter-spacing: 4px;
      line-height: 1.24;
      text-transform: uppercase;
      color: var(--text-main);
      margin-bottom: 20px;
      max-width: 840px;
      user-select: text;
    }

    .hero-subtitle {
      font-family: var(--font-sans);
      font-size: 14.5px;
      line-height: 1.65;
      color: var(--text-sub);
      max-width: 660px;
      margin-bottom: 28px;
    }

    .hero-cta-group {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 14px;
      margin-bottom: 24px;
      flex-wrap: wrap;
    }

    .btn-hero-primary {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      background: var(--btn-black-bg);
      color: var(--btn-black-text);
      border: 1px solid var(--border-dark);
      padding: 12px 22px;
      border-radius: 3px;
      font-family: var(--font-mono);
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      cursor: pointer;
    }

    .btn-hero-secondary {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      background: var(--btn-outline-bg);
      color: var(--text-main);
      border: 1px solid var(--btn-outline-border);
      padding: 12px 22px;
      border-radius: 3px;
      font-family: var(--font-mono);
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      cursor: pointer;
    }

    .hero-bullets-row {
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 24px;
      flex-wrap: wrap;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 1.2px;
      color: var(--text-dim);
      text-transform: uppercase;
      margin-bottom: 34px;
    }

    .hero-bullet-item { display: inline-flex; align-items: center; gap: 7px; }
    .bullet-red-square { display: inline-block; width: 6px; height: 6px; background: var(--pixel-red); }

    .workbench-container { width: 100%; max-width: 980px; margin: 0 auto 60px; padding: 0 16px; }
    .workbench-card {
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: 10px;
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.04);
      overflow: hidden;
    }
    .workbench-card.expanded { max-width: 1240px; }

    .workbench-titlebar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 10px 18px;
      background: var(--surface-header);
      border-bottom: 1px solid var(--border-main);
    }
    .titlebar-left { display: flex; align-items: center; gap: 12px; }
    .window-dots { display: flex; align-items: center; gap: 5px; }
    .window-square-dot { width: 8px; height: 8px; border-radius: 1px; }
    .dot-red { background: #ef4444; }
    .dot-amber { background: #f59e0b; }
    .dot-green { background: #10b981; }

    .window-title { font-family: var(--font-dot); font-size: 13px; letter-spacing: 1.5px; text-transform: uppercase; color: var(--text-sub); }
    .btn-expand-window {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      background: transparent;
      border: 1px solid var(--border-main);
      padding: 4px 10px;
      border-radius: 3px;
      font-family: var(--font-mono);
      font-size: 10px;
      font-weight: 700;
      letter-spacing: 1px;
      text-transform: uppercase;
      color: var(--text-main);
      cursor: pointer;
    }

    .workbench-content { padding: 20px; display: flex; flex-direction: column; gap: 20px; }
    .mode-selector-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 14px; }
    .mode-card {
      display: flex;
      align-items: center;
      gap: 14px;
      padding: 14px 16px;
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: 4px;
      cursor: pointer;
      position: relative;
    }
    .mode-card.active { background: var(--surface-elevated); border: 2px solid var(--border-dark); box-shadow: 0 4px 14px rgba(0,0,0,0.05); }
    .mode-icon-box {
      width: 44px;
      height: 44px;
      background: rgba(0, 0, 0, 0.05);
      border: 1px solid var(--border-main);
      border-radius: 3px;
      display: flex;
      align-items: center;
      justify-content: center;
      flex-shrink: 0;
      color: var(--text-main);
    }
    .mode-card.active .mode-icon-box { background: var(--btn-black-bg); border-color: var(--btn-black-bg); color: var(--btn-black-text); }
    .mode-card-title { font-family: var(--font-dot); font-size: 15px; letter-spacing: 1.2px; text-transform: uppercase; color: var(--text-main); margin-bottom: 2px; }
    .mode-card-desc { font-size: 11px; color: var(--text-sub); line-height: 1.35; }
    .mode-active-indicator { position: absolute; top: 9px; right: 9px; width: 6px; height: 6px; background: var(--pixel-red); }

    .workbench-panel { display: none; flex-direction: column; gap: 16px; }
    .workbench-panel.active { display: flex; }

    .telemetry-banner {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: 4px;
      padding: 14px 16px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }
    .telemetry-header { display: flex; justify-content: space-between; align-items: center; }
    .telemetry-tag { font-family: var(--font-mono); font-size: 11px; font-weight: 700; letter-spacing: 1px; text-transform: uppercase; color: var(--text-dim); }
    .state-badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      padding: 4px 12px;
      border-radius: 3px;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      background: rgba(16, 185, 129, 0.1);
      border: 1px solid rgba(16, 185, 129, 0.4);
      color: var(--pixel-green);
    }

    .progress-bar-bg { width: 100%; height: 7px; background: var(--surface-card); border: 1px solid var(--border-subtle); border-radius: 2px; overflow: hidden; }
    .progress-bar-fill { width: 0%; height: 100%; background: var(--text-main); transition: width 0.1s linear; }

    .motor-telemetry-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
    .motor-stat-card { background: var(--surface-card); padding: 10px 12px; border-radius: 3px; border: 1px solid var(--border-subtle); }
    .motor-stat-label { font-family: var(--font-mono); font-size: 10px; font-weight: 700; color: var(--text-dim); text-transform: uppercase; margin-bottom: 3px; display: flex; justify-content: space-between; }
    .motor-stat-val { font-family: var(--font-mono); font-size: 13px; font-weight: 700; color: var(--text-main); }

    .tactile-actions-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; }
    .btn-tactile {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: 4px;
      padding: 18px 14px;
      color: var(--text-main);
      font-family: var(--font-mono);
      font-size: 13px;
      font-weight: 700;
      letter-spacing: 1px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 8px;
      cursor: pointer;
    }
    .btn-tactile-deploy { border-left: 4px solid var(--pixel-red); }
    .btn-tactile-retract { border-left: 4px solid var(--pixel-blue); }
    .btn-tactile svg { width: 26px; height: 26px; stroke: currentColor; }

    .jog-buttons-row { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
    .btn-jog {
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: 3px;
      padding: 9px 12px;
      color: var(--text-sub);
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.5px;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 6px;
      cursor: pointer;
    }

    .actions-full-row { display: grid; grid-template-columns: 1fr 1.2fr; gap: 10px; }
    .btn-auto-demo {
      background: var(--surface-elevated);
      border: 1px solid var(--border-dark);
      border-radius: 3px;
      padding: 12px 14px;
      color: var(--text-main);
      font-family: var(--font-mono);
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      cursor: pointer;
    }
    .btn-emergency-stop {
      background: var(--pixel-red);
      border: 1px solid var(--pixel-red);
      border-radius: 3px;
      padding: 12px 14px;
      color: #ffffff;
      font-family: var(--font-mono);
      font-size: 13px;
      font-weight: 700;
      letter-spacing: 1.5px;
      text-transform: uppercase;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 8px;
      cursor: pointer;
      box-shadow: 0 4px 12px rgba(225, 29, 72, 0.25);
    }

    .speed-control-box {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: 4px;
      padding: 14px 16px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }
    .speed-box-header { display: flex; justify-content: space-between; align-items: center; }
    .speed-box-title { font-family: var(--font-mono); font-size: 11px; font-weight: 700; letter-spacing: 1px; text-transform: uppercase; color: var(--text-sub); }
    .speed-box-readout { font-family: var(--font-mono); font-size: 13px; font-weight: 700; color: var(--text-main); }

    .custom-range-slider {
      -webkit-appearance: none;
      width: 100%;
      height: 6px;
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: 3px;
      outline: none;
    }
    .custom-range-slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 16px;
      height: 16px;
      background: var(--btn-black-bg);
      border: 1px solid var(--border-dark);
      border-radius: 2px;
      cursor: pointer;
    }

    .speed-presets-row { display: grid; grid-template-columns: repeat(4, 1fr); gap: 8px; }
    .btn-speed-preset {
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: 3px;
      padding: 6px;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      color: var(--text-sub);
      cursor: pointer;
    }
    .btn-speed-preset.active { background: var(--btn-black-bg); color: var(--btn-black-text); border-color: var(--btn-black-bg); }

    .accordion-wrapper { display: flex; flex-direction: column; gap: 8px; }
    .accordion-item { border: 1px solid var(--border-main); border-radius: 4px; background: var(--surface-elevated); overflow: hidden; }
    .accordion-header {
      padding: 12px 14px;
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      color: var(--text-main);
      display: flex;
      justify-content: space-between;
      align-items: center;
      cursor: pointer;
      background: var(--surface-card);
    }
    .accordion-arrow { width: 14px; height: 14px; transition: transform 0.15s ease; }
    .accordion-item.open .accordion-arrow { transform: rotate(180deg); }
    .accordion-body { display: none; padding: 14px; flex-direction: column; gap: 12px; border-top: 1px solid var(--border-subtle); }
    .accordion-item.open .accordion-body { display: flex; }

    .diag-table { width: 100%; font-family: var(--font-mono); font-size: 11px; border-collapse: collapse; }
    .diag-table td { padding: 6px 0; border-bottom: 1px solid var(--border-subtle); color: var(--text-sub); }
    .diag-table tr:last-child td { border-bottom: none; }
    .diag-table .diag-val { text-align: right; font-weight: 700; color: var(--text-main); }

    .toast-notice {
      position: fixed;
      bottom: 24px;
      left: 50%;
      transform: translateX(-50%) translateY(100px);
      background: var(--btn-black-bg);
      border: 1px solid var(--border-dark);
      color: var(--btn-black-text);
      padding: 8px 18px;
      border-radius: 4px;
      font-family: var(--font-mono);
      font-size: 11.5px;
      font-weight: 700;
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.2);
      transition: transform 0.25s ease;
      z-index: 100;
      pointer-events: none;
    }
    .toast-notice.show { transform: translateX(-50%) translateY(0); }

    @media (max-width: 768px) {
      .navbar { padding: 12px 16px; flex-wrap: wrap; gap: 12px; }
      .nav-links { order: 3; width: 100%; justify-content: center; gap: 16px; padding-top: 6px; }
      .hero-container { padding: 24px 16px 16px; }
      .hero-title { letter-spacing: 2px; }
      .tactile-actions-grid { grid-template-columns: 1fr; }
      .actions-full-row { grid-template-columns: 1fr; }
    }
  </style>
</head>

<body>
  <!-- Header -->
  <header class="navbar">
    <a href="#" class="nav-brand">
      <div class="brand-icon-box">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round">
          <path d="M4 8h16M4 12h12M4 16h16" />
        </svg>
      </div>
      <span class="brand-title">HAWA</span>
    </a>

    <nav class="nav-links">
      <a class="nav-link active" onclick="selectMode('ota')">WEB FLASHER</a>
      <a class="nav-link" onclick="selectMode('tactile')">FEATURES</a>
      <a class="nav-link" onclick="selectMode('tactile')">CONTROLLER</a>
      <a class="nav-link" onclick="selectMode('command')">DOCS</a>
    </nav>

    <div class="nav-actions">
      <button class="btn-theme-toggle" id="theme-toggle-btn" onclick="toggleTheme()">
        <span id="theme-toggle-label">☾ DARK</span>
      </button>
      <button class="btn-command-center" onclick="selectMode('command')">COMMAND CENTER</button>
      <a href="/update" target="_blank" class="btn-connect-device" style="text-decoration:none;">CONNECT DEVICE</a>
    </div>
  </header>

  <!-- Hero Section -->
  <main class="hero-container">
    <div class="hero-tag-badge">ZERO DRIVERS NEEDED &bull; NATIVE WEB SERIAL</div>

    <h1 class="hero-title">
      TRANSFORM YOUR<br>
      HARDWARE INTO<br>
      A GLOBAL WIRELESS<br>
      FLEET
    </h1>

    <p class="hero-subtitle">
      Zero software, Arduino IDE, or driver installations needed. Plug your ESP32 or ESP8266
      into your browser via USB, enter your Wi-Fi credentials, and unlock frictionless over-the-air
      programming anywhere in the world.
    </p>

    <div class="hero-cta-group">
      <button class="btn-hero-primary" onclick="selectMode('tactile')">
        <svg width="15" height="15" viewBox="0 0 24 24" fill="currentColor">
          <polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2" />
        </svg>
        CONNECT USB &amp; FLASH
      </button>
      <button class="btn-hero-secondary" onclick="selectMode('tactile')">
        <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor">
          <polygon points="5 3 19 12 5 21 5 3" />
        </svg>
        OPEN FLEET DASHBOARD
      </button>
    </div>

    <div class="hero-bullets-row">
      <div class="hero-bullet-item"><span class="bullet-red-square"></span><span>ZERO DRIVER INSTALLATION</span></div>
      <div class="hero-bullet-item"><span class="bullet-red-square"></span><span>NATIVE WEB SERIAL API</span></div>
      <div class="hero-bullet-item"><span class="bullet-red-square"></span><span>ANTI-BRICK DUAL PARTITION</span></div>
    </div>
  </main>

  <!-- Workbench Window Card -->
  <section class="workbench-container">
    <div class="workbench-card" id="workbench-card">
      <div class="workbench-titlebar">
        <div class="titlebar-left">
          <div class="window-dots">
            <span class="window-square-dot dot-red"></span>
            <span class="window-square-dot dot-amber"></span>
            <span class="window-square-dot dot-green"></span>
          </div>
          <span class="window-title">HAWA WEB FLASHER &bull; FIRMWARE WORKBENCH</span>
        </div>
        <button class="btn-expand-window" onclick="toggleExpandWindow()">
          <span id="expand-btn-text">EXPAND WINDOW</span>
        </button>
      </div>

      <div class="workbench-content">
        <!-- Mode Cards -->
        <div class="mode-selector-grid">
          <div class="mode-card active" id="mode-card-ota" onclick="selectMode('ota')">
            <div class="mode-icon-box">
              <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <path d="M4 8h16M4 12h12M4 16h16" />
              </svg>
            </div>
            <div>
              <div class="mode-card-title">HAWA OTA PLATFORM</div>
              <div class="mode-card-desc">Zero-config Wi-Fi provisioning &amp; remote OTA links</div>
            </div>
            <span class="mode-active-indicator"></span>
          </div>

          <div class="mode-card" id="mode-card-firmware" onclick="window.open('/update','_blank')">
            <div class="mode-icon-box">
              <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                <polyline points="17 8 12 3 7 8"></polyline>
                <line x1="12" y1="3" x2="12" y2="15"></line>
              </svg>
            </div>
            <div>
              <div class="mode-card-title">CUSTOM FIRMWARE (.BIN)</div>
              <div class="mode-card-desc">Flash your own compiled binary from Arduino or PlatformIO</div>
            </div>
          </div>
        </div>

        <!-- Telemetry & Actuation Panel -->
        <div class="workbench-panel active" id="panel-ota">
          <div class="telemetry-banner">
            <div class="telemetry-header">
              <span class="telemetry-tag">System Mode &bull; Dual Cable State</span>
              <div class="state-badge" id="state-badge"><span id="state-text">IDLE / READY</span></div>
            </div>
            <div class="progress-bar-bg"><div class="progress-bar-fill" id="progress-bar"></div></div>
            <div class="motor-telemetry-grid">
              <div class="motor-stat-card">
                <div class="motor-stat-label"><span>Motor 1 (Contract)</span><span id="m1-badge-state">OFF</span></div>
                <div class="motor-stat-val" id="m1-val">0% (0 PWM)</div>
              </div>
              <div class="motor-stat-card">
                <div class="motor-stat-label"><span>Motor 2 (Retract)</span><span id="m2-badge-state">OFF</span></div>
                <div class="motor-stat-val" id="m2-val">0% (0 PWM)</div>
              </div>
            </div>
          </div>

          <div class="tactile-actions-grid">
            <button class="btn-tactile btn-tactile-deploy" onclick="triggerCommand('deploy')">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M12 2v20M17 7l-5-5-5 5" /></svg>
              <span>DEPLOY CLAWS (MOTOR 1)</span>
            </button>
            <button class="btn-tactile btn-tactile-retract" onclick="triggerCommand('retract')">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><path d="M12 22V2M7 17l5 5 5-5" /></svg>
              <span>RETRACT CLAWS (MOTOR 2)</span>
            </button>
          </div>

          <div class="jog-buttons-row">
            <button class="btn-jog" onclick="triggerJog('deploy')">JOG CONTRACT (0.5s)</button>
            <button class="btn-jog" onclick="triggerJog('retract')">JOG RETRACT (0.5s)</button>
          </div>

          <div class="actions-full-row">
            <button class="btn-auto-demo" onclick="triggerCommand('demo')">AUTO DEMO SEQUENCE</button>
            <button class="btn-emergency-stop" onclick="triggerCommand('stop')">EMERGENCY STOP</button>
          </div>

          <div class="speed-control-box">
            <div class="speed-box-header">
              <span class="speed-box-title">MOTOR SPEED PWM</span>
              <span class="speed-box-readout" id="speed-val-display">83% (850 PWM)</span>
            </div>
            <input type="range" min="300" max="1023" value="850" class="custom-range-slider" id="speed-slider" oninput="onSpeedSliderChange(this.value)">
            <div class="speed-presets-row">
              <button class="btn-speed-preset" onclick="setSpeedPreset(512, this)">50%</button>
              <button class="btn-speed-preset" onclick="setSpeedPreset(768, this)">75%</button>
              <button class="btn-speed-preset active" onclick="setSpeedPreset(850, this)">83%</button>
              <button class="btn-speed-preset" onclick="setSpeedPreset(1023, this)">100%</button>
            </div>
          </div>

          <div class="accordion-wrapper">
            <div class="accordion-item" id="acc-diag">
              <div class="accordion-header" onclick="toggleAccordion('acc-diag')">
                <span>📶 NETWORK &amp; HARDWARE DIAGNOSTICS</span>
                <svg class="accordion-arrow" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polyline points="6 9 12 15 18 9" /></svg>
              </div>
              <div class="accordion-body">
                <table class="diag-table">
                  <tr><td>Wi-Fi Access Point</td><td class="diag-val">Project-CLAW</td></tr>
                  <tr><td>IP Address</td><td class="diag-val">192.168.4.1</td></tr>
                  <tr><td>mDNS Hostname</td><td class="diag-val">http://claw.local</td></tr>
                  <tr><td>Drivers</td><td class="diag-val">BTS7960 43A (D1/D2, D5/D6)</td></tr>
                </table>
              </div>
            </div>
          </div>

        </div>

      </div>
    </div>
  </section>

  <div class="toast-notice" id="toast-notice">Settings Applied</div>

  <script>
    let targetPwm = 850;

    function toggleTheme() {
      const isDark = document.documentElement.getAttribute('data-theme') === 'dark';
      if (isDark) {
        document.documentElement.removeAttribute('data-theme');
        document.getElementById('theme-toggle-label').textContent = '☾ DARK';
      } else {
        document.documentElement.setAttribute('data-theme', 'dark');
        document.getElementById('theme-toggle-label').textContent = '☀ LIGHT';
      }
    }

    function toggleExpandWindow() {
      const card = document.getElementById('workbench-card');
      card.classList.toggle('expanded');
    }

    function toggleAccordion(id) {
      document.getElementById(id).classList.toggle('open');
    }

    function showToast(msg) {
      const toast = document.getElementById('toast-notice');
      toast.textContent = msg;
      toast.classList.add('show');
      setTimeout(() => toast.classList.remove('show'), 2000);
    }

    function triggerCommand(cmd) {
      fetch('/api/' + cmd, { method: 'POST' }).catch(() => {});
      showToast('Command: ' + cmd.toUpperCase());
    }

    function triggerJog(dir) {
      fetch('/api/' + dir, { method: 'POST' }).catch(() => {});
      setTimeout(() => fetch('/api/stop', { method: 'POST' }).catch(() => {}), 500);
      showToast('Jog: ' + dir.toUpperCase());
    }

    function onSpeedSliderChange(val) {
      targetPwm = parseInt(val);
      const pct = Math.round((targetPwm / 1023) * 100);
      document.getElementById('speed-val-display').textContent = pct + '% (' + targetPwm + ' PWM)';
      document.querySelectorAll('.btn-speed-preset').forEach(b => b.classList.remove('active'));
    }

    function setSpeedPreset(val, btn) {
      document.getElementById('speed-slider').value = val;
      onSpeedSliderChange(val);
      if (btn) btn.classList.add('active');
      fetch('/api/config?pwm=' + val, { method: 'POST' }).catch(() => {});
    }

    async function pollStatus() {
      try {
        const res = await fetch('/api/status');
        if (!res.ok) return;
        const data = await res.json();
        const badge = document.getElementById('state-badge');
        document.getElementById('state-text').textContent = data.state;
        
        if (data.state.includes('DEPLOY')) {
          badge.style.color = 'var(--pixel-red)';
        } else if (data.state.includes('RETRACT')) {
          badge.style.color = 'var(--pixel-blue)';
        } else if (data.state.includes('STOP')) {
          badge.style.color = 'var(--pixel-red)';
        } else {
          badge.style.color = 'var(--pixel-green)';
        }

        document.getElementById('m1-badge-state').textContent = data.m1_pwm > 0 ? 'ACTIVE' : 'OFF';
        document.getElementById('m1-val').textContent = data.m1_pwm > 0 ? Math.round((data.m1_pwm/1023)*100) + '% (' + data.m1_pwm + ' PWM)' : '0% (0 PWM)';

        document.getElementById('m2-badge-state').textContent = data.m2_pwm > 0 ? 'ACTIVE' : 'OFF';
        document.getElementById('m2-val').textContent = data.m2_pwm > 0 ? Math.round((data.m2_pwm/1023)*100) + '% (' + data.m2_pwm + ' PWM)' : '0% (0 PWM)';

        if (data.total_duration > 0 && data.remaining > 0) {
          const pct = Math.max(0, Math.min(100, ((data.total_duration - data.remaining) / data.total_duration) * 100));
          document.getElementById('progress-bar').style.width = pct + '%';
        } else {
          document.getElementById('progress-bar').style.width = '0%';
        }
      } catch (err) {}
    }

    setInterval(pollStatus, 450);
  </script>
</body>
</html>
)rawliteral";
