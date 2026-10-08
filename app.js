/**
 * Project CLAW / Himalix Projects — Precision Mobile Hardware Controller
 * Hold-to-Run (Momentary Actuation) & Anti-Zoom/Anti-Select Handling
 */

// Application State
let targetPwm = 850;
let deployDuration = 3000;
let retractDuration = 3000;
let holdDuration = 2000;

// Claw Travel Position Tracking (0 = Fully Retracted, deployDuration = Fully Deployed)
let clawTravelMs = 0;
let momentaryTimer = null;
let activeHoldDirection = null;

// Serial Communication Variables
let serialPort = null;
let serialReader = null;
let isSerialConnected = false;

// DOM Initialization
document.addEventListener('DOMContentLoaded', () => {
  initTheme();
  setupHoldToRunButtons();
  setupGlobalAntiSelect();
  setInterval(liveSyncStatus, 500);
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
  // Prevent context menu except inside input fields
  window.addEventListener('contextmenu', (e) => {
    if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA') return;
    e.preventDefault();
  }, { capture: true });

  // Prevent text selection highlights except in inputs
  document.addEventListener('selectstart', (e) => {
    if (e.target.tagName !== 'INPUT' && e.target.tagName !== 'SELECT' && e.target.tagName !== 'TEXTAREA') {
      e.preventDefault();
    }
  });

  // Prevent drag ghosting
  document.addEventListener('dragstart', (e) => {
    e.preventDefault();
  });

  // Prevent mobile pinch and zoom gestures
  document.addEventListener('gesturestart', (e) => e.preventDefault(), { passive: false });
  document.addEventListener('gesturechange', (e) => e.preventDefault(), { passive: false });
  document.addEventListener('gestureend', (e) => e.preventDefault(), { passive: false });

  // Prevent double-tap zoom on quick taps
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
// Momentary Hold-To-Run Motion (Dead-Man Switch)
// "Press to deploy, release to stop, repress to resume from where you left off"
// ==========================================
function setupHoldToRunButtons() {
  bindMomentaryControl('btn-deploy', 'deploy');
  bindMomentaryControl('btn-retract', 'retract');

  // Window-level safety fallback: if finger or mouse lifts anywhere outside the button
  window.addEventListener('mouseup', () => {
    if (activeHoldDirection) stopMomentaryMotion(activeHoldDirection);
  });
  window.addEventListener('touchend', (e) => {
    // If no touches remain on screen, stop any active hold
    if (e.touches && e.touches.length === 0 && activeHoldDirection) {
      stopMomentaryMotion(activeHoldDirection);
    }
  });
  window.addEventListener('touchcancel', () => {
    if (activeHoldDirection) stopMomentaryMotion(activeHoldDirection);
  });
}

function bindMomentaryControl(elementId, direction) {
  const btn = document.getElementById(elementId);
  if (!btn) return;

  const onStart = (e) => {
    if (e.cancelable) e.preventDefault();
    startMomentaryMotion(direction);
  };

  const onEnd = (e) => {
    if (e.cancelable) e.preventDefault();
    stopMomentaryMotion(direction);
  };

  // Touch Events (Mobile)
  btn.addEventListener('touchstart', onStart, { passive: false });
  btn.addEventListener('touchend', onEnd, { passive: false });
  btn.addEventListener('touchcancel', onEnd, { passive: false });

  // Mouse Events (Desktop)
  btn.addEventListener('mousedown', onStart);
  btn.addEventListener('mouseup', onEnd);
  btn.addEventListener('mouseleave', onEnd);

  // Prevent context menu on long-press
  btn.addEventListener('contextmenu', (e) => {
    e.preventDefault();
    e.stopPropagation();
    return false;
  });
}

function startMomentaryMotion(direction) {
  if (activeHoldDirection === direction) return;
  activeHoldDirection = direction;

  const btn = document.getElementById(direction === 'deploy' ? 'btn-deploy' : 'btn-retract');
  if (btn) btn.classList.add('holding');

  // Trigger hardware motor
  if (direction === 'deploy') {
    sendSerialOrApi('/api/deploy', 'DEPLOY\n');
  } else {
    sendSerialOrApi('/api/retract', 'RETRACT\n');
  }

  clearInterval(momentaryTimer);
  const stepMs = 50;

  momentaryTimer = setInterval(() => {
    if (direction === 'deploy') {
      clawTravelMs = Math.min(deployDuration, clawTravelMs + stepMs);
      const pct = Math.round((clawTravelMs / deployDuration) * 100);
      updateStatusDisplay(`DEPLOYING ${pct}%`, 'var(--pixel-red)', 'rgba(225, 29, 72, 0.15)');

      // Reached mechanical limit
      if (clawTravelMs >= deployDuration) {
        stopMomentaryMotion('deploy');
        updateStatusDisplay('DEPLOYED 100%', 'var(--pixel-red)', 'rgba(225, 29, 72, 0.2)');
        showToast('DEPLOYED (100%)');
      }
    } else {
      clawTravelMs = Math.max(0, clawTravelMs - stepMs);
      const pct = Math.round((clawTravelMs / deployDuration) * 100);
      updateStatusDisplay(`RETRACTING ${pct}%`, 'var(--pixel-blue)', 'rgba(37, 99, 235, 0.15)');

      // Reached mechanical limit
      if (clawTravelMs <= 0) {
        stopMomentaryMotion('retract');
        updateStatusDisplay('RETRACTED 0%', 'var(--pixel-blue)', 'rgba(37, 99, 235, 0.2)');
        showToast('RETRACTED (0%)');
      }
    }
  }, stepMs);
}

function stopMomentaryMotion(direction) {
  if (activeHoldDirection !== direction) return;
  clearInterval(momentaryTimer);
  activeHoldDirection = null;

  const btn = document.getElementById(direction === 'deploy' ? 'btn-deploy' : 'btn-retract');
  if (btn) btn.classList.remove('holding');

  // Immediately stop motors on hardware
  sendSerialOrApi('/api/stop', 'STOP\n');

  // Calculate paused position percentage
  const pct = Math.round((clawTravelMs / deployDuration) * 100);
  updateStatusDisplay(`PAUSED (${pct}%)`, 'var(--pixel-green)', 'rgba(16, 185, 129, 0.12)');
}

function triggerEmergencyStop() {
  clearInterval(momentaryTimer);
  activeHoldDirection = null;

  document.querySelectorAll('.btn-tactile').forEach(b => b.classList.remove('holding'));
  sendSerialOrApi('/api/stop', 'STOP\n');
  updateStatusDisplay('STOPPED', 'var(--pixel-red)', 'rgba(225, 29, 72, 0.25)');
  showToast('EMERGENCY STOP');
}

function triggerAutoDemo() {
  clearInterval(momentaryTimer);
  activeHoldDirection = null;
  sendSerialOrApi('/api/demo', 'DEMO\n');
  showToast('AUTO DEMO RUNNING');
  updateStatusDisplay('DEMO RUNNING', 'var(--pixel-amber)', 'rgba(245, 158, 11, 0.15)');

  // Simulate demo cycle
  const stepMs = 50;
  const demoInterval = setInterval(() => {
    clawTravelMs = Math.min(deployDuration, clawTravelMs + stepMs * 2);
    const pct = Math.round((clawTravelMs / deployDuration) * 100);
    updateStatusDisplay(`DEMO DEPLOY ${pct}%`, 'var(--pixel-amber)', 'rgba(245, 158, 11, 0.15)');

    if (clawTravelMs >= deployDuration) {
      clearInterval(demoInterval);
      updateStatusDisplay('DEMO HOLDING', 'var(--pixel-amber)', 'rgba(245, 158, 11, 0.2)');
      setTimeout(() => {
        const retractInterval = setInterval(() => {
          clawTravelMs = Math.max(0, clawTravelMs - stepMs * 2);
          const rPct = Math.round((clawTravelMs / deployDuration) * 100);
          updateStatusDisplay(`DEMO RETRACT ${rPct}%`, 'var(--pixel-amber)', 'rgba(245, 158, 11, 0.15)');

          if (clawTravelMs <= 0) {
            clearInterval(retractInterval);
            updateStatusDisplay('READY (0%)', 'var(--pixel-green)', 'rgba(16, 185, 129, 0.12)');
            showToast('DEMO COMPLETE');
          }
        }, stepMs);
      }, holdDuration);
    }
  }, stepMs);
}

function updateStatusDisplay(text, color, bg) {
  const pill = document.getElementById('status-pill');
  if (!pill) return;
  const label = pill.querySelector('span:last-child') || pill;
  label.textContent = text;
  if (color) pill.style.color = color;
  if (bg) pill.style.background = bg;
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

function saveCalibration() {
  deployDuration = parseInt(document.getElementById('deploy-slider').value);
  retractDuration = parseInt(document.getElementById('retract-slider').value);
  holdDuration = parseInt(document.getElementById('hold-slider').value);

  sendSerialOrApi(
    `/api/config?pwm=${targetPwm}&deploy=${deployDuration}&retract=${retractDuration}&hold=${holdDuration}`,
    `CALIB:${targetPwm},${deployDuration},${retractDuration},${holdDuration}\n`
  );
  showToast('CALIBRATION APPLIED');
}

// ==========================================
// Web Serial API & Hardware Communication
// ==========================================
function sendSerialOrApi(apiEndpoint, serialCmd) {
  if (isSerialConnected && serialCmd && serialPort && serialPort.writable) {
    const encoder = new TextEncoder();
    const writer = serialPort.writable.getWriter();
    writer.write(encoder.encode(serialCmd)).finally(() => writer.releaseLock());
  }
  fetch(apiEndpoint, { method: 'POST' }).catch(() => {});
}

// Live Status Polling
async function liveSyncStatus() {
  try {
    const res = await fetch('/api/status');
    if (!res.ok) return;
    const data = await res.json();
    if (!activeHoldDirection && data.state) {
      updateStatusDisplay(data.state);
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

  // Pre-fill inputs on initial load if they are untouched
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

function copyDeviceId() {
  const devId = document.getElementById('modal-device-id')?.textContent;
  if (devId && navigator.clipboard) {
    navigator.clipboard.writeText(devId).then(() => {
      showToast('DEVICE ID COPIED');
    }).catch(() => {
      showToast(devId);
    });
  } else if (devId) {
    showToast(devId);
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
  const server = document.getElementById('input-hawa-server')?.value?.trim() || '';
  const name = document.getElementById('input-device-name')?.value?.trim() || '';

  if (!ssid) {
    showToast('PLEASE ENTER WI-FI SSID');
    return;
  }

  showToast(`SAVING WI-FI: ${ssid}...`);

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
