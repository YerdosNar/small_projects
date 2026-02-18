package com.p2p.filetransfer.model;

/**
 * DTO for WebRTC signaling messages
 */
public class SignalingMessage {
    private String type;  // offer, answer, ice-candidate, connected, error
    private String from;
    private String to;
    private String payload;  // SDP or ICE candidate JSON
    
    public SignalingMessage() {}
    
    public SignalingMessage(String type, String from, String to, String payload) {
        this.type = type;
        this.from = from;
        this.to = to;
        this.payload = payload;
    }
    
    public static SignalingMessage error(String to, String errorMessage) {
        return new SignalingMessage("error", "server", to, errorMessage);
    }
    
    public static SignalingMessage connected(String to) {
        return new SignalingMessage("connected", "server", to, "P2P connection established");
    }
    
    // Getters and Setters
    public String getType() {
        return type;
    }
    
    public void setType(String type) {
        this.type = type;
    }
    
    public String getFrom() {
        return from;
    }
    
    public void setFrom(String from) {
        this.from = from;
    }
    
    public String getTo() {
        return to;
    }
    
    public void setTo(String to) {
        this.to = to;
    }
    
    public String getPayload() {
        return payload;
    }
    
    public void setPayload(String payload) {
        this.payload = payload;
    }
}
