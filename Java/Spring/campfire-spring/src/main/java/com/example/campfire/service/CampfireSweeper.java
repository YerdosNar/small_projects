package com.example.campfire.service;

import java.time.LocalDateTime;
import java.util.List;

import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import com.example.campfire.entity.Topic;
import com.example.campfire.repository.CommentRepository;
import com.example.campfire.repository.TopicRepository;

/**
 * CampfireSweeper
 */
@Service
public class CampfireSweeper {

        private final TopicRepository topicRepository;
        private final CommentRepository commentRepository;

        public CampfireSweeper(TopicRepository topicRepository, CommentRepository commentRepository) {
                this.topicRepository = topicRepository;
                this.commentRepository = commentRepository;
        }

        @Scheduled(fixedRate = 60000) // 60s = 60 * 1000ms
        @Transactional
        public void sweepExpiredTopics() {
                LocalDateTime now = LocalDateTime.now();

                // Find all topic where TTL is in the past
                List<Topic> expiredTopics = topicRepository.findByTtlBefore(now);

                for (Topic topic : expiredTopics) {
                        // Delete all comment in this topic
                        commentRepository.deleteAll(commentRepository.findByTopicId(topic.getId()));

                        // Now delete the topic itself
                        topicRepository.delete(topic);

                        System.out.println("🔥 Campfire " + topic.getId() + " has burned out and was removed.");
                }
        }
}
