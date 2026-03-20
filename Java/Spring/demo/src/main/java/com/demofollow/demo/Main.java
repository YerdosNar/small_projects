package com.demofollow.demo;


import org.springframework.context.annotation.AnnotationConfigApplicationContext;

import com.demofollow.demo.models.Comment;
import com.demofollow.demo.services.CommentService;
import com.demofollow.demo.services.UserService;

public class Main {
    public static void main(String[] args) {
        var context =
            new AnnotationConfigApplicationContext(
                ProjectConfiguration.class);

        var s1 = context.getBean(CommentService.class);
        var s2 = context.getBean(UserService.class);

        boolean b = s1.getCommentRepository() == s2.getCommentRepository();
        System.out.println(b);

        context.close();
    }
}
