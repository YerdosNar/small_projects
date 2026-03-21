package com.demofollow.demo;

import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;

/**
 * ProjectConfiguration
 */
@Configuration
@ComponentScan(basePackages = {"com.demofollow.demo.services", "com.demofollow.demo.repositories"})
public class ProjectConfiguration {
}
