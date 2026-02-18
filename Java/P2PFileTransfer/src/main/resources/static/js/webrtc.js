/**
 * WebRTC P2P Connection Module
 * Handles signaling, ICE candidates, and data channel for file transfer
 */
class WebRTCManager {
    constructor() {
        this.peerConnection = null;
        this.dataChannel = null;
        this.websocket = null;
        this.iceServers = [];
        this.myPeerId = null;
        this.remotePeerId = null;
        this.sessionToken = null;
        this.isInitiator = false;
        
        // Callbacks
        this.onConnected = null;
        this.onDisconnected = null;
        this.onDataChannelOpen = null;
        this.onDataChannelClose = null;
        this.onMessage = null;
        this.onError = null;
        
        // File transfer state
        this.receivedChunks = [];
        this.receivedSize = 0;
        this.expectedFileInfo = null;
    }

    /**
     * Fetch ICE server configuration from the server
     */
    async fetchIceServers() {
        try {
            const response = await fetch('/api/ice-servers');
            const data = await response.json();
            this.iceServers = data.iceServers;
            console.log('ICE servers:', this.iceServers);
        } catch (error) {
            console.error('Failed to fetch ICE servers:', error);
            // Fallback to Google STUN servers
            this.iceServers = [
                { urls: 'stun:stun.l.google.com:19302' },
                { urls: 'stun:stun1.l.google.com:19302' }
            ];
        }
    }

    /**
     * Connect to the signaling server
     */
    connectSignaling(peerId, sessionToken, isReceiver) {
        return new Promise((resolve, reject) => {
            this.myPeerId = peerId;
            this.sessionToken = sessionToken;
            this.isInitiator = !isReceiver; // Sender is initiator

            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            const wsUrl = `${protocol}//${window.location.host}/signaling`;
            
            this.websocket = new WebSocket(wsUrl);

            this.websocket.onopen = () => {
                console.log('WebSocket connected');
                // Register with signaling server
                this.sendSignalingMessage({
                    type: 'register',
                    from: peerId,
                    to: 'server',
                    payload: sessionToken
                });
            };

            this.websocket.onmessage = (event) => {
                const message = JSON.parse(event.data);
                this.handleSignalingMessage(message, resolve, reject);
            };

            this.websocket.onerror = (error) => {
                console.error('WebSocket error:', error);
                reject(error);
            };

            this.websocket.onclose = () => {
                console.log('WebSocket closed');
                if (this.onDisconnected) {
                    this.onDisconnected();
                }
            };
        });
    }

    /**
     * Handle incoming signaling messages
     */
    handleSignalingMessage(message, resolve, reject) {
        console.log('Signaling message:', message.type);

        switch (message.type) {
            case 'registered':
                console.log(`Registered as ${message.payload}`);
                resolve(message.payload);
                break;

            case 'offer':
                this.remotePeerId = message.from;
                this.handleOffer(message);
                break;

            case 'answer':
                this.handleAnswer(message);
                break;

            case 'ice-candidate':
                this.handleIceCandidate(message);
                break;

            case 'connected':
                console.log('P2P connection confirmed by server');
                break;

            case 'peer-disconnected':
                console.log('Peer disconnected:', message.payload);
                if (this.onDisconnected) {
                    this.onDisconnected();
                }
                break;

            case 'error':
                console.error('Signaling error:', message.payload);
                if (this.onError) {
                    this.onError(message.payload);
                }
                if (reject) reject(new Error(message.payload));
                break;
        }
    }

    /**
     * Create peer connection and data channel
     */
    async createPeerConnection() {
        await this.fetchIceServers();

        const config = {
            iceServers: this.iceServers
        };

        this.peerConnection = new RTCPeerConnection(config);

        // ICE candidate handling
        this.peerConnection.onicecandidate = (event) => {
            if (event.candidate) {
                this.sendSignalingMessage({
                    type: 'ice-candidate',
                    from: this.myPeerId,
                    to: this.remotePeerId,
                    payload: JSON.stringify(event.candidate)
                });
            }
        };

        // Connection state change
        this.peerConnection.onconnectionstatechange = () => {
            console.log('Connection state:', this.peerConnection.connectionState);
            if (this.peerConnection.connectionState === 'connected') {
                if (this.onConnected) {
                    this.onConnected();
                }
                // Notify server of successful connection
                this.sendSignalingMessage({
                    type: 'connected',
                    from: this.myPeerId,
                    to: 'server',
                    payload: 'connected'
                });
            } else if (this.peerConnection.connectionState === 'disconnected' ||
                       this.peerConnection.connectionState === 'failed') {
                if (this.onDisconnected) {
                    this.onDisconnected();
                }
            }
        };

        // Data channel event (for receiver)
        this.peerConnection.ondatachannel = (event) => {
            console.log('Data channel received');
            this.dataChannel = event.channel;
            this.setupDataChannel();
        };

        return this.peerConnection;
    }

    /**
     * Create data channel (sender side)
     */
    createDataChannel() {
        this.dataChannel = this.peerConnection.createDataChannel('fileTransfer', {
            ordered: true
        });
        this.setupDataChannel();
    }

    /**
     * Setup data channel event handlers
     */
    setupDataChannel() {
        this.dataChannel.binaryType = 'arraybuffer';

        this.dataChannel.onopen = () => {
            console.log('Data channel open');
            if (this.onDataChannelOpen) {
                this.onDataChannelOpen();
            }
        };

        this.dataChannel.onclose = () => {
            console.log('Data channel closed');
            if (this.onDataChannelClose) {
                this.onDataChannelClose();
            }
        };

        this.dataChannel.onmessage = (event) => {
            if (this.onMessage) {
                this.onMessage(event.data);
            }
        };

        this.dataChannel.onerror = (error) => {
            console.error('Data channel error:', error);
            if (this.onError) {
                this.onError(error);
            }
        };
    }

    /**
     * Create and send offer (sender side)
     */
    async createOffer(remotePeerId) {
        this.remotePeerId = remotePeerId;
        await this.createPeerConnection();
        this.createDataChannel();

        const offer = await this.peerConnection.createOffer();
        await this.peerConnection.setLocalDescription(offer);

        this.sendSignalingMessage({
            type: 'offer',
            from: this.myPeerId,
            to: remotePeerId,
            payload: JSON.stringify(offer)
        });
    }

    /**
     * Handle incoming offer (receiver side)
     */
    async handleOffer(message) {
        await this.createPeerConnection();

        const offer = JSON.parse(message.payload);
        await this.peerConnection.setRemoteDescription(new RTCSessionDescription(offer));

        const answer = await this.peerConnection.createAnswer();
        await this.peerConnection.setLocalDescription(answer);

        this.sendSignalingMessage({
            type: 'answer',
            from: this.myPeerId,
            to: message.from,
            payload: JSON.stringify(answer)
        });
    }

    /**
     * Handle incoming answer (sender side)
     */
    async handleAnswer(message) {
        const answer = JSON.parse(message.payload);
        await this.peerConnection.setRemoteDescription(new RTCSessionDescription(answer));
    }

    /**
     * Handle incoming ICE candidate
     */
    async handleIceCandidate(message) {
        if (this.peerConnection) {
            const candidate = JSON.parse(message.payload);
            try {
                await this.peerConnection.addIceCandidate(new RTCIceCandidate(candidate));
            } catch (error) {
                console.error('Error adding ICE candidate:', error);
            }
        }
    }

    /**
     * Send signaling message
     */
    sendSignalingMessage(message) {
        if (this.websocket && this.websocket.readyState === WebSocket.OPEN) {
            this.websocket.send(JSON.stringify(message));
        }
    }

    /**
     * Send data through data channel
     */
    send(data) {
        if (this.dataChannel && this.dataChannel.readyState === 'open') {
            this.dataChannel.send(data);
            return true;
        }
        return false;
    }

    /**
     * Send string message
     */
    sendMessage(message) {
        return this.send(JSON.stringify(message));
    }

    /**
     * Close all connections
     */
    close() {
        if (this.dataChannel) {
            this.dataChannel.close();
        }
        if (this.peerConnection) {
            this.peerConnection.close();
        }
        if (this.websocket) {
            this.websocket.close();
        }
    }
}

// Export for use
window.WebRTCManager = WebRTCManager;
