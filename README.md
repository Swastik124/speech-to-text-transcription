# **Cafeteria Order Management System using ESP32**  
<img src="https://www.amazon.in/REES52-WROOM-32-Development-Microcontroller/dp/B0BSV7GBV4" alt="ESP32"></img>



## **📌 Overview**  
This project is a **smart cafeteria ordering system** built using **ESP32, Bluetooth, WiFi, and an SD card module**. It allows users to place orders via **Bluetooth commands**, stores order history, and displays real-time order updates through a **web interface** hosted on the ESP32.  

## **🚀 Features**  
✅ **Bluetooth Ordering** – Users can place orders via Bluetooth commands sent to the ESP32.  
✅ **Web Interface** – View real-time order status, menu, and order history via a web server on ESP32.  
✅ **SD Card Storage** – Loads the menu from `menu.json` and saves order logs to `orders.json`.  
✅ **Live Menu & Pricing** – Fetches menu items and their prices from an SD card file.  
✅ **Order Management** – Users can:  
   - Add menu items using predefined commands.  
   - Remove the last added item (`CLEAR` command).  
   - Confirm and place an order (`DONE` command).  
✅ **WiFi Connectivity** – ESP32 connects to a WiFi network to enable web-based access.  
✅ **Auto-Refresh Web Page** – The order display updates every 3 seconds for live monitoring.  

## **🛠 Tech Stack**  
- **ESP32** – Microcontroller for Bluetooth and WiFi communication.  
- **C++ (Arduino Framework)** – Core programming language.  
- **BluetoothSerial.h** – Enables Bluetooth communication.  
- **WiFi.h & WebServer.h** – Allows ESP32 to host a web server.  
- **ArduinoJson.h** – Parses JSON files for menu and orders.  
- **SD.h & SPI.h** – Reads/writes order data from an SD card.  

## **📖 How It Works**  
1. **ESP32 Setup**:  
   - Initializes Bluetooth as `ESP32_Audio_Receiver`.  
   - Connects to the WiFi network.  
   - Loads the menu from `menu.json` stored on an SD card.  
   - Starts a web server to display orders.  

2. **Bluetooth Order Commands**:  
   - Users send food item codes via Bluetooth.  
   - The system validates the item, adds it to the order, and updates the total price.  
   - Special commands:  
     - **DONE** → Confirms and saves the order.  
     - **CLEAR** → Removes the last added item.  

3. **Web Dashboard**:  
   - Displays available menu items, current orders, and past orders.  
   - Updates automatically every **3 seconds**.  
   - Shows total price and invalid commands (if any).  

4. **Order Storage**:  
   - Confirmed orders are logged into `orders.json` on the SD card.  
   - Past orders can be accessed via the web interface.  

## **💾 File Structure**  
```
📂 /  
 ├── main.ino            # Main ESP32 code  
 ├── menu.json           # Menu items and prices  
 ├── orders.json         # Logs of past orders  
 ├── README.md           # Documentation  
```

## **🎯 Future Improvements**  
🔹 Add **voice command support** for hands-free ordering.  
🔹 Implement **QR code scanning** to fetch menu items.  
🔹 Introduce a **mobile app** for seamless order placement. 
