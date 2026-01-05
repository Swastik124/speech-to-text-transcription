from flask import Flask, request, jsonify, send_from_directory
import os
import json

app = Flask(__name__)
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

@app.route('/menu.json', methods=['GET'])
def get_menu():
    return send_from_directory(BASE_DIR, 'menu.json')

@app.route('/save_orders', methods=['POST'])
def save_orders():
    try:
        orders = request.get_json()
        with open(os.path.join(BASE_DIR, 'orders.json'), 'w') as f:
            json.dump(orders, f, indent=2)
        return jsonify({"message": "Orders saved successfully."}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/')
def home():
    return "<h1>ESP32 Server Running</h1>"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000)
