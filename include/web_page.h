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
      --radius-lg: 12px;
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

    * { box-sizing: border-box; margin: 0; padding: 0; -webkit-font-smoothing: antialiased; -webkit-tap-highlight-color: transparent; user-select: none; touch-action: manipulation; }
    html, body { min-height: 100vh; width: 100%; }
    body {
      background-color: var(--canvas-bg);
      background-image: radial-gradient(circle, var(--grid-dot) 1.2px, transparent 1.2px);
      background-size: var(--grid-dot-size) var(--grid-dot-size);
      color: var(--text-main);
      font-family: var(--font-sans);
      line-height: 1.4;
      display: flex;
      justify-content: center;
      align-items: flex-start;
      padding: 12px 10px 32px;
      overflow-x: hidden;
      transition: background-color 0.2s ease;
    }

    .mobile-app-container { width: 100%; max-width: 480px; display: flex; flex-direction: column; margin: 0 auto; }
    .workbench-card {
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-lg);
      box-shadow: 0 8px 30px rgba(0, 0, 0, 0.05);
      overflow: hidden;
      display: flex;
      flex-direction: column;
    }

    .workbench-titlebar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 10px 14px;
      background: var(--surface-header);
      border-bottom: 1px solid var(--border-main);
    }
    .titlebar-left { display: flex; align-items: center; gap: 10px; }
    .window-dots { display: flex; align-items: center; gap: 4px; }
    .window-square-dot { width: 7px; height: 7px; border-radius: 1px; }
    .dot-red { background: #ef4444; }
    .dot-amber { background: #f59e0b; }
    .dot-green { background: #10b981; }

    .window-title { font-family: var(--font-dot); font-size: 12px; letter-spacing: 1.2px; text-transform: uppercase; color: var(--text-sub); }
    .titlebar-actions { display: flex; align-items: center; gap: 8px; }
    .status-pill {
      font-family: var(--font-mono);
      font-size: 10px;
      font-weight: 700;
      padding: 2px 7px;
      border-radius: 2px;
      background: rgba(16, 185, 129, 0.12);
      border: 1px solid rgba(16, 185, 129, 0.4);
      color: var(--pixel-green);
      text-transform: uppercase;
    }

    .btn-theme-toggle {
      background: transparent;
      border: 1px solid var(--border-main);
      padding: 3px 8px;
      border-radius: var(--radius-sm);
      font-family: var(--font-mono);
      font-size: 10px;
      font-weight: 700;
      color: var(--text-sub);
      cursor: pointer;
    }

    .workbench-content { padding: 14px 12px; display: flex; flex-direction: column; gap: 12px; }

    .mode-selector-row { display: grid; grid-template-columns: repeat(3, 1fr); gap: 8px; }
    .mode-card {
      display: flex;
      flex-direction: column;
      align-items: center;
      text-align: center;
      padding: 8px 6px;
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-sm);
      cursor: pointer;
      position: relative;
    }
    .mode-card.active { background: var(--surface-elevated); border: 2px solid var(--border-dark); box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05); }
    .mode-icon-box {
      width: 28px;
      height: 28px;
      background: rgba(0, 0, 0, 0.06);
      border: 1px solid var(--border-main);
      border-radius: 2px;
      display: flex;
      align-items: center;
      justify-content: center;
      margin-bottom: 4px;
      color: var(--text-main);
    }
    .mode-card.active .mode-icon-box { background: var(--btn-black-bg); border-color: var(--btn-black-bg); color: var(--btn-black-text); }
    .mode-card-title { font-family: var(--font-dot); font-size: 10px; letter-spacing: 0.5px; text-transform: uppercase; color: var(--text-main); line-height: 1.1; width: 100%; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
    .mode-active-indicator { position: absolute; top: 5px; right: 5px; width: 5px; height: 5px; background: var(--pixel-red); }

    .tactile-actions-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
    .btn-tactile {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-sm);
      padding: 16px 8px;
      color: var(--text-main);
      font-family: var(--font-mono);
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.5px;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 6px;
      cursor: pointer;
      min-height: 68px;
    }
    .btn-tactile:active { transform: scale(0.97); }
    .btn-tactile-deploy { border-left: 4px solid var(--pixel-red); }
    .btn-tactile-retract { border-left: 4px solid var(--pixel-blue); }
    .btn-tactile svg { width: 22px; height: 22px; stroke: currentColor; }

    .actions-full-row { display: grid; grid-template-columns: 1fr 1.25fr; gap: 8px; }
    .btn-auto-demo {
      background: var(--surface-elevated);
      border: 1px solid var(--border-dark);
      border-radius: var(--radius-sm);
      padding: 12px 8px;
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
    .btn-auto-demo:active { transform: scale(0.97); }

    .btn-emergency-stop {
      background: var(--pixel-red);
      border: 1px solid var(--pixel-red);
      border-radius: var(--radius-sm);
      padding: 12px 8px;
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
      box-shadow: 0 4px 12px rgba(225, 29, 72, 0.25);
    }
    .btn-emergency-stop:active { transform: scale(0.96); opacity: 0.9; }

    .speed-control-box {
      background: var(--surface-elevated);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-sm);
      padding: 12px 14px;
      display: flex;
      flex-direction: column;
      gap: 10px;
    }
    .speed-box-header { display: flex; justify-content: space-between; align-items: center; }
    .speed-box-title { font-family: var(--font-mono); font-size: 10.5px; font-weight: 700; letter-spacing: 0.8px; text-transform: uppercase; color: var(--text-sub); }
    .speed-box-readout { font-family: var(--font-mono); font-size: 12px; font-weight: 700; color: var(--text-main); }

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
    }

    .speed-presets-row { display: grid; grid-template-columns: repeat(4, 1fr); gap: 6px; }
    .btn-speed-preset {
      background: var(--surface-card);
      border: 1px solid var(--border-main);
      border-radius: var(--radius-sm);
      padding: 8px 4px;
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
          <span class="status-pill" id="status-pill">READY</span>
          <button class="btn-theme-toggle" id="theme-toggle-btn" onclick="toggleTheme()">
            <span id="theme-toggle-label">&amp; DARK</span>
          </button>
        </div>
      </header>

      <div class="workbench-content">
        <!-- 3 Mode Cards -->
        <div class="mode-selector-row">
          <div class="mode-card active" id="mode-card-ota" onclick="showToast('Himalix Projects OTA')">
            <div class="mode-icon-box">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <path d="M4 8h16M4 12h12M4 16h16" />
              </svg>
            </div>
            <div class="mode-card-title">HIMALIX OTA</div>
            <span class="mode-active-indicator"></span>
          </div>

          <div class="mode-card" id="mode-card-firmware" onclick="window.open('/update','_blank')">
            <div class="mode-icon-box">
              <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
                <polyline points="17 8 12 3 7 8"></polyline>
                <line x1="12" y1="3" x2="12" y2="15"></line>
              </svg>
            </div>
            <div class="mode-card-title">CUSTOM (.BIN)</div>
          </div>

          <div class="mode-card" id="mode-card-tactile" onclick="showToast('Tactile Claw Actuator')">
            <div class="mode-icon-box">
              <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2">
                <circle cx="12" cy="12" r="3" />
                <path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83" />
              </svg>
            </div>
            <div class="mode-card-title">TACTILE CLAW</div>
          </div>
        </div>

        <!-- Primary Actuation Controls (No Motor 1 / Motor 2 labels) -->
        <div class="tactile-actions-grid">
          <button class="btn-tactile btn-tactile-deploy" onclick="triggerCommand('deploy')">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
              <line x1="12" y1="19" x2="12" y2="5"></line>
              <polyline points="5 12 12 5 19 12"></polyline>
            </svg>
            <span>DEPLOY CLAWS</span>
          </button>
          <button class="btn-tactile btn-tactile-retract" onclick="triggerCommand('retract')">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5">
              <line x1="12" y1="5" x2="12" y2="19"></line>
              <polyline points="19 12 12 19 5 12"></polyline>
            </svg>
            <span>RETRACT CLAWS</span>
          </button>
        </div>

        <!-- Auto Demo & Emergency Stop -->
        <div class="actions-full-row">
          <button class="btn-auto-demo" onclick="triggerCommand('demo')">AUTO DEMO SEQUENCE</button>
          <button class="btn-emergency-stop" onclick="triggerCommand('stop')">EMERGENCY STOP</button>
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

      </div>
    </main>
  </div>

  <div class="toast-notice" id="toast-notice">Settings Applied</div>

  <script>
    let targetPwm = 850;

    function toggleTheme() {
      const isDark = document.documentElement.getAttribute('data-theme') === 'dark';
      if (isDark) {
        document.documentElement.removeAttribute('data-theme');
        document.getElementById('theme-toggle-label').textContent = '& DARK';
      } else {
        document.documentElement.setAttribute('data-theme', 'dark');
        document.getElementById('theme-toggle-label').textContent = '☀ LIGHT';
      }
    }

    function showToast(msg) {
      const toast = document.getElementById('toast-notice');
      toast.textContent = msg;
      toast.classList.add('show');
      setTimeout(() => toast.classList.remove('show'), 1800);
    }

    function triggerCommand(cmd) {
      fetch('/api/' + cmd, { method: 'POST' }).catch(() => {});
      showToast(cmd.toUpperCase());
      const pill = document.getElementById('status-pill');
      if (pill) {
        pill.textContent = cmd.toUpperCase();
        if (cmd === 'stop') {
          pill.style.color = 'var(--pixel-red)';
          pill.style.background = 'rgba(225, 29, 72, 0.2)';
        }
      }
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
        const pill = document.getElementById('status-pill');
        if (pill) {
          pill.textContent = data.state;
          if (data.state.includes('DEPLOY')) pill.style.color = 'var(--pixel-red)';
          else if (data.state.includes('RETRACT')) pill.style.color = 'var(--pixel-blue)';
          else if (data.state.includes('STOP')) pill.style.color = 'var(--pixel-red)';
          else pill.style.color = 'var(--pixel-green)';
        }
      } catch (err) {}
    }

    setInterval(pollStatus, 500);
  </script>
</body>
</html>
)rawliteral";
