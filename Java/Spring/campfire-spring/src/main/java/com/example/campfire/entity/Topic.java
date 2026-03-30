package com.example.campfire.entity;

import java.time.LocalDateTime;
import java.util.UUID;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.FetchType;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.GenerationType;
import jakarta.persistence.Id;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.ManyToOne;
import jakarta.persistence.Table;

/**
 * Topic
 */
@Entity
@Table(name = "topics")
public class Topic {

        @Id
        @GeneratedValue(strategy = GenerationType.UUID)
        private UUID id;

        @ManyToOne(fetch = FetchType.LAZY)
        @JoinColumn(name = "owner_id", nullable = false)
        private User owner;

        @Column(nullable = false, length = 127)
        private String content;

        @Column(nullable = false, unique = true)
        private String topicHash;

        @Column(nullable = false)
        private LocalDateTime ttl;

        private int currentCommentNumber = 0;
        private int previousCommentNumber = 0;
        private int totalCommentNumber = 0;

        public void setId(UUID id) {this.id = id;}
        public void setOwner(User user) {this.owner= user;}
        public void setContent(String content) {this.content = content;}
        public void setTopicHash(String topicHash) {this.topicHash = topicHash;}
        public void setTtl(LocalDateTime ttl) {this.ttl = ttl;}
        public void setCurrentCommentNumber(int currentCommentNumber) {this.currentCommentNumber = currentCommentNumber;}
        public void setPreviousCommentNumber(int previousCommentNumber) {this.previousCommentNumber = previousCommentNumber;}
        public void setTotalCommentNubmer(int totalCommentNumber) {this.totalCommentNumber = totalCommentNumber;}

        public UUID getId() {return id;}
        public User getOwner() {return owner;}
        public String getContent() {return content;}
        public String getTopicHash() {return topicHash;}
        public LocalDateTime getTtl() {return ttl;}
        public int getCurrentCommentNumber() {return currentCommentNumber;}
        public int getPreviousCommentNumber() {return previousCommentNumber;}
        public int getTotalCommentNubmer() {return totalCommentNumber;}
}
