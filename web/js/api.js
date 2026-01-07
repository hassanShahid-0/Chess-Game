// ============================================================================
// API MODULE - Handles all communication with the backend
// ============================================================================

const API_URL = 'http://localhost:3001/api';

const ChessAPI = {
    // Get current board state
    async getBoard() {
        const response = await fetch(`${API_URL}/board`);
        return await response.json();
    },

    // Get game status
    async getStatus() {
        const response = await fetch(`${API_URL}/status`);
        return await response.json();
    },

    // Start new game
    async newGame() {
        const response = await fetch(`${API_URL}/newgame`, {
            method: 'POST'
        });
        return await response.json();
    },

    // Set AI difficulty
    async setAI(difficulty) {
        const response = await fetch(`${API_URL}/setai`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ difficulty })
        });
        return await response.json();
    },

    // Make a move
    async makeMove(from, to) {
        const response = await fetch(`${API_URL}/move`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ from, to })
        });
        return await response.json();
    },

    // Get AI move
    async getAIMove() {
        const response = await fetch(`${API_URL}/aimove`);
        return await response.json();
    },

    // Undo last move
    async undoMove() {
        const response = await fetch(`${API_URL}/undo`, {
            method: 'POST'
        });
        return await response.json();
    },

    // Get valid moves for a position
    async getValidMoves(position) {
        const response = await fetch(`${API_URL}/validmoves/${position}`);
        return await response.json();
    }
};