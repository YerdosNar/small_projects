package com.p2p.filetransfer;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.EnableScheduling;

@SpringBootApplication
@EnableScheduling
public class P2PFileTransferApplication {
    
    public static void main(String[] args) {
        SpringApplication.run(P2PFileTransferApplication.class, args);
    }
}
