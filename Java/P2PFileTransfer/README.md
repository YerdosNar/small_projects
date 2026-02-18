# P2P File Transfer

A secure, end-to-end encrypted peer-to-peer file transfer application built with Spring Boot and WebRTC.

## Features

- **Direct P2P Connection**: Files are transferred directly between peers using WebRTC, never touching the server
- **End-to-End Encryption**: All files are encrypted using AES-256-GCM with ECDH key exchange
- **Temporary Sessions**: Receiver creates a temporary ID and password that auto-expire
- **NAT Traversal**: Uses STUN servers for ICE candidate gathering to punch through NATs
- **No Registration Required**: Just create a session, share credentials, and transfer

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                       Spring Boot Server                        │
│  ┌─────────────────┐    ┌─────────────────────────────────────┐ │
│  │   REST API      │    │   WebSocket Signaling Server        │ │
│  │  - Create ID    │    │  - SDP Offer/Answer exchange        │ │
│  │  - Search Peer  │    │  - ICE Candidate exchange           │ │
│  │  - Authenticate │    │  - Connection state management      │ │
│  └─────────────────┘    └─────────────────────────────────────┘ │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │               H2 In-Memory Database                        │ │
│  │            (Temporary session storage)                     │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
                              │
         ┌────────────────────┼────────────────────┐
         │                    │                    │
         ▼                    │                    ▼
┌─────────────────┐           │          ┌─────────────────┐
│    Receiver     │           │          │     Sender      │
│    (PeerA)      │◄──────────┴─────────►│    (PeerB)      │
│                 │     WebRTC P2P       │                 │
│  - Creates ID   │   (Encrypted Data)   │  - Searches ID  │
│  - Waits        │                      │  - Authenticates│
│  - Receives     │                      │  - Sends file   │
└─────────────────┘                      └─────────────────┘
```

## How It Works

### Flow

1. **Receiver (PeerA)** creates a session:
   - Sets a password
   - Gets a unique 6-character ID
   - Shares ID and password with Sender via external messenger

2. **Sender (PeerB)** connects:
   - Searches for Receiver's ID
   - Authenticates with the shared password
   - Establishes direct P2P connection via WebRTC
   - Selects and sends file (encrypted)

3. **After Connection**:
   - Server deletes the temporary session
   - File transfers directly between peers
   - Server has no access to transferred data

### Security

- **ECDH Key Exchange**: Both peers generate ECDH P-256 key pairs
- **AES-256-GCM Encryption**: Files are encrypted with derived shared secret
- **Password Hashing**: Passwords are SHA-256 hashed before storage
- **No File Storage**: Server never sees or stores transferred files

## Requirements

- Java 17+
- Maven 3.6+

## Running the Application

### Using Maven

```bash
cd Java/P2PFileTransfer
./mvnw spring-boot:run
```

Or if you have Maven installed:

```bash
mvn spring-boot:run
```

### Using the run script

```bash
cd Java/P2PFileTransfer
./run.sh
```

### Access

Open your browser and navigate to:
```
http://localhost:8080
```

## API Endpoints

### REST API

| Method | Endpoint | Description |
|--------|----------|-------------|
| POST | `/api/session` | Create new receiver session |
| GET | `/api/peer/{peerId}` | Search for a peer by ID |
| POST | `/api/authenticate` | Authenticate sender |
| GET | `/api/ice-servers` | Get STUN/TURN server config |
| DELETE | `/api/session/{peerId}` | Cancel a session |

### WebSocket

| Endpoint | Description |
|----------|-------------|
| `/signaling` | WebRTC signaling server |

## Configuration

Edit `src/main/resources/application.properties`:

```properties
# Server port
server.port=8080

# Session timeout (in minutes)
p2p.session.timeout-minutes=30

# STUN servers (comma-separated)
p2p.stun.servers=stun:stun.l.google.com:19302,stun:stun1.l.google.com:19302
```

## Project Structure

```
P2PFileTransfer/
├── pom.xml
├── run.sh
├── README.md
└── src/main/
    ├── java/com/p2p/filetransfer/
    │   ├── P2PFileTransferApplication.java
    │   ├── config/
    │   │   └── WebSocketConfig.java
    │   ├── controller/
    │   │   └── PeerController.java
    │   ├── model/
    │   │   ├── PeerSession.java
    │   │   ├── CreateSessionRequest.java
    │   │   ├── CreateSessionResponse.java
    │   │   ├── AuthenticateRequest.java
    │   │   ├── AuthenticateResponse.java
    │   │   └── SignalingMessage.java
    │   ├── repository/
    │   │   └── PeerSessionRepository.java
    │   ├── service/
    │   │   └── PeerSessionService.java
    │   └── websocket/
    │       └── SignalingHandler.java
    └── resources/
        ├── application.properties
        └── static/
            ├── index.html
            ├── css/
            │   └── style.css
            └── js/
                ├── app.js
                ├── crypto.js
                └── webrtc.js
```

## Limitations

- WebRTC may not work in all network configurations (symmetric NAT)
- Large files may be slow due to encryption overhead
- Single-file transfer per session
- Sessions expire after 30 minutes (configurable)

## Production Considerations

For production deployment:

1. **Add TURN Server**: For users behind symmetric NAT
   ```properties
   p2p.stun.servers=stun:stun.l.google.com:19302,turn:your-turn-server.com
   ```

2. **Use HTTPS**: Required for WebRTC in production
   - Configure SSL in Spring Boot or use a reverse proxy

3. **Use Persistent Database**: Replace H2 with PostgreSQL/MySQL for reliability

4. **Add Rate Limiting**: Prevent abuse of session creation

5. **Add Monitoring**: Log and monitor P2P connection success rates

## License

MIT License
