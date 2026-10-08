#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
  <title>Himalix Projects — Project CLAW Controller</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=DotGothic16&family=Inter:wght@400;500;600;700;800&family=Space+Mono:ital,wght@0,400;0,700;1,400&display=swap" rel="stylesheet">
  <style>
    :root {
      --canvas-bg: #ece7de;
      --grid-dot: #b8b3a5;
      --grid-dot-size: 18px;
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
      --pixel-red: #e11d48;
      --pixel-amber: #f59e0b;
      --pixel-green: #10b981;
      --pixel-blue: #2563eb;
      --font-dot: 'DotGothic16', monospace;
      --font-mono: 'Space Mono', monospace;
      --font-sans: 'Inter', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
      --radius-sm: 4px;
      --radius-md: 8px;
      --radius-lg: 14px;
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
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      -webkit-font-smoothing: antialiased;
      -webkit-tap-highlight-color: transparent;
      -webkit-touch-callout: none !important;
      -webkit-user-select: none !important;
      -khtml-user-select: none !important;
      -moz-user-select: none !important;
      -ms-user-select: none !important;
      user-select: none !important;
      touch-action: manipulation;
    }

    button, a {
      touch-action: none !important;
      -webkit-touch-callout: none !important;
      -webkit-user-select: none !important;
      user-select: none !important;
    }

    input[type="range"] {
      touch-action: pan-x;
    }

    html, body { min-height: 100%; width: 100%; overflow-x: hidden; }
    body {
      min-height: 100vh;
      min-height: 100dvh;
      background-color: var(--canvas-bg);
      background-image: radial-gradient(circle, var(--grid-dot) 1.2px, transparent 1.2px);
      background-size: var(--grid-dot-size) var(--grid-dot-size);
      color: var(--text-main);
      font-family: var(--font-sans);
      line-height: 1.4;
      display: flex;
      justify-content: center;
      align-items: center;
      padding: max(10px, env(safe-area-inset-top, 10px)) max(10px, env(safe-area-inset-right, 10px)) max(12px, env(safe-area-inset-bottom, 12px)) max(10px, env(safe-area-inset-left, 10px));
      transition: background-color 0.2s ease;
    }

    .mobile-app-container {
      width: 100%;
      max-width: 440px;
      display: flex;
      flex-direction: column;
      margin: auto 0;
    }

    .workbench-card {
      width: 100%;
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-lg);
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.06);
      overflow: hidden;
      display: flex;
      flex-direction: column;
    }

    .workbench-titlebar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 11px 14px;
      background: var(--surface-header);
      border-bottom: 1px solid var(--border-main);
    }

    .titlebar-left { display: flex; align-items: center; gap: 10px; }
    .window-dots { display: flex; align-items: center; gap: 4px; }
    .window-square-dot { width: 7px; height: 7px; border-radius: 1px; }
    .dot-red { background: #ef4444; }
    .dot-amber { background: #f59e0b; }
    .dot-green { background: #10b981; }

    .window-title {
      font-family: var(--font-dot);
      font-size: 13px;
      letter-spacing: 1.4px;
      text-transform: uppercase;
      color: var(--text-main);
      font-weight: 700;
      white-space: nowrap;
    }

    .titlebar-actions { display: flex; align-items: center; gap: 6px; }

    .btn-ota {
      display: inline-flex;
      align-items: center;
      gap: 4px;
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      color: var(--text-main);
      padding: 4px 8px;
      border-radius: var(--radius-sm);
      font-family: var(--font-mono);
      font-size: 10px;
      font-weight: 700;
      text-decoration: none;
      cursor: pointer;
    }
    .btn-ota svg { width: 12px; height: 12px; stroke: currentColor; }

    .status-pill {
      font-family: var(--font-mono);
      font-size: 9.5px;
      font-weight: 700;
      padding: 4px 7px;
      border-radius: 2px;
      background: rgba(16, 185, 129, 0.12);
      border: 1px solid rgba(16, 185, 129, 0.4);
      color: var(--pixel-green);
      text-transform: uppercase;
      display: inline-flex;
      align-items: center;
      gap: 5px;
    }
    .status-pulse-dot { width: 5px; height: 5px; border-radius: 50%; background: var(--pixel-green); }

    .btn-theme-toggle {
      background: transparent;
      border: 1px solid var(--border-main);
      padding: 4px 8px;
      border-radius: var(--radius-sm);
      font-family: var(--font-mono);
      font-size: 9.5px;
      font-weight: 700;
      color: var(--text-sub);
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 4px;
    }
    .btn-theme-toggle svg { width: 12px; height: 12px; stroke: currentColor; }

    .workbench-content {
      padding: 12px 14px 14px;
      display: flex;
      flex-direction: column;
      gap: 11px;
    }

    .tactile-actions-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
    }

    .btn-tactile {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-md);
      padding: 16px 10px;
      color: var(--text-main);
      font-family: var(--font-mono);
      font-size: 13px;
      font-weight: 700;
      letter-spacing: 0.6px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 8px;
      cursor: pointer;
      min-height: 86px;
      box-shadow: 0 2px 6px rgba(0, 0, 0, 0.03);
      transition: transform 0.08s ease, background 0.08s ease;
      position: relative;
    }
    .btn-tactile svg { width: 24px; height: 24px; stroke: currentColor; }
    .btn-tactile-deploy { border-left: 5px solid var(--pixel-red); }
    .btn-tactile-retract { border-left: 5px solid var(--pixel-blue); }

    .btn-tactile:active, .btn-tactile.holding { transform: scale(0.96); }
    .btn-tactile-deploy:active, .btn-tactile-deploy.holding { background: rgba(225, 29, 72, 0.09); border-color: var(--pixel-red); }
    .btn-tactile-retract:active, .btn-tactile-retract.holding { background: rgba(37, 99, 235, 0.09); border-color: var(--pixel-blue); }

    .actions-full-row {
      display: grid;
      grid-template-columns: 1fr 1.25fr;
      gap: 10px;
    }

    .btn-auto-demo {
      background: var(--surface-elevated);
      border: 1px solid var(--border-dark);
      border-radius: var(--radius-sm);
      padding: 12px 10px;
      color: var(--text-main);
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.6px;
      text-transform: uppercase;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 6px;
      cursor: pointer;
      min-height: 48px;
    }
    .btn-auto-demo svg { width: 14px; height: 14px; }
    .btn-auto-demo:active { transform: scale(0.97); }

    .btn-emergency-stop {
      background: var(--pixel-red);
      border: 1px solid var(--pixel-red);
      border-radius: var(--radius-sm);
      padding: 12px 10px;
      color: #ffffff;
      font-family: var(--font-mono);
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 1px;
      text-transform: uppercase;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 6px;
      cursor: pointer;
      min-height: 48px;
      box-shadow: 0 4px 14px rgba(225, 29, 72, 0.28);
    }
    .btn-emergency-stop svg { width: 14px; height: 14px; }
    .btn-emergency-stop:active { transform: scale(0.96); opacity: 0.9; }

    .speed-control-box {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-md);
      padding: 11px 14px 13px;
      display: flex;
      flex-direction: column;
      gap: 9px;
      box-shadow: 0 2px 6px rgba(0, 0, 0, 0.02);
    }

    .speed-box-header { display: flex; justify-content: space-between; align-items: center; }
    .speed-box-title { font-family: var(--font-mono); font-size: 10.5px; font-weight: 700; letter-spacing: 0.8px; text-transform: uppercase; color: var(--text-sub); }
    .speed-box-readout { font-family: var(--font-mono); font-size: 12.5px; font-weight: 700; color: var(--text-main); }

    .custom-range-slider {
      -webkit-appearance: none;
      width: 100%;
      height: 8px;
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: 4px;
      outline: none;
    }
    .custom-range-slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 24px;
      height: 24px;
      background: var(--btn-black-bg);
      border: 1px solid var(--border-dark);
      border-radius: 3px;
      cursor: pointer;
      box-shadow: 0 2px 6px rgba(0, 0, 0, 0.18);
    }

    .speed-presets-row { display: grid; grid-template-columns: repeat(4, 1fr); gap: 6px; }
    .btn-speed-preset {
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-sm);
      padding: 7px 4px;
      font-family: var(--font-mono);
      font-size: 10.5px;
      font-weight: 700;
      color: var(--text-sub);
      cursor: pointer;
      min-height: 36px;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    .btn-speed-preset.active { background: var(--btn-black-bg); color: var(--btn-black-text); border-color: var(--btn-black-bg); }

    .timing-calibration-box {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-md);
      padding: 11px 14px 13px;
      display: flex;
      flex-direction: column;
      gap: 9px;
      box-shadow: 0 2px 6px rgba(0, 0, 0, 0.02);
    }

    .timing-box-header {
      display: flex;
      align-items: center;
      gap: 6px;
      font-family: var(--font-mono);
      font-size: 10.5px;
      font-weight: 700;
      letter-spacing: 0.8px;
      text-transform: uppercase;
      color: var(--text-main);
    }
    .timing-box-header svg { width: 13px; height: 13px; stroke: currentColor; }

    .timing-row { display: flex; flex-direction: column; gap: 3px; }
    .timing-row-labels { display: flex; justify-content: space-between; font-family: var(--font-mono); font-size: 10px; color: var(--text-sub); }
    .timing-val-tag { font-weight: 700; color: var(--text-main); }

    .btn-save-calib {
      background: var(--surface-card);
      border: 1px solid var(--border-dark);
      border-radius: var(--radius-sm);
      padding: 9px;
      color: var(--text-main);
      font-family: var(--font-mono);
      font-size: 10.5px;
      font-weight: 700;
      letter-spacing: 0.6px;
      text-transform: uppercase;
      display: flex;
      align-items: center;
      justify-content: center;
      gap: 6px;
      cursor: pointer;
      margin-top: 2px;
    }
    .btn-save-calib svg { width: 13px; height: 13px; stroke: currentColor; }
    .btn-save-calib:active { background: var(--btn-black-bg); color: var(--btn-black-text); transform: scale(0.97); }

    .toast-notice {
      position: fixed;
      bottom: 18px;
      left: 50%;
      transform: translateX(-50%) translateY(80px);
      background: var(--btn-black-bg);
      border: 1px solid var(--border-dark);
      color: var(--btn-black-text);
      padding: 8px 16px;
      border-radius: var(--radius-sm);
      font-family: var(--font-mono);
      font-size: 11px;
      font-weight: 700;
      box-shadow: 0 6px 20px rgba(0, 0, 0, 0.25);
      transition: transform 0.25s ease;
      z-index: 100;
      pointer-events: none;
      white-space: nowrap;
    }
    .toast-notice.show { transform: translateX(-50%) translateY(0); }
  </style>
</head>

<body>
  <div class="mobile-app-container">
    <main class="workbench-card" id="workbench-card">
      <header class="workbench-titlebar">
        <div class="titlebar-left">
          <div class="window-dots">
            <span class="window-square-dot dot-red"></span>
            <span class="window-square-dot dot-amber"></span>
            <span class="window-square-dot dot-green"></span>
          </div>
          <span class="window-title">HIMALIX PROJECTS</span>
        </div>
        <div class="titlebar-actions">
          <a href="/update" target="_blank" class="btn-ota" title="OTA Firmware Update (/update)">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
              <polyline points="17 8 12 3 7 8"></polyline>
              <line x1="12" y1="3" x2="12" y2="15"></line>
            </svg>
            <span>OTA</span>
          </a>
          <span class="status-pill" id="status-pill">
            <span class="status-pulse-dot"></span>
            <span>READY</span>
          </span>
          <button class="btn-theme-toggle" id="theme-toggle-btn" onclick="toggleTheme()" title="Toggle Light / Dark Mode">
            <svg id="theme-icon-moon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path>
            </svg>
            <svg id="theme-icon-sun" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" style="display:none;">
              <circle cx="12" cy="12" r="5"></circle>
              <line x1="12" y1="1" x2="12" y2="3"></line>
              <line x1="12" y1="21" x2="12" y2="23"></line>
              <line x1="4.22" y1="4.22" x2="5.64" y2="5.64"></line>
              <line x1="18.36" y1="18.36" x2="19.78" y2="19.78"></line>
              <line x1="1" y1="12" x2="3" y2="12"></line>
              <line x1="21" y1="12" x2="23" y2="12"></line>
              <line x1="4.22" y1="19.78" x2="5.64" y2="18.36"></line>
              <line x1="18.36" y1="5.64" x2="19.78" y2="4.22"></line>
            </svg>
            <span id="theme-toggle-label">DARK</span>
          </button>
        </div>
      </header>

      <div class="workbench-content">
        <!-- Hold-to-Run Tactile Buttons -->
        <div class="tactile-actions-grid">
          <button class="btn-tactile btn-tactile-deploy" id="btn-deploy" title="Hold to Deploy Claws (Release to Stop)">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <line x1="12" y1="19" x2="12" y2="5"></line>
              <polyline points="5 12 12 5 19 12"></polyline>
            </svg>
            <span>DEPLOY CLAWS</span>
          </button>
          <button class="btn-tactile btn-tactile-retract" id="btn-retract" title="Hold to Retract Claws (Release to Stop)">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <line x1="12" y1="5" x2="12" y2="19"></line>
              <polyline points="19 12 12 19 5 12"></polyline>
            </svg>
            <span>RETRACT CLAWS</span>
          </button>
        </div>

        <!-- Auto Demo & Emergency Stop -->
        <div class="actions-full-row">
          <button class="btn-auto-demo" onclick="triggerAutoDemo()">
            <svg viewBox="0 0 24 24" fill="currentColor">
              <polygon points="5 3 19 12 5 21 5 3" />
            </svg>
            <span>AUTO DEMO SEQUENCE</span>
          </button>
          <button class="btn-emergency-stop" onclick="triggerEmergencyStop()" title="Instant Hardware Motor Cutoff">
            <svg viewBox="0 0 24 24" fill="currentColor">
              <rect x="5" y="5" width="14" height="14" rx="2" />
            </svg>
            <span>EMERGENCY STOP</span>
          </button>
        </div>

        <!-- Motor Speed PWM Slider -->
        <div class="speed-control-box">
          <div class="speed-box-header">
            <span class="speed-box-title">MOTOR SPEED PWM (BTS7960)</span>
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

        <!-- Permanently Revealed Timing Calibration -->
        <div class="timing-calibration-box">
          <div class="timing-box-header">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" stroke-linejoin="round">
              <circle cx="12" cy="12" r="9"></circle>
              <polyline points="12 7 12 12 15 15"></polyline>
            </svg>
            <span>MOTION TIMING &amp; CALIBRATION</span>
          </div>
          <div class="timing-row">
            <div class="timing-row-labels">
              <span>Deploy Duration</span>
              <span class="timing-val-tag" id="deploy-time-label">3.0 s</span>
            </div>
            <input type="range" min="1000" max="8000" step="100" value="3000" class="custom-range-slider" id="deploy-slider"
              oninput="document.getElementById('deploy-time-label').textContent = (this.value/1000).toFixed(1) + ' s'">
          </div>
          <div class="timing-row">
            <div class="timing-row-labels">
              <span>Retract Duration</span>
              <span class="timing-val-tag" id="retract-time-label">3.0 s</span>
            </div>
            <input type="range" min="1000" max="8000" step="100" value="3000" class="custom-range-slider" id="retract-slider"
              oninput="document.getElementById('retract-time-label').textContent = (this.value/1000).toFixed(1) + ' s'">
          </div>
          <div class="timing-row">
            <div class="timing-row-labels">
              <span>Demo Hold Duration</span>
              <span class="timing-val-tag" id="hold-time-label">2.0 s</span>
            </div>
            <input type="range" min="1000" max="6000" step="100" value="2000" class="custom-range-slider" id="hold-slider"
              oninput="document.getElementById('hold-time-label').textContent = (this.value/1000).toFixed(1) + ' s'">
          </div>
          <button class="btn-save-calib" onclick="saveCalibration()">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <polyline points="20 6 9 17 4 12"></polyline>
            </svg>
            <span>APPLY CALIBRATION</span>
          </button>
        </div>
      </div>
    </main>
  </div>

  <div class="toast-notice" id="toast-notice">Settings Applied</div>

  <script>
    let targetPwm = 850;
    let deployDuration = 3000;
    let retractDuration = 3000;
    let holdDuration = 2000;

    let clawTravelMs = 0;
    let momentaryTimer = null;
    let activeHoldDirection = null;

    document.addEventListener('DOMContentLoaded', () => {
      initTheme();
      setupHoldToRunButtons();
      setupGlobalAntiSelect();
      setInterval(liveSyncStatus, 500);
    });

    function initTheme() {
      const savedTheme = localStorage.getItem('claw_theme') || 'light';
      applyTheme(savedTheme);
    }

    function toggleTheme() {
      const currentTheme = document.documentElement.getAttribute('data-theme') === 'dark' ? 'dark' : 'light';
      const newTheme = currentTheme === 'dark' ? 'light' : 'dark';
      applyTheme(newTheme);
      localStorage.setItem('claw_theme', newTheme);
    }

    function applyTheme(theme) {
      const label = document.getElementById('theme-toggle-label');
      const iconMoon = document.getElementById('theme-icon-moon');
      const iconSun = document.getElementById('theme-icon-sun');
      if (theme === 'dark') {
        document.documentElement.setAttribute('data-theme', 'dark');
        if (label) label.textContent = 'LIGHT';
        if (iconMoon) iconMoon.style.display = 'none';
        if (iconSun) iconSun.style.display = 'inline-block';
      } else {
        document.documentElement.removeAttribute('data-theme');
        if (label) label.textContent = 'DARK';
        if (iconMoon) iconMoon.style.display = 'inline-block';
        if (iconSun) iconSun.style.display = 'none';
      }
    }

    function setupGlobalAntiSelect() {
      window.addEventListener('contextmenu', (e) => e.preventDefault(), { capture: true });
      document.addEventListener('selectstart', (e) => {
        if (e.target.tagName !== 'INPUT') e.preventDefault();
      });
      document.addEventListener('dragstart', (e) => e.preventDefault());
      document.addEventListener('gesturestart', (e) => e.preventDefault(), { passive: false });
      document.addEventListener('gesturechange', (e) => e.preventDefault(), { passive: false });
      document.addEventListener('gestureend', (e) => e.preventDefault(), { passive: false });

      let lastTouchEnd = 0;
      document.addEventListener('touchend', (e) => {
        const now = Date.now();
        if (now - lastTouchEnd <= 300) e.preventDefault();
        lastTouchEnd = now;
      }, { passive: false });
    }

    function setupHoldToRunButtons() {
      bindMomentary('btn-deploy', 'deploy');
      bindMomentary('btn-retract', 'retract');

      window.addEventListener('mouseup', () => {
        if (activeHoldDirection) stopHold(activeHoldDirection);
      });
      window.addEventListener('touchend', (e) => {
        if (e.touches && e.touches.length === 0 && activeHoldDirection) {
          stopHold(activeHoldDirection);
        }
      });
      window.addEventListener('touchcancel', () => {
        if (activeHoldDirection) stopHold(activeHoldDirection);
      });
    }

    function bindMomentary(id, dir) {
      const btn = document.getElementById(id);
      if (!btn) return;

      const onStart = (e) => {
        if (e.cancelable) e.preventDefault();
        startHold(dir);
      };
      const onEnd = (e) => {
        if (e.cancelable) e.preventDefault();
        stopHold(dir);
      };

      btn.addEventListener('touchstart', onStart, { passive: false });
      btn.addEventListener('touchend', onEnd, { passive: false });
      btn.addEventListener('touchcancel', onEnd, { passive: false });
      btn.addEventListener('mousedown', onStart);
      btn.addEventListener('mouseup', onEnd);
      btn.addEventListener('mouseleave', onEnd);
      btn.addEventListener('contextmenu', (e) => { e.preventDefault(); return false; });
    }

    function startHold(dir) {
      if (activeHoldDirection === dir) return;
      activeHoldDirection = dir;

      const btn = document.getElementById(dir === 'deploy' ? 'btn-deploy' : 'btn-retract');
      if (btn) btn.classList.add('holding');

      fetch('/api/' + dir, { method: 'POST' }).catch(() => {});

      clearInterval(momentaryTimer);
      const stepMs = 50;
      momentaryTimer = setInterval(() => {
        if (dir === 'deploy') {
          clawTravelMs = Math.min(deployDuration, clawTravelMs + stepMs);
          const pct = Math.round((clawTravelMs / deployDuration) * 100);
          updateStatus(`DEPLOYING ${pct}%`, 'var(--pixel-red)', 'rgba(225, 29, 72, 0.15)');
          if (clawTravelMs >= deployDuration) {
            stopHold('deploy');
            updateStatus('DEPLOYED 100%', 'var(--pixel-red)', 'rgba(225, 29, 72, 0.2)');
          }
        } else {
          clawTravelMs = Math.max(0, clawTravelMs - stepMs);
          const pct = Math.round((clawTravelMs / deployDuration) * 100);
          updateStatus(`RETRACTING ${pct}%`, 'var(--pixel-blue)', 'rgba(37, 99, 235, 0.15)');
          if (clawTravelMs <= 0) {
            stopHold('retract');
            updateStatus('RETRACTED 0%', 'var(--pixel-blue)', 'rgba(37, 99, 235, 0.2)');
          }
        }
      }, stepMs);
    }

    function stopHold(dir) {
      if (activeHoldDirection !== dir) return;
      clearInterval(momentaryTimer);
      activeHoldDirection = null;

      const btn = document.getElementById(dir === 'deploy' ? 'btn-deploy' : 'btn-retract');
      if (btn) btn.classList.remove('holding');

      fetch('/api/stop', { method: 'POST' }).catch(() => {});
      const pct = Math.round((clawTravelMs / deployDuration) * 100);
      updateStatus(`PAUSED (${pct}%)`, 'var(--pixel-green)', 'rgba(16, 185, 129, 0.12)');
    }

    function triggerEmergencyStop() {
      clearInterval(momentaryTimer);
      activeHoldDirection = null;
      document.querySelectorAll('.btn-tactile').forEach(b => b.classList.remove('holding'));
      fetch('/api/stop', { method: 'POST' }).catch(() => {});
      updateStatus('STOPPED', 'var(--pixel-red)', 'rgba(225, 29, 72, 0.25)');
      showToast('EMERGENCY STOP');
    }

    function triggerAutoDemo() {
      clearInterval(momentaryTimer);
      activeHoldDirection = null;
      fetch('/api/demo', { method: 'POST' }).catch(() => {});
      showToast('AUTO DEMO RUNNING');
      updateStatus('DEMO RUNNING', 'var(--pixel-amber)', 'rgba(245, 158, 11, 0.15)');
    }

    function updateStatus(text, color, bg) {
      const pill = document.getElementById('status-pill');
      if (!pill) return;
      const label = pill.querySelector('span:last-child') || pill;
      label.textContent = text;
      if (color) pill.style.color = color;
      if (bg) pill.style.background = bg;
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

    function saveCalibration() {
      deployDuration = parseInt(document.getElementById('deploy-slider').value);
      retractDuration = parseInt(document.getElementById('retract-slider').value);
      holdDuration = parseInt(document.getElementById('hold-slider').value);
      fetch('/api/config?pwm=' + targetPwm + '&deploy=' + deployDuration + '&retract=' + retractDuration, { method: 'POST' })
        .then(() => showToast('CALIBRATION SAVED'))
        .catch(() => showToast('SAVED LOCALLY'));
    }

    async function liveSyncStatus() {
      try {
        const res = await fetch('/api/status');
        if (!res.ok) return;
        const data = await res.json();
        if (!activeHoldDirection && data.state) {
          updateStatus(data.state);
        }
      } catch (err) {}
    }

    function showToast(msg) {
      const toast = document.getElementById('toast-notice');
      if (!toast) return;
      toast.textContent = msg;
      toast.classList.add('show');
      setTimeout(() => toast.classList.remove('show'), 1800);
    }
  </script>
</body>
</html>
)rawliteral";
