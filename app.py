from flask import Flask, request, jsonify, render_template

app = Flask(__name__)

# Store sensor data
sensor_data = {
    "temperature": 0,
    "humidity": 0,
    "co2": 0,
    "nh3": 0,
    "benzene": 0,
    "smoke": 0,
    "alcohol": 0
}

@app.route("/")
def dashboard():
    return render_template("dashboard.html")

@app.route("/data")
def get_data():
    """Send the latest sensor data to the frontend."""
    return jsonify(sensor_data)

@app.route("/update", methods=["GET"])
def update_data():
    """Receive data from NodeMCU and update sensor_data."""
    global sensor_data
    try:
        sensor_data["temperature"] = float(request.args.get("temp", 0))
        sensor_data["humidity"] = float(request.args.get("hum", 0))
        sensor_data["co2"] = float(request.args.get("co2", 0))
        sensor_data["nh3"] = float(request.args.get("nh3", 0))
        sensor_data["benzene"] = float(request.args.get("benzene", 0))
        sensor_data["smoke"] = float(request.args.get("smoke", 0))
        sensor_data["alcohol"] = float(request.args.get("alcohol", 0))
        return jsonify({"message": "Data updated successfully"}), 200
    except ValueError:
        return jsonify({"error": "Invalid data format"}), 400

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
