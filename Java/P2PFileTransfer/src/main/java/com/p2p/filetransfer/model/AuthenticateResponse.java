package com.p2p.filetransfer.model;

/**
 * DTO for authentication response
 */
public class AuthenticateResponse {
    private boolean success;
    private String message;
    private String receiverPublicKey;
    private String sessionToken;
    
    public AuthenticateResponse() {}
    
    public AuthenticateResponse(boolean success, String message) {
        this.success = success;
        this.message = message;
    }
    
    public static AuthenticateResponse success(String receiverPublicKey, String sessionToken) {
        AuthenticateResponse response = new AuthenticateResponse(true, "Authentication successful");
        response.setReceiverPublicKey(receiverPublicKey);
        response.setSessionToken(sessionToken);
        return response;
    }
    
    public static AuthenticateResponse error(String message) {
        return new AuthenticateResponse(false, message);
    }
    
    // Getters and Setters
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
    
    public String getReceiverPublicKey() {
        return receiverPublicKey;
    }
    
    public void setReceiverPublicKey(String receiverPublicKey) {
        this.receiverPublicKey = receiverPublicKey;
    }
    
    public String getSessionToken() {
        return sessionToken;
    }
    
    public void setSessionToken(String sessionToken) {
        this.sessionToken = sessionToken;
    }
}
