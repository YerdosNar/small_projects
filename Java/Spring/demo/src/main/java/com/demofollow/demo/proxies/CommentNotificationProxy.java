package com.demofollow.demo.proxies;

import com.demofollow.demo.models.Comment;

/**
 * CommentNotificationProxy
 */
public interface CommentNotificationProxy {
    void sendComment(Comment comment);
}
