let ws;
let isConnected = false;

function initWebSocket() {
    // Determine WS URL based on current host, fallback to local IP for testing
    const wsUrl = `ws://${window.location.hostname}:81/`;
    ws = new WebSocket(wsUrl);

    ws.onopen = () => {
        isConnected = true;
        document.getElementById('ws-status').classList.add('connected');
        console.log('WebSocket Connected');
    };

    ws.onclose = () => {
        isConnected = false;
        document.getElementById('ws-status').classList.remove('connected');
        console.log('WebSocket Disconnected. Reconnecting...');
        setTimeout(initWebSocket, 2000);
    };

    ws.onerror = (error) => {
        console.error('WebSocket Error:', error);
    };
}

function sendMsg(obj) {
    if (isConnected) {
        ws.send(JSON.stringify(obj));
    }
}

function setMode(mode) {
    document.getElementById('btn-dashboard').classList.toggle('active', mode === 0);
    document.getElementById('btn-game').classList.toggle('active', mode === 1);
    
    // Dim controller if not in game mode
    document.getElementById('game-controller').style.opacity = mode === 1 ? '1' : '0.4';
    document.getElementById('game-controller').style.pointerEvents = mode === 1 ? 'auto' : 'none';
    
    sendMsg({ action: "mode", mode: mode });
}

// Controller Logic
const btnLeft = document.getElementById('btn-left');
const btnRight = document.getElementById('btn-right');

function attachEvents(element, dir) {
    const handleDown = (e) => {
        e.preventDefault();
        element.classList.add('pressed');
        sendMsg({ action: "move", dir: dir, state: "down" });
    };
    const handleUp = (e) => {
        e.preventDefault();
        element.classList.remove('pressed');
        sendMsg({ action: "move", dir: dir, state: "up" });
    };

    element.addEventListener('mousedown', handleDown);
    element.addEventListener('touchstart', handleDown);
    
    element.addEventListener('mouseup', handleUp);
    element.addEventListener('mouseleave', handleUp);
    element.addEventListener('touchend', handleUp);
    element.addEventListener('touchcancel', handleUp);
}

attachEvents(btnLeft, "left");
attachEvents(btnRight, "right");

// Init
window.onload = () => {
    initWebSocket();
    setMode(0); // Default to dashboard
};
