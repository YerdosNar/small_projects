package com.p2p.filetransfer.config;

import com.p2p.filetransfer.websocket.SignalingHandler;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.socket.config.annotation.EnableWebSocket;
import org.springframework.web.socket.config.annotation.WebSocketConfigurer;
import org.springframework.web.socket.config.annotation.WebSocketHandlerRegistry;
import org.springframework.web.socket.server.standard.ServletServerContainerFactoryBean;

@Configuration
@EnableWebSocket
public class WebSocketConfig implements WebSocketConfigurer {

    // 64 KB – enough to hold large SDP offers/answers with many ICE candidates
    private static final int MAX_WS_MESSAGE_SIZE = 64 * 1024;

    private final SignalingHandler signalingHandler;
    
    public WebSocketConfig(SignalingHandler signalingHandler) {
        this.signalingHandler = signalingHandler;
    }
    
    @Override
    public void registerWebSocketHandlers(WebSocketHandlerRegistry registry) {
        registry.addHandler(signalingHandler, "/signaling")
                .setAllowedOrigins("*");
    }

    /**
     * Increase the WebSocket container message buffer sizes so that large SDP
     * messages (offer/answer) are not silently truncated or rejected by Tomcat.
     */
    @Bean
    public ServletServerContainerFactoryBean createWebSocketContainer() {
        ServletServerContainerFactoryBean container = new ServletServerContainerFactoryBean();
        container.setMaxTextMessageBufferSize(MAX_WS_MESSAGE_SIZE);
        container.setMaxBinaryMessageBufferSize(MAX_WS_MESSAGE_SIZE);
        return container;
    }
}
