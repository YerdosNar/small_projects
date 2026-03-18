package com.demofollow.demo.proxies;

import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.stereotype.Component;

import com.demofollow.demo.models.Comment;

/**
 * EmailCommentNotificationProxy
 */
@Component
@Qualifier("EMAIL")
public class EmailCommentNotificationProxy
    implements CommentNotificationProxy {

    @Override
    public void sendComment(Comment comment) {
        System.out.println("Sending notification for comment: "
            + comment.getText());
    }
}
