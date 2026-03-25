package com.demofollow.demo.services;

import org.springframework.stereotype.Service;

import com.demofollow.demo.repositories.CommentRepository;

/**
 * UserService
 */
@Service
public class UserService {

    private CommentRepository commentRepository;

    public CommentRepository getCommentRepository() {
        return commentRepository;
    }
}
