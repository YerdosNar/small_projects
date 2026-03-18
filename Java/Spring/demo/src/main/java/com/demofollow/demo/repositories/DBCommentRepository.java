package com.demofollow.demo.repositories;

import org.springframework.stereotype.Repository;

import com.demofollow.demo.models.Comment;

/**
 * DBCommentRepository
 */
@Repository
public class DBCommentRepository implements CommentRepository {

    @Override
    public void storeComment(Comment comment) {
        System.out.println("Storing comment: " + comment.getText());
    }
}
