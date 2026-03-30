package com.example.campfire.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.Size;

/**
 * UserCreateRequest
 */
public record UserCreateRequest(
        @NotBlank
        @Size(max = 127)
        String username,

        @NotBlank
        String password
) {}
