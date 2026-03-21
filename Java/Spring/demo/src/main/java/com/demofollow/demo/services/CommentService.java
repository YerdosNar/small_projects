package com.demofollow.demo.services;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import com.demofollow.demo.models.Comment;
import com.demofollow.demo.processor.CommentProcessor;

/**
 * CommentService
 */
@Service
public class CommentService {

    @Autowired
    private CommentProcessor p;

    public void sendComment(Comment c) {
        p.setComment(c);
        p.processComment();
        p.validateComment();

        c = p.getComment();
    }
}
