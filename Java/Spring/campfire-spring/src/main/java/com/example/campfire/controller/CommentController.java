package com.example.campfire.controller;

import java.util.List;
import java.util.UUID;

import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;

import com.example.campfire.dto.CommentCreateRequest;
import com.example.campfire.dto.CommentResponse;
import com.example.campfire.service.CommentService;

import jakarta.validation.Valid;

/**
 * CommentController
 */
@RestController
@RequestMapping("/topics/{topicId}/comments")
public class CommentController {

        private final CommentService commentService;

        public CommentController(CommentService commentService) {
                this.commentService = commentService;
        }

        @PostMapping
        @ResponseStatus(HttpStatus.CREATED)
        public CommentResponse createComment(
                @PathVariable UUID topicId,
                @RequestParam("owner_id") UUID ownerId,
                @Valid @RequestBody CommentCreateRequest request
        ) {
                return commentService.createComment(topicId, ownerId, request);
        }

        @GetMapping
        public List<CommentResponse> getTopicComments(@PathVariable UUID topicId) {
                return commentService.getTopicComments(topicId);
        }
}
