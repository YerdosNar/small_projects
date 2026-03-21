package com.demofollow.demo.processor;

import org.springframework.beans.factory.config.BeanDefinition;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import com.demofollow.demo.models.Comment;

/**
 * CommentProcessor
 */
@Component
@Scope(BeanDefinition.SCOPE_PROTOTYPE)
public class CommentProcessor {

    private Comment comment;

    public void setComment(Comment comment) {
        this.comment = comment;
    }

    public Comment getComment() {
        return comment;
    }

    public void processComment() {
        // changing the comment attribute
        System.out.println("processComment()");
    }

    public void validateComment() {
        // validating comment
        System.out.println("validateComment()");
    }
}

