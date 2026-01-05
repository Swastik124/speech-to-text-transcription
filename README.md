# 🥗 Smart Cafeteria Ordering System using ESP32, Bluetooth & WiFi

A smart **voice-assisted cafeteria ordering system** built with **ESP32**, enabling hands-free order placement using Bluetooth voice commands, live menu fetching from a PC server, and real-time order monitoring via a web dashboard.

This project is ideal for **IoT + Speech-to-Text + Smart Automation** use-cases in cafeterias, restaurants and college canteens.

---

## ✨ Features

* 🎙️ **Bluetooth Voice Command Input**
  Receives food codes via mobile Bluetooth voice recognition.

* 🌐 **Live Menu Fetching from Server**
  Automatically loads menu from PC server using HTTP + JSON.

* 🧾 **Dynamic Order Management**

  * Add items by food code
  * Delete items using *DELETE → FoodCode*
  * Clear current order using *CLEAR*
  * Place order using *DONE*

* 📊 **Real-Time Web Dashboard**

  * View live orders on browser
  * Auto refresh every 5 seconds
  * Displays menu, current order & full order history

* 💾 **Order Storage on PC Server**
  Orders are sent to a backend server and saved for record keeping.

---

## 🛠 Hardware Used

| Component   | Purpose                       |
| ----------- | ----------------------------- |
| ESP32       | Main controller               |
| Smartphone  | Voice to Text Bluetooth input |
| PC / Laptop | Backend Server                |
| WiFi Router | Network communication         |

---

## 🧠 System Architecture

```
User Voice → Mobile Bluetooth App → ESP32 → WiFi HTTP → PC Server
                                      ↓
                                  Web Dashboard
```

---

## 🔧 Software Requirements

* Arduino IDE
* ESP32 Board Package
* Libraries:

  * BluetoothSerial
  * WiFi
  * HTTPClient
  * ArduinoJson
  * WebServer

---

## 🚀 Setup Instructions

### 1️⃣ Configure WiFi

Update inside code:

```cpp
const char* ssid = "POCO";
const char* password = "okbyebye";
```

---

### 2️⃣ Setup PC Server

Host:

```
http://<Your-PC-IP>:8000/
```

Endpoints required:

| Endpoint       | Method | Purpose            |
| -------------- | ------ | ------------------ |
| `/menu.json`   | GET    | Send menu data     |
| `/save_orders` | POST   | Save placed orders |

Example `menu.json`:

```json
{
  "menu": [
    {"code":"A1","name":"Burger","price":50},
    {"code":"B2","name":"Pizza","price":100}
  ]
}
```

---

### 3️⃣ Upload Code to ESP32

* Select board → ESP32 Dev Module
* Upload the code.

---

### 4️⃣ Connect Bluetooth

Search Bluetooth device:

```
ESP32_Audio_Receiver
```

Send voice commands like:

```
A1
DELETE
A1
CLEAR
DONE
```

---

### 5️⃣ Open Web Dashboard

Open browser:

```
http://<ESP32-IP>
```

Example:

```
http://192.168.252.135
```

---

## 🧾 Available Voice Commands

| Command      | Action              |
| ------------ | ------------------- |
| `A1`, `B2`   | Add item            |
| `DELETE`     | Enable delete mode  |
| `<FoodCode>` | Remove item         |
| `CLEAR`      | Clear current order |
| `DONE`       | Place order         |

---

## 📷 Output Preview

* Menu Display
* Live Current Order
* Total Bill Calculation
* Order History Log

---

## 💡 Applications

* Smart Cafeterias
* Touchless Restaurant Ordering
* College Canteens
* Hospitals & Public Dining Areas

---

## 🔒 Future Enhancements

* 🔊 Audio feedback using speaker
* 📱 Mobile App integration
* 📦 Cloud Database support
* 🔐 Admin Authentication Panel
* 📊 Sales Analytics Dashboard

---

## 👨‍💻 Author

**Swastik Mohanty**
Electronics Engineering | Cybersecurity & IoT Enthusiast

---

⭐ If you like this project, don’t forget to **star this repository!**
