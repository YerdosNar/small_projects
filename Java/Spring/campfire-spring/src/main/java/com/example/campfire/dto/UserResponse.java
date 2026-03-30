package com.example.campfire.dto;

import java.util.UUID;

/**
 * UserResponse
 */
public record UserResponse(
        UUID id,
        String username
) {}
