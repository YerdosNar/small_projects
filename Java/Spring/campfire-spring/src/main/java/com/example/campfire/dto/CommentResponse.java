package com.example.campfire.dto;

import java.util.UUID;

/**
 * CommentResponse
 */
public record CommentResponse(
        UUID id,
        UUID ownerId,
        UUID topicId,
        String content
) {}
