package com.p2p.filetransfer.model;

/**
 * DTO for session creation response
 */
public class CreateSessionResponse {
    private String peerId;
    private String sessionToken;
    private long expiresInSeconds;
    private boolean success;
    private String message;
    
    public CreateSessionResponse() {}
    
    public CreateSessionResponse(String peerId, String sessionToken, long expiresInSeconds) {
        this.peerId = peerId;
        this.sessionToken = sessionToken;
        this.expiresInSeconds = expiresInSeconds;
        this.success = true;
        this.message = "Session created successfully";
    }
    
    public static CreateSessionResponse error(String message) {
        CreateSessionResponse response = new CreateSessionResponse();
        response.success = false;
        response.message = message;
        return response;
    }
    
    // Getters and Setters
    public String getPeerId() {
        return peerId;
    }
    
    public void setPeerId(String peerId) {
        this.peerId = peerId;
    }
    
    public String getSessionToken() {
        return sessionToken;
    }
    
    public void setSessionToken(String sessionToken) {
        this.sessionToken = sessionToken;
    }
    
    public long getExpiresInSeconds() {
        return expiresInSeconds;
    }
    
    public void setExpiresInSeconds(long expiresInSeconds) {
        this.expiresInSeconds = expiresInSeconds;
    }
    
    public boolean isSuccess() {
        return success;
    }
    
    public void setSuccess(boolean success) {
        this.success = success;
    }
    
    public String getMessage() {
        return message;
    }
    
    public void setMessage(String message) {
        this.message = message;
    }
}
