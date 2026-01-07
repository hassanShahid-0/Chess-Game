const express = require('express');
const cors = require('cors');
const { exec } = require('child_process');
const path = require('path');

const app = express();
const PORT = 3001;

// Middleware
app.use(cors());
app.use(express.json());
// Serve static files from web directory
const webPath = path.resolve(__dirname, '..', 'web');
console.log(`📁 Web directory: ${webPath}`);
app.use(express.static(webPath));
// Path to chess CLI executable
const CLI_PATH = path.resolve(__dirname, '..', 'chess_cli.exe');
console.log(`🎮 Chess CLI: ${CLI_PATH}`);

// Helper function to execute chess commands
function executeChessCommand(command, args = []) {
    return new Promise((resolve, reject) => {
        const fullCommand = `"${CLI_PATH}" ${command} ${args.join(' ')}`;
        
        exec(fullCommand, (error, stdout, stderr) => {
            if (error) {
                reject({ error: stderr || error.message });
                return;
            }
            
            try {
                const result = JSON.parse(stdout);
                resolve(result);
            } catch (e) {
                reject({ error: 'Invalid JSON response' });
            }
        });
    });
}

// ============================================================================
// API ENDPOINTS
// ============================================================================

// Get current board state
app.get('/api/board', async (req, res) => {
    try {
        const result = await executeChessCommand('board');
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Get game status
app.get('/api/status', async (req, res) => {
    try {
        const result = await executeChessCommand('status');
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Start new game
app.post('/api/newgame', async (req, res) => {
    try {
        const result = await executeChessCommand('newgame');
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Set AI difficulty
app.post('/api/setai', async (req, res) => {
    try {
        const { difficulty } = req.body;
        const result = await executeChessCommand('setai', [difficulty]);
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Make a move
app.post('/api/move', async (req, res) => {
    try {
        const { from, to } = req.body;
        const result = await executeChessCommand('move', [from, to]);
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Get AI move
app.get('/api/aimove', async (req, res) => {
    try {
        const result = await executeChessCommand('aimove');
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Undo last move
app.post('/api/undo', async (req, res) => {
    try {
        const result = await executeChessCommand('undo');
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Get valid moves for a piece
app.get('/api/validmoves/:position', async (req, res) => {
    try {
        const { position } = req.params;
        const result = await executeChessCommand('validmoves', [position]);
        res.json(result);
    } catch (error) {
        res.status(500).json(error);
    }
});

// Serve the main page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '../web/index.html'));
});

// Start server
app.listen(PORT, () => {
    console.log(`♟️  Chess Server running at http://localhost:${PORT}`);
    console.log(`📁 Serving web files from: ${path.join(__dirname, '../web')}`);
    console.log(`🎮 Chess CLI: ${CLI_PATH}`);
});