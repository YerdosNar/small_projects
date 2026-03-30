package com.example.campfire.service;

import com.example.campfire.dto.UserCreateRequest;
import com.example.campfire.dto.UserResponse;
import com.example.campfire.entity.User;
import com.example.campfire.repository.UserRepository;
import org.mindrot.jbcrypt.BCrypt;
import org.springframework.http.HttpStatus;
import org.springframework.web.server.ResponseStatusException;

import java.util.UUID;

/**
 * UserService
 */
public class UserService {

        private final UserRepository userRepository;
        public UserService(UserRepository userRepository) {
                this.userRepository = userRepository;
        }

        public UserResponse register(UserCreateRequest request) {
                // Chekc duplicates
                if (userRepository.findByUsername(request.username()).isPresent()) {
                        throw new ResponseStatusException(HttpStatus.BAD_REQUEST, "Username already taken");
                }

                User user = new User();
                user.setUsername(request.username());
                user.setPassword(BCrypt.hashpw(request.password(), BCrypt.gensalt()));

                User savedUser = userRepository.save(user);

                return new UserResponse(savedUser.getId(), savedUser.getUsername());
        }

        public UUID login(UserCreateRequest request) {
                User user = userRepository.findByUsername(request.username())
                        .orElseThrow(() -> new ResponseStatusException(HttpStatus.UNAUTHORIZED, "Invalid credentials"));

                if (!BCrypt.checkpw(request.password(), user.getPassword())) {
                        throw new ResponseStatusException(HttpStatus.UNAUTHORIZED, "Invalid credentials");
                }

                return user.getId();
        }
}
