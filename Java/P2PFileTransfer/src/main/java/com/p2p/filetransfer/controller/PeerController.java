package com.p2p.filetransfer.controller;

import com.p2p.filetransfer.model.*;
import com.p2p.filetransfer.service.PeerSessionService;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

@RestController
@RequestMapping("/api")
@CrossOrigin(origins = "*")
public class PeerController {
    
    private final PeerSessionService sessionService;
    
    @Value("${p2p.stun.servers}")
    private String stunServers;
    
    public PeerController(PeerSessionService sessionService) {
        this.sessionService = sessionService;
    }
    
    /**
     * Create a new peer session (Receiver/PeerA registration)
     * POST /api/session
     */
    @PostMapping("/session")
    public ResponseEntity<CreateSessionResponse> createSession(
            @RequestBody CreateSessionRequest request) {
        
        if (request.getPassword() == null || request.getPassword().length() < 4) {
            return ResponseEntity.badRequest()
                .body(CreateSessionResponse.error("Password must be at least 4 characters"));
        }
        
        if (request.getPublicKey() == null || request.getPublicKey().isEmpty()) {
            return ResponseEntity.badRequest()
                .body(CreateSessionResponse.error("Public key is required"));
        }
        
        CreateSessionResponse response = sessionService.createSession(request);
        return ResponseEntity.ok(response);
    }
    
    /**
     * Search for a peer by ID
     * GET /api/peer/{peerId}
     */
    @GetMapping("/peer/{peerId}")
    public ResponseEntity<Map<String, Object>> searchPeer(@PathVariable String peerId) {
        Map<String, Object> response = new HashMap<>();
        
        Optional<PeerSession> session = sessionService.findByPeerId(peerId.toUpperCase());
        
        if (session.isEmpty()) {
            response.put("found", false);
            response.put("message", "Peer not found or session expired");
            return ResponseEntity.ok(response);
        }
        
        response.put("found", true);
        response.put("peerId", session.get().getPeerId());
        response.put("status", session.get().getStatus().toString());
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * Authenticate sender (PeerB) to connect to receiver (PeerA)
     * POST /api/authenticate
     */
    @PostMapping("/authenticate")
    public ResponseEntity<AuthenticateResponse> authenticate(
            @RequestBody AuthenticateRequest request) {
        
        if (request.getPeerId() == null || request.getPeerId().isEmpty()) {
            return ResponseEntity.badRequest()
                .body(AuthenticateResponse.error("Peer ID is required"));
        }
        
        if (request.getPassword() == null || request.getPassword().isEmpty()) {
            return ResponseEntity.badRequest()
                .body(AuthenticateResponse.error("Password is required"));
        }
        
        if (request.getSenderPublicKey() == null || request.getSenderPublicKey().isEmpty()) {
            return ResponseEntity.badRequest()
                .body(AuthenticateResponse.error("Sender public key is required"));
        }
        
        request.setPeerId(request.getPeerId().toUpperCase());
        AuthenticateResponse response = sessionService.authenticate(request);
        
        return ResponseEntity.ok(response);
    }
    
    /**
     * Get ICE server configuration
     * GET /api/ice-servers
     */
    @GetMapping("/ice-servers")
    public ResponseEntity<Map<String, Object>> getIceServers() {
        Map<String, Object> response = new HashMap<>();
        
        String[] servers = stunServers.split(",");
        Map<String, Object>[] iceServers = new Map[servers.length];
        
        for (int i = 0; i < servers.length; i++) {
            Map<String, Object> server = new HashMap<>();
            server.put("urls", servers[i].trim());
            iceServers[i] = server;
        }
        
        response.put("iceServers", iceServers);
        return ResponseEntity.ok(response);
    }
    
    /**
     * Cancel/delete a session manually
     * DELETE /api/session/{peerId}
     */
    @DeleteMapping("/session/{peerId}")
    public ResponseEntity<Map<String, Object>> deleteSession(
            @PathVariable String peerId,
            @RequestHeader(value = "X-Session-Token", required = false) String sessionToken) {
        
        Map<String, Object> response = new HashMap<>();
        
        Optional<PeerSession> session = sessionService.findByPeerId(peerId.toUpperCase());
        
        if (session.isEmpty()) {
            response.put("success", false);
            response.put("message", "Session not found");
            return ResponseEntity.ok(response);
        }
        
        // Verify session token for security
        if (sessionToken == null || !sessionToken.equals(session.get().getSessionToken())) {
            response.put("success", false);
            response.put("message", "Invalid session token");
            return ResponseEntity.status(401).body(response);
        }
        
        sessionService.deleteSession(peerId.toUpperCase());
        response.put("success", true);
        response.put("message", "Session deleted");
        
        return ResponseEntity.ok(response);
    }
}
