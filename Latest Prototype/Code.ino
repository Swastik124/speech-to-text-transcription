#include <BluetoothSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>
#include <WebServer.h>

BluetoothSerial SerialBT; // Bluetooth serial object

const char* ssid = "Test";
const char* password = "password";

const int chipSelect = 5; // SD card chip select pin
bool isRecording = false;

String transcription = "Transcription will appear here after processing."; // Default transcription
std::vector<std::pair<String, float>> currentOrder; // Current order
std::vector<std::vector<std::pair<String, float>>> allOrders; // Order log
std::map<String, std::pair<String, float>> menuItems; // Menu items from menu.json
float totalPrice = 0.0; // Total price for the current order
String invalidCommand = ""; // Invalid command string

WebServer server(80); // Web server on port 80
String chefPassword = "123456"; // Chef password

void setup() {
    Serial.begin(115200);
    SerialBT.begin("ESP32_Audio_Receiver"); // Bluetooth name
    Serial.println("Bluetooth device started, waiting for commands...");

    if (!SD.begin(chipSelect)) {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized.");

    loadMenuFromFile();

    connectToWiFi();

    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    // Web server routes
    server.on("/", HTTP_GET, []() {
    String page = "<html><body><h1>Cafeteria Orders</h1>";
    page += "<meta http-equiv='refresh' content='3'>"; // Auto-refresh every 3 seconds
    page += "<h2>Available Menu:</h2><ul>";
    for (const auto& item : menuItems) {
        page += "<li>" + item.first + " - " + item.second.first + " - Rs. " + String(item.second.second, 2) + "</li>";
    }
    page += "</ul>";

    page += "<h2>Current Order:</h2><ul>";
    for (const auto& order : currentOrder) {
        page += "<li>" + order.first + " - Rs. " + String(order.second, 2) + "</li>";
    }
    page += "</ul>";

    page += "<h3>Total Price: Rs. " + String(totalPrice, 2) + "</h3>";

    if (!invalidCommand.isEmpty()) {
        page += "<h3 style='color:red;'>Invalid Command: " + invalidCommand + "</h3>";
    }

    page += "<h2>Order Log:</h2>";
    for (int i = allOrders.size() - 1; i >= 0; i--) { // Reverse order iteration
        page += "<h3>Order #" + String(i + 1) + "</h3><ul>";
        for (const auto& item : allOrders[i]) {
            page += "<li>" + item.first + " - Rs. " + String(item.second, 2) + "</li>";
        }
        page += "</ul>";
    }

    page += "</body></html>";
    server.send(200, "text/html", page);
});

    server.begin();
    Serial.println("Web server started.");
}

void loop() {
    server.handleClient(); // Handle incoming HTTP requests

    if (SerialBT.available()) {
        String command = SerialBT.readString();
        command.trim(); // Remove extra spaces
        Serial.println("Received command: " + command);

        // Make the command case-insensitive
        command.toUpperCase();

        if (command == "DONE") {
            placeOrder();
        } else if (command == "CLEAR") {
            clearLastItem();
        } else {
            String menuItem = processCommand(command);
            if (menuItem != "") {
                float price = addOrder(menuItem);
                Serial.println("Added to order: " + menuItem + " - Rs. " + String(price, 2));
                invalidCommand = ""; // Clear invalid command if new valid command is received
            } else {
                Serial.println("Invalid command received.");
                invalidCommand = command; // Set invalid command
            }
        }
    }
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void loadMenuFromFile() {
    File file = SD.open("/menu.json", FILE_READ);
    if (!file) {
        Serial.println("Failed to open menu.json");
        return;
    }
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.print("Error parsing menu.json: ");
        Serial.println(error.c_str());
        file.close();
        return;
    }
    for (JsonObject item : doc["menu"].as<JsonArray>()) {
        String code = item["code"].as<String>();
        String name = item["name"].as<String>();
        float price = item["price"].as<float>();
        menuItems[code] = {name, price};
    }
    file.close();
    Serial.println("Menu loaded successfully.");
}

void saveOrdersToFile() {
    File file = SD.open("/orders.json", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open orders.json for writing.");
        return;
    }
    StaticJsonDocument<2048> doc;
    JsonArray ordersArray = doc.createNestedArray("orders");

    for (const auto& order : allOrders) {
        JsonArray orderArray = ordersArray.createNestedArray();
        for (const auto& item : order) {
            JsonObject obj = orderArray.createNestedObject();
            obj["name"] = item.first;
            obj["price"] = item.second;
        }
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write to orders.json.");
    } else {
        Serial.println("Orders saved to orders.json.");
    }

    file.close();
}

String processCommand(String command) {
    if (menuItems.find(command) != menuItems.end()) {
        return menuItems[command].first;
    }
    return "";
}

float addOrder(String menuItem) {
    for (const auto& item : menuItems) {
        if (item.second.first == menuItem) {
            currentOrder.push_back(item.second);
            totalPrice += item.second.second;
            return item.second.second;
        }
    }
    return 0.0;
}

void clearLastItem() {
    if (!currentOrder.empty()) {
        auto lastItem = currentOrder.back();
        totalPrice -= lastItem.second;
        currentOrder.pop_back();
        Serial.println("Last item removed: " + lastItem.first + " - Rs. " + String(lastItem.second, 2));
    } else {
        Serial.println("No items to remove.");
    }
}

void placeOrder() {
    if (currentOrder.empty()) {
        Serial.println("No items in the current order to place.");
        return;
    }

    Serial.println("Placing order...");
    allOrders.push_back(currentOrder); // Save the current order
    saveOrdersToFile(); // Save orders to `orders.json`

    Serial.println("Order placed successfully. Total: Rs. " + String(totalPrice, 2));

    // Clear current order
    currentOrder.clear();
    totalPrice = 0.0;
    Serial.println("Ready for a new order.");
}
