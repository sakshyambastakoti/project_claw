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
/* ==========================================================================
   Project CLAW / Himalix Projects — Precision Mobile Hardware Controller
   ========================================================================== */

@import url('https://fonts.googleapis.com/css2?family=DotGothic16&family=Inter:wght@400;500;600;700;800&family=Space+Mono:ital,wght@0,400;0,700;1,400&display=swap');

:root {
  /* Warm paper / sand canvas aesthetic */
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
  --btn-outline-bg: #f5f1ea;
  --btn-outline-border: #bcb7aa;
  
  /* Retro Accent Pixels */
  --pixel-red: #e11d48;
  --pixel-amber: #f59e0b;
  --pixel-green: #10b981;
  --pixel-blue: #2563eb;
  
  /* Typography */
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
  --btn-outline-bg: #1c222c;
  --btn-outline-border: #334155;
}

/* Absolute Prevention of Copying, Selecting, Callout, & Zooming */
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

html, body {
  min-height: 100%;
  width: 100%;
  overflow-x: hidden;
}

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

/* Mobile Screen Frame */
.mobile-app-container {
  width: 100%;
  max-width: 440px;
  display: flex;
  flex-direction: column;
  margin: auto 0;
}

/* Workbench Card */
.workbench-card {
  width: 100%;
  background: var(--surface-card);
  border: 1px solid var(--border-main);
  border-radius: var(--radius-lg);
  box-shadow: 0 8px 30px rgba(0, 0, 0, 0.06), 0 2px 8px rgba(0, 0, 0, 0.04);
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

/* Title Bar Header */
.workbench-titlebar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 11px 14px;
  background: var(--surface-header);
  border-bottom: 1px solid var(--border-main);
}

.titlebar-left {
  display: flex;
  align-items: center;
  gap: 10px;
}

.window-dots {
  display: flex;
  align-items: center;
  gap: 4px;
}

.window-square-dot {
  width: 7px;
  height: 7px;
  border-radius: 1px;
}

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

.titlebar-actions {
  display: flex;
  align-items: center;
  gap: 6px;
}

/* Small SVG OTA Feature Button */
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
  transition: all 0.12s ease;
}

.btn-ota svg {
  width: 12px;
  height: 12px;
  stroke: currentColor;
}

.btn-ota:active {
  background: var(--btn-black-bg);
  color: var(--btn-black-text);
  border-color: var(--btn-black-bg);
}

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

.status-pulse-dot {
  width: 5px;
  height: 5px;
  border-radius: 50%;
  background: var(--pixel-green);
}

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
  transition: all 0.12s ease;
}

.btn-theme-toggle svg {
  width: 12px;
  height: 12px;
  stroke: currentColor;
}

.btn-theme-toggle:hover {
  border-color: var(--text-main);
  color: var(--text-main);
}

/* Card Body Content (Consistent, Harmonious Spacing) */
.workbench-content {
  padding: 12px 14px 14px;
  display: flex;
  flex-direction: column;
  gap: 11px;
}

/* Tactile Claws Actuation Grid (Hold to Run) */
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
  overflow: hidden;
}

.btn-tactile svg {
  width: 24px;
  height: 24px;
  stroke: currentColor;
}

.btn-tactile-deploy {
  border-left: 5px solid var(--pixel-red);
}

.btn-tactile-retract {
  border-left: 5px solid var(--pixel-blue);
}

/* Tactile Press & Holding States (Visual Feedback during Hold-to-Run) */
.btn-tactile:active,
.btn-tactile.holding {
  transform: scale(0.96);
}

.btn-tactile-deploy:active,
.btn-tactile-deploy.holding {
  background: rgba(225, 29, 72, 0.09);
  border-color: var(--pixel-red);
}

.btn-tactile-retract:active,
.btn-tactile-retract.holding {
  background: rgba(37, 99, 235, 0.09);
  border-color: var(--pixel-blue);
}

/* Auto Routine & Emergency Stop Row */
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
  transition: all 0.1s ease;
}

.btn-auto-demo svg {
  width: 14px;
  height: 14px;
}

.btn-auto-demo:active {
  transform: scale(0.97);
  background: rgba(0, 0, 0, 0.05);
}

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
  transition: all 0.1s ease;
}

.btn-emergency-stop svg {
  width: 14px;
  height: 14px;
}

.btn-emergency-stop:active {
  transform: scale(0.96);
  opacity: 0.9;
}

/* Speed Slider Box */
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

.speed-box-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.speed-box-title {
  font-family: var(--font-mono);
  font-size: 10.5px;
  font-weight: 700;
  letter-spacing: 0.8px;
  text-transform: uppercase;
  color: var(--text-sub);
}

.speed-box-readout {
  font-family: var(--font-mono);
  font-size: 12.5px;
  font-weight: 700;
  color: var(--text-main);
}

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

.speed-presets-row {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 6px;
}

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
  transition: all 0.1s ease;
}

.btn-speed-preset:active {
  transform: scale(0.96);
}

.btn-speed-preset.active {
  background: var(--btn-black-bg);
  color: var(--btn-black-text);
  border-color: var(--btn-black-bg);
}

/* Permanently Revealed Motion Timing & Calibration Box */
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

.timing-box-header svg {
  width: 13px;
  height: 13px;
  stroke: currentColor;
}

.timing-row {
  display: flex;
  flex-direction: column;
  gap: 3px;
}

.timing-row-labels {
  display: flex;
  justify-content: space-between;
  font-family: var(--font-mono);
  font-size: 10px;
  color: var(--text-sub);
}

.timing-val-tag {
  font-weight: 700;
  color: var(--text-main);
}

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
  transition: all 0.12s ease;
}

.btn-save-calib svg {
  width: 13px;
  height: 13px;
  stroke: currentColor;
}

.btn-save-calib:active {
  background: var(--btn-black-bg);
  color: var(--btn-black-text);
  transform: scale(0.97);
}

/* Toast Notification */
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
  z-index: 200;
  pointer-events: none;
  white-space: nowrap;
}

.toast-notice.show {
  transform: translateX(-50%) translateY(0);
}

/* Allow text input interaction */
input[type="text"],
input[type="password"],
select,
textarea {
  -webkit-touch-callout: default !important;
  -webkit-user-select: text !important;
  -khtml-user-select: text !important;
  -moz-user-select: text !important;
  -ms-user-select: text !important;
  user-select: text !important;
  touch-action: manipulation !important;
}

/* Wi-Fi & Hawa Wireless Titlebar Button */
.btn-wifi-nav {
  display: inline-flex;
  align-items: center;
  gap: 5px;
  background: var(--surface-elevated);
  border: 1px solid var(--border-main);
  color: var(--text-main);
  padding: 4px 8px;
  border-radius: var(--radius-sm);
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.12s ease;
}

.btn-wifi-nav svg {
  width: 12px;
  height: 12px;
  stroke: currentColor;
}

.btn-wifi-nav:active {
  background: var(--btn-black-bg);
  color: var(--btn-black-text);
  border-color: var(--btn-black-bg);
}

.btn-wifi-nav.connected {
  border-color: var(--pixel-green);
  color: var(--pixel-green);
}

/* Modal Overlay & Dialog */
.modal-backdrop {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(10, 14, 20, 0.65);
  backdrop-filter: blur(5px);
  -webkit-backdrop-filter: blur(5px);
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 16px;
  z-index: 150;
  opacity: 0;
  pointer-events: none;
  transition: opacity 0.2s ease;
}

.modal-backdrop.open {
  opacity: 1;
  pointer-events: auto;
}

.modal-dialog {
  width: 100%;
  max-width: 420px;
  max-height: 90vh;
  background: var(--surface-card);
  border: 1px solid var(--border-main);
  border-radius: var(--radius-lg);
  box-shadow: 0 16px 40px rgba(0, 0, 0, 0.22);
  display: flex;
  flex-direction: column;
  overflow: hidden;
  animation: modalPopIn 0.2s cubic-bezier(0.16, 1, 0.3, 1);
}

@keyframes modalPopIn {
  0% { transform: scale(0.95); opacity: 0; }
  100% { transform: scale(1); opacity: 1; }
}

.modal-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 11px 14px;
  background: var(--surface-header);
  border-bottom: 1px solid var(--border-main);
}

.modal-title {
  font-family: var(--font-dot);
  font-size: 12.5px;
  letter-spacing: 1.2px;
  font-weight: 700;
  color: var(--text-main);
  text-transform: uppercase;
}

.btn-modal-close {
  background: transparent;
  border: none;
  color: var(--text-sub);
  cursor: pointer;
  padding: 4px;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: var(--radius-sm);
  transition: color 0.12s ease;
}

.btn-modal-close:hover,
.btn-modal-close:active {
  color: var(--text-main);
}

.btn-modal-close svg {
  width: 16px;
  height: 16px;
  stroke: currentColor;
}

.modal-body {
  padding: 14px;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 12px;
}

/* Live Wireless Telemetry Card */
.telemetry-card {
  background: var(--surface-elevated);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-md);
  padding: 10px 12px;
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.telemetry-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-family: var(--font-mono);
  font-size: 10.5px;
}

.telemetry-label {
  color: var(--text-sub);
}

.telemetry-val {
  font-weight: 700;
  color: var(--text-main);
  display: flex;
  align-items: center;
  gap: 6px;
}

.telemetry-badge {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  padding: 2px 6px;
  border-radius: 2px;
  font-size: 9.5px;
  font-weight: 700;
  text-transform: uppercase;
}

.badge-online {
  background: rgba(16, 185, 129, 0.15);
  color: var(--pixel-green);
  border: 1px solid rgba(16, 185, 129, 0.3);
}

.badge-offline {
  background: rgba(245, 158, 11, 0.15);
  color: var(--pixel-amber);
  border: 1px solid rgba(245, 158, 11, 0.3);
}

.badge-cloud {
  background: rgba(37, 99, 235, 0.15);
  color: var(--pixel-blue);
  border: 1px solid rgba(37, 99, 235, 0.3);
}

.device-id-code {
  font-family: var(--font-mono);
  font-size: 10px;
  background: var(--surface-card);
  border: 1px solid var(--border-main);
  padding: 2px 6px;
  border-radius: var(--radius-sm);
  color: var(--text-main);
  word-break: break-all;
}

/* Wi-Fi Form Elements */
.field-group {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.field-label {
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  letter-spacing: 0.5px;
  color: var(--text-sub);
  text-transform: uppercase;
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.field-input-box {
  display: flex;
  align-items: stretch;
  gap: 6px;
}

.form-control-input {
  flex: 1;
  background: var(--surface-elevated);
  border: 1px solid var(--border-main);
  border-radius: var(--radius-sm);
  padding: 8px 10px;
  font-family: var(--font-mono);
  font-size: 11px;
  color: var(--text-main);
  outline: none;
  transition: border-color 0.15s ease;
}

.form-control-input:focus {
  border-color: var(--border-dark);
}

.btn-inline-action {
  background: var(--surface-elevated);
  border: 1px solid var(--border-main);
  border-radius: var(--radius-sm);
  padding: 0 10px;
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  color: var(--text-main);
  cursor: pointer;
  display: flex;
  align-items: center;
  gap: 4px;
  white-space: nowrap;
  transition: all 0.12s ease;
}

.btn-inline-action svg {
  width: 12px;
  height: 12px;
  stroke: currentColor;
}

.btn-inline-action:active {
  background: var(--btn-black-bg);
  color: var(--btn-black-text);
}

/* Scanned Wi-Fi Networks Dropdown / Container */
.scanned-networks-box {
  max-height: 120px;
  overflow-y: auto;
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-sm);
  background: var(--surface-elevated);
  display: flex;
  flex-direction: column;
}

.scanned-network-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 7px 10px;
  border-bottom: 1px solid var(--border-subtle);
  font-family: var(--font-mono);
  font-size: 10.5px;
  color: var(--text-main);
  cursor: pointer;
  transition: background 0.12s ease;
}

.scanned-network-item:last-child {
  border-bottom: none;
}

.scanned-network-item:hover,
.scanned-network-item:active {
  background: rgba(37, 99, 235, 0.08);
}

.network-item-rssi {
  font-size: 9.5px;
  color: var(--text-dim);
  display: flex;
  align-items: center;
  gap: 4px;
}

/* Modal Actions Footer */
.modal-footer-actions {
  display: flex;
  flex-direction: column;
  gap: 6px;
  margin-top: 4px;
}

.btn-modal-primary {
  background: var(--btn-black-bg);
  color: var(--btn-black-text);
  border: 1px solid var(--border-dark);
  border-radius: var(--radius-sm);
  padding: 10px;
  font-family: var(--font-mono);
  font-size: 11px;
  font-weight: 700;
  letter-spacing: 0.8px;
  text-transform: uppercase;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  cursor: pointer;
  transition: all 0.12s ease;
}

.btn-modal-primary svg {
  width: 14px;
  height: 14px;
  stroke: currentColor;
}

.btn-modal-primary:active {
  transform: scale(0.98);
}

.modal-secondary-row {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 6px;
}

.btn-modal-subtle {
  background: var(--surface-elevated);
  color: var(--text-main);
  border: 1px solid var(--border-main);
  border-radius: var(--radius-sm);
  padding: 7px;
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  text-transform: uppercase;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 4px;
  transition: all 0.12s ease;
}

.btn-modal-subtle:active {
  background: var(--surface-card);
  border-color: var(--border-dark);
}

.spinning-icon {
  animation: spin 0.9s linear infinite;
}

@keyframes spin {
  100% { transform: rotate(360deg); }
}

</style>
</head>

<body>

  <div class="mobile-app-container">

    <!-- Workbench Card Controller Window -->
    <main class="workbench-card" id="workbench-card">

      <!-- Workbench Titlebar -->
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
          <!-- Wi-Fi & Hawa Wireless Config Button -->
          <button class="btn-wifi-nav" id="btn-wifi-nav" onclick="openWifiModal()" title="Configure Wi-Fi & Hawa Wireless OTA">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <path d="M5 12.55a11 11 0 0 1 14.08 0"></path>
              <path d="M1.42 9a16 16 0 0 1 21.16 0"></path>
              <path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path>
              <line x1="12" y1="20" x2="12.01" y2="20"></line>
            </svg>
            <span id="nav-wifi-label">WI-FI</span>
          </button>

          <!-- Small SVG OTA Feature Button -->
          <a href="/update" target="_blank" class="btn-ota" title="OTA Firmware Update (/update)">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path>
              <polyline points="17 8 12 3 7 8"></polyline>
              <line x1="12" y1="3" x2="12" y2="15"></line>
            </svg>
            <span>OTA</span>
          </a>

          <!-- Live Status Badge with Pulsing Dot (No Emojis) -->
          <span class="status-pill" id="status-pill">
            <span class="status-pulse-dot"></span>
            <span>READY</span>
          </span>

          <!-- Theme Toggle with Pure SVG Moon/Sun (No Emojis) -->
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

      <!-- Workbench Content -->
      <div class="workbench-content">

        <!-- Tactile Motion Buttons: Hold-To-Run Dead-Man Actuation -->
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

        <!-- Auto Routine & Emergency Stop -->
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

        <!-- Motor Speed Slider Box & Presets -->
        <div class="speed-control-box">
          <div class="speed-box-header">
            <span class="speed-box-title">MOTOR SPEED PWM (BTS7960)</span>
            <span class="speed-box-readout" id="speed-val-display">83% (850 PWM)</span>
          </div>

          <input type="range" min="300" max="1023" value="850" class="custom-range-slider" id="speed-slider"
            oninput="onSpeedSliderChange(this.value)">

          <div class="speed-presets-row">
            <button class="btn-speed-preset" onclick="setSpeedPreset(512, this)">50%</button>
            <button class="btn-speed-preset" onclick="setSpeedPreset(768, this)">75%</button>
            <button class="btn-speed-preset active" onclick="setSpeedPreset(850, this)">83%</button>
            <button class="btn-speed-preset" onclick="setSpeedPreset(1023, this)">100%</button>
          </div>
        </div>

        <!-- Permanently Revealed Motion Timing & Calibration (No Hiding / No Arrow) -->
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

  <!-- Wi-Fi & Hawa Wireless Configuration Modal -->
  <div class="modal-backdrop" id="wifi-modal-backdrop" onclick="closeWifiModalOnBackdrop(event)">
    <div class="modal-dialog">
      <div class="modal-header">
        <div class="titlebar-left">
          <div class="window-dots">
            <span class="window-square-dot dot-red"></span>
            <span class="window-square-dot dot-amber"></span>
            <span class="window-square-dot dot-green"></span>
          </div>
          <span class="modal-title">WIRELESS &amp; HAWA OTA</span>
        </div>
        <button class="btn-modal-close" onclick="closeWifiModal()" title="Close">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
            <line x1="18" y1="6" x2="6" y2="18"></line>
            <line x1="6" y1="6" x2="18" y2="18"></line>
          </svg>
        </button>
      </div>

      <div class="modal-body">
        <!-- Live Telemetry Card -->
        <div class="telemetry-card">
          <div class="telemetry-row">
            <span class="telemetry-label">Wi-Fi Status</span>
            <span class="telemetry-val">
              <span class="telemetry-badge badge-offline" id="modal-wifi-badge">AP ONLY</span>
              <span id="modal-wifi-ssid">Project-CLAW</span>
            </span>
          </div>
          <div class="telemetry-row">
            <span class="telemetry-label">Station IP</span>
            <span class="telemetry-val" id="modal-station-ip">—</span>
          </div>
          <div class="telemetry-row">
            <span class="telemetry-label">Access Point IP</span>
            <span class="telemetry-val" id="modal-ap-ip">192.168.4.1</span>
          </div>
          <div class="telemetry-row">
            <span class="telemetry-label">Hawa Cloud Link</span>
            <span class="telemetry-val">
              <span class="telemetry-badge badge-offline" id="modal-hawa-badge">STANDBY</span>
            </span>
          </div>
          <div class="telemetry-row">
            <span class="telemetry-label">Hardware Device ID</span>
            <span class="telemetry-val">
              <code class="device-id-code" id="modal-device-id">hawa-esp8266-claw</code>
              <button class="btn-inline-action" style="padding: 2px 6px; font-size: 9px;" onclick="copyDeviceId()" title="Copy Device ID">COPY</button>
            </span>
          </div>
        </div>

        <!-- Wi-Fi Network Provisioning Form -->
        <div class="field-group">
          <div class="field-label">
            <span>Wi-Fi Network (SSID)</span>
            <button class="btn-inline-action" id="btn-scan-networks" onclick="scanWifiNetworks()" title="Scan Nearby Wi-Fi">
              <svg id="scan-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
                <path d="M21.5 2v6h-6M21.34 15.57a10 10 0 1 1-.57-8.38l5.67-5.67"/>
              </svg>
              <span id="scan-btn-text">SCAN</span>
            </button>
          </div>
          <!-- Scanned Networks Dropdown Container -->
          <div class="scanned-networks-box" id="scanned-networks-box" style="display: none;"></div>
          <input type="text" class="form-control-input" id="input-wifi-ssid" placeholder="Enter Wi-Fi SSID" autocomplete="off" autocapitalize="none">
        </div>

        <div class="field-group">
          <div class="field-label">
            <span>Wi-Fi Password</span>
            <button class="btn-inline-action" onclick="togglePasswordVisibility()" title="Show/Hide Password" id="btn-toggle-pass" style="padding: 2px 6px; font-size: 9px;">SHOW</button>
          </div>
          <input type="password" class="form-control-input" id="input-wifi-pass" placeholder="Enter network password" autocomplete="off">
        </div>

        <div class="field-group">
          <div class="field-label">
            <span>Hawa Hub Gateway URL (Optional)</span>
          </div>
          <input type="text" class="form-control-input" id="input-hawa-server" placeholder="wss://your-hawa-hub.com/ws" autocomplete="off">
        </div>

        <div class="field-group">
          <div class="field-label">
            <span>Device Nickname (Optional)</span>
          </div>
          <input type="text" class="form-control-input" id="input-device-name" placeholder="Project-CLAW" autocomplete="off">
        </div>

        <!-- Modal Actions Footer -->
        <div class="modal-footer-actions">
          <button class="btn-modal-primary" onclick="saveWifiCredentials()">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
              <polyline points="20 6 9 17 4 12"></polyline>
            </svg>
            <span>CONNECT &amp; SAVE WI-FI</span>
          </button>
          <div class="modal-secondary-row">
            <button class="btn-modal-subtle" onclick="clearWifiCredentials()" title="Disconnect & Reset to AP">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="width:12px; height:12px;">
                <line x1="18" y1="6" x2="6" y2="18"></line>
                <line x1="6" y1="6" x2="18" y2="18"></line>
              </svg>
              <span>CLEAR WI-FI</span>
            </button>
            <button class="btn-modal-subtle" onclick="rebootHardware()" title="Reboot Hardware">
              <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="width:12px; height:12px;">
                <path d="M23 4v6h-6"></path>
                <path d="M20.49 15a9 9 0 1 1-2.12-9.36L23 10"></path>
              </svg>
              <span>REBOOT</span>
            </button>
          </div>
        </div>
      </div>
    </div>
  </div>

  <!-- Toast Notification -->
  <div class="toast-notice" id="toast-notice">Settings Applied</div>

  <script>
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

</script>
</body>

</html>
)rawliteral";
