package com.example.campfire.repository;

import com.example.campfire.entity.Comment;

import java.util.List;
import java.util.UUID;

import org.springframework.data.jpa.repository.JpaRepository;

/**
 * CommentRepository
 */
public interface CommentRepository extends JpaRepository<Comment, UUID> {

        // SELECT * FROM comments WHERE topic_id = <topic_id>
        List<Comment> findByTopicId(UUID topicId);
}
