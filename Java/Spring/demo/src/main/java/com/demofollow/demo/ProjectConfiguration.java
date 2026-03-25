package com.demofollow.demo;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

import com.demofollow.demo.services.CommentService;

/**
 * ProjectConfiguration
 */
@Configuration
@ComponentScan(basePackages = "com.demofollow.demo.services")
public class ProjectConfiguration {
}
