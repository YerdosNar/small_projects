package com.demofollow.demo;


import org.springframework.context.annotation.AnnotationConfigApplicationContext;

import com.demofollow.demo.models.Comment;
import com.demofollow.demo.services.CommentService;

public class Main {
    public static void main(String[] args) {
        var context =
            new AnnotationConfigApplicationContext(
                ProjectConfiguration.class);

        var comment = new Comment();
        comment.setAuthor("Yerdos");
        comment.setText("Demo comment");

        var commentService = context.getBean(CommentService.class);
        commentService.publishComment(comment);

        context.close();
    }
}
