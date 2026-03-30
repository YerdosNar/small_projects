package com.example.campfire.repository;

import com.example.campfire.entity.Topic;

import java.time.LocalDateTime;
import java.util.Optional;
import java.util.UUID;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

/**
 * TopicRepository
 */

@Repository
public interface TopicRepository extends JpaRepository<Topic, UUID> {

        // SELECT * FROM topics WHERE ttl < <ttl>
        Optional<Topic> findByTtlBefore(LocalDateTime time);

        Optional<Topic> findByTopicHash(String hash);
}
