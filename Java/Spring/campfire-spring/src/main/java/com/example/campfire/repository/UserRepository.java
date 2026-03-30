package com.example.campfire.repository;

import com.example.campfire.entity.User;

import java.util.Optional;
import java.util.UUID;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

/**
 * UserRepository
 */
@Repository
public interface UserRepository extends JpaRepository<User, UUID>{

        // SELECT * FROM users WHERE username = <username>
        Optional<User> findByUsername(String username);
}
