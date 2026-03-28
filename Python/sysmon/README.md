# SysMon — System Monitoring Dashboard

A terminal-style admin dashboard that shows real-time CPU, RAM, Disk, Network, and Process info.

## Quick Start

```bash
# 1. Install dependencies
pip install -r requirements.txt

# 2. Run the server
python server.py

# 3. Open in browser
http://localhost:5000
```

## What It Shows

- **CPU** — Total load % + per-core bars + frequency
- **Memory** — RAM % used + swap usage
- **Storage** — All mounted disks with fill bars
- **Network** — Bytes sent/received + interface IPs
- **Processes** — Top 25 by CPU: PID, name, user, status, CPU%, MEM%

## Access From Other Machines

The server binds to `0.0.0.0:5000`, so any machine on your network can reach it:

```
http://<your-server-ip>:5000
```

## Security Note

This dashboard exposes system info — run it only on trusted networks.
To add basic password protection, install flask-httpauth:

```bash
pip install flask-httpauth
```

Then wrap the `/api/stats` route with HTTP Basic Auth.
