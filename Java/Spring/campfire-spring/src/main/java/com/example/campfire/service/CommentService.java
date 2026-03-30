package com.example.campfire.service;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.web.server.ResponseStatusException;

import com.example.campfire.dto.CommentCreateRequest;
import com.example.campfire.dto.CommentResponse;
import com.example.campfire.entity.Comment;
import com.example.campfire.entity.Topic;
import com.example.campfire.entity.User;
import com.example.campfire.repository.CommentRepository;
import com.example.campfire.repository.TopicRepository;
import com.example.campfire.repository.UserRepository;

/**
 * CommentService
 */
@Service
public class CommentService {
        private final CommentRepository commentRepository;
        private final TopicRepository topicRepository;
        private final UserRepository userRepository;

        public CommentService(
                CommentRepository commentRepository,
                TopicRepository topicRepository,
                UserRepository userRepository
        ) {
                this.commentRepository = commentRepository;
                this.topicRepository = topicRepository;
                this.userRepository = userRepository;
        }

        @Transactional
        public CommentResponse createComment(UUID topicId, UUID ownerId, CommentCreateRequest request) {
                User owner = userRepository.findById(ownerId)
                        .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "User not found"));

                Topic topic = topicRepository.findById(topicId)
                        .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "Topic not found"));

                if (LocalDateTime.now().isAfter(topic.getTtl())) {
                        throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "This campfire has burned out");
                }

                // Fueling topic (campfire)
                topic.setTotalCommentNubmer(topic.getTotalCommentNubmer() + 1);
                topic.setTtl(topic.getTtl().plusMinutes(1));
                topicRepository.save(topic);

                Comment comment = new Comment();
                comment.setUser(owner);
                comment.setTopic(topic);
                comment.setContent(request.content());

                Comment savedComment = commentRepository.save(comment);
                return new CommentResponse(
                        savedComment.getId(),
                        savedComment.getOwner().getId(),
                        savedComment.getTopic().getId(),
                        savedComment.getContent()
                );
        }

        public List<CommentResponse> getTopicComments(UUID topicId) {
                if (!topicRepository.existsById(topicId)) {
                        throw new ResponseStatusException(HttpStatus.NOT_FOUND, "Topic Not found");
                }

                return commentRepository.findByTopicId(topicId)
                        .stream()
                        .map(comment -> new CommentResponse(
                                comment.getId(),
                                comment.getOwner().getId(),
                                comment.getTopic().getId(),
                                comment.getContent()
                        )).toList();
        }
}
