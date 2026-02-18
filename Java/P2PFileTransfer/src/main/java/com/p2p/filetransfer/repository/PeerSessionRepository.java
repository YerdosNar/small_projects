package com.p2p.filetransfer.repository;

import com.p2p.filetransfer.model.PeerSession;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Repository
public interface PeerSessionRepository extends JpaRepository<PeerSession, Long> {
    
    Optional<PeerSession> findByPeerId(String peerId);
    
    Optional<PeerSession> findBySessionToken(String sessionToken);
    
    boolean existsByPeerId(String peerId);
    
    @Query("SELECT p FROM PeerSession p WHERE p.expiresAt < :now")
    List<PeerSession> findExpiredSessions(LocalDateTime now);
    
    @Modifying
    @Transactional
    @Query("DELETE FROM PeerSession p WHERE p.expiresAt < :now")
    int deleteExpiredSessions(LocalDateTime now);
    
    @Modifying
    @Transactional
    void deleteByPeerId(String peerId);
}
