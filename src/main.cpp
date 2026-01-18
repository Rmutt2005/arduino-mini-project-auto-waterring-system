#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
 
// --- Wi-Fi ---
const char* ssid = "G12";      
const char* password = "12345678t";  
 
// --- Pin Setup ---
const int RELAY_PIN = D2;    // Low-Level Trigger
const int MOISTURE_PIN = A0; // Analog Input
 
// --- Web Server ---
ESP8266WebServer server(80);
 
// --- State Machine ---
enum ControlState {
  STATE_MANUAL,
  STATE_AUTO
};
ControlState currentState = STATE_MANUAL; // เริ่มต้นที่โหมด Manual
 
// Pump status (สำหรับรายงานหน้าเว็บ)
bool pumpStatus = false;
 
// --- Automatic Watering ---
const int MOISTURE_THRESHOLD = 40; // % ต่ำกว่าจุดนี้จะรดน้ำ
const unsigned long AUTO_WATER_INTERVAL = 10000; // ตรวจทุก 10 วินาที
unsigned long lastAutoWater = 0;
 
// --- Read Moisture ---
int readMoisture() {
  return analogRead(MOISTURE_PIN);
}
 
// --- Automatic Watering Function ---
// (เปลี่ยนชื่อจาก autoWatering เป็น runAutoWateringLogic เพื่อให้ชัดเจน)
void runAutoWateringLogic() {
  int moistureRaw = readMoisture();
  int moisturePercent = map(moistureRaw, 1023, 500, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);
 
  if (moisturePercent < MOISTURE_THRESHOLD) {
    digitalWrite(RELAY_PIN, HIGH); // เปิดปั๊ม
    pumpStatus = true;
  } else {
    digitalWrite(RELAY_PIN, LOW); // ปิดปั๊ม
    pumpStatus = false;
  }
 
  Serial.print("Auto Check: Moisture=");
  Serial.print(moisturePercent);
  Serial.print("%, Pump=");
  Serial.println(pumpStatus ? "ON" : "OFF");
}
 
// --- Serve Main Page ---
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>🌱 Smart Watering System</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Kanit:wght@300;400;500;600;700&display=swap" rel="stylesheet">
<style>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}
 
:root {
  --primary: #2d6a4f;
  --primary-light: #40916c;
  --primary-dark: #1b4332;
  --secondary: #52b788;
  --accent: #74c69d;
  --danger: #e63946;
  --warning: #f77f00;
  --success: #06ffa5;
  --bg-gradient: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  --card-bg: rgba(255, 255, 255, 0.95);
  --shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
  --shadow-hover: 0 12px 48px rgba(0, 0, 0, 0.15);
}
 
body {
  font-family: 'Kanit', sans-serif;
  background: linear-gradient(135deg, #73ff91ff 0%, #91ffbfff 50%, #9dfff7ff 100%);
  min-height: 100vh;
  padding: 20px;
  position: relative;
  overflow-x: hidden;
}
 
body::before {
  content: '';
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background:
    radial-gradient(circle at 20% 50%, rgba(102, 126, 234, 0.2) 0%, transparent 50%),
    radial-gradient(circle at 80% 80%, rgba(118, 75, 162, 0.2) 0%, transparent 50%);
  pointer-events: none;
  z-index: 0;
}
 
.container {
  max-width: 1200px;
  margin: 0 auto;
  position: relative;
  z-index: 1;
}
 
header {
  text-align: center;
  margin-bottom: 40px;
  animation: fadeInDown 0.8s ease;
}
 
h1 {
  color: #ffffff;
  font-size: 3rem;
  font-weight: 700;
  text-shadow: 0 4px 20px rgba(0, 0, 0, 0.2);
  margin-bottom: 10px;
  letter-spacing: -1px;
}
 
.subtitle {
  color: rgba(255, 255, 255, 0.9);
  font-size: 1.2rem;
  font-weight: 300;
}
 
.dashboard {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 24px;
  margin-bottom: 24px;
}
 
.card {
  background: var(--card-bg);
  backdrop-filter: blur(10px);
  border-radius: 24px;
  padding: 28px;
  box-shadow: var(--shadow);
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  border: 1px solid rgba(255, 255, 255, 0.5);
  animation: fadeInUp 0.8s ease;
}
 
.card:hover {
  transform: translateY(-4px);
  box-shadow: var(--shadow-hover);
}
 
.card-header {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 20px;
  padding-bottom: 16px;
  border-bottom: 2px solid rgba(0, 0, 0, 0.05);
}
 
.card-icon {
  font-size: 2rem;
  filter: drop-shadow(0 2px 4px rgba(0, 0, 0, 0.1));
}
 
.card-title {
  font-size: 1.4rem;
  font-weight: 600;
  color: var(--primary-dark);
}
 
.status-badge {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  padding: 12px 20px;
  border-radius: 50px;
  font-weight: 500;
  font-size: 1.1rem;
  margin: 8px 0;
  transition: all 0.3s ease;
}
 
.status-on {
  background: linear-gradient(135deg, #06ffa5 0%, #00d4aa 100%);
  color: var(--primary-dark);
  box-shadow: 0 4px 15px rgba(6, 255, 165, 0.3);
}
 
.status-off {
  background: linear-gradient(135deg, #ff6b6b 0%, #ee5a6f 100%);
  color: white;
  box-shadow: 0 4px 15px rgba(255, 107, 107, 0.3);
}
 
.pulse {
  animation: pulse 2s cubic-bezier(0.4, 0, 0.6, 1) infinite;
}
 
@keyframes pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.7; }
}
 
.controls {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
  margin-top: 20px;
}
 
button {
  flex: 1;
  min-width: 140px;
  padding: 16px 24px;
  font-size: 1.1rem;
  font-weight: 500;
  font-family: 'Kanit', sans-serif;
  border: none;
  border-radius: 16px;
  cursor: pointer;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
  position: relative;
  overflow: hidden;
}
 
button::before {
  content: '';
  position: absolute;
  top: 50%;
  left: 50%;
  width: 0;
  height: 0;
  border-radius: 50%;
  background: rgba(255, 255, 255, 0.3);
  transform: translate(-50%, -50%);
  transition: width 0.6s, height 0.6s;
}
 
button:active::before {
  width: 300px;
  height: 300px;
}
 
.btn-on {
  background: linear-gradient(135deg, #16b7bdff 0%, #3ebabeff 100%);
  color: white;
}
 
.btn-on:hover {
  background: linear-gradient(135deg, #71d7ffff 0%, #4fd3ebff 100%);
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(82, 183, 136, 0.4);
}
 
.btn-off {
  background: linear-gradient(135deg, #ff6b6b 0%, #ee5a6f 100%);
  color: white;
}
 
.btn-off:hover {
  background: linear-gradient(135deg, #ee5a6f 0%, #e63946 100%);
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(230, 57, 70, 0.4);
}
 
.btn-auto-on {
  background: linear-gradient(135deg, #3dd63dff 0%, #5fc271ff 100%);
  color: white;
}
 
.btn-auto-on:hover {
  background: linear-gradient(135deg, #85d841ff 0%, #83f079ff 100%);
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(102, 126, 234, 0.4);
}
 
.btn-auto-off {
  background: linear-gradient(135deg, #bd2da5ff 0%, #bf76c2ff 100%);
  color: white;
}
 
.btn-auto-off:hover {
  background: linear-gradient(135deg, #ff4fb5ff 0%, #ffb4e6ff 100%);
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(240, 147, 251, 0.4);
}
 
.moisture-display {
  text-align: center;
  margin: 24px 0;
}
 
.moisture-value {
  font-size: 4rem;
  font-weight: 700;
  background: linear-gradient(135deg, var(--primary) 0%, var(--secondary) 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  margin: 16px 0;
  text-shadow: 0 2px 10px rgba(0, 0, 0, 0.05);
}
 
.moisture-label {
  font-size: 1.2rem;
  color: #666;
  font-weight: 400;
}
 
.moisture-status {
  display: inline-block;
  padding: 8px 20px;
  border-radius: 50px;
  font-weight: 500;
  margin-top: 12px;
  background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.1) 100%);
  border: 2px solid rgba(102, 126, 234, 0.3);
  color: var(--primary-dark);
}
 
.chart-container {
  margin-top: 24px;
  padding: 20px;
  background: white;
  border-radius: 16px;
  box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.05);
}
 
canvas {
  max-width: 100%;
  height: auto !important;
}
 
.info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
  gap: 16px;
  margin-top: 20px;
}
 
.info-item {
  text-align: center;
  padding: 16px;
  background: linear-gradient(135deg, rgba(255, 255, 255, 0.8) 0%, rgba(255, 255, 255, 0.4) 100%);
  border-radius: 12px;
  border: 1px solid rgba(255, 255, 255, 0.6);
}
 
.info-value {
  font-size: 1.8rem;
  font-weight: 700;
  color: var(--primary);
  margin-bottom: 4px;
}
 
.info-label {
  font-size: 0.9rem;
  color: #666;
  font-weight: 400;
}
 
footer {
  text-align: center;
  margin-top: 40px;
  padding: 20px;
  color: rgba(255, 255, 255, 0.8);
  font-size: 0.9rem;
}
 
@keyframes fadeInDown {
  from {
    opacity: 0;
    transform: translateY(-30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}
 
@keyframes fadeInUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}
 
@media (max-width: 768px) {
  h1 {
    font-size: 2rem;
  }
 
  .dashboard {
    grid-template-columns: 1fr;
  }
 
  .controls {
    flex-direction: column;
  }
 
  button {
    width: 100%;
  }
 
  .moisture-value {
    font-size: 3rem;
  }
}
 
.loading {
  display: inline-block;
  width: 20px;
  height: 20px;
  border: 3px solid rgba(255, 255, 255, 0.3);
  border-radius: 50%;
  border-top-color: white;
  animation: spin 1s ease-in-out infinite;
}
 
@keyframes spin {
  to { transform: rotate(360deg); }
}
 
/* Plant Character Styles */
.plant-container {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 20px;
  min-height: 300px;
}
 
.plant {
  position: relative;
  width: 150px;
  height: 250px;
  display: flex;
  flex-direction: column;
  align-items: center;
  transition: all 0.5s ease;
}
 
.plant-face {
  position: absolute;
  top: 30px;
  width: 80px;
  height: 80px;
  background: linear-gradient(135deg, #8bc34a 0%, #689f38 100%);
  border-radius: 50%;
  z-index: 10;
  transition: all 0.5s ease;
  animation: bob 3s ease-in-out infinite;
}
 
@keyframes bob {
  0%, 100% { transform: translateY(0px); }
  50% { transform: translateY(-10px); }
}
 
.eye {
  position: absolute;
  width: 16px;
  height: 20px;
  background: white;
  border-radius: 50%;
  top: 25px;
  animation: blink 4s infinite;
}
 
.eye.left { left: 18px; }
.eye.right { right: 18px; }
 
.pupil {
  position: absolute;
  width: 8px;
  height: 10px;
  background: #333;
  border-radius: 50%;
  top: 7px;
  left: 4px;
  animation: look 3s ease-in-out infinite;
}
 
@keyframes blink {
  0%, 98%, 100% { height: 20px; }
  99% { height: 2px; }
}
 
@keyframes look {
  0%, 100% { transform: translateX(0); }
  50% { transform: translateX(2px); }
}
 
.mouth {
  position: absolute;
  width: 30px;
  height: 15px;
  border: 3px solid #333;
  border-top: none;
  border-radius: 0 0 30px 30px;
  bottom: 15px;
  left: 50%;
  transform: translateX(-50%);
  transition: all 0.5s ease;
}
 
.leaves {
  position: absolute;
  top: 0;
  width: 100%;
  height: 120px;
}
 
.leaf {
  position: absolute;
  width: 50px;
  height: 70px;
  background: linear-gradient(135deg, #8bc34a 0%, #689f38 100%);
  border-radius: 0 50% 50% 0;
  transition: all 0.5s ease;
}
 
.leaf-1 {
  top: 10px;
  left: -20px;
  transform: rotate(-30deg);
  animation: sway1 3s ease-in-out infinite;
}
 
.leaf-2 {
  top: 20px;
  right: -20px;
  transform: rotate(30deg) scaleX(-1);
  animation: sway2 3s ease-in-out infinite 0.5s;
}
 
.leaf-3 {
  top: 0px;
  left: 50%;
  transform: translateX(-50%) rotate(-90deg);
  animation: sway3 3s ease-in-out infinite 1s;
}
 
@keyframes sway1 {
  0%, 100% { transform: rotate(-30deg); }
  50% { transform: rotate(-20deg); }
}
 
@keyframes sway2 {
  0%, 100% { transform: rotate(30deg) scaleX(-1); }
  50% { transform: rotate(40deg) scaleX(-1); }
}
 
@keyframes sway3 {
  0%, 100% { transform: translateX(-50%) rotate(-90deg); }
  50% { transform: translateX(-50%) rotate(-85deg); }
}
 
.stem {
  position: absolute;
  top: 110px;
  width: 15px;
  height: 90px;
  background: linear-gradient(to bottom, #689f38 0%, #558b2f 100%);
  border-radius: 10px;
  transition: all 0.5s ease;
}
 
.pot {
  position: absolute;
  bottom: 0;
  width: 80px;
  height: 50px;
  background: linear-gradient(135deg, #d84315 0%, #bf360c 100%);
  border-radius: 0 0 10px 10px;
  clip-path: polygon(15% 0%, 85% 0%, 100% 100%, 0% 100%);
  box-shadow: 0 4px 10px rgba(0,0,0,0.2);
}
 
.plant-status-text {
  margin-top: 20px;
  font-size: 1.3rem;
  font-weight: 600;
  color: var(--primary-dark);
  text-align: center;
  transition: all 0.5s ease;
}
 
/* Plant States */
.plant.very-dry .plant-face,
.plant.very-dry .leaf {
  background: linear-gradient(135deg, #a1887f 0%, #795548 100%);
}
 
.plant.very-dry .stem {
  background: linear-gradient(to bottom, #8d6e63 0%, #6d4c41 100%);
}
 
.plant.very-dry .mouth {
  border-radius: 30px 30px 0 0;
  border-top: 3px solid #333;
  border-bottom: none;
  bottom: 20px;
}
 
.plant.very-dry {
  animation: droop 2s ease-in-out infinite;
}
 
@keyframes droop {
  0%, 100% { transform: rotate(0deg); }
  50% { transform: rotate(-5deg); }
}
 
.plant.moderate .plant-face,
.plant.moderate .leaf {
  background: linear-gradient(135deg, #9ccc65 0%, #7cb342 100%);
}
 
.plant.moderate .stem {
  background: linear-gradient(to bottom, #7cb342 0%, #689f38 100%);
}
 
.plant.wet .plant-face,
.plant.wet .leaf {
  background: linear-gradient(135deg, #66bb6a 0%, #43a047 100%);
  filter: brightness(1.1);
}
 
.plant.wet .stem {
  background: linear-gradient(to bottom, #43a047 0%, #388e3c 100%);
}
 
.plant.wet .mouth {
  width: 35px;
  height: 18px;
}
 
.plant.wet {
  animation: happy-bounce 1.5s ease-in-out infinite;
}
 
@keyframes happy-bounce {
  0%, 100% { transform: translateY(0) scale(1); }
  50% { transform: translateY(-5px) scale(1.02); }
}
 
.plant.very-wet .plant-face,
.plant.very-wet .leaf {
  background: linear-gradient(135deg, #4caf50 0%, #2e7d32 100%);
  filter: brightness(1.2) drop-shadow(0 0 10px rgba(76, 175, 80, 0.5));
}
 
.plant.very-wet .stem {
  background: linear-gradient(to bottom, #2e7d32 0%, #1b5e20 100%);
}
 
.plant.very-wet .mouth {
  width: 40px;
  height: 20px;
}
 
.plant.very-wet {
  animation: excited-wiggle 1s ease-in-out infinite;
}
 
@keyframes excited-wiggle {
  0%, 100% { transform: rotate(0deg) translateY(0); }
  25% { transform: rotate(-3deg) translateY(-3px); }
  75% { transform: rotate(3deg) translateY(-3px); }
}
</style>
</head>
<body>
<div class="container">
  <header>
    <h1>🌱 Smart Watering System</h1>
    <p class="subtitle">ระบบรดน้ำอัจฉริยะ</p>
  </header>
 
  <div class="dashboard">
    <div class="card">
      <div class="card-header">
        <span class="card-icon">💧</span>
        <h2 class="card-title">ควบคุมปั๊มน้ำ</h2>
      </div>
      <div id="pumpStatus" class="status-badge status-off">
        <span class="loading"></span> กำลังโหลด...
      </div>
      <div class="controls">
        <button class="btn-on" onclick="togglePump('on')">
          ⚡ เปิดปั๊ม
        </button>
        <button class="btn-off" onclick="togglePump('off')">
          🛑 ปิดปั๊ม
        </button>
      </div>
    </div>
 
    <div class="card">
      <div class="card-header">
        <span class="card-icon">🤖</span>
        <h2 class="card-title">ระบบอัตโนมัติ</h2>
      </div>
      <div id="autoStatus" class="status-badge status-off">
        <span class="loading"></span> กำลังโหลด...
      </div>
      <div class="controls">
        <button class="btn-auto-on" onclick="toggleAuto('on')">
          🌟 เปิด Auto
        </button>
        <button class="btn-auto-off" onclick="toggleAuto('off')">
          ⏸️ ปิด Auto
        </button>
      </div>
    </div>
 
    <div class="card">
      <div class="card-header">
        <span class="card-icon">🌡️</span>
        <h2 class="card-title">ความชื้นดิน</h2>
      </div>
      <div class="moisture-display">
        <div class="moisture-label">ค่าความชื้นปัจจุบัน</div>
        <div class="moisture-value" id="moistureValue">--</div>
        <div class="moisture-status" id="moistureStatus">กำลังโหลด...</div>
      </div>
    </div>
  </div>
 
  <div class="card">
    <div class="card-header">
      <span class="card-icon">🌿</span>
      <h2 class="card-title">สถานะต้นไม้</h2>
    </div>
    <div class="plant-container">
      <div class="plant" id="animatedPlant">
        <div class="plant-face" id="plantFace">
          <div class="eye left">
            <div class="pupil"></div>
          </div>
          <div class="eye right">
            <div class="pupil"></div>
          </div>
          <div class="mouth" id="plantMouth"></div>
        </div>
        <div class="leaves">
          <div class="leaf leaf-1"></div>
          <div class="leaf leaf-2"></div>
          <div class="leaf leaf-3"></div>
        </div>
        <div class="stem"></div>
        <div class="pot"></div>
      </div>
      <div class="plant-status-text" id="plantStatusText">กำลังโหลด...</div>
    </div>
  </div>
 
  <div class="card">
    <div class="card-header">
      <span class="card-icon">📊</span>
      <h2 class="card-title">กราฟความชื้นแบบ Real-time</h2>
    </div>
    <div class="chart-container">
      <canvas id="moistureChart"></canvas>
    </div>
  </div>
 
  <footer>
    <p>🌿 Smart Watering System | Made with กลุ่ม2คน💚</p>
  </footer>
</div>
 
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script>
// Initialize Chart
const ctx = document.getElementById('moistureChart').getContext('2d');
const moistureChart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: [],
    datasets: [{
      label: '💚 ความชื้นดิน (%)',
      data: [],
      borderColor: '#52b788',
      backgroundColor: 'rgba(82, 183, 136, 0.1)',
      borderWidth: 3,
      fill: true,
      tension: 0.4,
      pointRadius: 4,
      pointBackgroundColor: '#52b788',
      pointBorderColor: '#fff',
      pointBorderWidth: 2,
      pointHoverRadius: 6
    }]
  },
  options: {
    responsive: true,
    maintainAspectRatio: true,
    animation: {
      duration: 750,
      easing: 'easeInOutQuart'
    },
    scales: {
      y: {
        min: 0,
        max: 100,
        ticks: {
          callback: function(value) {
            return value + '%';
          },
          font: {
            size: 12,
            family: "'Kanit', sans-serif"
          }
        },
        grid: {
          color: 'rgba(0, 0, 0, 0.05)'
        }
      },
      x: {
        ticks: {
          font: {
            size: 11,
            family: "'Kanit', sans-serif"
          }
        },
        grid: {
          color: 'rgba(0, 0, 0, 0.05)'
        }
      }
    },
    plugins: {
      legend: {
        labels: {
          font: {
            size: 14,
            family: "'Kanit', sans-serif"
          }
        }
      }
    }
  }
});
 
// Update Data Function
function updateData() {
  fetch('/data')
    .then(response => response.json())
    .then(data => {
      // Update Pump Status
      const pumpStatusEl = document.getElementById('pumpStatus');
      if (data.pump) {
        pumpStatusEl.className = 'status-badge status-on pulse';
        pumpStatusEl.innerHTML = '💦 ปั๊มกำลังทำงาน (ON)';
      } else {
        pumpStatusEl.className = 'status-badge status-off';
        pumpStatusEl.innerHTML = '⭕ ปั๊มปิดอยู่ (OFF)';
      }
 
      // Update Auto Status
      const autoStatusEl = document.getElementById('autoStatus');
      if (data.auto) {
        autoStatusEl.className = 'status-badge status-on pulse';
        autoStatusEl.innerHTML = '🤖 Auto-Watering: ON';
      } else {
        autoStatusEl.className = 'status-badge status-off';
        autoStatusEl.innerHTML = '⏸️ Auto-Watering: OFF';
      }
 
      // Update Moisture Display
      document.getElementById('moistureValue').textContent = data.moisture + '%';
      document.getElementById('moistureStatus').textContent = '🌱 ' + data.moistureText;
 
      // Update Animated Plant
      const plant = document.getElementById('animatedPlant');
      const plantStatusText = document.getElementById('plantStatusText');
     
      // Remove all state classes
      plant.classList.remove('very-dry', 'moderate', 'wet', 'very-wet');
     
      // Add appropriate class based on moisture level
      if (data.moisture < 20) {
        plant.classList.add('very-dry');
        plantStatusText.textContent = '😢 ต้นไม้กำลังเหี่ยว! ต้องการน้ำด่วน!';
        plantStatusText.style.color = '#d84315';
      } else if (data.moisture < 50) {
        plant.classList.add('moderate');
        plantStatusText.textContent = '😐 ต้นไม้ต้องการน้ำเพิ่มเติม';
        plantStatusText.style.color = '#f57c00';
      } else if (data.moisture < 80) {
        plant.classList.add('wet');
        plantStatusText.textContent = '😊 ต้นไม้มีความสุข! สุขภาพดี!';
        plantStatusText.style.color = '#388e3c';
      } else {
        plant.classList.add('very-wet');
        plantStatusText.textContent = '🤩 ต้นไม้สดชื่นมาก! เยี่ยมเลย!';
        plantStatusText.style.color = '#1b5e20';
      }
 
      // Update Chart
      const now = new Date().toLocaleTimeString('th-TH', {
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit'
      });
     
      moistureChart.data.labels.push(now);
      moistureChart.data.datasets[0].data.push(data.moisture);
     
      // Keep only last 10 data points
      if (moistureChart.data.labels.length > 10) {
        moistureChart.data.labels.shift();
        moistureChart.data.datasets[0].data.shift();
      }
     
      moistureChart.update('none'); // Update without animation for smoother performance
    })
    .catch(error => {
      console.error('Error fetching data:', error);
    });
}
 
// Update every 2 seconds
setInterval(updateData, 2000);
updateData(); // Initial call
 
// Control Functions
function togglePump(cmd) {
  fetch('/' + cmd)
    .then(response => {
      if (response.ok) {
        updateData(); // Immediate update
      }
    })
    .catch(error => console.error('Error:', error));
}
 
function toggleAuto(cmd) {
  fetch('/auto/' + cmd)
    .then(response => {
      if (response.ok) {
        updateData(); // Immediate update
      }
    })
    .catch(error => console.error('Error:', error));
}
</script>
</body>
</html>
)rawliteral";
 
  server.send(200, "text/html; charset=UTF-8", html);
}
 
// --- Serve JSON Data ---
void handleData() {
  int moistureRaw = readMoisture();
  int moisturePercent = map(moistureRaw, 1023, 500, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);
 
  String moistureText;
  if (moisturePercent < 20) moistureText = "แห้งมาก";
  else if (moisturePercent < 50) moistureText = "ปานกลาง";
  else if (moisturePercent < 80) moistureText = "ชื้น";
  else moistureText = "เปียกมาก";
 
  String json = "{";
  json += "\"moisture\":" + String(moisturePercent) + ",";
  json += "\"moistureText\":\"" + moistureText + "\",";
  json += "\"pump\":" + String(pumpStatus ? "true" : "false") + ",";
  // **MODIFIED:** ส่งสถานะ Auto โดยอิงจาก State Machine
  json += "\"auto\":" + String(currentState == STATE_AUTO ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}
 
// --- Manual Pump Control ---
void handlePumpOn() {
  // **MODIFIED:** ทำงานเฉพาะเมื่ออยู่ในโหมด MANUAL
  if (currentState == STATE_MANUAL) {
    digitalWrite(RELAY_PIN, HIGH);
    pumpStatus = true;
    Serial.println("Manual Pump ON");
  } else {
    Serial.println("Manual pump command ignored (AUTO mode on)");
  }
  server.send(200, "text/plain", "OK");
}
 
void handlePumpOff() {
  // **MODIFIED:** ทำงานเฉพาะเมื่ออยู่ในโหมด MANUAL
  if (currentState == STATE_MANUAL) {
    digitalWrite(RELAY_PIN, LOW);
    pumpStatus = false;
    Serial.println("Manual Pump OFF");
  } else {
    Serial.println("Manual pump command ignored (AUTO mode on)");
  }
  server.send(200, "text/plain", "OK");
}
 
// --- Auto-Watering Toggle (State Transitions) ---
void handleAutoOn() {
  // **MODIFIED:** นี่คือการ "เปลี่ยนสถานะ" เป็น AUTO
  if (currentState != STATE_AUTO) {
    currentState = STATE_AUTO;
    Serial.println("State changed to: AUTO");
    // เมื่อเปิดโหมด Auto ให้รันการตรวจสอบ 1 ครั้งทันที
    lastAutoWater = millis(); // รีเซ็ตไทม์เมอร์
    runAutoWateringLogic();
  }
  server.send(200, "text/plain", "OK");
}
 
void handleAutoOff() {
  // **MODIFIED:** นี่คือการ "เปลี่ยนสถานะ" เป็น MANUAL
  if (currentState != STATE_MANUAL) {
    currentState = STATE_MANUAL;
    Serial.println("State changed to: MANUAL");
    // **สำคัญ:** เมื่อกลับมาโหมด Manual, สั่งปิดปั๊มก่อนเพื่อความปลอดภัย
    digitalWrite(RELAY_PIN, LOW);
    pumpStatus = false;
    Serial.println("Pump OFF (Switched to Manual)");
  }
  server.send(200, "text/plain", "OK");
}
 
// --- Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // เริ่มต้นโดยการปิดปั๊ม
 
  WiFi.begin(ssid, password);
  Serial.print("Connecting Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("Web Server: http://");
  Serial.println(WiFi.localIP());
 
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/on", handlePumpOn);
  server.on("/off", handlePumpOff);
  server.on("/auto/on", handleAutoOn);
  server.on("/auto/off", handleAutoOff);
 
  server.begin();
  Serial.println("System started in MANUAL mode.");
}
 
// --- Loop ---
void loop() {
  server.handleClient();
 
  // **MODIFIED:** นี่คือหัวใจของ State Machine
  switch (currentState) {
    case STATE_MANUAL:
      // เมื่ออยู่ในโหมด Manual, loop() ไม่ต้องทำอะไร
      // การควบคุมทั้งหมดมาจาก Web Handlers
      break;
   
    case STATE_AUTO:
      // เมื่ออยู่ในโหมด Auto, ตรวจสอบไทม์เมอร์และรัน logic
      if (millis() - lastAutoWater > AUTO_WATER_INTERVAL) {
        lastAutoWater = millis();
        runAutoWateringLogic();
      }
      break;
  }
}