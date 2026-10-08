#pragma once
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>PROJECT CLAW — Animatronic Control Deck</title>
  <style>
    :root {
      --bg-dark: #0a0d14;
      --card-bg: rgba(18, 24, 38, 0.85);
      --card-border: rgba(64, 128, 255, 0.2);
      --accent-cyan: #00f0ff;
      --accent-red: #ff2a5f;
      --accent-green: #00ff88;
      --accent-orange: #ffaa00;
      --text-main: #f0f4fc;
      --text-dim: #7e8c9f;
      --radius: 14px;
      --glow: 0 0 20px rgba(0, 240, 255, 0.35);
      --glow-red: 0 0 25px rgba(255, 42, 95, 0.6);
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
      user-select: none;
      -webkit-tap-highlight-color: transparent;
    }

    body {
      background: radial-gradient(circle at 50% 10%, #151f38, var(--bg-dark) 90%);
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 16px 12px 30px;
    }

    .container {
      width: 100%;
      max-width: 480px;
      display: flex;
      flex-direction: column;
      gap: 16px;
    }

    header {
      text-align: center;
      padding: 12px 0 6px;
    }

    .logo-badge {
      display: inline-block;
      font-size: 11px;
      letter-spacing: 3px;
      text-transform: uppercase;
      color: var(--accent-cyan);
      background: rgba(0, 240, 255, 0.1);
      padding: 4px 12px;
      border-radius: 20px;
      border: 1px solid rgba(0, 240, 255, 0.3);
      margin-bottom: 6px;
    }

    h1 {
      font-size: 26px;
      font-weight: 800;
      letter-spacing: 1.5px;
      background: linear-gradient(135deg, #fff, var(--accent-cyan));
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }

    .subtitle {
      font-size: 12px;
      color: var(--text-dim);
      margin-top: 2px;
    }

    .card {
      background: var(--card-bg);
      backdrop-filter: blur(16px);
      -webkit-backdrop-filter: blur(16px);
      border: 1px solid var(--card-border);
      border-radius: var(--radius);
      padding: 18px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.4);
    }

    .status-panel {
      display: flex;
      flex-direction: column;
      gap: 12px;
    }

    .status-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .state-indicator {
      display: inline-flex;
      align-items: center;
      gap: 8px;
      font-size: 14px;
      font-weight: 700;
      letter-spacing: 1px;
      text-transform: uppercase;
      padding: 6px 14px;
      border-radius: 30px;
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid rgba(255, 255, 255, 0.1);
    }

    .status-dot {
      width: 10px;
      height: 10px;
      border-radius: 50%;
      background: var(--accent-cyan);
      box-shadow: var(--glow);
      animation: pulseDot 1.6s infinite ease-in-out;
    }

    @keyframes pulseDot {
      0%, 100% { transform: scale(1); opacity: 1; }
      50% { transform: scale(1.3); opacity: 0.6; }
    }

    .progress-bar-bg {
      width: 100%;
      height: 8px;
      background: rgba(255, 255, 255, 0.08);
      border-radius: 6px;
      overflow: hidden;
      position: relative;
    }

    .progress-bar-fill {
      width: 0%;
      height: 100%;
      background: linear-gradient(90deg, var(--accent-cyan), var(--accent-green));
      transition: width 0.15s linear;
    }

    .telemetry-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 10px;
      margin-top: 4px;
    }

    .telemetry-item {
      background: rgba(0, 0, 0, 0.35);
      padding: 10px;
      border-radius: 10px;
      border: 1px solid rgba(255, 255, 255, 0.05);
    }

    .telemetry-label {
      font-size: 11px;
      color: var(--text-dim);
      text-transform: uppercase;
      margin-bottom: 4px;
    }

    .telemetry-val {
      font-size: 15px;
      font-weight: 700;
      color: #fff;
    }

    .btn-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
    }

    .btn {
      position: relative;
      border: none;
      outline: none;
      padding: 16px 12px;
      border-radius: var(--radius);
      font-size: 15px;
      font-weight: 700;
      letter-spacing: 0.5px;
      cursor: pointer;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 6px;
      transition: all 0.15s ease;
      overflow: hidden;
    }

    .btn:active {
      transform: scale(0.97);
    }

    .btn-icon {
      font-size: 22px;
    }

    .btn-deploy {
      background: linear-gradient(145deg, #0e5b8a, #008cb3);
      color: #fff;
      border: 1px solid rgba(0, 240, 255, 0.4);
      box-shadow: 0 4px 15px rgba(0, 180, 220, 0.25);
    }

    .btn-deploy:hover {
      box-shadow: var(--glow);
    }

    .btn-retract {
      background: linear-gradient(145deg, #184877, #243b6b);
      color: #fff;
      border: 1px solid rgba(64, 128, 255, 0.4);
    }

    .btn-demo {
      grid-column: span 2;
      background: linear-gradient(145deg, #2b1f5e, #4c2f9d);
      color: #fff;
      border: 1px solid rgba(160, 90, 255, 0.4);
      box-shadow: 0 4px 15px rgba(120, 50, 220, 0.2);
    }

    .btn-stop {
      grid-column: span 2;
      background: linear-gradient(145deg, #8a0c28, #bd133a);
      color: #fff;
      font-size: 18px;
      padding: 18px;
      border: 1px solid rgba(255, 42, 95, 0.8);
      box-shadow: var(--glow-red);
      animation: stopPulse 2s infinite ease-in-out;
    }

    @keyframes stopPulse {
      0%, 100% { box-shadow: 0 0 15px rgba(255, 42, 95, 0.5); }
      50% { box-shadow: 0 0 30px rgba(255, 42, 95, 0.85); }
    }

    .settings-title {
      font-size: 14px;
      font-weight: 700;
      letter-spacing: 1px;
      text-transform: uppercase;
      color: var(--accent-cyan);
      margin-bottom: 12px;
      display: flex;
      align-items: center;
      gap: 6px;
    }

    .slider-row {
      margin-bottom: 14px;
    }

    .slider-header {
      display: flex;
      justify-content: space-between;
      font-size: 12px;
      color: var(--text-dim);
      margin-bottom: 6px;
    }

    .slider-val {
      font-weight: 700;
      color: var(--text-main);
    }

    input[type=range] {
      width: 100%;
      height: 6px;
      background: rgba(255, 255, 255, 0.15);
      border-radius: 4px;
      outline: none;
      -webkit-appearance: none;
    }

    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 18px;
      height: 18px;
      border-radius: 50%;
      background: var(--accent-cyan);
      box-shadow: var(--glow);
      cursor: pointer;
    }

    .save-btn {
      width: 100%;
      background: rgba(255, 255, 255, 0.08);
      border: 1px solid rgba(255, 255, 255, 0.2);
      color: #fff;
      padding: 10px;
      border-radius: 10px;
      font-weight: 600;
      font-size: 13px;
      cursor: pointer;
      transition: background 0.2s;
    }

    .save-btn:hover {
      background: rgba(0, 240, 255, 0.2);
      border-color: var(--accent-cyan);
    }

    footer {
      text-align: center;
      font-size: 11px;
      color: var(--text-dim);
      margin-top: 10px;
    }
  </style>
</head>
<body>

  <div class="container">
    <header>
      <div class="logo-badge">NodeMCU &bull; BTS7960</div>
      <h1>PROJECT CLAW</h1>
      <p class="subtitle">Animatronic Six-Claw Dual Cable Controller</p>
    </header>

    <!-- LIVE TELEMETRY & STATE CARD -->
    <div class="card status-panel">
      <div class="status-header">
        <span style="font-size:12px; color:var(--text-dim); text-transform:uppercase;">System State</span>
        <div class="state-indicator" id="stateBadge">
          <div class="status-dot" id="statusDot"></div>
          <span id="stateText">IDLE</span>
        </div>
      </div>

      <div class="progress-bar-bg">
        <div class="progress-bar-fill" id="progressBar"></div>
      </div>

      <div class="telemetry-grid">
        <div class="telemetry-item">
          <div class="telemetry-label">Motor 1 (Deploy)</div>
          <div class="telemetry-val" id="m1Status">OFF (0%)</div>
        </div>
        <div class="telemetry-item">
          <div class="telemetry-label">Motor 2 (Retract)</div>
          <div class="telemetry-val" id="m2Status">OFF (0%)</div>
        </div>
        <div class="telemetry-item">
          <div class="telemetry-label">Time Remaining</div>
          <div class="telemetry-val" id="timeRemaining">0.0 s</div>
        </div>
        <div class="telemetry-item">
          <div class="telemetry-label">Target PWM</div>
          <div class="telemetry-val" id="pwmDisplay">850 / 1023</div>
        </div>
      </div>
    </div>

    <!-- MAIN CONTROL BUTTONS -->
    <div class="btn-grid">
      <button class="btn btn-deploy" onclick="sendCommand('/api/deploy')">
        <span class="btn-icon">⚡</span>
        <span>DEPLOY</span>
      </button>

      <button class="btn btn-retract" onclick="sendCommand('/api/retract')">
        <span class="btn-icon">🔄</span>
        <span>RETRACT</span>
      </button>

      <button class="btn btn-demo" onclick="sendCommand('/api/demo')">
        <span class="btn-icon">🎭</span>
        <span>FULL DEMO SEQUENCE</span>
      </button>

      <button class="btn btn-stop" onclick="sendCommand('/api/stop')">
        <span class="btn-icon">🛑</span>
        <span>EMERGENCY STOP</span>
      </button>
    </div>

    <!-- TIMING & SPEED TUNING -->
    <div class="card">
      <div class="settings-title">⚙ Configuration & Tuning</div>

      <div class="slider-row">
        <div class="slider-header">
          <span>Motor Power (PWM)</span>
          <span class="slider-val" id="powerVal">83% (850)</span>
        </div>
        <input type="range" id="powerSlider" min="300" max="1023" value="850" oninput="updateSliderLabels()">
      </div>

      <div class="slider-row">
        <div class="slider-header">
          <span>Deploy Duration</span>
          <span class="slider-val" id="deployTimeVal">3.0 s</span>
        </div>
        <input type="range" id="deploySlider" min="1000" max="8000" step="100" value="3000" oninput="updateSliderLabels()">
      </div>

      <div class="slider-row">
        <div class="slider-header">
          <span>Retract Duration</span>
          <span class="slider-val" id="retractTimeVal">3.0 s</span>
        </div>
        <input type="range" id="retractSlider" min="1000" max="8000" step="100" value="3000" oninput="updateSliderLabels()">
      </div>

      <button class="save-btn" onclick="saveSettings()">Apply Configuration</button>
    </div>

    <footer>
      Project CLAW &bull; ESP8266 Animatronic Firmware
    </footer>
  </div>

  <script>
    let isDragging = false;

    function updateSliderLabels() {
      const p = document.getElementById('powerSlider').value;
      const d = document.getElementById('deploySlider').value;
      const r = document.getElementById('retractSlider').value;
      
      document.getElementById('powerVal').textContent = Math.round((p/1023)*100) + '% (' + p + ')';
      document.getElementById('deployTimeVal').textContent = (d/1000).toFixed(1) + ' s';
      document.getElementById('retractTimeVal').textContent = (r/1000).toFixed(1) + ' s';
    }

    async function sendCommand(endpoint) {
      try {
        await fetch(endpoint, { method: 'POST' });
        pollStatus();
      } catch (err) {
        console.error('Command failed:', err);
      }
    }

    async function saveSettings() {
      const pwm = document.getElementById('powerSlider').value;
      const deployTime = document.getElementById('deploySlider').value;
      const retractTime = document.getElementById('retractSlider').value;
      
      try {
        await fetch(`/api/config?pwm=${pwm}&deploy=${deployTime}&retract=${retractTime}`, { method: 'POST' });
        alert('Settings updated successfully!');
      } catch (err) {
        alert('Failed to save settings.');
      }
    }

    async function pollStatus() {
      try {
        const res = await fetch('/api/status');
        if (!res.ok) return;
        const data = await res.json();

        // Update State
        const stateEl = document.getElementById('stateText');
        const badgeEl = document.getElementById('stateBadge');
        const dotEl = document.getElementById('statusDot');
        stateEl.textContent = data.state;

        if (data.state === 'DEPLOYING') {
          dotEl.style.background = '#00f0ff';
          badgeEl.style.borderColor = 'rgba(0, 240, 255, 0.6)';
        } else if (data.state === 'RETRACTING') {
          dotEl.style.background = '#4080ff';
          badgeEl.style.borderColor = 'rgba(64, 128, 255, 0.6)';
        } else if (data.state === 'DEMO') {
          dotEl.style.background = '#a05aff';
          badgeEl.style.borderColor = 'rgba(160, 90, 255, 0.6)';
        } else if (data.state === 'STOPPED') {
          dotEl.style.background = '#ff2a5f';
          badgeEl.style.borderColor = 'rgba(255, 42, 95, 0.6)';
        } else {
          dotEl.style.background = '#00ff88';
          badgeEl.style.borderColor = 'rgba(0, 255, 136, 0.4)';
        }

        // Motors status
        document.getElementById('m1Status').textContent = data.m1_pwm > 0 ? `RUNNING (${Math.round((data.m1_pwm/1023)*100)}%)` : 'OFF (0%)';
        document.getElementById('m2Status').textContent = data.m2_pwm > 0 ? `RUNNING (${Math.round((data.m2_pwm/1023)*100)}%)` : 'OFF (0%)';

        // Timer & Progress
        if (data.total_duration > 0 && data.remaining > 0) {
          const remSec = (data.remaining / 1000).toFixed(1);
          document.getElementById('timeRemaining').textContent = remSec + ' s';
          const pct = Math.max(0, Math.min(100, ((data.total_duration - data.remaining) / data.total_duration) * 100));
          document.getElementById('progressBar').style.width = pct + '%';
        } else {
          document.getElementById('timeRemaining').textContent = '0.0 s';
          document.getElementById('progressBar').style.width = '0%';
        }

        document.getElementById('pwmDisplay').textContent = `${data.target_pwm} / 1023`;

      } catch (err) {
        // Heartbeat error ignore
      }
    }

    // Initial load
    updateSliderLabels();
    setInterval(pollStatus, 400);
  </script>
</body>
</html>
)rawliteral";
