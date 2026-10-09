/**
 * Project CLAW / Himalix Projects — Precision Mobile Hardware Controller
 * Automated 6-Step Sequence with Mathematical Homing & Flash EEPROM Persistence
 */

// Application State
let cycleEnabled = false;
let isHoming = false;
let activeStep = 0;
let rotationTime = 3000; // Parameter x (ms)
let pauseTime = 2000;    // Parameter y (ms)
let motorSpeed = 850;    // PWM (200 - 1023)

let isUserAdjustingSlider = false;
let sliderSaveTimeout = null;

// Serial Communication Variables (Web Serial API fallback)
let serialPort = null;
let serialReader = null;
let isSerialConnected = false;

// DOM Initialization
document.addEventListener('DOMContentLoaded', () => {
  initTheme();
  setupGlobalAntiSelect();
  setTimeout(() => {
    liveSyncStatus();
    setInterval(liveSyncStatus, 750);
  }, 250);
});

// ==========================================
// Theme Management (Pure SVG Icons, No Emojis)
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

// ==========================================
// Anti-Select / Anti-Zoom / Anti-Copy Setup
// ==========================================
function setupGlobalAntiSelect() {
  window.addEventListener('contextmenu', (e) => {
    if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;
    e.preventDefault();
  }, { capture: true });

  document.addEventListener('selectstart', (e) => {
    if (e.target.tagName !== 'INPUT' && e.target.tagName !== 'SELECT' && e.target.tagName !== 'TEXTAREA') {
      e.preventDefault();
    }
  });

  document.addEventListener('dragstart', (e) => {
    e.preventDefault();
  });

  document.addEventListener('gesturestart', (e) => e.preventDefault(), { passive: false });
  document.addEventListener('gesturechange', (e) => e.preventDefault(), { passive: false });
  document.addEventListener('gestureend', (e) => e.preventDefault(), { passive: false });

  let lastTouchEnd = 0;
  document.addEventListener('touchend', (e) => {
    const now = Date.now();
    if (now - lastTouchEnd <= 300) {
      e.preventDefault();
    }
    lastTouchEnd = now;
  }, { passive: false });
}

// ==========================================
// Master Power / Cycle Toggle Controller
// ==========================================
function toggleMasterCycle() {
  if (isHoming) {
    showToast('RETURNING MOTORS TO 0 ROTATION...');
    return;
  }

  const newState = !cycleEnabled;
  cycleEnabled = newState;

  // Optimistic UI state update
  updateMasterToggleUI(cycleEnabled, false, activeStep);

  const apiEndpoint = `/api/toggle?state=${newState ? '1' : '0'}`;
  const serialCmd = newState ? 'START\n' : 'STOP\n';

  sendSerialOrApi(apiEndpoint, serialCmd);

  if (newState) {
    showToast('AUTOMATION CYCLE STARTED');
  } else {
    showToast('STOPPING: RETURNING MOTORS TO 0...');
  }

  // Immediate poll refresh
  setTimeout(liveSyncStatus, 200);
}

function updateMasterToggleUI(enabled, homing, step, progress) {
  const card = document.getElementById('master-toggle-card');
  const btn = document.getElementById('master-switch-btn');
  const label = document.getElementById('switch-text');
  const badge = document.getElementById('master-state-tag');
  const title = document.getElementById('master-status-title');
  const desc = document.getElementById('master-status-desc');
  const statusPill = document.getElementById('status-pill');
  const statusText = document.getElementById('status-text');
  const statusDot = document.getElementById('status-pulse-dot');
  const activeLabel = document.getElementById('pipeline-active-label');

  if (homing) {
    if (card) { card.className = 'master-toggle-card homing'; }
    if (btn) { btn.className = 'hero-toggle-switch homing'; }
    if (label) label.textContent = 'ZEROING';
    if (badge) { badge.className = 'master-badge badge-homing'; badge.textContent = 'HOMING'; }
    if (title) title.textContent = 'RETURNING TO 0...';
    if (desc) desc.textContent = 'Calculating net rotation & reversing active motor back to 0 initial position.';
    if (statusText) statusText.textContent = 'HOMING (TO 0°)';
    if (statusPill) {
      statusPill.style.color = 'var(--pixel-amber)';
      statusPill.style.background = 'rgba(245, 158, 11, 0.12)';
      statusPill.style.borderColor = 'rgba(245, 158, 11, 0.4)';
    }
    if (statusDot) statusDot.style.background = 'var(--pixel-amber)';
    if (activeLabel) activeLabel.textContent = 'HOMING RESET';
  } else if (enabled) {
    if (card) { card.className = 'master-toggle-card active'; }
    if (btn) { btn.className = 'hero-toggle-switch active'; }
    if (label) label.textContent = 'ON';
    if (badge) { badge.className = 'master-badge badge-running'; badge.textContent = 'ACTIVE'; }
    if (title) title.textContent = 'AUTOMATION ACTIVE';
    if (desc) desc.textContent = 'Continuous 6-step loop running across Motor 1 and Motor 2.';
    if (statusText) statusText.textContent = `STEP ${step || 1} RUNNING`;
    if (statusPill) {
      statusPill.style.color = 'var(--pixel-green)';
      statusPill.style.background = 'rgba(16, 185, 129, 0.12)';
      statusPill.style.borderColor = 'rgba(16, 185, 129, 0.4)';
    }
    if (statusDot) statusDot.style.background = 'var(--pixel-green)';
    if (activeLabel) activeLabel.textContent = `STEP ${step || 1} / 5`;
  } else {
    if (card) { card.className = 'master-toggle-card'; }
    if (btn) { btn.className = 'hero-toggle-switch'; }
    if (label) label.textContent = 'OFF';
    if (badge) { badge.className = 'master-badge'; badge.textContent = 'STOPPED'; }
    if (title) title.textContent = 'SYSTEM IDLE';
    if (desc) desc.textContent = 'Both motors resting at calibrated 0 initial rotation.';
    if (statusText) statusText.textContent = 'OFF (0° ROTATION)';
    if (statusPill) {
      statusPill.style.color = 'var(--text-sub)';
      statusPill.style.background = 'rgba(0, 0, 0, 0.05)';
      statusPill.style.borderColor = 'var(--border-main)';
    }
    if (statusDot) statusDot.style.background = 'var(--text-dim)';
    if (activeLabel) activeLabel.textContent = 'STEP 0 / 5';
  }

  // Update Step Pipeline visual cards
  for (let i = 1; i <= 5; i++) {
    const stepCard = document.getElementById(`step-card-${i}`);
    const fill = document.getElementById(`fill-step-${i}`);
    if (!stepCard) continue;

    if (enabled && step === i) {
      let typeClass = 'step-m1';
      if (i === 3) typeClass = 'step-pause';
      else if (i >= 4) typeClass = 'step-m2';
      stepCard.className = `pipeline-step-card active ${typeClass}`;
      if (fill) fill.style.width = `${progress || 0}%`;
    } else {
      stepCard.className = 'pipeline-step-card';
      if (fill) fill.style.width = '0%';
    }
  }
}

// ==========================================
// Parameter Sliders & Hardware Flash Storage
// ==========================================
function onRotationSliderInput(val) {
  isUserAdjustingSlider = true;
  rotationTime = parseInt(val);
  const sec = (rotationTime / 1000).toFixed(1);
  const label = document.getElementById('rotation-val-label');
  if (label) label.textContent = `${sec} s`;

  debounceAutoSave();
}

function onPauseSliderInput(val) {
  isUserAdjustingSlider = true;
  pauseTime = parseInt(val);
  const sec = (pauseTime / 1000).toFixed(1);
  const label = document.getElementById('pause-val-label');
  if (label) label.textContent = `${sec} s`;

  debounceAutoSave();
}

function onSpeedSliderInput(val) {
  isUserAdjustingSlider = true;
  motorSpeed = parseInt(val);
  const pct = Math.round((motorSpeed / 1023) * 100);
  const label = document.getElementById('speed-val-label');
  if (label) label.textContent = `${pct}% (${motorSpeed} PWM)`;

  debounceAutoSave();
}

function debounceAutoSave() {
  clearTimeout(sliderSaveTimeout);
  sliderSaveTimeout = setTimeout(() => {
    saveParametersToHardware(true);
    isUserAdjustingSlider = false;
  }, 900);
}

function saveParametersToHardware(isQuiet = false) {
  rotationTime = parseInt(document.getElementById('slider-rotation-time')?.value || rotationTime);
  pauseTime = parseInt(document.getElementById('slider-pause-time')?.value || pauseTime);
  motorSpeed = parseInt(document.getElementById('slider-motor-speed')?.value || motorSpeed);

  const apiEndpoint = `/api/config?rotation_time=${rotationTime}&pause_time=${pauseTime}&speed=${motorSpeed}`;
  const serialCmd = `SET_X=${rotationTime}\nSET_Y=${pauseTime}\nSET_PWM=${motorSpeed}\n`;

  sendSerialOrApi(apiEndpoint, serialCmd);
  if (!isQuiet) {
    showToast('SAVED TO MCU FLASH');
  }
}

// ==========================================
// Emergency Hard Stop
// ==========================================
function triggerEmergencyHardStop() {
  cycleEnabled = false;
  isHoming = false;
  activeStep = 0;

  updateMasterToggleUI(false, false, 0, 0);
  sendSerialOrApi('/api/stop', 'KILL\n');
  showToast('EMERGENCY HARD STOP');
}

// ==========================================
// Live Hardware Status Synchronization
// ==========================================
async function liveSyncStatus() {
  try {
    const res = await fetch('/api/status');
    if (!res.ok) return;
    const data = await res.json();

    cycleEnabled = data.enabled || false;
    isHoming = data.is_homing || false;
    activeStep = data.step || 0;

    updateMasterToggleUI(cycleEnabled, isHoming, activeStep, data.step_progress || 0);

    // Sync sliders from MCU persistent storage if user isn't currently dragging them
    if (!isUserAdjustingSlider) {
      if (data.rotation_time && data.rotation_time !== rotationTime) {
        rotationTime = data.rotation_time;
        const slider = document.getElementById('slider-rotation-time');
        const label = document.getElementById('rotation-val-label');
        if (slider) slider.value = rotationTime;
        if (label) label.textContent = (rotationTime / 1000).toFixed(1) + ' s';
      }

      if (data.pause_time !== undefined && data.pause_time !== pauseTime) {
        pauseTime = data.pause_time;
        const slider = document.getElementById('slider-pause-time');
        const label = document.getElementById('pause-val-label');
        if (slider) slider.value = pauseTime;
        if (label) label.textContent = (pauseTime / 1000).toFixed(1) + ' s';
      }

      if (data.speed && data.speed !== motorSpeed) {
        motorSpeed = data.speed;
        const slider = document.getElementById('slider-motor-speed');
        const label = document.getElementById('speed-val-label');
        if (slider) slider.value = motorSpeed;
        if (label) {
          const pct = Math.round((motorSpeed / 1023) * 100);
          label.textContent = `${pct}% (${motorSpeed} PWM)`;
        }
      }
    }

    updateWirelessStatus(data);
  } catch (err) {}
}

function updateWirelessStatus(data) {
  if (!data) return;

  const navLabel = document.getElementById('nav-wifi-label');
  const navBtn = document.getElementById('btn-wifi-nav');
  const wifiBadge = document.getElementById('modal-wifi-badge');
  const wifiSsid = document.getElementById('modal-wifi-ssid');
  const stationIp = document.getElementById('modal-station-ip');
  const apIp = document.getElementById('modal-ap-ip');
  const hawaBadge = document.getElementById('modal-hawa-badge');
  const deviceId = document.getElementById('modal-device-id');

  if (data.wifi_connected) {
    if (navBtn) navBtn.classList.add('connected');
    if (navLabel) navLabel.textContent = data.wifi_ssid || 'ONLINE';
    if (wifiBadge) {
      wifiBadge.className = 'telemetry-badge badge-online';
      wifiBadge.textContent = 'CONNECTED';
    }
    if (wifiSsid) wifiSsid.textContent = data.wifi_ssid + (data.rssi ? ` (${data.rssi} dBm)` : '');
    if (stationIp) stationIp.textContent = data.station_ip || '—';
  } else {
    if (navBtn) navBtn.classList.remove('connected');
    if (navLabel) navLabel.textContent = 'AP MODE';
    if (wifiBadge) {
      wifiBadge.className = 'telemetry-badge badge-offline';
      wifiBadge.textContent = 'AP ONLY';
    }
    if (wifiSsid) wifiSsid.textContent = 'Project-CLAW (AP)';
    if (stationIp) stationIp.textContent = '—';
  }

  if (apIp && data.ap_ip) apIp.textContent = data.ap_ip;

  if (data.hawa_connected) {
    if (hawaBadge) {
      hawaBadge.className = 'telemetry-badge badge-online';
      hawaBadge.textContent = 'ONLINE (WSS)';
    }
  } else if (data.wifi_connected) {
    if (hawaBadge) {
      hawaBadge.className = 'telemetry-badge badge-cloud';
      hawaBadge.textContent = 'CONNECTING...';
    }
  } else {
    if (hawaBadge) {
      hawaBadge.className = 'telemetry-badge badge-offline';
      hawaBadge.textContent = 'STANDBY';
    }
  }

  if (deviceId && data.hawa_device_id) deviceId.textContent = data.hawa_device_id;

  const inputSsid = document.getElementById('input-wifi-ssid');
  const inputServer = document.getElementById('input-hawa-server');
  const inputName = document.getElementById('input-device-name');
  if (inputSsid && !inputSsid.value && data.wifi_ssid && data.wifi_ssid !== 'Project-CLAW') {
    inputSsid.value = data.wifi_ssid;
  }
  if (inputServer && !inputServer.value && data.hawa_server) {
    inputServer.value = data.hawa_server;
  }
  if (inputName && !inputName.value && data.hawa_device_name) {
    inputName.value = data.hawa_device_name;
  }

  if (data.hawa_ota_running) {
    showToast('WIRELESS OTA UPDATE RUNNING...');
  }
}

// ==========================================
// Serial / HTTP Communication Helper
// ==========================================
function sendSerialOrApi(apiEndpoint, serialCmd) {
  if (isSerialConnected && serialCmd && serialPort && serialPort.writable) {
    const encoder = new TextEncoder();
    const writer = serialPort.writable.getWriter();
    writer.write(encoder.encode(serialCmd)).finally(() => writer.releaseLock());
  }
  fetch(apiEndpoint, { method: 'POST' }).catch(() => {});
}

// ==========================================
// Wi-Fi & Hawa Modal Controller Functions
// ==========================================
function openWifiModal() {
  const modal = document.getElementById('wifi-modal-backdrop');
  if (modal) modal.classList.add('open');
}

function closeWifiModal() {
  const modal = document.getElementById('wifi-modal-backdrop');
  if (modal) modal.classList.remove('open');
}

function closeWifiModalOnBackdrop(e) {
  if (e.target.id === 'wifi-modal-backdrop') {
    closeWifiModal();
  }
}

function togglePasswordVisibility() {
  const passInput = document.getElementById('input-wifi-pass');
  const btn = document.getElementById('btn-toggle-pass');
  if (!passInput) return;
  if (passInput.type === 'password') {
    passInput.type = 'text';
    if (btn) btn.textContent = 'HIDE';
  } else {
    passInput.type = 'password';
    if (btn) btn.textContent = 'SHOW';
  }
}

function applyHawaCloudPreset() {
  const inputServer = document.getElementById('input-hawa-server');
  if (inputServer) {
    inputServer.value = 'wss://hawa-platform.onrender.com/ws';
    inputServer.focus();
    showToast('PRESET APPLIED: HAWA CLOUD');
  }
}

function copyDeviceId() {
  const devId = document.getElementById('modal-device-id')?.textContent?.trim();
  if (devId) {
    if (navigator.clipboard && navigator.clipboard.writeText) {
      navigator.clipboard.writeText(devId).then(() => {
        showToast(`COPIED ID: ${devId}`);
      }).catch(() => {
        prompt('Copy Device Pairing ID:', devId);
      });
    } else {
      prompt('Copy Device Pairing ID:', devId);
    }
  }
}

async function scanWifiNetworks() {
  const btnText = document.getElementById('scan-btn-text');
  const scanIcon = document.getElementById('scan-icon');
  const container = document.getElementById('scanned-networks-box');

  if (btnText) btnText.textContent = 'SCANNING...';
  if (scanIcon) scanIcon.classList.add('spinning-icon');

  try {
    const res = await fetch('/api/wifi-scan');
    const networks = await res.json();

    if (Array.isArray(networks) && networks.length > 0) {
      if (container) {
        container.innerHTML = '';
        container.style.display = 'flex';
        networks.forEach(net => {
          const item = document.createElement('div');
          item.className = 'scanned-network-item';
          item.innerHTML = `
            <span>${net.ssid}</span>
            <span class="network-item-rssi">${net.rssi} dBm ${net.secure ? '🔒' : ''}</span>
          `;
          item.onclick = () => {
            const inputSsid = document.getElementById('input-wifi-ssid');
            const inputPass = document.getElementById('input-wifi-pass');
            if (inputSsid) inputSsid.value = net.ssid;
            if (inputPass) inputPass.focus();
            container.style.display = 'none';
          };
          container.appendChild(item);
        });
      }
      showToast(`FOUND ${networks.length} NETWORKS`);
    } else {
      showToast('NO NETWORKS FOUND');
    }
  } catch (err) {
    showToast('SCAN COMPLETE');
  } finally {
    if (btnText) btnText.textContent = 'SCAN';
    if (scanIcon) scanIcon.classList.remove('spinning-icon');
  }
}

async function saveWifiCredentials() {
  const ssid = document.getElementById('input-wifi-ssid')?.value?.trim();
  const pass = document.getElementById('input-wifi-pass')?.value || '';
  let server = document.getElementById('input-hawa-server')?.value?.trim() || '';
  const name = document.getElementById('input-device-name')?.value?.trim() || '';

  if (!ssid) {
    showToast('PLEASE ENTER WI-FI SSID');
    return;
  }

  if (!server || server === 'hawa-platform.onrender.com') {
    server = 'wss://hawa-platform.onrender.com/ws';
  } else if (!server.startsWith('ws://') && !server.startsWith('wss://')) {
    if (server.startsWith('https://')) {
      server = 'wss://' + server.substring(8);
    } else if (server.startsWith('http://')) {
      server = 'ws://' + server.substring(7);
    } else {
      server = 'wss://' + server;
    }
    if (!server.includes('/', 8)) {
      server += '/ws';
    }
  }

  const inputServer = document.getElementById('input-hawa-server');
  if (inputServer) inputServer.value = server;

  showToast(`CONNECTING TO WI-FI & HAWA CLOUD...`);

  const params = new URLSearchParams();
  params.append('ssid', ssid);
  params.append('password', pass);
  params.append('server', server);
  params.append('name', name);

  try {
    const res = await fetch('/api/wifi-save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: params.toString()
    });
    const result = await res.json();
    showToast(result.message || 'CREDENTIALS SAVED!');
  } catch (err) {
    sendSerialOrApi(`/api/wifi-save?${params.toString()}`, `WIFI:${ssid},${pass}\n`);
    showToast('CONNECTING TO WI-FI...');
  }
}

async function clearWifiCredentials() {
  if (!confirm('Clear saved Wi-Fi credentials and switch to AP-only mode?')) return;
  try {
    const res = await fetch('/api/wifi-clear', { method: 'POST' });
    const result = await res.json();
    showToast(result.message || 'WI-FI CLEARED');
  } catch (err) {
    showToast('WI-FI CLEARED');
  }
}

async function rebootHardware() {
  if (!confirm('Reboot the ESP8266 controller now?')) return;
  try {
    await fetch('/api/reboot', { method: 'POST' });
    showToast('REBOOTING CONTROLLER...');
  } catch (err) {
    showToast('REBOOT COMMAND SENT');
  }
}

function showToast(msg) {
  const toast = document.getElementById('toast-notice');
  if (!toast) return;
  toast.textContent = msg;
  toast.classList.add('show');
  setTimeout(() => toast.classList.remove('show'), 2000);
}
