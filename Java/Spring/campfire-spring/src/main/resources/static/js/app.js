const API_BASE = ''; // Same origin
let currentUser = null; // { id, username }
let activeTopicsInterval = null;
let currentOpenTopicId = null;

// DOM Elements
const authSection = document.getElementById('auth-section');
const appSection = document.getElementById('app-section');
const authForm = document.getElementById('auth-form');
const usernameInput = document.getElementById('username');
const passwordInput = document.getElementById('password');
const currentUserDisplay = document.getElementById('current-user-display');
const btnLogout = document.getElementById('btn-logout');

const createTopicForm = document.getElementById('create-topic-form');
const topicContentInput = document.getElementById('topic-content');

const topicsContainer = document.getElementById('topics-container');
const activeCount = document.getElementById('active-count');

const commentsModal = document.getElementById('comments-modal');
const closeModal = document.getElementById('close-modal');
const modalTopicText = document.getElementById('modal-topic-text');
const modalTimeLeft = document.getElementById('modal-time-left');
const commentsList = document.getElementById('comments-list');
const addCommentForm = document.getElementById('add-comment-form');
const commentInput = document.getElementById('comment-input');

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    const savedUser = localStorage.getItem('campfire_user');
    if (savedUser) {
        currentUser = JSON.parse(savedUser);
        showApp();
    }
});

// Auth Helpers
authForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const username = usernameInput.value.trim();
    const password = passwordInput.value.trim();
    
    if (!username || !password) return;

    const payload = { username, password };

    try {
        // Try to login first
        let response = await fetch(`${API_BASE}/users/login`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (response.ok) {
            const data = await response.json();
            currentUser = { id: data.user_id, username };
            completeLogin();
            return;
        }

        // If login fails, try to register
        response = await fetch(`${API_BASE}/users/register`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (response.ok) {
            const data = await response.json();
            currentUser = { id: data.id, username: data.username };
            completeLogin();
            showToast('Account created and logged in!');
            return;
        } else {
            const err = await response.text();
            showToast(err || 'Authentication failed', true);
        }
    } catch (err) {
        console.error(err);
        showToast('Network error during authentication', true);
    }
});

function completeLogin() {
    localStorage.setItem('campfire_user', JSON.stringify(currentUser));
    showApp();
}

btnLogout.addEventListener('click', () => {
    currentUser = null;
    localStorage.removeItem('campfire_user');
    clearInterval(activeTopicsInterval);
    authSection.classList.remove('hidden');
    appSection.classList.add('hidden');
    usernameInput.value = '';
    passwordInput.value = '';
});

function showApp() {
    authSection.classList.add('hidden');
    appSection.classList.remove('hidden');
    currentUserDisplay.innerText = currentUser.username;
    fetchTopics();
    
    if (activeTopicsInterval) clearInterval(activeTopicsInterval);
    activeTopicsInterval = setInterval(() => {
        updateTimers();
        // every 10 seconds refresh data completely to get new topics
        if (new Date().getSeconds() % 10 === 0) fetchTopics(false); 
    }, 1000);
}

// Topics
createTopicForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    if (!currentUser) return;

    const content = topicContentInput.value.trim();
    if (!content) return;

    try {
        const response = await fetch(`${API_BASE}/topics?owner_id=${currentUser.id}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ content })
        });

        if (response.ok) {
            topicContentInput.value = '';
            showToast('New topic tossed into the fire! 🔥');
            fetchTopics();
        } else {
            const errList = await response.json();
            showToast(errList.message || 'Failed to create topic', true);
        }
    } catch (err) {
        console.error(err);
        showToast('Error creating topic', true);
    }
});

let allTopics = [];

async function fetchTopics(showLoading = true) {
    try {
        const response = await fetch(`${API_BASE}/topics`);
        if (response.ok) {
            allTopics = await response.json();
            renderTopics();
            if (currentOpenTopicId) refreshModalTopic(currentOpenTopicId);
        }
    } catch (err) {
        console.error('Failed fetching topics', err);
    }
}

function renderTopics() {
    topicsContainer.innerHTML = '';
    const template = document.getElementById('topic-card-template');
    
    let active = 0;
    const now = new Date();

    allTopics.forEach(topic => {
        const ttl = new Date(topic.ttl);
        if (ttl <= now) return; // Ignore if actually burned out
        active++;

        const clone = template.content.cloneNode(true);
        const card = clone.querySelector('.topic-card');
        
        clone.querySelector('.topic-content').textContent = topic.content;
        clone.querySelector('.comments-value').textContent = topic.totalCommentNumber || 0;
        
        card.dataset.id = topic.id;
        card.dataset.ttl = topic.ttl;
        
        card.addEventListener('click', () => openTopicModal(topic));
        
        topicsContainer.appendChild(clone);
    });

    activeCount.innerText = active;
    updateTimers();
}

function updateTimers() {
    const cards = document.querySelectorAll('.topic-card');
    const now = new Date();

    cards.forEach(card => {
        const ttlStr = card.dataset.ttl;
        if (!ttlStr) return;
        const ttl = new Date(ttlStr);
        const diff = ttl - now;

        const timeNode = card.querySelector('.time-value');
        if (diff <= 0) {
            timeNode.textContent = 'Burned out';
            card.style.opacity = '0.5';
            card.classList.remove('dying');
            card.style.pointerEvents = 'none';
        } else {
            const secondsLeft = Math.floor(diff / 1000);
            const m = Math.floor(secondsLeft / 60);
            const s = secondsLeft % 60;
            timeNode.textContent = `${m}:${s.toString().padStart(2, '0')}`;
            
            // Visual indicators for dying fires (< 30s)
            if (secondsLeft < 30) {
                card.classList.add('dying');
                timeNode.classList.add('urgent');
            } else {
                card.classList.remove('dying');
                timeNode.classList.remove('urgent');
            }

            // Update flame bar based on 1 minute max (assuming standard fuel is ~60s)
            // It could be more if highly commented, let's normalize to max 2 mins for visual bar
            const maxSecondsForBar = 120; 
            const pct = Math.min(100, Math.max(0, (secondsLeft / maxSecondsForBar) * 100));
            const bar = card.querySelector('.topic-flame-indicator');
            if(bar) bar.style.width = `${pct}%`;
        }
    });

    // Update modal timer if open
    if (currentOpenTopicId && !commentsModal.classList.contains('hidden')) {
        const t = allTopics.find(x => x.id === currentOpenTopicId);
        if (t) {
            const diff = new Date(t.ttl) - now;
            if (diff > 0) {
                const sec = Math.floor(diff / 1000);
                modalTimeLeft.textContent = `⏱ Time left: ${Math.floor(sec/60)}:${(sec%60).toString().padStart(2, '0')}`;
                if (sec < 30) modalTimeLeft.style.color = 'var(--danger)';
                else modalTimeLeft.style.color = '#aaa';
            } else {
                modalTimeLeft.textContent = '🔥 Fire went out';
                modalTimeLeft.style.color = '#777';
            }
        }
    }
}

// Modal and Comments
closeModal.addEventListener('click', () => {
    commentsModal.classList.add('hidden');
    currentOpenTopicId = null;
});

async function openTopicModal(topic) {
    currentOpenTopicId = topic.id;
    modalTopicText.textContent = topic.content;
    commentsModal.classList.remove('hidden');
    commentsList.innerHTML = '<div class="text-center" style="color: #666; padding: 20px;">Loading fuel logs...</div>';
    
    await fetchComments(topic.id);
}

function refreshModalTopic(topicId) {
    const t = allTopics.find(x => x.id === topicId);
    if(t) modalTopicText.textContent = t.content;
}

async function fetchComments(topicId) {
    try {
        const res = await fetch(`${API_BASE}/topics/${topicId}/comments`);
        if (res.ok) {
            const comments = await res.json();
            renderComments(comments);
        }
    } catch (err) {
        console.error(err);
    }
}

function renderComments(comments) {
    commentsList.innerHTML = '';
    const tpl = document.getElementById('comment-template');
    
    if (comments.length === 0) {
        commentsList.innerHTML = '<div class="text-center" style="color: #666; padding: 20px;">No fuel added yet. Be the first!</div>';
        return;
    }

    comments.forEach(c => {
        const clone = tpl.content.cloneNode(true);
        // Display short version of UUID so it fits nicely
        clone.querySelector('.user-id-short').textContent = c.ownerId.split('-')[0];
        clone.querySelector('.comment-text').textContent = c.content;
        commentsList.appendChild(clone);
    });

    // Scroll to bottom
    commentsList.scrollTop = commentsList.scrollHeight;
}

addCommentForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    if (!currentUser || !currentOpenTopicId) return;

    const content = commentInput.value.trim();
    if (!content) return;

    try {
        const res = await fetch(`${API_BASE}/topics/${currentOpenTopicId}/comments?owner_id=${currentUser.id}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ content })
        });

        if (res.ok) {
            commentInput.value = '';
            await fetchComments(currentOpenTopicId);
            // Re-fetch all topics to update TTL visually
            fetchTopics(false);
        } else {
            const err = await res.text();
            showToast(err || 'Failed to add comment', true);
        }
    } catch (err) {
        showToast('Error adding comment', err);
    }
});


// UI Helpers
function showToast(msg, isError = false) {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast ${isError ? 'error' : ''}`;
    toast.innerText = msg;
    container.appendChild(toast);
    setTimeout(() => {
        if(toast.parentNode) toast.parentNode.removeChild(toast);
    }, 3000);
}
