Smart Auto Watering System (Arduino / ESP8266)

Smart Auto Watering System เป็นโปรเจคระบบรดน้ำต้นไม้อัตโนมัติ พัฒนาด้วย ESP8266 (NodeMCU)
รองรับการควบคุมผ่าน Web Dashboard ทั้งโหมด Manual และ Automatic
เหมาะสำหรับการเรียนรู้และประยุกต์ใช้งานด้าน IoT, Embedded Systems และ Smart Agriculture

Project Overview

ระบบนี้ออกแบบมาเพื่อช่วยลดภาระในการดูแลต้นไม้ โดยใช้เซนเซอร์วัดความชื้นในดินเป็นตัวตัดสินใจการรดน้ำ
ผู้ใช้สามารถเข้าถึงระบบผ่านเว็บเบราว์เซอร์ภายในเครือข่ายเดียวกันกับอุปกรณ์

Key Features

Web-based control dashboard

Automatic watering mode based on soil moisture

Manual pump control

Real-time soil moisture monitoring

Real-time data visualization

State machine–based control logic

Embedded web server on ESP8266

System Architecture

Controller: ESP8266 (NodeMCU)

Sensor: Soil Moisture Sensor (Analog)

Actuator: Relay Module + Water Pump

Interface: Embedded Web Server (HTML, CSS, JavaScript)

Hardware Requirements
Component	Description
ESP8266	NodeMCU Development Board
Soil Moisture Sensor	Analog type
Relay Module	Low-level trigger
Water Pump	DC water pump
Power Supply	5V / 12V (depending on hardware)
Pin Mapping
Device	ESP8266 Pin
Relay Module	D2
Soil Moisture Sensor	A0
Control Logic Design

ระบบใช้แนวคิด Finite State Machine (FSM) เพื่อควบคุมการทำงาน ช่วยลดความซับซ้อนของโค้ดและเพิ่มความเสถียร

States

MANUAL

ผู้ใช้ควบคุมปั๊มน้ำผ่านเว็บ

ระบบไม่ทำการตัดสินใจอัตโนมัติ

AUTO

ระบบอ่านค่าความชื้นดินเป็นระยะ

เปิดหรือปิดปั๊มน้ำอัตโนมัติตามค่า Threshold

Automatic Watering Algorithm

อ่านค่า Analog จาก Soil Moisture Sensor

แปลงค่าเป็นเปอร์เซ็นต์ความชื้น

เปรียบเทียบกับค่า Threshold

สั่งงาน Relay เพื่อเปิดหรือปิดปั๊มน้ำ

ค่าพื้นฐาน:

Soil Moisture Threshold: 40%

Sampling Interval: 10 seconds

Web API Endpoints
Endpoint	Method	Description
/	GET	Web dashboard
/data	GET	Sensor data (JSON)
/on	GET	Turn pump ON (manual mode)
/off	GET	Turn pump OFF (manual mode)
/auto/on	GET	Enable automatic mode
/auto/off	GET	Disable automatic mode
Web Dashboard

แสดงข้อมูลความชื้นดินแบบเรียลไทม์

แสดงกราฟข้อมูลด้วย Chart.js

แสดงสถานะปั๊มน้ำ

แสดงโหมดการทำงานของระบบ

อัปเดตข้อมูลทุกช่วงเวลา

Getting Started
1. Clone Repository
git clone https://github.com/Rmutt2005/arduino-mini-project-auto-waterring-system.git

2. Configure Wi-Fi

แก้ไขค่า Wi-Fi ภายในไฟล์ .ino

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

3. Upload Code

เปิดไฟล์ .ino ด้วย Arduino IDE

เลือก Board: NodeMCU 1.0 (ESP-12E Module)

Upload โค้ดไปยัง ESP8266

4. Access Web Interface

เปิด Serial Monitor เพื่อดู IP Address

เข้าใช้งานผ่านเว็บเบราว์เซอร์

http://<ESP8266-IP>

Project Structure
arduino-mini-project-auto-waterring-system/
│
├── auto_watering.ino
└── README.md

Learning Outcomes

Embedded web server development

IoT system design

State machine implementation

Sensor data processing

Hardware and software integration

Future Improvements

Cloud data logging

Mobile application support

MQTT or Firebase integration

Multiple sensor support

Weather-based watering logic

Author

Arduino Mini IoT Project
Developed for learning and internship preparation

License

This project is developed for educational purposes.
