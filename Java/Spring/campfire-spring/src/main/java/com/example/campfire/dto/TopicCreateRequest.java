package com.example.campfire.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

/**
 * TopicCreateRequest
 */
public record TopicCreateRequest(
        @NotBlank
        @Size(max = 127)
        String content
) {}
