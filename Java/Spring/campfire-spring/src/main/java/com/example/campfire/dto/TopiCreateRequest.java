package com.example.campfire.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

/**
 * TopiCreateRequest
 */
public record TopiCreateRequest(
        @NotBlank
        @Size(max = 127)
        String content
) {}
