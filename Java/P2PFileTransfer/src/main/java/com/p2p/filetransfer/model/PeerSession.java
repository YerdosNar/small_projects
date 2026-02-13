package com.p2p.filetransfer.model;

import jakarta.persistence.*;
import java.time.LocalDateTime;

/**
 * Entity representing a temporary peer session.
 * This stores the Receiver's (PeerA) temporary ID and password.
 * The session is automatically deleted after:
 * 1. A successful P2P connection is established
 * 2. The session timeout expires
 */
@Entity
@Table(name = "peer_sessions")
public class PeerSession {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(unique = true, nullable = false)
    private String peerId;  // Unique identifier for searching
    
    @Column(nullable = false)
    private String passwordHash;  // Hashed password for authentication
    
    @Column(nullable = false)
    private String publicKey;  // Public key for E2E encryption
    
    @Column
    private String sessionToken;  // WebSocket session identifier
    
    @Column(nullable = false)
    private LocalDateTime createdAt;
    
    @Column(nullable = false)
    private LocalDateTime expiresAt;
    
    @Column(nullable = false)
    @Enumerated(EnumType.STRING)
    private SessionStatus status = SessionStatus.WAITING;
    
    public enum SessionStatus {
        WAITING,      // Waiting for sender to connect
        CONNECTING,   // P2P connection in progress
        CONNECTED,    // P2P connection established
        EXPIRED       // Session timed out
    }
    
    // Constructors
    public PeerSession() {}
    
    public PeerSession(String peerId, String passwordHash, String publicKey, int timeoutMinutes) {
        this.peerId = peerId;
        this.passwordHash = passwordHash;
        this.publicKey = publicKey;
        this.createdAt = LocalDateTime.now();
        this.expiresAt = this.createdAt.plusMinutes(timeoutMinutes);
        this.status = SessionStatus.WAITING;
    }
    
    // Getters and Setters
    public Long getId() {
        return id;
    }
    
    public void setId(Long id) {
        this.id = id;
    }
    
    public String getPeerId() {
        return peerId;
    }
    
    public void setPeerId(String peerId) {
        this.peerId = peerId;
    }
    
    public String getPasswordHash() {
        return passwordHash;
    }
    
    public void setPasswordHash(String passwordHash) {
        this.passwordHash = passwordHash;
    }
    
    public String getPublicKey() {
        return publicKey;
    }
    
    public void setPublicKey(String publicKey) {
        this.publicKey = publicKey;
    }
    
    public String getSessionToken() {
        return sessionToken;
    }
    
    public void setSessionToken(String sessionToken) {
        this.sessionToken = sessionToken;
    }
    
    public LocalDateTime getCreatedAt() {
        return createdAt;
    }
    
    public void setCreatedAt(LocalDateTime createdAt) {
        this.createdAt = createdAt;
    }
    
    public LocalDateTime getExpiresAt() {
        return expiresAt;
    }
    
    public void setExpiresAt(LocalDateTime expiresAt) {
        this.expiresAt = expiresAt;
    }
    
    public SessionStatus getStatus() {
        return status;
    }
    
    public void setStatus(SessionStatus status) {
        this.status = status;
    }
    
    public boolean isExpired() {
        return LocalDateTime.now().isAfter(expiresAt);
    }
}
