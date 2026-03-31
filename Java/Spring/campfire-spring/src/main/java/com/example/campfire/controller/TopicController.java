package com.example.campfire.controller;

import java.util.List;
import java.util.UUID;

import org.springframework.http.HttpStatus;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;

import com.example.campfire.dto.TopicCreateRequest;
import com.example.campfire.dto.TopicResponse;
import com.example.campfire.service.TopicService;

import jakarta.validation.Valid;

/**
 * TopicController
 */
@RestController
@RequestMapping("/topics")
public class TopicController {

        private final TopicService topicService;

        public TopicController(TopicService topicService) {
                this.topicService = topicService;
        }

        @PostMapping
        @ResponseStatus(HttpStatus.CREATED)
        public TopicResponse createTopic(
                @Valid @RequestBody TopicCreateRequest request,
                @RequestParam("owner_id") UUID ownerId) {
                return topicService.createTopic(request, ownerId);
        }

        @GetMapping
        public List<TopicResponse> getActiveTopics() {
                return topicService.getActiveTopics();
        }
}
