package com.demofollow.demo.repositories;

import com.demofollow.demo.models.Comment;

/**
 * CommentRepository
 */
public interface CommentRepository {

    void storeComment(Comment comment);
}
