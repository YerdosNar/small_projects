package com.p2p.filetransfer.model;

/**
 * DTO for creating a new peer session (Receiver registration)
 */
public class CreateSessionRequest {
    private String password;
    private String publicKey;
    
    public CreateSessionRequest() {}
    
    public CreateSessionRequest(String password, String publicKey) {
        this.password = password;
        this.publicKey = publicKey;
    }
    
    public String getPassword() {
        return password;
    }
    
    public void setPassword(String password) {
        this.password = password;
    }
    
    public String getPublicKey() {
        return publicKey;
    }
    
    public void setPublicKey(String publicKey) {
        this.publicKey = publicKey;
    }
}
