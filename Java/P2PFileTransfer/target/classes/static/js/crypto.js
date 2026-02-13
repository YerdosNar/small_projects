/**
 * E2E Encryption Module using Web Crypto API
 * Uses ECDH for key exchange and AES-GCM for encryption
 */
class CryptoManager {
    constructor() {
        this.keyPair = null;
        this.sharedSecret = null;
        this.derivedKey = null;
    }

    /**
     * Generate ECDH key pair
     */
    async generateKeyPair() {
        this.keyPair = await window.crypto.subtle.generateKey(
            {
                name: 'ECDH',
                namedCurve: 'P-256'
            },
            true,
            ['deriveKey', 'deriveBits']
        );
        return this.keyPair;
    }

    /**
     * Export public key to base64 string
     */
    async exportPublicKey() {
        if (!this.keyPair) {
            await this.generateKeyPair();
        }
        const exported = await window.crypto.subtle.exportKey(
            'raw',
            this.keyPair.publicKey
        );
        return this.arrayBufferToBase64(exported);
    }

    /**
     * Import peer's public key from base64 string
     */
    async importPublicKey(base64Key) {
        const keyData = this.base64ToArrayBuffer(base64Key);
        return await window.crypto.subtle.importKey(
            'raw',
            keyData,
            {
                name: 'ECDH',
                namedCurve: 'P-256'
            },
            false,
            []
        );
    }

    /**
     * Derive shared secret from peer's public key
     */
    async deriveSharedKey(peerPublicKeyBase64) {
        if (!this.keyPair) {
            throw new Error('Generate key pair first');
        }

        const peerPublicKey = await this.importPublicKey(peerPublicKeyBase64);

        // Derive AES-GCM key from shared secret
        this.derivedKey = await window.crypto.subtle.deriveKey(
            {
                name: 'ECDH',
                public: peerPublicKey
            },
            this.keyPair.privateKey,
            {
                name: 'AES-GCM',
                length: 256
            },
            false,
            ['encrypt', 'decrypt']
        );

        return this.derivedKey;
    }

    /**
     * Encrypt data using AES-GCM
     */
    async encrypt(data) {
        if (!this.derivedKey) {
            throw new Error('Derive shared key first');
        }

        // Generate random IV
        const iv = window.crypto.getRandomValues(new Uint8Array(12));

        // Encrypt
        const encrypted = await window.crypto.subtle.encrypt(
            {
                name: 'AES-GCM',
                iv: iv
            },
            this.derivedKey,
            data
        );

        // Combine IV + encrypted data
        const combined = new Uint8Array(iv.length + encrypted.byteLength);
        combined.set(iv, 0);
        combined.set(new Uint8Array(encrypted), iv.length);

        return combined.buffer;
    }

    /**
     * Decrypt data using AES-GCM
     */
    async decrypt(encryptedData) {
        if (!this.derivedKey) {
            throw new Error('Derive shared key first');
        }

        const dataArray = new Uint8Array(encryptedData);
        
        // Extract IV (first 12 bytes)
        const iv = dataArray.slice(0, 12);
        
        // Extract encrypted data
        const data = dataArray.slice(12);

        // Decrypt
        const decrypted = await window.crypto.subtle.decrypt(
            {
                name: 'AES-GCM',
                iv: iv
            },
            this.derivedKey,
            data
        );

        return decrypted;
    }

    /**
     * Encrypt a file chunk
     */
    async encryptChunk(chunk) {
        const arrayBuffer = chunk instanceof ArrayBuffer ? chunk : await chunk.arrayBuffer();
        return await this.encrypt(arrayBuffer);
    }

    /**
     * Decrypt a file chunk
     */
    async decryptChunk(encryptedChunk) {
        return await this.decrypt(encryptedChunk);
    }

    // Utility functions
    arrayBufferToBase64(buffer) {
        const bytes = new Uint8Array(buffer);
        let binary = '';
        for (let i = 0; i < bytes.byteLength; i++) {
            binary += String.fromCharCode(bytes[i]);
        }
        return btoa(binary);
    }

    base64ToArrayBuffer(base64) {
        const binary = atob(base64);
        const bytes = new Uint8Array(binary.length);
        for (let i = 0; i < binary.length; i++) {
            bytes[i] = binary.charCodeAt(i);
        }
        return bytes.buffer;
    }
}

// Export for use
window.CryptoManager = CryptoManager;
