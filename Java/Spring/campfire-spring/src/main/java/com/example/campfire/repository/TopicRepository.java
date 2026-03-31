package com.example.campfire.repository;

import com.example.campfire.entity.Topic;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

/**
 * TopicRepository
 */

@Repository
public interface TopicRepository extends JpaRepository<Topic, UUID> {

        Optional<Topic> findByTopicHash(String hash);

        // SELECT * FROM topics WHERE ttl < <ttl>
        List<Topic> findByTtlBefore(LocalDateTime time);
}
