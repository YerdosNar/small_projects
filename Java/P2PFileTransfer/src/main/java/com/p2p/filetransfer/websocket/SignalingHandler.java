package com.p2p.filetransfer.websocket;

import com.google.gson.Gson;
import com.p2p.filetransfer.model.PeerSession;
import com.p2p.filetransfer.model.SignalingMessage;
import com.p2p.filetransfer.service.PeerSessionService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.io.IOException;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;

/**
 * WebSocket handler for WebRTC signaling.
 * Handles SDP offer/answer and ICE candidate exchange between peers.
 */
@Component
public class SignalingHandler extends TextWebSocketHandler {
    
    private static final Logger logger = LoggerFactory.getLogger(SignalingHandler.class);
    
    private final Gson gson = new Gson();
    private final PeerSessionService sessionService;
    
    // Map of peerId -> WebSocketSession
    private final Map<String, WebSocketSession> peerSessions = new ConcurrentHashMap<>();
    
    // Map of WebSocketSession ID -> peerId
    private final Map<String, String> sessionToPeerId = new ConcurrentHashMap<>();
    
    // Map of peerId -> connected peerId (for routing messages)
    private final Map<String, String> connectedPeers = new ConcurrentHashMap<>();
    
    public SignalingHandler(PeerSessionService sessionService) {
        this.sessionService = sessionService;
    }
    
    @Override
    public void afterConnectionEstablished(WebSocketSession session) throws Exception {
        logger.info("WebSocket connection established: {}", session.getId());
    }
    
    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
        try {
            SignalingMessage sigMsg = gson.fromJson(message.getPayload(), SignalingMessage.class);
            logger.debug("Received signaling message: type={}, from={}, to={}", 
                sigMsg.getType(), sigMsg.getFrom(), sigMsg.getTo());
            
            switch (sigMsg.getType()) {
                case "register":
                    handleRegister(session, sigMsg);
                    break;
                case "offer":
                    handleOffer(session, sigMsg);
                    break;
                case "answer":
                    handleAnswer(session, sigMsg);
                    break;
                case "ice-candidate":
                    handleIceCandidate(session, sigMsg);
                    break;
                case "connected":
                    handleConnected(session, sigMsg);
                    break;
                case "disconnect":
                    handleDisconnect(session, sigMsg);
                    break;
                default:
                    sendError(session, "Unknown message type: " + sigMsg.getType());
            }
        } catch (Exception e) {
            logger.error("Error handling message", e);
            sendError(session, "Error processing message: " + e.getMessage());
        }
    }
    
    /**
     * Register a peer with the signaling server
     */
    private void handleRegister(WebSocketSession session, SignalingMessage msg) throws IOException {
        String peerId = msg.getFrom();
        String sessionToken = msg.getPayload();
        
        // Verify the session token
        Optional<PeerSession> peerSession = sessionService.findBySessionToken(sessionToken);
        
        if (peerSession.isEmpty()) {
            sendError(session, "Invalid session token");
            return;
        }
        
        // Check if this is the receiver (session owner) or sender
        boolean isReceiver = peerSession.get().getPeerId().equals(peerId);
        
        // Register the WebSocket session
        peerSessions.put(peerId, session);
        sessionToPeerId.put(session.getId(), peerId);
        
        // Send confirmation
        SignalingMessage response = new SignalingMessage(
            "registered", "server", peerId, 
            isReceiver ? "receiver" : "sender"
        );
        session.sendMessage(new TextMessage(gson.toJson(response)));
        
        logger.info("Peer {} registered as {}", peerId, isReceiver ? "receiver" : "sender");
    }
    
    /**
     * Handle WebRTC offer from sender
     */
    private void handleOffer(WebSocketSession session, SignalingMessage msg) throws IOException {
        String targetPeerId = msg.getTo();
        WebSocketSession targetSession = peerSessions.get(targetPeerId);
        
        if (targetSession == null || !targetSession.isOpen()) {
            sendError(session, "Target peer not connected");
            return;
        }
        
        // Store the connection mapping
        connectedPeers.put(msg.getFrom(), targetPeerId);
        connectedPeers.put(targetPeerId, msg.getFrom());
        
        // Forward the offer to the receiver
        targetSession.sendMessage(new TextMessage(gson.toJson(msg)));
        logger.debug("Forwarded offer from {} to {}", msg.getFrom(), targetPeerId);
    }
    
    /**
     * Handle WebRTC answer from receiver
     */
    private void handleAnswer(WebSocketSession session, SignalingMessage msg) throws IOException {
        String targetPeerId = msg.getTo();
        WebSocketSession targetSession = peerSessions.get(targetPeerId);
        
        if (targetSession == null || !targetSession.isOpen()) {
            sendError(session, "Target peer not connected");
            return;
        }
        
        // Forward the answer to the sender
        targetSession.sendMessage(new TextMessage(gson.toJson(msg)));
        logger.debug("Forwarded answer from {} to {}", msg.getFrom(), targetPeerId);
    }
    
    /**
     * Handle ICE candidate exchange
     */
    private void handleIceCandidate(WebSocketSession session, SignalingMessage msg) throws IOException {
        String targetPeerId = msg.getTo();
        WebSocketSession targetSession = peerSessions.get(targetPeerId);
        
        if (targetSession == null || !targetSession.isOpen()) {
            // ICE candidates can arrive before/after connection, just log it
            logger.debug("Target peer {} not connected for ICE candidate", targetPeerId);
            return;
        }
        
        // Forward the ICE candidate
        targetSession.sendMessage(new TextMessage(gson.toJson(msg)));
        logger.debug("Forwarded ICE candidate from {} to {}", msg.getFrom(), targetPeerId);
    }
    
    /**
     * Handle P2P connection established notification
     */
    private void handleConnected(WebSocketSession session, SignalingMessage msg) throws IOException {
        String peerId = msg.getFrom();
        logger.info("P2P connection established for peer: {}", peerId);
        
        // Mark the session as connected
        sessionService.markConnected(peerId);
        
        // Get the connected peer
        String connectedPeerId = connectedPeers.get(peerId);
        
        // Delete the session from database (cleanup after successful connection)
        // We only delete when both peers have confirmed connection
        // For simplicity, delete immediately - in production you might want to wait
        sessionService.deleteSession(peerId);
        
        // Notify both peers
        SignalingMessage response = SignalingMessage.connected(peerId);
        session.sendMessage(new TextMessage(gson.toJson(response)));
        
        if (connectedPeerId != null) {
            WebSocketSession otherSession = peerSessions.get(connectedPeerId);
            if (otherSession != null && otherSession.isOpen()) {
                SignalingMessage otherResponse = SignalingMessage.connected(connectedPeerId);
                otherSession.sendMessage(new TextMessage(gson.toJson(otherResponse)));
            }
        }
    }
    
    /**
     * Handle peer disconnect
     */
    private void handleDisconnect(WebSocketSession session, SignalingMessage msg) throws IOException {
        String peerId = msg.getFrom();
        cleanup(session);
        
        logger.info("Peer {} disconnected", peerId);
    }
    
    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
        cleanup(session);
        logger.info("WebSocket connection closed: {} with status {}", session.getId(), status);
    }
    
    @Override
    public void handleTransportError(WebSocketSession session, Throwable exception) throws Exception {
        logger.error("WebSocket transport error for session {}", session.getId(), exception);
        cleanup(session);
    }
    
    /**
     * Clean up session data
     */
    private void cleanup(WebSocketSession session) {
        String peerId = sessionToPeerId.remove(session.getId());
        if (peerId != null) {
            peerSessions.remove(peerId);
            
            // Notify connected peer if any
            String connectedPeerId = connectedPeers.remove(peerId);
            if (connectedPeerId != null) {
                connectedPeers.remove(connectedPeerId);
                WebSocketSession connectedSession = peerSessions.get(connectedPeerId);
                if (connectedSession != null && connectedSession.isOpen()) {
                    try {
                        SignalingMessage disconnectMsg = new SignalingMessage(
                            "peer-disconnected", "server", connectedPeerId, peerId
                        );
                        connectedSession.sendMessage(new TextMessage(gson.toJson(disconnectMsg)));
                    } catch (IOException e) {
                        logger.error("Failed to notify peer of disconnect", e);
                    }
                }
            }
        }
    }
    
    /**
     * Send error message to a session
     */
    private void sendError(WebSocketSession session, String errorMessage) throws IOException {
        SignalingMessage error = SignalingMessage.error(
            sessionToPeerId.getOrDefault(session.getId(), "unknown"), 
            errorMessage
        );
        session.sendMessage(new TextMessage(gson.toJson(error)));
    }
}
