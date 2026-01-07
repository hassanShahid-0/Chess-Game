// ============================================================================
// GAME MODULE - Main game logic and controller
// ============================================================================

const ChessGame = {
    selectedSquare: null,
    validMoves: [],
    moveHistory: [],
    lastMove: null,
    gameMode: null,
    player1Name: 'Player 1',
    player2Name: 'Player 2',
    isAITurnInProgress: false,
    // UI Functions
    startSetup(mode) {
        this.gameMode = mode;
        document.getElementById('mainMenu').style.display = 'none';
        document.getElementById('setupScreen').style.display = 'block';

        if (mode === 'pvc') {
            document.getElementById('setupTitle').textContent = '🤖 Player vs Computer';
            document.getElementById('player2Group').style.display = 'none';
            document.getElementById('difficultyGroup').style.display = 'block';
        } else {
            document.getElementById('setupTitle').textContent = '👥 Player vs Player';
            document.getElementById('player2Group').style.display = 'block';
            document.getElementById('difficultyGroup').style.display = 'none';
        }
    },

    backToMenu() {
        document.getElementById('setupScreen').style.display = 'none';
        document.getElementById('mainMenu').style.display = 'block';
    },

    async startGame() {
        this.player1Name = document.getElementById('player1Name').value.trim() || 'Player 1';

        document.getElementById('player1NameDisplay').textContent = this.player1Name;

        document.getElementById('setupScreen').style.display = 'none';
        document.getElementById('gameScreen').style.display = 'block';

        // FIRST: Start new game
        await ChessAPI.newGame();

        // THEN: Set AI mode and difficulty
        if (this.gameMode === 'pvc') {
            this.player2Name = '🤖 Computer';
            const difficulty = document.getElementById('aiDifficulty').value;
            console.log('Setting AI difficulty:', difficulty);
            await ChessAPI.setAI(difficulty);
        } else {
            this.player2Name = document.getElementById('player2Name').value.trim() || 'Player 2';
            await ChessAPI.setAI('none');
        }

        document.getElementById('player2NameDisplay').textContent = this.player2Name;

        ChessBoard.initialize();
        await this.loadGameState();
        this.showMessage('🎮 Game started! Good luck!');
    },

    quitToMenu() {
        if (confirm('Are you sure you want to quit? Your game will be lost.')) {
            document.getElementById('gameScreen').style.display = 'none';
            document.getElementById('mainMenu').style.display = 'block';
            this.moveHistory = [];
            this.selectedSquare = null;
            this.validMoves = [];
            this.lastMove = null;
        }
    },

    async newGame() {
        if (confirm('Start a new game? Current game will be lost.')) {
            await ChessAPI.newGame();
            this.moveHistory = [];
            this.lastMove = null;
            this.selectedSquare = null;
            this.validMoves = [];
            document.getElementById('moveList').innerHTML = '';
            await this.loadGameState();
            this.showMessage('🎮 New game started!');
        }
    },

    exitGame() {
        if (confirm('Are you sure you want to exit?')) {
            alert('Thank you for playing! 👋');
            window.close();
        }
    },

    // Load current game state
    async loadGameState() {
        try {
            const [boardData, statusData] = await Promise.all([
                ChessAPI.getBoard(),
                ChessAPI.getStatus()
            ]);

            ChessBoard.render(boardData);
            this.updateGameStatus(statusData);
        } catch (error) {
            console.error('Error loading game state:', error);
            this.showMessage('❌ Error connecting to server');
        }
    },

    // Handle square click
    async handleSquareClick(row, col) {
        if(this.isAITurnInProgress) return;
        const position = ChessBoard.positionToString(row, col);
        console.log('🖱️ Clicked square:', position, 'at row:', row, 'col:', col);

        // If a square is already selected
        if (this.selectedSquare) {
            console.log('📍 Already selected:', this.selectedSquare);
            console.log('✅ Valid moves:', this.validMoves);
            console.log('❓ Is valid move?', this.validMoves.includes(position));

            // Try to make a move
            if (this.validMoves.includes(position)) {
                console.log('🎯 Making move:', this.selectedSquare, '→', position);
                await this.makeMove(this.selectedSquare, position);
            } else {
                console.log('❌ Not a valid move');
            }

            // Deselect
            this.clearSelection();
        } else {
            console.log('🔍 Selecting square:', position);
            // Select this square
            await this.selectSquare(position);
        }
    },

    // Select a square and show valid moves
    async selectSquare(position) {
        try {
            const data = await ChessAPI.getValidMoves(position);

            console.log('Valid moves data:', data); // DEBUG

            if (data.moves && data.moves.length > 0) {
                this.selectedSquare = position;
                // Handle both formats: {to: "e3"} and {from: "e2", to: "e3"}
                this.validMoves = data.moves.map(m => m.to || m);

                console.log('Valid moves:', this.validMoves); // DEBUG

                ChessBoard.selectSquare(position);
                ChessBoard.highlightValidMoves(this.validMoves);
            } else {
                console.log('No valid moves for', position);
            }
        } catch (error) {
            console.error('Error getting valid moves:', error);
        }
    },

    // Clear selection
    clearSelection() {
        ChessBoard.clearHighlights();

        // Re-highlight last move if exists
        if (this.lastMove) {
            ChessBoard.highlightLastMove(this.lastMove.from, this.lastMove.to);
        }

        this.selectedSquare = null;
        this.validMoves = [];
    },

    // Make a move
    // Make a move
    async makeMove(from, to) {
        try {
            const data = await ChessAPI.makeMove(from, to);

            if (data.success) {
                this.lastMove = { from, to };
                this.moveHistory.push(`${from}-${to}`);
                this.updateMoveHistory();

                await this.loadGameState();
                ChessBoard.highlightLastMove(from, to);
                ChessBoard.animatePieceMove(to);

                // FIXED: Check if AI should move (check mode AND that it's AI's turn now)
                console.log('Game mode:', this.gameMode); // Debug log
                if (this.gameMode === 'pvc') {
                    console.log('AI should move now'); // Debug log
                    // Add delay so user can see their move
                    setTimeout(() => this.makeAIMove(), 1000);
                }
            } else {
                this.showMessage('❌ Invalid move!');
            }
        } catch (error) {
            console.error('Error making move:', error);
            this.showMessage('❌ Error making move');
        }
    },

    // Make AI move
    // Make AI move
    async makeAIMove() {
        // 🚫 AI must only move on its own turn
        if (this.currentTurn !== this.aiColor) {
            console.warn('AI move blocked: not AI turn');
            return;
        }
        this.isAITurnInProgress = true;
        try {
            console.log('makeAIMove called'); // Debug log
            this.showMessage('🤖 AI is thinking... <span class="spinner"></span>');

            const data = await ChessAPI.getAIMove();
            console.log('AI response:', data); // Debug log
            // ❌ AI returned no move (checkmate / stalemate)
            if (!data || !data.from || !data.to) {
                console.error('Invalid AI move received:', data);
                this.isAITurnInProgress = false;
                return;
            }
            if (data.error) {
                console.error('AI Error:', data.error); // Debug log
                this.showMessage(`❌ ${data.error}`);
            } else {
                this.lastMove = { from: data.from, to: data.to };
                this.moveHistory.push(`${data.from}-${data.to} (AI)`);
                this.updateMoveHistory();

                await this.loadGameState();
                ChessBoard.highlightLastMove(data.from, data.to);
                ChessBoard.animatePieceMove(data.to);

                // Clear the "AI thinking" message
                setTimeout(() => {
                    const msg = document.getElementById('gameMessage');
                    if (msg.innerHTML.includes('thinking')) {
                        msg.innerHTML = '';
                    }
                }, 500);
                this.isAITurnInProgress = false;
            }
        } catch (error) {
            console.error('Error getting AI move:', error);
            this.showMessage('❌ AI error: ' + error.message);
            this.isAITurnInProgress = false;
        }
    },

    // Undo move
    async undoMove() {
        try {
            const data = await ChessAPI.undoMove();

            if (data.success) {
                this.moveHistory.pop();
                this.lastMove = null;
                this.clearSelection();
                this.updateMoveHistory();
                await this.loadGameState();
                this.showMessage('↩️ Move undone');
            } else {
                this.showMessage('❌ Cannot undo');
            }
        } catch (error) {
            console.error('Error undoing move:', error);
        }
    },

    // Update game status display
    updateGameStatus(status) {
        const player1Info = document.getElementById('player1Info');
        const player2Info = document.getElementById('player2Info');
        const messageElement = document.getElementById('gameMessage');

        // Update active player
        if (status.currentPlayer === 'White') {
            player1Info.classList.add('active');
            player1Info.classList.remove('inactive');
            player2Info.classList.remove('active');
            player2Info.classList.add('inactive');
        } else {
            player2Info.classList.add('active');
            player2Info.classList.remove('inactive');
            player1Info.classList.remove('active');
            player1Info.classList.add('inactive');
        }

        // Update messages
        messageElement.className = 'game-message';

        if (status.isCheckmate) {
            const winner = status.currentPlayer === 'White' ? this.player2Name : this.player1Name;
            messageElement.textContent = `🏆 Checkmate! ${winner} wins!`;
            messageElement.classList.add('checkmate');
        } else if (status.isCheck) {
            messageElement.textContent = `⚠️ Check! ${status.currentPlayer} is in check!`;
            messageElement.classList.add('check');
        } else if (status.isGameOver) {
            messageElement.textContent = '🤝 Game Over - Draw';
        } else {
            messageElement.textContent = '';
        }
    },

    // Update move history display
    updateMoveHistory() {
        const moveList = document.getElementById('moveList');
        moveList.innerHTML = '';

        this.moveHistory.forEach((move, index) => {
            const moveItem = document.createElement('div');
            moveItem.className = 'move-item';
            moveItem.textContent = `${index + 1}. ${move}`;
            moveList.appendChild(moveItem);
        });

        // Scroll to bottom
        moveList.parentElement.scrollTop = moveList.parentElement.scrollHeight;
    },

    // Show temporary message
    showMessage(message) {
        const messageElement = document.getElementById('gameMessage');
        messageElement.className = 'game-message';
        messageElement.innerHTML = message;

        setTimeout(() => {
            if (messageElement.innerHTML === message) {
                messageElement.innerHTML = '';
            }
        }, 3000);
    }
};