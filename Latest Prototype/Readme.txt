Awesome! Here's a simple Python Flask server that:

1. Serves `menu.json` to the ESP32.
2. Accepts a POST request to `/save_orders` to store `orders.json`.

---

### ✅ Step 1: Install Flask (on your PC)

If you don't already have Flask, install it with:

```bash
pip install flask
```

---

### ✅ Step 2: Create `server.py`

Save this as `server.py` in a folder that also contains your `menu.json` file:

```python
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
```

---

### ✅ Step 3: Run the server

In the terminal, run:

```bash
python server.py
```

You should see:
```
* Running on http://0.0.0.0:8000
```

Now:
- ESP32 can fetch `http://<your-pc-ip>:8000/menu.json`
- It can also `POST` to `http://<your-pc-ip>:8000/save_orders`

---

### ✅ Step 4: Update ESP32 Code with PC IP

Make sure your ESP32 code uses your **actual** PC IP, for example:

```cpp
const String pcServerUrl = "http://192.168.1.100:8000";  // Replace with actual IP
```

---

Let me know if you'd like help writing a sample `menu.json` file or troubleshooting the server if anything goes wrong.