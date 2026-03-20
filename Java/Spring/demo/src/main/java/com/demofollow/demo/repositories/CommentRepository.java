package com.demofollow.demo.repositories;

import org.springframework.stereotype.Repository;

import com.demofollow.demo.models.Comment;

/**
 * CommentRepository
 */
@Repository
public interface CommentRepository {

    void storeComment(Comment comment);
}
