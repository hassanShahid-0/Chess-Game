// ============================================================================
// BOARD MODULE - Handles board rendering and piece display
// ============================================================================

// Piece symbols mapping
const PIECE_SYMBOLS = {
    'K': '♔', 'Q': '♕', 'R': '♖', 'B': '♗', 'N': '♘', 'P': '♙',  // White
    'k': '♚', 'q': '♛', 'r': '♜', 'b': '♝', 'n': '♞', 'p': '♟'   // Black
};

const ChessBoard = {
    // Initialize the board HTML
    initialize() {
        //console.log('🎨 Initializing chess board...');
        const board = document.getElementById('chessBoard');

        if (!board) {
            console.error('❌ Chess board element not found!');
            return;
        }

        board.innerHTML = '';

        // Create all squares
        for (let row = 7; row >= 0; row--) {
            for (let col = 0; col < 8; col++) {
                const square = document.createElement('div');
                square.className = `square ${(row + col) % 2 === 0 ? 'light' : 'dark'}`;
                square.dataset.row = row;
                square.dataset.col = col;
                square.dataset.position = this.positionToString(row, col);

                board.appendChild(square);
            }
        }

        //console.log('✅ Created', board.children.length, 'squares');

        // Add event listener to board (using event delegation)
        board.onclick = (e) => {
            const square = e.target.closest('.square');
            if (square) {
                const row = parseInt(square.dataset.row);
                const col = parseInt(square.dataset.col);
                //console.log('🖱️ Board clicked - Square:', square.dataset.position);
                ChessGame.handleSquareClick(row, col);
            }
        };

        //console.log('✅ Click handler attached to board');
    },
    // FPS-friendly instant move using requestAnimationFrame
    movePiece(from, to) {
        requestAnimationFrame(() => {
            const fromSquare = document.querySelector(`[data-position="${from}"]`);
            const toSquare = document.querySelector(`[data-position="${to}"]`);

            if (!fromSquare || !toSquare) return;

            toSquare.textContent = fromSquare.textContent;
            fromSquare.textContent = '';
        });
    },
    // Render pieces on the board
    render(boardData) {
        //console.log('🎨 Rendering pieces...');

        // First, clear ALL squares
        document.querySelectorAll('.square').forEach(sq => {
            sq.textContent = '';
            sq.classList.remove('piece-move');
        });



        // Then render each piece
        let pieceCount = 0;
        boardData.squares.forEach(square => {
            const squareElement = document.querySelector(
                `[data-row="${square.row}"][data-col="${square.col}"]`
            );

            if (squareElement && square.piece) {
                squareElement.textContent = PIECE_SYMBOLS[square.piece.symbol] || square.piece.symbol;
                pieceCount++;
            }
        });

        //console.log('✅ Rendered', pieceCount, 'pieces');
    },

    // Highlight selected square
    selectSquare(position) {
        //console.log('🟢 Selecting square:', position);
        requestAnimationFrame(() => {
            this.clearHighlights();
            const square = document.querySelector(`[data-position="${position}"]`);
            if (square) {
                square.classList.add('selected');
            }
            else {
                console.error('❌ Square to select not found:', position);
            }
        });
    },

    // Highlight valid moves
    highlightValidMoves(moves) {
        //console.log('🟡 Highlighting valid moves:', moves);
        requestAnimationFrame(() => {
            moves.forEach(move => {
                const square = document.querySelector(`[data-position="${move}"]`);
                if (square) {
                    square.classList.add('valid-move');
                }
            });
        });
        //console.log('✅ Highlighted', highlighted, 'moves');
    },

    // Clear all highlights
    clearHighlights() {
        document.querySelectorAll('.square').forEach(sq => {
            sq.classList.remove('selected', 'valid-move', 'last-move');
        });
    },

    // Highlight last move
    highlightLastMove(from, to) {
        const fromSquare = document.querySelector(`[data-position="${from}"]`);
        const toSquare = document.querySelector(`[data-position="${to}"]`);

        if (fromSquare) fromSquare.classList.add('last-move');
        if (toSquare) toSquare.classList.add('last-move');
    },

    // Animate piece movement
    animatePieceMove(position) {
        const square = document.querySelector(`[data-position="${position}"]`);
        if (square) {
            square.classList.add('piece-move');
            setTimeout(() => {
                square.classList.remove('piece-move');
            }, 120);
        }
    },

    // Convert row/col to chess notation (e.g., e2)
    positionToString(row, col) {
        const files = 'abcdefgh';
        return `${files[col]}${row + 1}`;
    }
};

console.log('✅ ChessBoard module loaded');