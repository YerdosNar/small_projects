package com.p2p.filetransfer.model;

/**
 * DTO for sender authentication request
 */
public class AuthenticateRequest {
    private String peerId;
    private String password;
    private String senderPublicKey;
    
    public AuthenticateRequest() {}
    
    public AuthenticateRequest(String peerId, String password, String senderPublicKey) {
        this.peerId = peerId;
        this.password = password;
        this.senderPublicKey = senderPublicKey;
    }
    
    public String getPeerId() {
        return peerId;
    }
    
    public void setPeerId(String peerId) {
        this.peerId = peerId;
    }
    
    public String getPassword() {
        return password;
    }
    
    public void setPassword(String password) {
        this.password = password;
    }
    
    public String getSenderPublicKey() {
        return senderPublicKey;
    }
    
    public void setSenderPublicKey(String senderPublicKey) {
        this.senderPublicKey = senderPublicKey;
    }
}
