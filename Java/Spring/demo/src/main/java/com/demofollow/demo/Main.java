package com.demofollow.demo;

import org.springframework.context.annotation.AnnotationConfigApplicationContext;

import com.demofollow.demo.models.Comment;
import com.demofollow.demo.services.CommentService;

public class Main {
    public static void main(String[] args) {
        var context =
            new AnnotationConfigApplicationContext(
                ProjectConfiguration.class);

        var service = context.getBean(CommentService.class);

        Comment comment = new Comment();
        comment.setText("Demo comment");
        comment.setAuthor("Natasha");

        service.publishComment(comment);

        context.close();
    }
}
