function diceToUnicode(value) {
    const map = ["", "⚀", "⚁", "⚂", "⚃", "⚄", "⚅"];
    return map[value] || "-";
}

let socket = null;
let socketReady = false;
let sessionId = "";
let hasValidName = false;
let gameStarted = false;
let myPlayerName = "";
let myPlayerBoxId = "";
let isMyTurn = false;
let diceWereRolled = false;

function updateStatus(message) {
    document.getElementById("statusBar").innerText = message;
}

function setNameError(message) {
    document.getElementById("nameError").innerText = message;
}

function setStartButtonEnabled(enabled) {
    document.getElementById("startGameButton").disabled = !enabled;
}

function setRollEnabled(enabled) {
    document.getElementById("rollButton").disabled = !enabled;
}

function getDieElement(index) {
    return document.getElementById("dice" + index);
}

function getDieFaceElement(index) {
    const die = getDieElement(index);
    return die ? die.querySelector(".dice-face") : null;
}

function getDieLabelElement(index) {
    const die = getDieElement(index);
    return die ? die.querySelector(".dice-label") : null;
}

function setDieState(index, state) {
    const die = getDieElement(index);
    if (!die) return;

    die.dataset.state = state;
    die.classList.remove("dice-state-available", "dice-state-played", "dice-state-locked", "dice-state-shared");
    die.classList.add("dice-state-" + state);

    const label = getDieLabelElement(index);
    if (!label) return;

    if (state === "played") {
        label.innerText = "PLAYED";
    } else if (state === "locked") {
        label.innerText = "LOCKED";
    } else if (state === "shared") {
        label.innerText = "OTHERS";
    } else {
        label.innerText = "";
    }
}

function setDieColor(index, color) {
    const die = getDieElement(index);
    if (!die) return;

    die.classList.remove("white", "blue", "yellow", "green", "orange", "purple");
    if (color) {
        die.classList.add(color);
    }
}

function normalizeDieState(die) {
    if (!die) return "locked";

    if (die.state) {
        return String(die.state).toLowerCase();
    }

    if (String(die.isPlayed) === "1" || die.isPlayed === true) {
        return "played";
    }

    if (String(die.isShared) === "1" || die.isShared === true) {
        return "shared";
    }

    if (String(die.isLocked) === "1" || die.isLocked === true) {
        return "locked";
    }

    return "available";
}

function renderDice(data) {
    for (let i = 0; i < 6; i++) {
        const el = getDieElement(i + 1);
        const faceEl = getDieFaceElement(i + 1);
        const die = data ? data[i] : null;

        if (!el || !faceEl) continue;

        if (!die) {
            faceEl.innerText = "-";
            setDieColor(i + 1, "");
            setDieState(i + 1, "locked");
            continue;
        }

        faceEl.innerText = diceToUnicode(die.value);
        setDieColor(i + 1, die.color);
        setDieState(i + 1, normalizeDieState(die));
    }

    updateTurnBasedControls();
}

function disableAllBoardSquares() {
    document.querySelectorAll(".white-square").forEach(square => {
        square.disabled = true;
        square.style.pointerEvents = "none";
        square.classList.remove("playable-square");
    });
}

function disableAllDice() {
    for (let i = 1; i <= 6; i++) {
        const die = getDieElement(i);
        if (!die) continue;
        die.classList.add("disabled-by-turn");
        die.style.pointerEvents = "none";
    }
}

function enableDiceByCurrentState() {
    if (!hasValidName || !gameStarted || !isMyTurn || !diceWereRolled) {
        disableAllDice();
        return;
    }

    for (let i = 1; i <= 6; i++) {
        const die = getDieElement(i);
        if (!die) continue;

        const state = die.dataset.state || "locked";
        if (state === "available") {
            die.classList.remove("disabled-by-turn");
            die.style.pointerEvents = "auto";
        } else {
            die.classList.add("disabled-by-turn");
            die.style.pointerEvents = "none";
        }
    }
}

function applyInteractionState() {
    if (!hasValidName || !gameStarted) {
        setRollEnabled(false);
        disableAllDice();
        disableAllBoardSquares();
        return;
    }

    updateTurnBasedControls();
}

function unlockAfterValidName() {
    hasValidName = true;
    document.getElementById("nameModal").classList.add("hidden");
    applyInteractionState();

    if (!gameStarted) {
        updateStatus("Joined successfully. Waiting for enough players to start...");
    }
}

function resetPlayerLabels() {
    document.getElementById("player1").innerText = "";
    document.getElementById("player2").innerText = "";
    document.getElementById("player3").innerText = "";
    document.getElementById("player4").innerText = "";
}

function updatePlayers(data) {
    resetPlayerLabels();

    const numberOfPlayers = Number(data.numberOfPlayers || 0);
    const players = data.players || [];

    myPlayerBoxId = "";

    for (let i = 0; i < numberOfPlayers && i < 4; i++) {
        const el = document.getElementById("player" + (i + 1));
        if (el && players[i] && players[i].name && players[i].name.trim() !== "") {
            el.innerText = players[i].name;

            if (myPlayerName && players[i].name === myPlayerName) {
                myPlayerBoxId = "player" + (i + 1);
            }
        }
    }

    if (String(data.startGame) === "true") {
        gameStarted = true;
        updateStatus("Game started.");
        applyInteractionState();
        startGame();
    } else {
        gameStarted = false;
        applyInteractionState();

        if (numberOfPlayers < 4) {
            updateStatus("Waiting for " + (4 - numberOfPlayers) + " more players...");
        } else {
            updateStatus("All players joined. Preparing game...");
        }
    }
}

function updatePlayableSquares(playable) {
    disableAllBoardSquares();

    if (!playable || !hasValidName || !gameStarted || !isMyTurn) {
        return;
    }

    Object.entries(playable).forEach(([board, indexes]) => {
        if (!Array.isArray(indexes)) return;

        indexes.forEach(index => {
            const selector = `.white-square[data-board="${board}"][data-index="${index}"]`;
            const el = document.querySelector(selector);

            if (el && !el.classList.contains("played-square")) {
                el.disabled = false;
                el.style.pointerEvents = "auto";
            }
        });
    });
}

function updateBoard(state) {
    if (!state) return;

    const yellowElements = document.querySelectorAll('.white-square[data-board="yellow"]');
    yellowElements.forEach(element => {
        const index = parseInt(element.dataset.index);
        if (state.yellowBoard && state.yellowBoard[index] === "T") {
            element.classList.add("played-square");
            element.disabled = true;
            element.style.pointerEvents = "none";
        } else {
            element.classList.remove("played-square");
        }
    });

    const blueElements = document.querySelectorAll('.white-square[data-board="blue"]');
    blueElements.forEach(element => {
        const index = parseInt(element.dataset.index);
        if (state.blueBoard && state.blueBoard[index] === "T") {
            element.classList.add("played-square");
            element.disabled = true;
            element.style.pointerEvents = "none";
        } else {
            element.classList.remove("played-square");
        }
    });

    const greenElements = document.querySelectorAll('.white-square[data-board="green"]');
    greenElements.forEach(element => {
        const index = parseInt(element.dataset.index);
        if (state.greenBoard && state.greenBoard[index] === "T") {
            element.classList.add("played-square");
            element.disabled = true;
            element.style.pointerEvents = "none";
        } else {
            element.classList.remove("played-square");
        }
    });

    const orangeElements = document.querySelectorAll('.white-square[data-board="orange"]');
    orangeElements.forEach(element => {
        const index = parseInt(element.dataset.index);
        if (state.orangeBoard && state.orangeBoard[index] === "F") {
            element.classList.remove("played-square");
            element.innerText = "";
        } else if (state.orangeBoard) {
            element.classList.add("played-square");
            element.style.pointerEvents = "none";
            element.disabled = true;
            element.innerText = state.orangeBoard[index];
        }
    });

    const purpleElements = document.querySelectorAll('.white-square[data-board="purple"]');
    purpleElements.forEach(element => {
        const index = parseInt(element.dataset.index);
        if (state.purpleBoard && state.purpleBoard[index] === "F") {
            element.classList.remove("played-square");
            element.innerText = "";
        } else if (state.purpleBoard) {
            element.classList.add("played-square");
            element.style.pointerEvents = "none";
            element.disabled = true;
            element.innerText = state.purpleBoard[index];
        }
    });
}

function updateGameInfo(gameState, playerState, dice) {
    const previousIsMyTurn = isMyTurn;
    isMyTurn = false;

    if (gameState) {
        for (let i = 1; i <= 4; i++) {
            const el = document.getElementById(`round${i}`);
            if (el) {
                if (gameState.round === i) {
                    el.classList.add("active-round");
                } else {
                    el.classList.remove("active-round");
                }
            }
        }
    }

    if (playerState) {
        Object.entries(playerState).forEach(([player, turnState]) => {
            const el = document.getElementById(player);

            el.classList.remove("active-player", "current-turn");

            if (turnState === "1") {
                el.classList.add("active-player");
            }

            if (myPlayerBoxId && player === myPlayerBoxId && turnState === "1") {
                el.classList.add("current-turn");
            }

            if (myPlayerBoxId && player === myPlayerBoxId && turnState === "1") {
                isMyTurn = true;
            }
        });
    }

    if (previousIsMyTurn !== isMyTurn) {
        diceWereRolled = false;
    }

    if (dice && Array.isArray(dice)) {
        renderDice(dice);
    }

    updateTurnBasedControls();
}

function dicePlayed(playable) {
    document.querySelectorAll(".white-square").forEach(square => {
        square.classList.remove("playable-square");
    });

    if (!playable || !hasValidName || !gameStarted) {
        return;
    }

    Object.entries(playable).forEach(([board, indexes]) => {
        if (!Array.isArray(indexes)) return;

        indexes.forEach(index => {
            const selector = `.white-square[data-board="${board}"][data-index="${index}"]`;
            const el = document.querySelector(selector);

            if (el && !el.disabled) {
                el.classList.add("playable-square");
                el.style.pointerEvents = "auto";
            }
        });
    });
}

function updateTurnBasedControls() {
    const canRoll = hasValidName && gameStarted && isMyTurn;
    setRollEnabled(canRoll);

    if (!hasValidName || !gameStarted || !isMyTurn || !diceWereRolled) {
        disableAllDice();
        return;
    }

    enableDiceByCurrentState();
}

function connectWebSocket() {
    const protocol = window.location.protocol === "https:" ? "wss://" : "ws://";
    socket = new WebSocket(protocol + window.location.host + "/ws");

    socket.onopen = () => {
        updateStatus("WebSocket connected. Waiting for session...");
    };

    socket.onmessage = (event) => {
        const msg = JSON.parse(event.data);

        if (msg.type === "sessionCreated") {
            socketReady = true;
            sessionId = msg.sessionId || "";
            setStartButtonEnabled(true);
            updateStatus("Session created. Enter your name.");
            return;
        }

        if (msg.type === "playerJoined") {
            updatePlayers(msg);
            return;
        }

        if (msg.type === "diceRolled") {
            diceWereRolled = true;
            renderDice(msg.dice);
            updatePlayableSquares(msg.playebleFields);
            return;
        }

        if (msg.type === "updateBoard") {
            updateBoard(msg.state);
            return;
        }

        if (msg.type === "updateGameInfo") {
            updateGameInfo(msg.gameState, msg.playerState, msg.dice || (msg.state ? msg.state.dice : null));
            return;
        }

        if (msg.type === "dicePlayed") {
            dicePlayed(msg.state);
            updateTurnBasedControls();
            return;
        }
    };

    socket.onclose = () => {
        socketReady = false;
        setStartButtonEnabled(false);
        setRollEnabled(false);
        disableAllDice();
        disableAllBoardSquares();
        updateStatus("WebSocket disconnected.");
    };

    socket.onerror = () => {
        updateStatus("WebSocket error.");
    };
}

async function submitName() {
    const nameInput = document.getElementById("playerName");
    const name = nameInput.value.trim();

    if (!socketReady) {
        setNameError("Session is not ready yet.");
        return;
    }

    if (!name) {
        setNameError("Please enter a valid name.");
        return;
    }

    setNameError("");
    setStartButtonEnabled(false);

    try {
        const response = await fetch("/setName", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                name: name,
                sessionId: sessionId
            })
        });

        if (!response.ok) {
            setNameError("Failed to set name on server.");
            setStartButtonEnabled(true);
            return;
        }

        unlockAfterValidName();
        myPlayerName = name;
    } catch (err) {
        console.error(err);
        setNameError("Error contacting server.");
        setStartButtonEnabled(true);
    }
}

async function startGame() {
    if (!sessionId) return;

    const response = await fetch("/updateBoard", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            sessionId: sessionId
        })
    });

    if (!response.ok) {
        console.error("Failed to initialize board state.");
    }
}

async function playMove(board, index) {
    const response = await fetch("/playMove", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            board: board,
            index: index,
            sessionId: sessionId
        })
    });

    if (!response.ok) {
        alert("Failed to play move.");
    }

    disableAllBoardSquares();
}

async function sendDicePlayed(diceNumber) {
    if (!hasValidName || !gameStarted || !isMyTurn || !diceWereRolled) return;

    const die = getDieElement(diceNumber);
    if (!die || (die.dataset.state || "locked") !== "available") return;

    const response = await fetch("/dicePlayed", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            dice: String(diceNumber),
            sessionId: sessionId
        })
    });

    if (!response.ok) {
        alert("Failed to play dice.");
    }
}

document.getElementById("playerName").addEventListener("keydown", (event) => {
    if (event.key === "Enter") {
        submitName();
    }
});

document.getElementById("rollButton").onclick = async () => {
    if (!hasValidName || !gameStarted || !isMyTurn) return;

    const response = await fetch("/rollDice", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            sessionId: sessionId
        })
    });

    if (!response.ok) {
        alert("Failed to roll dice.");
    }
};

document.getElementById("dice1").onclick = () => sendDicePlayed(1);
document.getElementById("dice2").onclick = () => sendDicePlayed(2);
document.getElementById("dice3").onclick = () => sendDicePlayed(3);
document.getElementById("dice4").onclick = () => sendDicePlayed(4);
document.getElementById("dice5").onclick = () => sendDicePlayed(5);
document.getElementById("dice6").onclick = () => sendDicePlayed(6);

document.querySelectorAll(".white-square").forEach(square => {
    square.addEventListener("click", () => {
        if (!hasValidName || !gameStarted || square.disabled) return;

        const board = square.dataset.board;
        const index = square.dataset.index;

        if (board && index !== undefined) {
            playMove(board, index);
        }
    });
});

resetPlayerLabels();
disableAllBoardSquares();
disableAllDice();
setRollEnabled(false);
connectWebSocket();