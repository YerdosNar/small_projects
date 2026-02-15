const pc = new RTCPeerConnection({
    iceServers: [{ urls: 'stun:stun.l.google.com:19302' }] // STUN server for NAT punching
});

let dataChannel;

// UI Elements
const receiveBtn = document.getElementById('receiveBtn');
const sendBtn = document.getElementById('sendBtn');
const receiveSection = document.getElementById('receiveSection');
const sendSection = document.getElementById('sendSection');

receiveBtn.onclick = () => { receiveSection.classList.remove('hidden'); sendSection.classList.add('hidden'); };
sendBtn.onclick = () => { sendSection.classList.remove('hidden'); receiveSection.classList.add('hidden'); };

// 1. Receiver Logic
document.getElementById('generateIdBtn').onclick = async () => {
    const password = document.getElementById('setPass').value;
    const randomId = Math.random().toString(36).substr(2, 6).toUpperCase();

    document.getElementById('displayId').innerText = randomId;
    document.getElementById('displayPass').innerText = password;
    document.getElementById('receiverInfo').classList.remove('hidden');

    // Here you would send (randomId, password) to your signaling server via WebSocket
    // server.emit('register-receiver', { randomId, password });
};

// 2. Sender Logic (Drag and Drop)
const dropZone = document.getElementById('dropZone');
dropZone.ondragover = (e) => { e.preventDefault(); dropZone.style.background = '#f0f0f0'; };
dropZone.ondrop = (e) => {
    e.preventDefault();
    const file = e.dataTransfer.files[0];
    handleFile(file);
};

function handleFile(file) {
    console.log("File selected:", file.name);
    // Logic to read file as ArrayBuffer and send via dataChannel.send()
}

// 3. WebRTC Core Logic
pc.onicecandidate = (event) => {
    if (event.candidate) {
        // Send this candidate to the other peer via your signaling server
    }
};

// Receiver side listens for data channel
pc.ondatachannel = (event) => {
    const receiveChannel = event.channel;
    receiveChannel.onmessage = (e) => {
        // Logic to reconstruct the file from chunks
        console.log("Received chunk");
    };
};

// Sender creates data channel
function initiateTransfer() {
    dataChannel = pc.createDataChannel("fileTransfer");
    pc.createOffer().then(offer => pc.setLocalDescription(offer));
    // Send 'offer' to receiver via signaling server
}
