const container = document.getElementById('input-container');
const ghostInput = document.getElementById('ghost-input');
const typedTextSpan = document.getElementById('typed-text');
const cursor = document.querySelector('.cursor');

// 1. Focus the hidden input when clicking anywhere in the terminal area
container.addEventListener('click', () => {
    ghostInput.focus();
});

// 2. Sync hidden input value to visible span
ghostInput.addEventListener('input', () => {
    typedTextSpan.textContent = ghostInput.value;
});

// Optional: Stop blinking while typing for realism
ghostInput.addEventListener('keydown', () => {
    cursor.style.animation = 'none';
    setTimeout(() => cursor.style.animation = 'blink 1s step-end infinite', 100);
});
