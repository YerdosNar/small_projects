package com.demofollow.demo.services;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.stereotype.Service;

import com.demofollow.demo.models.Comment;
import com.demofollow.demo.proxies.CommentNotificationProxy;
import com.demofollow.demo.repositories.CommentRepository;

/**
 * CommentService
 */
@Service
public class CommentService {

    @Autowired
    private CommentRepository commentRepository;
    private final CommentNotificationProxy commentNotificationProxy;

    public CommentService(
            CommentRepository commentRepository,
            @Qualifier("PUSH")CommentNotificationProxy commentNotificationProxy) {

        this.commentRepository = commentRepository;
        this.commentNotificationProxy = commentNotificationProxy;
    }

    public CommentRepository getCommentRepository() {
        return commentRepository;
    }

    public void publishComment(Comment comment) {
        commentRepository.storeComment(comment);
        commentNotificationProxy.sendComment(comment);
    }
}
