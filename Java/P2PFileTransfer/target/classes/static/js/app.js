/**
 * Main Application Logic
 * Handles UI interactions and coordinates WebRTC and Crypto modules
 */
class App {
    constructor() {
        // Managers
        this.crypto = new CryptoManager();
        this.webrtc = new WebRTCManager();
        
        // State
        this.role = null; // 'receiver' or 'sender'
        this.peerId = null;
        this.sessionToken = null;
        this.selectedFile = null;
        this.peerPublicKey = null;
        this.timerInterval = null;
        
        // File transfer state
        this.CHUNK_SIZE = 16 * 1024; // 16KB chunks
        this.receivedChunks = [];
        this.receivedSize = 0;
        this.expectedFileInfo = null;
        this.transferStartTime = null;
        
        // Initialize
        this.initializeEventListeners();
    }

    /**
     * Initialize all event listeners
     */
    initializeEventListeners() {
        // Role selection
        document.getElementById('btn-receiver').addEventListener('click', () => this.selectRole('receiver'));
        document.getElementById('btn-sender').addEventListener('click', () => this.selectRole('sender'));
        document.getElementById('btn-back').addEventListener('click', () => this.goBack());

        // Receiver events
        document.getElementById('btn-create-session').addEventListener('click', () => this.createSession());
        document.getElementById('btn-cancel-session').addEventListener('click', () => this.cancelSession());

        // Sender events
        document.getElementById('btn-search-peer').addEventListener('click', () => this.searchPeer());
        document.getElementById('btn-authenticate').addEventListener('click', () => this.authenticate());
        document.getElementById('btn-select-file').addEventListener('click', () => document.getElementById('file-input').click());
        document.getElementById('file-input').addEventListener('change', (e) => this.handleFileSelect(e));
        document.getElementById('btn-send-file').addEventListener('click', () => this.sendFile());

        // Copy buttons
        document.querySelectorAll('.btn-copy').forEach(btn => {
            btn.addEventListener('click', (e) => this.copyToClipboard(e));
        });

        // New transfer
        document.getElementById('btn-new-transfer').addEventListener('click', () => this.reset());

        // Enter key handlers
        document.getElementById('receiver-password').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.createSession();
        });
        document.getElementById('search-peer-id').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.searchPeer();
        });
        document.getElementById('auth-password').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.authenticate();
        });
    }

    /**
     * Select role (receiver or sender)
     */
    selectRole(role) {
        this.role = role;
        this.hide('role-selection');
        this.show('btn-back');

        if (role === 'receiver') {
            this.show('receiver-section');
        } else {
            this.show('sender-section');
        }
    }

    /**
     * Go back to role selection
     */
    goBack() {
        this.reset();
    }

    /**
     * Create receiver session
     */
    async createSession() {
        const password = document.getElementById('receiver-password').value;
        
        if (password.length < 4) {
            this.showToast('Password must be at least 4 characters', 'error');
            return;
        }

        try {
            // Generate key pair for E2E encryption
            await this.crypto.generateKeyPair();
            const publicKey = await this.crypto.exportPublicKey();

            // Create session on server
            const response = await fetch('/api/session', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ password, publicKey })
            });

            const data = await response.json();

            if (!data.success) {
                this.showToast(data.message, 'error');
                return;
            }

            this.peerId = data.peerId;
            this.sessionToken = data.sessionToken;

            // Update UI
            document.getElementById('display-peer-id').textContent = this.peerId;
            document.getElementById('display-password').textContent = password;
            this.hide('receiver-create');
            this.show('receiver-waiting');

            // Start timer
            this.startSessionTimer(data.expiresInSeconds);

            // Connect to signaling server and wait for sender
            await this.connectAsReceiver();

        } catch (error) {
            console.error('Error creating session:', error);
            this.showToast('Failed to create session', 'error');
        }
    }

    /**
     * Connect as receiver and wait for sender
     */
    async connectAsReceiver() {
        try {
            await this.webrtc.connectSignaling(this.peerId, this.sessionToken, true);
            
            // Setup WebRTC callbacks
            this.webrtc.onConnected = () => {
                this.showToast('P2P connection established!', 'success');
                this.hide('receiver-waiting');
                this.show('transfer-section');
                document.getElementById('transfer-status').textContent = 'Connected, waiting for file...';
            };

            this.webrtc.onDisconnected = () => {
                this.showToast('Connection lost', 'error');
            };

            this.webrtc.onDataChannelOpen = async () => {
                console.log('Data channel ready');
            };

            this.webrtc.onMessage = (data) => this.handleReceivedData(data);

            this.webrtc.onError = (error) => {
                this.showToast(error, 'error');
            };

        } catch (error) {
            console.error('Error connecting:', error);
            this.showToast('Connection failed', 'error');
        }
    }

    /**
     * Search for peer
     */
    async searchPeer() {
        const peerId = document.getElementById('search-peer-id').value.toUpperCase();
        
        if (peerId.length !== 6) {
            this.showToast('Please enter a 6-character ID', 'error');
            return;
        }

        try {
            const response = await fetch(`/api/peer/${peerId}`);
            const data = await response.json();
            
            const resultDiv = document.getElementById('search-result');
            this.show('search-result');

            if (data.found) {
                resultDiv.className = 'search-result found';
                resultDiv.textContent = `✓ Peer ${data.peerId} found!`;
                this.peerId = data.peerId;
                this.show('sender-auth');
            } else {
                resultDiv.className = 'search-result not-found';
                resultDiv.textContent = '✗ Peer not found or session expired';
            }

        } catch (error) {
            console.error('Search error:', error);
            this.showToast('Search failed', 'error');
        }
    }

    /**
     * Authenticate and connect to receiver
     */
    async authenticate() {
        const password = document.getElementById('auth-password').value;
        
        if (!password) {
            this.showToast('Please enter the password', 'error');
            return;
        }

        try {
            // Generate key pair for E2E encryption
            await this.crypto.generateKeyPair();
            const publicKey = await this.crypto.exportPublicKey();

            const response = await fetch('/api/authenticate', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    peerId: this.peerId,
                    password,
                    senderPublicKey: publicKey
                })
            });

            const data = await response.json();

            if (!data.success) {
                this.showToast(data.message, 'error');
                return;
            }

            // Store receiver's public key for E2E encryption
            this.peerPublicKey = data.receiverPublicKey;
            this.sessionToken = data.sessionToken;

            // Derive shared encryption key
            await this.crypto.deriveSharedKey(this.peerPublicKey);

            this.showToast('Authentication successful!', 'success');
            
            // Show file selection
            this.hide('sender-search');
            this.hide('sender-auth');
            this.show('sender-file');

            // Connect to signaling server and initiate P2P
            await this.connectAsSender();

        } catch (error) {
            console.error('Authentication error:', error);
            this.showToast('Authentication failed', 'error');
        }
    }

    /**
     * Connect as sender and initiate P2P connection
     */
    async connectAsSender() {
        try {
            // Generate a unique ID for sender
            const senderPeerId = 'S' + Math.random().toString(36).substr(2, 5).toUpperCase();
            
            await this.webrtc.connectSignaling(senderPeerId, this.sessionToken, false);
            
            // Setup WebRTC callbacks
            this.webrtc.onConnected = () => {
                this.showToast('P2P connection established!', 'success');
            };

            this.webrtc.onDisconnected = () => {
                this.showToast('Connection lost', 'error');
            };

            this.webrtc.onDataChannelOpen = async () => {
                console.log('Data channel ready for sending');
                // Send public key to receiver for key derivation
                this.webrtc.sendMessage({
                    type: 'publicKey',
                    publicKey: await this.crypto.exportPublicKey()
                });
            };

            this.webrtc.onMessage = (data) => this.handleReceivedData(data);

            this.webrtc.onError = (error) => {
                this.showToast(error, 'error');
            };

            // Create offer to receiver
            await this.webrtc.createOffer(this.peerId);

        } catch (error) {
            console.error('Error connecting:', error);
            this.showToast('Connection failed', 'error');
        }
    }

    /**
     * Handle file selection
     */
    handleFileSelect(event) {
        const file = event.target.files[0];
        if (!file) return;

        this.selectedFile = file;
        
        document.getElementById('selected-file-name').textContent = file.name;
        document.getElementById('file-name').textContent = file.name;
        document.getElementById('file-size').textContent = this.formatSize(file.size);
        document.getElementById('file-type').textContent = file.type || 'Unknown';
        
        this.show('file-info');
        document.getElementById('btn-send-file').disabled = false;
    }

    /**
     * Send the selected file
     */
    async sendFile() {
        if (!this.selectedFile) {
            this.showToast('Please select a file first', 'error');
            return;
        }

        if (!this.webrtc.dataChannel || this.webrtc.dataChannel.readyState !== 'open') {
            this.showToast('Connection not ready', 'error');
            return;
        }

        try {
            this.hide('sender-file');
            this.show('transfer-section');
            
            document.getElementById('transfer-file-name').textContent = this.selectedFile.name;
            document.getElementById('transfer-file-size').textContent = this.formatSize(this.selectedFile.size);
            document.getElementById('transfer-status').textContent = 'Encrypting and sending...';

            this.transferStartTime = Date.now();

            // Send file info first
            const fileInfo = {
                type: 'fileInfo',
                name: this.selectedFile.name,
                size: this.selectedFile.size,
                mimeType: this.selectedFile.type
            };
            this.webrtc.sendMessage(fileInfo);

            // Read and send file in chunks
            const reader = new FileReader();
            let offset = 0;
            let sentSize = 0;

            const readChunk = () => {
                const slice = this.selectedFile.slice(offset, offset + this.CHUNK_SIZE);
                reader.readAsArrayBuffer(slice);
            };

            reader.onload = async (e) => {
                const chunk = e.target.result;
                
                // Encrypt chunk
                const encryptedChunk = await this.crypto.encryptChunk(chunk);
                
                // Send encrypted chunk
                this.webrtc.send(encryptedChunk);
                
                sentSize += chunk.byteLength;
                offset += chunk.byteLength;

                // Update progress
                const progress = (sentSize / this.selectedFile.size) * 100;
                this.updateProgress(progress, sentSize);

                if (offset < this.selectedFile.size) {
                    // Continue with next chunk (with small delay to prevent buffer overflow)
                    setTimeout(readChunk, 10);
                } else {
                    // Send completion message
                    setTimeout(() => {
                        this.webrtc.sendMessage({ type: 'fileComplete' });
                        document.getElementById('transfer-status').textContent = 'File sent successfully!';
                        setTimeout(() => this.showComplete('File sent successfully!'), 1000);
                    }, 100);
                }
            };

            readChunk();

        } catch (error) {
            console.error('Error sending file:', error);
            this.showToast('Failed to send file', 'error');
        }
    }

    /**
     * Handle received data
     */
    async handleReceivedData(data) {
        try {
            // Check if it's a string message
            if (typeof data === 'string') {
                const message = JSON.parse(data);
                
                switch (message.type) {
                    case 'publicKey':
                        // Receiver: derive shared key from sender's public key
                        await this.crypto.deriveSharedKey(message.publicKey);
                        console.log('Shared key derived');
                        break;
                    
                    case 'fileInfo':
                        this.expectedFileInfo = message;
                        this.receivedChunks = [];
                        this.receivedSize = 0;
                        this.transferStartTime = Date.now();
                        
                        document.getElementById('transfer-file-name').textContent = message.name;
                        document.getElementById('transfer-file-size').textContent = this.formatSize(message.size);
                        document.getElementById('transfer-status').textContent = 'Receiving file...';
                        break;
                    
                    case 'fileComplete':
                        await this.assembleAndDownloadFile();
                        break;
                }
            } else {
                // Binary data - encrypted file chunk
                const decryptedChunk = await this.crypto.decryptChunk(data);
                this.receivedChunks.push(new Uint8Array(decryptedChunk));
                this.receivedSize += decryptedChunk.byteLength;

                // Update progress
                if (this.expectedFileInfo) {
                    const progress = (this.receivedSize / this.expectedFileInfo.size) * 100;
                    this.updateProgress(progress, this.receivedSize);
                }
            }
        } catch (error) {
            console.error('Error handling received data:', error);
        }
    }

    /**
     * Assemble received chunks and trigger download
     */
    async assembleAndDownloadFile() {
        try {
            document.getElementById('transfer-status').textContent = 'Assembling file...';

            // Combine chunks
            const totalSize = this.receivedChunks.reduce((acc, chunk) => acc + chunk.byteLength, 0);
            const combined = new Uint8Array(totalSize);
            let offset = 0;
            
            for (const chunk of this.receivedChunks) {
                combined.set(chunk, offset);
                offset += chunk.byteLength;
            }

            // Create download
            const blob = new Blob([combined], { type: this.expectedFileInfo.mimeType || 'application/octet-stream' });
            const url = URL.createObjectURL(blob);
            
            const a = document.createElement('a');
            a.href = url;
            a.download = this.expectedFileInfo.name;
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);

            this.showComplete('File received and saved!');

        } catch (error) {
            console.error('Error assembling file:', error);
            this.showToast('Failed to save file', 'error');
        }
    }

    /**
     * Update progress bar
     */
    updateProgress(percent, bytesTransferred) {
        document.getElementById('progress-fill').style.width = `${percent}%`;
        document.getElementById('progress-percent').textContent = `${percent.toFixed(1)}%`;
        
        // Calculate speed
        const elapsed = (Date.now() - this.transferStartTime) / 1000;
        if (elapsed > 0) {
            const speed = bytesTransferred / elapsed;
            document.getElementById('progress-speed').textContent = `${this.formatSize(speed)}/s`;
        }
    }

    /**
     * Show completion screen
     */
    showComplete(message) {
        this.hide('transfer-section');
        this.show('complete-section');
        document.getElementById('complete-message').textContent = message;
    }

    /**
     * Cancel session
     */
    async cancelSession() {
        try {
            if (this.peerId && this.sessionToken) {
                await fetch(`/api/session/${this.peerId}`, {
                    method: 'DELETE',
                    headers: { 'X-Session-Token': this.sessionToken }
                });
            }
            this.webrtc.close();
            this.reset();
        } catch (error) {
            console.error('Error canceling session:', error);
            this.reset();
        }
    }

    /**
     * Start session timer
     */
    startSessionTimer(seconds) {
        let remaining = seconds;
        
        const updateTimer = () => {
            const mins = Math.floor(remaining / 60);
            const secs = remaining % 60;
            document.getElementById('session-timer').textContent = 
                `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
            
            if (remaining <= 0) {
                clearInterval(this.timerInterval);
                this.showToast('Session expired', 'error');
                this.reset();
            }
            remaining--;
        };

        updateTimer();
        this.timerInterval = setInterval(updateTimer, 1000);
    }

    /**
     * Reset application state
     */
    reset() {
        // Clear timer
        if (this.timerInterval) {
            clearInterval(this.timerInterval);
           }

        // Close connections
        this.webrtc.close();
        
        // Reset state
        this.role = null;
        this.peerId = null;
        this.sessionToken = null;
        this.selectedFile = null;
        this.peerPublicKey = null;
        this.receivedChunks = [];
        this.receivedSize = 0;
        this.expectedFileInfo = null;
        
        // Reset managers
        this.crypto = new CryptoManager();
        this.webrtc = new WebRTCManager();

        // Reset UI
        this.hideAll();
        this.show('role-selection');
        
        // Clear inputs
        document.getElementById('receiver-password').value = '';
        document.getElementById('search-peer-id').value = '';
        document.getElementById('auth-password').value = '';
        document.getElementById('file-input').value = '';
        document.getElementById('selected-file-name').textContent = 'No file selected';
        document.getElementById('btn-send-file').disabled = true;
        
        // Reset progress
        document.getElementById('progress-fill').style.width = '0%';
        document.getElementById('progress-percent').textContent = '0%';
        document.getElementById('progress-speed').textContent = '0 KB/s';
    }

    /**
     * Copy to clipboard
     */
    copyToClipboard(event) {
        const targetId = event.target.dataset.copy;
        const element = document.getElementById(targetId);
        const text = element.value || element.textContent;
        
        navigator.clipboard.writeText(text).then(() => {
            this.showToast('Copied to clipboard!', 'success');
        }).catch(() => {
            this.showToast('Failed to copy', 'error');
        });
    }

    /**
     * Format file size
     */
    formatSize(bytes) {
        if (bytes < 1024) return bytes + ' B';
        if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB';
        if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(1) + ' MB';
        return (bytes / (1024 * 1024 * 1024)).toFixed(2) + ' GB';
    }

    /**
     * Show toast notification
     */
    showToast(message, type = 'info') {
        const container = document.getElementById('toast-container');
        const toast = document.createElement('div');
        toast.className = `toast ${type}`;
        toast.textContent = message;
        container.appendChild(toast);
        
        setTimeout(() => {
            toast.remove();
        }, 3000);
    }

    // UI helper methods
    show(id) {
        document.getElementById(id).classList.remove('hidden');
    }

    hide(id) {
        document.getElementById(id).classList.add('hidden');
    }

    hideAll() {
        [
            'role-selection', 'receiver-section', 'sender-section',
            'transfer-section', 'complete-section', 'btn-back',
            'receiver-create', 'receiver-waiting', 'sender-search',
            'sender-auth', 'sender-file', 'search-result', 'file-info'
        ].forEach(id => {
            const el = document.getElementById(id);
            if (el) el.classList.add('hidden');
        });
        
        // Reset visible steps
        document.getElementById('receiver-create').classList.remove('hidden');
        document.getElementById('sender-search').classList.remove('hidden');
    }
}

// Initialize app when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    window.app = new App();
});
