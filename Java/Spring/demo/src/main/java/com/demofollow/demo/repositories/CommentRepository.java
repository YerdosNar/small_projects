package com.demofollow.demo.repositories;

import org.springframework.beans.factory.config.BeanDefinition;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Repository;

import com.demofollow.demo.models.Comment;

/**
 * CommentRepository
 */
@Repository
@Scope(BeanDefinition.SCOPE_PROTOTYPE)
public interface CommentRepository {

    void storeComment(Comment comment);
}
