package com.example.campfire.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

/**
 * CommentCreateRequest
 */
public record CommentCreateRequest(
        @NotBlank
        @Size(max = 1023)
        String content
) {}
