package com.example.campfire.controller;

import java.util.Map;
import java.util.UUID;

import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;

import com.example.campfire.dto.UserCreateRequest;
import com.example.campfire.dto.UserResponse;
import com.example.campfire.service.UserService;

import jakarta.validation.Valid;

/**
 * UserController
 */
@RestController
@RequestMapping("/users")
public class UserController {

        private final UserService userService;

        public UserController(UserService userService) {
                this.userService = userService;
        }

        @PostMapping("/register")
        @ResponseStatus(HttpStatus.CREATED)
        public UserResponse register(@Valid @RequestBody UserCreateRequest request) {
                return userService.register(request);
        }

        @PostMapping("/login")
        public Map<String, Object> login(@Valid @RequestBody UserCreateRequest request) {
                UUID userId = userService.login(request);
                return Map.of(
                        "message", "Login successful",
                        "user_id", userId
                );
        }
}
