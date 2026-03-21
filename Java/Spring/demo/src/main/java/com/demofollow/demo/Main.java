package com.demofollow.demo;

import org.springframework.context.annotation.AnnotationConfigApplicationContext;

import com.demofollow.demo.services.CommentService;
import com.demofollow.demo.services.UserService;

public class Main {
    public static void main(String[] args) {
        var context =
            new AnnotationConfigApplicationContext(
                ProjectConfiguration.class);

        var s1 = context.getBean(CommentService.class);
        var s2 = context.getBean(UserService.class);

        System.out.println(s1);
        System.out.println(s2);

        context.close();
    }
}
