package com.demofollow.demo.services;

import java.util.logging.Logger;

import org.springframework.stereotype.Service;

import com.demofollow.demo.models.Comment;

/**
 * CommentService
 */
@Service
public class CommentService {

    private Logger logger =
        Logger.getLogger(CommentService.class.getName());

    public void publishComment(Comment comment) {
        logger.info("Publishing comment: "+comment.getText());
    }
}
