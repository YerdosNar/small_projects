package com.p2p.filetransfer.service;

import com.p2p.filetransfer.model.*;
import com.p2p.filetransfer.repository.PeerSessionRepository;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;
import java.util.Base64;
import java.util.Optional;

@Service
public class PeerSessionService {
    
    private static final Logger logger = LoggerFactory.getLogger(PeerSessionService.class);
    
    private final PeerSessionRepository repository;
    private final SecureRandom secureRandom = new SecureRandom();
    
    @Value("${p2p.session.timeout-minutes:30}")
    private int sessionTimeoutMinutes;
    
    public PeerSessionService(PeerSessionRepository repository) {
        this.repository = repository;
    }
    
    /**
     * Creates a new peer session for the receiver (PeerA)
     */
    public CreateSessionResponse createSession(CreateSessionRequest request) {
        try {
            // Generate unique peer ID
            String peerId = generatePeerId();
            
            // Hash the password
            String passwordHash = hashPassword(request.getPassword());
            
            // Generate session token
            String sessionToken = generateSessionToken();
            
            // Create and save session
            PeerSession session = new PeerSession(
                peerId, 
                passwordHash, 
                request.getPublicKey(),
                sessionTimeoutMinutes
            );
            session.setSessionToken(sessionToken);
            
            repository.save(session);
            
            long expiresInSeconds = ChronoUnit.SECONDS.between(
                LocalDateTime.now(), 
                session.getExpiresAt()
            );
            
            logger.info("Created session for peer: {}", peerId);
            
            return new CreateSessionResponse(peerId, sessionToken, expiresInSeconds);
            
        } catch (Exception e) {
            logger.error("Failed to create session", e);
            return CreateSessionResponse.error("Failed to create session: " + e.getMessage());
        }
    }
    
    /**
     * Searches for a peer by ID
     */
    public Optional<PeerSession> findByPeerId(String peerId) {
        Optional<PeerSession> session = repository.findByPeerId(peerId);
        
        // Check if session exists and is not expired
        if (session.isPresent() && session.get().isExpired()) {
            repository.delete(session.get());
            return Optional.empty();
        }
        
        return session;
    }
    
    /**
     * Authenticates a sender (PeerB) trying to connect to a receiver (PeerA)
     */
    public AuthenticateResponse authenticate(AuthenticateRequest request) {
        Optional<PeerSession> sessionOpt = findByPeerId(request.getPeerId());
        
        if (sessionOpt.isEmpty()) {
            return AuthenticateResponse.error("Peer not found or session expired");
        }
        
        PeerSession session = sessionOpt.get();
        
        // Verify password
        String inputPasswordHash = hashPassword(request.getPassword());
        if (!session.getPasswordHash().equals(inputPasswordHash)) {
            return AuthenticateResponse.error("Invalid password");
        }
        
        // Check if already connecting/connected
        if (session.getStatus() != PeerSession.SessionStatus.WAITING) {
            return AuthenticateResponse.error("Peer is already in a connection");
        }
        
        // Update status to connecting
        session.setStatus(PeerSession.SessionStatus.CONNECTING);
        repository.save(session);
        
        logger.info("Authenticated sender for peer: {}", request.getPeerId());
        
        return AuthenticateResponse.success(session.getPublicKey(), session.getSessionToken());
    }
    
    /**
     * Marks a session as connected (P2P connection established)
     */
    public void markConnected(String peerId) {
        Optional<PeerSession> sessionOpt = repository.findByPeerId(peerId);
        if (sessionOpt.isPresent()) {
            PeerSession session = sessionOpt.get();
            session.setStatus(PeerSession.SessionStatus.CONNECTED);
            repository.save(session);
            logger.info("Peer {} marked as connected", peerId);
        }
    }
    
    /**
     * Deletes a session after successful P2P connection
     */
    public void deleteSession(String peerId) {
        repository.deleteByPeerId(peerId);
        logger.info("Deleted session for peer: {}", peerId);
    }
    
    /**
     * Finds session by WebSocket session token
     */
    public Optional<PeerSession> findBySessionToken(String sessionToken) {
        return repository.findBySessionToken(sessionToken);
    }
    
    /**
     * Scheduled task to clean up expired sessions
     */
    @Scheduled(fixedRate = 60000) // Run every minute
    public void cleanupExpiredSessions() {
        int deleted = repository.deleteExpiredSessions(LocalDateTime.now());
        if (deleted > 0) {
            logger.info("Cleaned up {} expired sessions", deleted);
        }
    }
    
    // Helper methods
    
    private String generatePeerId() {
        // Generate a 6-character alphanumeric ID (easy to share)
        String chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"; // Excluded similar chars like 0/O, 1/I
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < 6; i++) {
            sb.append(chars.charAt(secureRandom.nextInt(chars.length())));
        }
        
        // Ensure uniqueness
        String peerId = sb.toString();
        while (repository.existsByPeerId(peerId)) {
            sb = new StringBuilder();
            for (int i = 0; i < 6; i++) {
                sb.append(chars.charAt(secureRandom.nextInt(chars.length())));
            }
            peerId = sb.toString();
        }
        
        return peerId;
    }
    
    private String generateSessionToken() {
        byte[] bytes = new byte[32];
        secureRandom.nextBytes(bytes);
        return Base64.getUrlEncoder().withoutPadding().encodeToString(bytes);
    }
    
    private String hashPassword(String password) {
        try {
            MessageDigest md = MessageDigest.getInstance("SHA-256");
            byte[] hash = md.digest(password.getBytes(StandardCharsets.UTF_8));
            return Base64.getEncoder().encodeToString(hash);
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException("SHA-256 not available", e);
        }
    }
}
