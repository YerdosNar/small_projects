from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import psutil
import datetime
import os

app = Flask(__name__, static_folder='static')
CORS(app)

def bytes_to_human(n):
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if n < 1024:
            return f"{n:.1f} {unit}"
        n /= 1024
    return f"{n:.1f} PB"

@app.route('/')
def index():
    return send_from_directory('static', 'index.html')

@app.route('/api/stats')
def stats():
    # CPU
    cpu_total = psutil.cpu_percent(interval=0.5)
    cpu_per_core = psutil.cpu_percent(interval=0, percpu=True)
    cpu_freq = psutil.cpu_freq()
    cpu_count = psutil.cpu_count()

    # Memory
    mem = psutil.virtual_memory()
    swap = psutil.swap_memory()

    # Disk
    disks = []
    for part in psutil.disk_partitions():
        try:
            usage = psutil.disk_usage(part.mountpoint)
            disks.append({
                "device": part.device,
                "mountpoint": part.mountpoint,
                "fstype": part.fstype,
                "total": bytes_to_human(usage.total),
                "used": bytes_to_human(usage.used),
                "free": bytes_to_human(usage.free),
                "percent": usage.percent
            })
        except:
            pass

    # Network
    net = psutil.net_io_counters()
    net_if = {}
    for iface, addrs in psutil.net_if_addrs().items():
        for addr in addrs:
            if addr.family == 2:  # AF_INET
                net_if[iface] = addr.address
                break

    # Processes (top 25 by CPU)
    processes = []
    for p in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent', 'status', 'username', 'cmdline']):
        try:
            info = p.info
            info['memory_percent'] = round(info['memory_percent'] or 0, 2)
            info['cpu_percent'] = round(info['cpu_percent'] or 0, 2)
            processes.append(info)
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass

    processes = sorted(processes, key=lambda x: x['cpu_percent'], reverse=True)[:25]

    # Boot time
    boot_time = datetime.datetime.fromtimestamp(psutil.boot_time())
    uptime = datetime.datetime.now() - boot_time

    return jsonify({
        "timestamp": datetime.datetime.now().isoformat(),
        "uptime": str(uptime).split('.')[0],
        "cpu": {
            "total": cpu_total,
            "per_core": cpu_per_core,
            "count": cpu_count,
            "freq_current": round(cpu_freq.current, 0) if cpu_freq else 0,
            "freq_max": round(cpu_freq.max, 0) if cpu_freq else 0,
        },
        "memory": {
            "total": bytes_to_human(mem.total),
            "used": bytes_to_human(mem.used),
            "available": bytes_to_human(mem.available),
            "percent": mem.percent,
            "swap_total": bytes_to_human(swap.total),
            "swap_used": bytes_to_human(swap.used),
            "swap_percent": swap.percent
        },
        "disks": disks,
        "network": {
            "bytes_sent": bytes_to_human(net.bytes_sent),
            "bytes_recv": bytes_to_human(net.bytes_recv),
            "packets_sent": net.packets_sent,
            "packets_recv": net.packets_recv,
            "interfaces": net_if
        },
        "processes": processes
    })

if __name__ == '__main__':
    print("🖥  SysMon starting on http://0.0.0.0:5000")
    app.run(host='0.0.0.0', port=5000, debug=False)
