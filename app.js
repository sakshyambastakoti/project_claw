/**
 * Project CLAW / HAWA Web Flasher — Controller Logic & Web Serial API
 */

// Application State
let currentState = 'IDLE';
let targetPwm = 850;
let deployDuration = 3000;
let retractDuration = 3000;
let holdDuration = 2000;
let remainingMs = 0;
let totalDurationMs = 0;
let motionTimer = null;

// Serial Communication Variables
let serialPort = null;
let serialReader = null;
let serialWriter = null;
let isSerialConnected = false;

// DOM Initialization
document.addEventListener('DOMContentLoaded', () => {
  initTheme();
  setupEventListeners();
  appendTerminalLine('System initialized. Ready for USB Web Serial or Wi-Fi AP.', 'info');
  setInterval(liveSyncStatus, 500);
});

// ==========================================
// Theme Management (Light Sand / Dark Terminal)
// ==========================================
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
  if (!label) return;
  if (theme === 'dark') {
    document.documentElement.setAttribute('data-theme', 'dark');
    label.textContent = '☀ LIGHT';
  } else {
    document.documentElement.removeAttribute('data-theme');
    label.textContent = '& DARK';
  }
}

// ==========================================
// Mode Card Selection
// ==========================================
function selectMode(modeName) {
  document.querySelectorAll('.mode-card').forEach(card => card.classList.remove('active'));
  document.querySelectorAll('.workbench-panel').forEach(panel => panel.classList.remove('active'));

  const targetCard = document.getElementById(`mode-card-${modeName}`);
  const targetPanel = document.getElementById(`panel-${modeName}`);

  if (targetCard) targetCard.classList.add('active');
  if (targetPanel) targetPanel.classList.add('active');

  showToast(`Active Mode: ${modeName.toUpperCase()}`);
}

// Window Expand / Fullscreen
function toggleExpandWindow() {
  const card = document.getElementById('workbench-card');
  card.classList.toggle('expanded');
  const isExp = card.classList.contains('expanded');
  document.getElementById('expand-btn-text').textContent = isExp ? 'RESTORE' : 'EXPAND WINDOW';
}

// ==========================================
// Motion Control & Telemetry Simulation
// ==========================================
function triggerCommand(cmd) {
  if (cmd === 'stop') {
    clearInterval(motionTimer);
    updateUIState('STOPPED', 0, 0);
    showToast('EMERGENCY STOPPED');
    sendSerialOrApi('/api/stop', 'STOP\n');
    appendTerminalLine('[ACTUATION] Emergency Stop triggered.', 'error');
    return;
  }

  if (cmd === 'deploy') {
    sendSerialOrApi('/api/deploy', 'DEPLOY\n');
    appendTerminalLine(`[ACTUATION] Deploying Claws (Motor 1 Red Cable, PWM ${targetPwm})`, 'info');
    startMotionSimulation('DEPLOYING', deployDuration, () => {
      updateUIState('DEPLOYED', 0, 0);
      appendTerminalLine('[ACTUATION] Deploy completed.', 'success');
    });
    showToast('Deploying Claws (Motor 1)...');
  } else if (cmd === 'retract') {
    sendSerialOrApi('/api/retract', 'RETRACT\n');
    appendTerminalLine(`[ACTUATION] Retracting Claws (Motor 2 Blue Cable, PWM ${targetPwm})`, 'info');
    startMotionSimulation('RETRACTING', retractDuration, () => {
      updateUIState('RETRACTED', 0, 0);
      appendTerminalLine('[ACTUATION] Retract completed.', 'success');
    });
    showToast('Retracting Claws (Motor 2)...');
  } else if (cmd === 'demo') {
    sendSerialOrApi('/api/demo', 'DEMO\n');
    appendTerminalLine('[DEMO] Starting Full Demo Sequence...', 'info');
    showToast('Starting Auto Demo Sequence...');
    startMotionSimulation('DEMO (DEPLOYING)', deployDuration, () => {
      updateUIState('DEMO (HOLDING)', 0, 0);
      appendTerminalLine(`[DEMO] Holding open for ${(holdDuration / 1000).toFixed(1)}s...`, 'info');
      setTimeout(() => {
        startMotionSimulation('DEMO (RETRACTING)', retractDuration, () => {
          updateUIState('IDLE / READY', 0, 0);
          appendTerminalLine('[DEMO] Sequence completed successfully.', 'success');
          showToast('Auto Demo Sequence Complete!');
        });
      }, holdDuration);
    });
  }
}

function triggerJog(dir) {
  const jogMs = 500;
  if (dir === 'deploy') {
    sendSerialOrApi('/api/deploy', 'JOG_DEPLOY\n');
    startMotionSimulation('DEPLOYING', jogMs, () => updateUIState('IDLE / READY', 0, 0));
    appendTerminalLine('[JOG] Motor 1 forward 500ms.', 'info');
    setTimeout(() => sendSerialOrApi('/api/stop', 'STOP\n'), jogMs);
  } else {
    sendSerialOrApi('/api/retract', 'JOG_RETRACT\n');
    startMotionSimulation('RETRACTING', jogMs, () => updateUIState('IDLE / READY', 0, 0));
    appendTerminalLine('[JOG] Motor 2 forward 500ms.', 'info');
    setTimeout(() => sendSerialOrApi('/api/stop', 'STOP\n'), jogMs);
  }
  showToast(`Jogging ${dir.toUpperCase()} (0.5s)`);
}

function startMotionSimulation(stateName, duration, onComplete) {
  clearInterval(motionTimer);
  currentState = stateName;
  totalDurationMs = duration;
  remainingMs = duration;

  const isM1 = stateName.includes('DEPLOY');
  const isM2 = stateName.includes('RETRACT');
  updateUIState(stateName, isM1 ? targetPwm : 0, isM2 ? targetPwm : 0);

  const stepMs = 50;
  motionTimer = setInterval(() => {
    remainingMs -= stepMs;
    const pBar = document.getElementById('progress-bar');
    if (remainingMs <= 0) {
      clearInterval(motionTimer);
      if (pBar) pBar.style.width = '0%';
      if (onComplete) onComplete();
    } else {
      if (pBar) {
        const pct = ((totalDurationMs - remainingMs) / totalDurationMs) * 100;
        pBar.style.width = `${pct}%`;
      }
    }
  }, stepMs);
}

function updateUIState(stateName, m1, m2) {
  const pill = document.getElementById('status-pill');
  if (pill) {
    pill.textContent = stateName;
    if (stateName.includes('DEPLOY')) {
      pill.style.color = 'var(--pixel-red)';
      pill.style.borderColor = 'rgba(225, 29, 72, 0.4)';
      pill.style.background = 'rgba(225, 29, 72, 0.12)';
    } else if (stateName.includes('RETRACT')) {
      pill.style.color = 'var(--pixel-blue)';
      pill.style.borderColor = 'rgba(37, 99, 235, 0.4)';
      pill.style.background = 'rgba(37, 99, 235, 0.12)';
    } else if (stateName.includes('STOP')) {
      pill.style.color = 'var(--pixel-red)';
      pill.style.borderColor = 'var(--pixel-red)';
      pill.style.background = 'rgba(225, 29, 72, 0.2)';
    } else {
      pill.style.color = 'var(--pixel-green)';
      pill.style.borderColor = 'rgba(16, 185, 129, 0.4)';
      pill.style.background = 'rgba(16, 185, 129, 0.12)';
    }
  }

  const badge = document.getElementById('state-badge');
  const text = document.getElementById('state-text');
  if (text) text.textContent = stateName;

  if (badge) {
    if (stateName.includes('DEPLOY')) {
      badge.style.color = 'var(--pixel-red)';
      badge.style.borderColor = 'var(--pixel-red)';
      badge.style.background = 'rgba(225, 29, 72, 0.1)';
    } else if (stateName.includes('RETRACT')) {
      badge.style.color = 'var(--pixel-blue)';
      badge.style.borderColor = 'var(--pixel-blue)';
      badge.style.background = 'rgba(37, 99, 235, 0.1)';
    } else if (stateName.includes('STOP')) {
      badge.style.color = 'var(--pixel-red)';
      badge.style.borderColor = 'var(--pixel-red)';
      badge.style.background = 'rgba(225, 29, 72, 0.15)';
    } else {
      badge.style.color = 'var(--pixel-green)';
      badge.style.borderColor = 'var(--pixel-green)';
      badge.style.background = 'rgba(16, 185, 129, 0.1)';
    }
  }

  // Optional Motor Readouts (null-checked)
  const m1State = document.getElementById('m1-badge-state');
  const m1Val = document.getElementById('m1-val');
  if (m1State) {
    m1State.textContent = m1 > 0 ? 'ACTIVE' : 'OFF';
    m1State.style.color = m1 > 0 ? 'var(--pixel-red)' : 'var(--text-dim)';
  }
  if (m1Val) {
    m1Val.textContent = m1 > 0 ? `${Math.round((m1 / 1023) * 100)}% (${m1} PWM)` : '0% (0 PWM)';
  }

  const m2State = document.getElementById('m2-badge-state');
  const m2Val = document.getElementById('m2-val');
  if (m2State) {
    m2State.textContent = m2 > 0 ? 'ACTIVE' : 'OFF';
    m2State.style.color = m2 > 0 ? 'var(--pixel-blue)' : 'var(--text-dim)';
  }
  if (m2Val) {
    m2Val.textContent = m2 > 0 ? `${Math.round((m2 / 1023) * 100)}% (${m2} PWM)` : '0% (0 PWM)';
  }
}

// ==========================================
// Speed & Calibration Controls
// ==========================================
function onSpeedSliderChange(val) {
  targetPwm = parseInt(val);
  const pct = Math.round((targetPwm / 1023) * 100);
  document.getElementById('speed-val-display').textContent = `${pct}% (${targetPwm} PWM)`;

  document.querySelectorAll('.btn-speed-preset').forEach(btn => btn.classList.remove('active'));
}

function setSpeedPreset(val, btnElement) {
  document.getElementById('speed-slider').value = val;
  onSpeedSliderChange(val);
  document.querySelectorAll('.btn-speed-preset').forEach(btn => btn.classList.remove('active'));
  if (btnElement) btnElement.classList.add('active');

  sendSerialOrApi(`/api/config?pwm=${val}`, `SET_PWM=${val}\n`);
}

function toggleAccordion(id) {
  document.getElementById(id).classList.toggle('open');
}

function saveCalibration() {
  deployDuration = parseInt(document.getElementById('deploy-slider').value);
  retractDuration = parseInt(document.getElementById('retract-slider').value);
  holdDuration = parseInt(document.getElementById('hold-slider').value);

  sendSerialOrApi(
    `/api/config?pwm=${targetPwm}&deploy=${deployDuration}&retract=${retractDuration}&hold=${holdDuration}`,
    `CALIB:${targetPwm},${deployDuration},${retractDuration},${holdDuration}\n`
  );
  showToast('Calibration Applied Successfully!');
  appendTerminalLine(`[CONFIG] Applied Calibration: Deploy=${deployDuration}ms, Retract=${retractDuration}ms, Hold=${holdDuration}ms`, 'success');
}

// ==========================================
// Web Serial API Connection
// ==========================================
async function connectWebSerial() {
  if (!('serial' in navigator)) {
    alert('Web Serial API is not supported in this browser. Please use Chrome, Edge, or Opera.');
    return;
  }

  try {
    const baudRate = parseInt(document.getElementById('baud-rate-select')?.value || 115200);
    appendTerminalLine('Requesting Web Serial port...', 'info');

    serialPort = await navigator.serial.requestPort();
    await serialPort.open({ baudRate: baudRate });

    isSerialConnected = true;
    updateConnectionStatusUI(true, 'USB SERIAL CONNECTED');
    appendTerminalLine(`Connected to USB Serial at ${baudRate} baud.`, 'success');
    showToast('USB Web Serial Connected!');

    readSerialStream();
  } catch (err) {
    appendTerminalLine(`Serial connection failed: ${err.message}`, 'error');
  }
}

async function readSerialStream() {
  const decoder = new TextDecoderStream();
  serialPort.readable.pipeTo(decoder.writable);
  serialReader = decoder.readable.getReader();

  try {
    while (true) {
      const { value, done } = await serialReader.read();
      if (done) break;
      if (value) {
        appendTerminalLine(value.trim(), 'info');
      }
    }
  } catch (err) {
    appendTerminalLine(`Serial read error: ${err.message}`, 'error');
  }
}

async function writeSerial(text) {
  if (!serialPort || !serialPort.writable) return;
  const encoder = new TextEncoder();
  const writer = serialPort.writable.getWriter();
  await writer.write(encoder.encode(text));
  writer.releaseLock();
}

function sendSerialOrApi(apiEndpoint, serialCmd) {
  if (isSerialConnected && serialCmd) {
    writeSerial(serialCmd).catch(() => {});
  }
  fetch(apiEndpoint, { method: 'POST' }).catch(() => {});
}

function updateConnectionStatusUI(connected, label) {
  const dot = document.getElementById('device-connect-btn');
  if (dot) {
    dot.textContent = connected ? `✓ ${label}` : 'CONNECT DEVICE';
  }
}

// ==========================================
// Live Telemetry Sync with ESP8266
// ==========================================
async function liveSyncStatus() {
  try {
    const res = await fetch('/api/status');
    if (!res.ok) return;
    const data = await res.json();
    updateUIState(data.state, data.m1_pwm, data.m2_pwm);
  } catch (err) {
    // In standalone preview mode, local simulation runs
  }
}

// ==========================================
// Helpers
// ==========================================
function appendTerminalLine(text, type = 'info') {
  const term = document.getElementById('serial-terminal');
  if (!term) return;
  const line = document.createElement('div');
  line.className = `terminal-line ${type}`;
  const time = new Date().toLocaleTimeString();
  line.textContent = `[${time}] ${text}`;
  term.appendChild(line);
  term.scrollTop = term.scrollHeight;
}

function showToast(msg) {
  const toast = document.getElementById('toast-notice');
  if (!toast) return;
  toast.textContent = msg;
  toast.classList.add('show');
  setTimeout(() => toast.classList.remove('show'), 2200);
}

function setupEventListeners() {
  // Drag & drop firmware file
  const dropzone = document.getElementById('firmware-dropzone');
  const fileInput = document.getElementById('firmware-file-input');

  if (dropzone && fileInput) {
    dropzone.addEventListener('click', () => fileInput.click());
    fileInput.addEventListener('change', (e) => {
      if (e.target.files.length > 0) {
        const file = e.target.files[0];
        appendTerminalLine(`Loaded firmware binary: ${file.name} (${(file.size / 1024).toFixed(1)} KB)`, 'success');
        showToast(`Loaded ${file.name}`);
      }
    });
  }
}
