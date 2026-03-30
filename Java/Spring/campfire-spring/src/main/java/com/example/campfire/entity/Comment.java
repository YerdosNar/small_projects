package com.example.campfire.entity;

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
 * Comment
 */
@Entity
@Table(name = "comments")
public class Comment {

        @Id
        @GeneratedValue(strategy = GenerationType.UUID)
        private UUID id;

        @ManyToOne(fetch = FetchType.LAZY)
        @JoinColumn(name = "owner_id", nullable = false)
        private User owner;

        @ManyToOne(fetch = FetchType.LAZY)
        @JoinColumn(name = "topic_id", nullable = false)
        private Topic topic;

        @Column(nullable = false, length = 1023)
        private String content;

        public void setId(UUID id) {this.id = id;}
        public void setUser(User owner) {this.owner= owner;}
        public void setTopic(Topic topic) {this.topic = topic;}
        public void setContent(String content) {this.content = content;}

        public UUID getId() {return id;}
        public User getOwner() {return owner;}
        public Topic getTopic() {return topic;}
        public String getContent() {return content;}
}
