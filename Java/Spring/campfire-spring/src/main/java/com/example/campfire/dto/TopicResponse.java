package com.example.campfire.dto;

import java.time.LocalDateTime;
import java.util.UUID;

/**
 * TopicResponse
 */
public record TopicResponse(
        UUID id,
        UUID ownerId,
        String content,
        String topicHash,
        LocalDateTime ttl,
        int currentCommentNumber,
        int previousCommentNumber,
        int totalCommentNumber
) {}
