package com.demofollow.demo;

import com.demofollow.demo.proxies.CommentNotificationProxy;
import com.demofollow.demo.proxies.CommentPushNotificationProxy;
import com.demofollow.demo.proxies.EmailCommentNotificationProxy;
import com.demofollow.demo.repositories.CommentRepository;
import com.demofollow.demo.repositories.DBCommentRepository;
import com.demofollow.demo.services.CommentService;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

/**
 * ProjectConfiguration
 */
@Configuration
@ComponentScan(basePackages = "com.demofollow.demo")
public class ProjectConfiguration {

    // @Bean
    // public CommentRepository commentRepository() {
    //     return new DBCommentRepository();
    // }
    //
    // @Bean
    // public CommentNotificationProxy emailCommentNotificationProxy() {
    //     return new EmailCommentNotificationProxy();
    // }
    //
    // @Bean
    // public CommentNotificationProxy commentPushNotificationProxy() {
    //     return new CommentPushNotificationProxy();
    // }
    //
    // @Bean
    // CommentService commentService(
    //         CommentRepository commentRepository,
    //         CommentNotificationProxy commentNotificationProxy) {
    //     return new CommentService(
    //         commentRepository,
    //         commentNotificationProxy);
    // }
}
