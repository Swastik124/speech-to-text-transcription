#include <BluetoothSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>

BluetoothSerial SerialBT;

const char* ssid = "POCO";
const char* password = "okbyebye";

bool deleteMode = false;
String transcription = "Transcription will appear here after processing.";
std::vector<std::pair<String, float>> currentOrder;
std::vector<std::vector<std::pair<String, float>>> allOrders;
std::map<String, std::pair<String, float>> menuItems;
float totalPrice = 0.0;
String invalidCommand = "";

WebServer server(80);
String chefPassword = "123456";

const String pcServerUrl = "http://192.168.252.161:8000/";

void setup() {
    Serial.begin(115200);
    SerialBT.begin("ESP32_Audio_Receiver");
    Serial.println("Bluetooth device started, waiting for commands...");

    connectToWiFi();
    loadMenuFromServer();

    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, []() {
        String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
        page += "<meta http-equiv='refresh' content='5'>";
        page += "<title>Cafeteria Orders</title>";
        page += "<style>";
        page += "body { font-family: Arial; background-color: #f4f4f4; padding: 20px; }";
        page += ".total { font-weight: bold; font-size: 1.2em; }";
        page += ".error { color: red; font-weight: bold; }";
        page += "</style></head><body>";

        page += "<h1>🥗 Cafeteria Orders</h1><h2>Menu:</h2><ul>";
        for (const auto& item : menuItems) {
            page += "<li>" + item.first + " - " + item.second.first + " - Rs. " + String(item.second.second, 2) + "</li>";
        }
        page += "</ul><h2>Current Order:</h2><ul>";
        for (const auto& order : currentOrder) {
            page += "<li>" + order.first + " - Rs. " + String(order.second, 2) + "</li>";
        }
        page += "</ul><p class='total'>Total: Rs. " + String(totalPrice, 2) + "</p>";

        if (!invalidCommand.isEmpty()) {
            page += "<p class='error'>Invalid Command: " + invalidCommand + "</p>";
        }

        page += "<h2>Order Log:</h2>";
        for (int i = allOrders.size() - 1; i >= 0; i--) {
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
    server.handleClient();

    if (SerialBT.available()) {
        String command = SerialBT.readString();
        command.trim();
        command.toUpperCase();
        Serial.println("Received command: " + command);

        if (command == "DONE") {
            placeOrder();
            deleteMode = false;

        } else if (command == "CLEAR") {
            currentOrder.clear();
            totalPrice = 0.0;
            Serial.println("Current order cleared.");
            deleteMode = false;

        } else if (command == "DELETE") {
            deleteMode = true;
            Serial.println("Delete mode activated. Say food code to delete.");

        } else {
            if (deleteMode) {
                bool removed = removeItemByCode(command);
                if (removed) {
                    Serial.println("Item removed: " + command);
                } else {
                    Serial.println("Item not found for removal: " + command);
                }
                deleteMode = false;
            } else {
                String menuItem = processCommand(command);
                if (menuItem != "") {
                    float price = addOrder(menuItem);
                    Serial.println("Added to order: " + menuItem + " - Rs. " + String(price, 2));
                    invalidCommand = "";
                } else {
                    Serial.println("Invalid command received.");
                    invalidCommand = command;
                }
            }
        }
    }
    delay(10);
}

void connectToWiFi() {
    WiFi.setSleep(false);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
    } else {
        Serial.println("\nFailed to connect to WiFi.");
    }
}

void loadMenuFromServer() {
    HTTPClient http;
    http.begin(pcServerUrl + "/menu.json");
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        StaticJsonDocument<1024> doc;
        if (deserializeJson(doc, payload)) {
            Serial.println("JSON parse error.");
            return;
        }
        for (JsonObject item : doc["menu"].as<JsonArray>()) {
            String code = item["code"].as<String>();
            String name = item["name"].as<String>();
            float price = item["price"].as<float>();
            menuItems[code] = {name, price};
        }
        Serial.println("Menu loaded from server.");
    } else {
        Serial.print("Failed to fetch menu. HTTP code: ");
        Serial.println(httpCode);
    }
    http.end();
}

void saveOrdersToFile() {
    HTTPClient http;
    http.begin(pcServerUrl + "/save_orders");
    http.addHeader("Content-Type", "application/json");

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

    String output;
    serializeJson(doc, output);
    int httpCode = http.POST(output);

    if (httpCode > 0) {
        Serial.println("Orders saved to server.");
    } else {
        Serial.print("Failed to send orders. HTTP error: ");
        Serial.println(httpCode);
    }
    http.end();
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

bool removeItemByCode(String code) {
    if (menuItems.find(code) == menuItems.end()) return false;

    String targetName = menuItems[code].first;

    for (auto it = currentOrder.rbegin(); it != currentOrder.rend(); ++it) {
        if (it->first == targetName) {
            totalPrice -= it->second;
            currentOrder.erase(std::next(it).base());
            return true;
        }
    }
    return false;
}

void placeOrder() {
    if (currentOrder.empty()) {
        Serial.println("No items to place.");
        return;
    }

    Serial.println("Placing order...");
    allOrders.push_back(currentOrder);
    saveOrdersToFile();

    Serial.println("Order placed. Total: Rs. " + String(totalPrice, 2));
    currentOrder.clear();
    totalPrice = 0.0;
    Serial.println("Ready for new order.");
}
