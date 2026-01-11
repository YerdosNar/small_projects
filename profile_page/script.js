/**
 * Terminal Profile Page
 * A terminal-style interactive profile page
 */

// =============================================================================
// CONFIGURATION
// =============================================================================

const CONFIG = {
    API_BASE: '/api', // Spring Boot backend base URL
    MAX_COMMENT_LENGTH: 256,
    CURSOR_BLINK_DELAY: 100,
};

// =============================================================================
// STATE MANAGEMENT
// =============================================================================

const state = {
    currentSection: 'home',
    currentPath: '~/',
    isAuthenticated: false,
    isInLeaveMessage: false,
    commandHistory: [],
    historyIndex: -1,
};

// =============================================================================
// DOM ELEMENTS
// =============================================================================

const elements = {
    container: document.getElementById('input-container'),
    ghostInput: document.getElementById('ghost-input'),
    typedText: document.getElementById('typed-text'),
    cursor: document.querySelector('.cursor'),
    terminalTitle: document.querySelector('.terminal-title'),
    promptPwd: document.querySelector('#input-container .pwd'),
    sections: document.querySelectorAll('.terminal-section'),
    dirButtons: document.querySelectorAll('.dir-button'),
};

// =============================================================================
// TERMINAL INPUT HANDLING
// =============================================================================

const terminal = {
    init() {
        this.bindEvents();
        this.focus();
    },

    bindEvents() {
        // Focus input on container click
        elements.container.addEventListener('click', () => this.focus());
        
        // Sync input to display
        elements.ghostInput.addEventListener('input', () => this.syncDisplay());
        
        // Handle special keys
        elements.ghostInput.addEventListener('keydown', (e) => this.handleKeydown(e));
        
        // Directory button clicks
        elements.dirButtons.forEach(btn => {
            btn.addEventListener('click', () => {
                const section = btn.dataset.section;
                this.navigateTo(section);
            });
        });
    },

    focus() {
        elements.ghostInput.focus();
    },

    syncDisplay() {
        elements.typedText.textContent = elements.ghostInput.value;
    },

    pauseCursorBlink() {
        elements.cursor.style.animation = 'none';
        setTimeout(() => {
            elements.cursor.style.animation = 'blink 1s step-end infinite';
        }, CONFIG.CURSOR_BLINK_DELAY);
    },

    handleKeydown(e) {
        this.pauseCursorBlink();

        if (e.key === 'Enter') {
            e.preventDefault();
            const command = elements.ghostInput.value.trim();
            if (command) {
                this.executeCommand(command);
                state.commandHistory.push(command);
                state.historyIndex = state.commandHistory.length;
            }
            this.clear();
        } else if (e.key === 'ArrowUp') {
            e.preventDefault();
            this.navigateHistory(-1);
        } else if (e.key === 'ArrowDown') {
            e.preventDefault();
            this.navigateHistory(1);
        }
    },

    navigateHistory(direction) {
        const newIndex = state.historyIndex + direction;
        if (newIndex >= 0 && newIndex < state.commandHistory.length) {
            state.historyIndex = newIndex;
            elements.ghostInput.value = state.commandHistory[newIndex];
            this.syncDisplay();
        } else if (newIndex >= state.commandHistory.length) {
            state.historyIndex = state.commandHistory.length;
            this.clear();
        }
    },

    clear() {
        elements.ghostInput.value = '';
        this.syncDisplay();
    },

    executeCommand(input) {
        const [command, ...args] = input.split(' ');
        const argument = args.join(' ');

        // Handle LeaveMessage mode separately
        if (state.isInLeaveMessage) {
            comments.handleInput(input);
            return;
        }

        switch (command.toLowerCase()) {
            case 'cd':
                this.handleCd(argument);
                break;
            case 'ls':
                this.handleLs(argument);
                break;
            case 'auth':
                this.handleAuth(argument);
                break;
            case 'logout':
                this.handleLogout();
                break;
            case 'clear':
                this.handleClear();
                break;
            case 'help':
                this.showHelp();
                break;
            default:
                this.showError(`Command not found: ${command}. Type 'help' for available commands.`);
        }
    },

    handleCd(path) {
        if (!path || path === '~' || path === '~/') {
            this.navigateTo('home');
        } else if (path === '..' || path === '../') {
            if (state.isInLeaveMessage) {
                state.isInLeaveMessage = false;
                this.navigateTo('contact');
            } else {
                this.navigateTo('home');
            }
        } else {
            const cleanPath = path.replace(/^~?\/?/, '').replace(/\/$/, '').toLowerCase();
            
            if (cleanPath === 'leavemessage' && state.currentSection === 'contact') {
                state.isInLeaveMessage = true;
                comments.showForm();
                this.updatePath('~/Contact/LeaveMessage');
            } else if (['contact', 'projects', 'about', 'assets'].includes(cleanPath)) {
                this.navigateTo(cleanPath);
            } else {
                this.showError(`Directory not found: ${path}`);
            }
        }
    },

    handleLs(args) {
        // Trigger ls display in current section
        const section = document.getElementById(state.currentSection);
        const lsOutput = section?.querySelector('.ls-output');
        if (lsOutput) {
            lsOutput.classList.add('visible');
        }
    },

    handleAuth(password) {
        if (!password) {
            this.showError('Usage: auth <password>');
            return;
        }
        auth.login(password);
    },

    handleLogout() {
        auth.logout();
    },

    handleClear() {
        const section = document.getElementById(state.currentSection);
        const output = section?.querySelector('.dynamic-output');
        if (output) {
            output.innerHTML = '';
        }
    },

    showHelp() {
        const helpText = `
Available commands:
  cd <dir>      - Navigate to directory
  ls            - List contents
  auth <pass>   - Authenticate to view private assets
  logout        - Log out of private assets
  clear         - Clear terminal output
  help          - Show this help message

Directories: ~/, ~/Contact, ~/Projects, ~/About, ~/Assets
        `;
        this.appendOutput(helpText, 'help-output');
    },

    showError(message) {
        this.appendOutput(message, 'error-output');
    },

    appendOutput(text, className = '') {
        const section = document.getElementById(state.currentSection);
        let output = section?.querySelector('.dynamic-output');
        
        if (!output) {
            output = document.createElement('div');
            output.className = 'dynamic-output';
            section?.appendChild(output);
        }

        const line = document.createElement('pre');
        line.className = `output-line ${className}`;
        line.textContent = text;
        output.appendChild(line);
    },

    navigateTo(sectionId) {
        state.isInLeaveMessage = false;
        state.currentSection = sectionId;

        // Update active states
        elements.sections.forEach(s => s.classList.remove('active'));
        elements.dirButtons.forEach(b => b.classList.remove('active'));

        const section = document.getElementById(sectionId);
        const button = document.querySelector(`[data-section="${sectionId}"]`);
        
        section?.classList.add('active');
        button?.classList.add('active');

        // Update path display
        const path = sectionId === 'home' ? '~/' : `~/${sectionId.charAt(0).toUpperCase() + sectionId.slice(1)}`;
        this.updatePath(path);
    },

    updatePath(path) {
        state.currentPath = path;
        elements.promptPwd.textContent = path;
        elements.terminalTitle.textContent = `Terminal - ${path}`;
        
        // Update all prompt pwds in sections
        document.querySelectorAll('.terminal-section .pwd').forEach(pwd => {
            pwd.textContent = path;
        });
    },
};

// =============================================================================
// AUTHENTICATION (for private assets)
// =============================================================================

const auth = {
    async login(password) {
        try {
            // TODO: Replace with actual API call
            // const response = await fetch(`${CONFIG.API_BASE}/auth`, {
            //     method: 'POST',
            //     headers: { 'Content-Type': 'application/json' },
            //     body: JSON.stringify({ password }),
            // });
            // const data = await response.json();
            
            // Mock authentication for now
            terminal.appendOutput('Authenticating...', 'info-output');
            
            // Simulated delay
            setTimeout(() => {
                state.isAuthenticated = true;
                terminal.appendOutput('Authentication successful. Private assets are now visible.', 'success-output');
                assets.loadPrivate();
            }, 500);

        } catch (error) {
            terminal.showError('Authentication failed. Please try again.');
        }
    },

    logout() {
        state.isAuthenticated = false;
        terminal.appendOutput('Logged out. Private assets hidden.', 'info-output');
        assets.hidePrivate();
    },
};

// =============================================================================
// ASSETS MANAGEMENT
// =============================================================================

const assets = {
    async loadPublic() {
        try {
            // TODO: Replace with actual API call
            // const response = await fetch(`${CONFIG.API_BASE}/assets/public`);
            // const data = await response.json();
            // this.renderAssets(data, 'public');
            
            // Mock data for now
            const mockAssets = [
                { name: 'resume.pdf', size: '245K', date: '2025-12-01' },
                { name: 'portfolio.png', size: '1.2M', date: '2025-11-15' },
            ];
            this.renderAssets(mockAssets, 'public');
        } catch (error) {
            console.error('Failed to load public assets:', error);
        }
    },

    async loadPrivate() {
        try {
            // TODO: Replace with actual API call
            // const response = await fetch(`${CONFIG.API_BASE}/assets/private`, {
            //     headers: { 'Authorization': `Bearer ${token}` },
            // });
            // const data = await response.json();
            // this.renderAssets(data, 'private');
            
            // Mock data for now
            const mockAssets = [
                { name: 'certificate_aws.pdf', size: '512K', date: '2025-10-20' },
                { name: 'certificate_java.pdf', size: '480K', date: '2025-09-15' },
            ];
            this.renderAssets(mockAssets, 'private');
            
            document.querySelector('.private-assets')?.classList.add('visible');
        } catch (error) {
            console.error('Failed to load private assets:', error);
        }
    },

    hidePrivate() {
        document.querySelector('.private-assets')?.classList.remove('visible');
    },

    renderAssets(assetList, type) {
        const container = document.querySelector(`.${type}-assets .asset-list`);
        if (!container) return;

        container.innerHTML = assetList.map(asset => `
            <div class="ls-row">
                <span class="ls-permissions">-rw-r--r--</span>
                <span class="ls-links">1</span>
                <span class="ls-owner">user</span>
                <span class="ls-group">user</span>
                <span class="ls-size">${asset.size}</span>
                <span class="ls-date">${asset.date}</span>
                <span class="ls-name file">${asset.name}</span>
            </div>
        `).join('');
    },
};

// =============================================================================
// COMMENTS SYSTEM
// =============================================================================

const comments = {
    name: '',
    
    showForm() {
        const form = document.querySelector('.leave-message-form');
        form?.classList.add('visible');
        terminal.appendOutput('Enter your name and message. Type "cancel" to go back.', 'info-output');
    },

    hideForm() {
        const form = document.querySelector('.leave-message-form');
        form?.classList.remove('visible');
    },

    handleInput(input) {
        if (input.toLowerCase() === 'cancel') {
            state.isInLeaveMessage = false;
            this.hideForm();
            terminal.navigateTo('contact');
            return;
        }
        // Form handles its own submission
    },

    async submit(name, comment) {
        if (!name.trim() || !comment.trim()) {
            terminal.showError('Name and comment are required.');
            return false;
        }

        if (comment.length > CONFIG.MAX_COMMENT_LENGTH) {
            terminal.showError(`Comment must be ${CONFIG.MAX_COMMENT_LENGTH} characters or less.`);
            return false;
        }

        try {
            // TODO: Replace with actual API call
            // const response = await fetch(`${CONFIG.API_BASE}/comments`, {
            //     method: 'POST',
            //     headers: { 'Content-Type': 'application/json' },
            //     body: JSON.stringify({ name, comment }),
            // });
            
            terminal.appendOutput('Message sent successfully! Thank you.', 'success-output');
            this.hideForm();
            state.isInLeaveMessage = false;
            terminal.navigateTo('contact');
            return true;
        } catch (error) {
            terminal.showError('Failed to send message. Please try again.');
            return false;
        }
    },
};

// =============================================================================
// FORM HANDLING
// =============================================================================

const forms = {
    init() {
        const commentForm = document.getElementById('comment-form');
        const commentText = document.getElementById('comment-text');
        const charCurrent = document.getElementById('char-current');

        // Character counter
        commentText?.addEventListener('input', () => {
            charCurrent.textContent = commentText.value.length;
        });

        commentForm?.addEventListener('submit', async (e) => {
            e.preventDefault();
            const name = document.getElementById('comment-name').value;
            const comment = document.getElementById('comment-text').value;
            
            const success = await comments.submit(name, comment);
            if (success) {
                commentForm.reset();
                charCurrent.textContent = '0';
            }
        });
    },
};

// =============================================================================
// INITIALIZATION
// =============================================================================

document.addEventListener('DOMContentLoaded', () => {
    terminal.init();
    forms.init();
    assets.loadPublic();
});