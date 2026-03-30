package com.example.campfire.service;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Service;
import org.springframework.web.server.ResponseStatusException;

import com.example.campfire.dto.TopicCreateRequest;
import com.example.campfire.dto.TopicResponse;
import com.example.campfire.entity.Topic;
import com.example.campfire.entity.User;
import com.example.campfire.repository.TopicRepository;
import com.example.campfire.repository.UserRepository;

/**
 * TopicService
 */
@Service
public class TopicService {

        private final TopicRepository topicRepository;
        private final UserRepository userRepository;

        public TopicService(TopicRepository topicRepository, UserRepository userRepository) {
                this.topicRepository = topicRepository;
                this.userRepository = userRepository;
        }

        public TopicResponse createTopic(TopicCreateRequest request, UUID ownerId) {
                User owner = userRepository.findById(ownerId)
                        .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "Not Found"));

                String hash = generateHash(request.content());

                if (topicRepository.findByTopicHash(hash).isPresent()) {
                        throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "A topic with the same content already exists");
                }

                Topic topic = new Topic();
                topic.setOwner(owner);
                topic.setContent(request.content());
                topic.setTopicHash(hash);
                topic.setTtl(LocalDateTime.now().plusHours(1));

                Topic savedTopic = topicRepository.save(topic);
                return mapToResponse(savedTopic);
        }

        public List<TopicResponse> getActiveTopics() {
                return topicRepository.findAll()
                        .stream()
                        .map(this::mapToResponse)
                        .toList();
        }

        private String generateHash(String content) {
                try {
                        MessageDigest digest = MessageDigest.getInstance("SHA-256");
                        byte[] encodehash = digest.digest(content.getBytes(StandardCharsets.UTF_8));
                        StringBuilder hexString = new StringBuilder(2 * encodehash.length);
                        for (byte b : encodehash) {
                                String hex = Integer.toHexString(0xFF & b);
                                if (hex.length() == 1) hexString.append('0');
                                hexString.append(hex);
                        }
                        return hexString.toString();
                }
                catch (NoSuchAlgorithmException e) {
                        throw new RuntimeException("ERROR: Hashing topi content failed", e);
                }
        }

        public TopicResponse mapToResponse(Topic topic) {
                return new TopicResponse(
                        topic.getId(),
                        topic.getOwner().getId(),
                        topic.getContent(),
                        topic.getTopicHash(),
                        topic.getTtl(),
                        topic.getCurrentCommentNumber(),
                        topic.getPreviousCommentNumber(),
                        topic.getTotalCommentNubmer()
                );
        }
}
