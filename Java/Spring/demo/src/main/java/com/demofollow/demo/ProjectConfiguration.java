package com.demofollow.demo;

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
