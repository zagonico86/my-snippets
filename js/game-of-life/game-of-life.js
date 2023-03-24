class GameOfLife {
	constructor(canvasSelector, width = 100, height = 100, cellSize = 5, speed = 100) {
		this.canvas = document.querySelector(canvasSelector);
		this.canvas.width = width * (cellSize+1);
		this.canvas.height = height * (cellSize+1);
		this.ctx = this.canvas.getContext("2d");
		this.width = width;
		this.height = height;
		this.cellSize = cellSize;
		this.grid = this.createGrid();
		this.intervalId = null;
		this.speed = speed;
		this.addClickListener();
		
		
		this.CHAR_TO_INDEX = {
			"0": 0,
			"4": 1,
			" ": 2 // Undefined character
		};
		this.CHAR_WIDTH = {
			"0": 6,
			"4": 6,
			" ": 6 // Undefined character
		};

		this.CHAR_GRID = [
			// 0
			[
				[0, 0, 1, 1, 0, 0],
				[0, 1, 0, 0, 1, 0],
				[1, 0, 0, 0, 0, 1],
				[1, 0, 0, 0, 0, 1],
				[1, 0, 0, 0, 0, 1],
				[1, 0, 0, 0, 0, 1],
				[1, 0, 0, 0, 0, 1],
				[0, 1, 0, 0, 1, 0],
				[0, 0, 1, 1, 0, 0]
			],
			// 4
			[
				[1, 0, 0, 0, 0, 1],
				[1, 0, 0, 0, 0, 1],
				[1, 0, 0, 0, 0, 1],
				[1, 0, 0, 0, 0, 1],
				[1, 1, 1, 1, 1, 1],
				[0, 0, 0, 0, 0, 1],
				[0, 0, 0, 0, 0, 1],
				[0, 0, 0, 0, 0, 1],
				[0, 0, 0, 0, 0, 1]
			],
			// space
			[
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0],
				[0, 0, 0, 0, 0, 0]
			]
		];
	}

	createGrid() {
		return Array.from({ length: this.width }, () =>
			Array.from({ length: this.height }, () => false)
		);
	}

	clearCanvas() {
		this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
	}


	drawGrid() {
		this.ctx.strokeStyle = '#aaa'; // Grid line color
		this.ctx.lineWidth = 0.5;

		for (let x = 0; x <= this.canvas.width; x += this.cellSize) {
			this.ctx.beginPath();
			this.ctx.moveTo(x, 0);
			this.ctx.lineTo(x, this.canvas.height);
			this.ctx.stroke();
		}

		for (let y = 0; y <= this.canvas.height; y += this.cellSize) {
			this.ctx.beginPath();
			this.ctx.moveTo(0, y);
			this.ctx.lineTo(this.canvas.width, y);
			this.ctx.stroke();
		}
	}

	updateGrid() {
		// Create a copy of the current grid to store the new state
		let newGrid = new Array(this.width).fill(null).map(() => new Array(this.height).fill(false));

		// Apply the Game of Life rules to each cell in the grid
		for (let x = 0; x < this.width; x++) {
			for (let y = 0; y < this.height; y++) {
				let neighbors = this.countNeighbors(x, y);

				if (this.grid[x][y]) { // Cell is alive
					if (neighbors < 2 || neighbors > 3) {
						newGrid[x][y] = false; // Cell dies from underpopulation or overpopulation
					} else {
						newGrid[x][y] = true; // Cell survives
					}
				} else { // Cell is dead
					if (neighbors === 3) {
						newGrid[x][y] = true; // Cell is born from reproduction
					}
				}
			}
		}

		// Replace the old grid with the new one
		this.grid = newGrid;

		// Draw the updated grid on the this.canvas
		this.clearCanvas();
		this.drawGrid();
		this.drawCells();
	}

	countNeighbors(x, y) {
		let count = 0;

		for (let i = -1; i <= 1; i++) {
			for (let j = -1; j <= 1; j++) {
				if (i === 0 && j === 0) continue; // Skip the current cell
				let neighborX = x + i;
				let neighborY = y + j;

				// Check if the neighbor is within the grid bounds
				if (neighborX >= 0 && neighborX < this.width && neighborY >= 0 && neighborY < this.height) {
					if (this.grid[neighborX][neighborY]) count++;
				}
			}
		}

		return count;
	}

	drawCells() {
		this.ctx.fillStyle = '#333'; // Cell color

		for (let x = 0; x < this.width; x++) {
			for (let y = 0; y < this.height; y++) {
				if (this.grid[x][y]) {
					let cellX = x * this.cellSize;
					let cellY = y * this.cellSize;
					this.ctx.fillRect(cellX, cellY, this.cellSize, this.cellSize);
				}
			}
		}
	}
	
	startGame() {
		if (this.intervalId) return; // Game is already running
		this.intervalId = setInterval(() => { this.updateGrid(); }, this.speed);
	}

	stopGame() {
		clearInterval(this.intervalId);
		this.intervalId = null;
	}

	resetGrid() {
		for (let x = 0; x < this.width; x++) {
			for (let y = 0; y < this.height; y++) {
				this.grid[x][y] = false;
			}
		}
		this.clearCanvas();
		this.drawGrid();
	}

	setSpeed(speed) {
		this.speed = parseInt(speed) || 100; // Use the value in the textbox or the default value if the value is not valid
		if (this.intervalId) {
			clearInterval(this.intervalId);
			this.intervalId = setInterval(() => { this.updateGrid(); }, this.speed);
		}
	}

	initGridCustom(newGrid) {
		this.grid = newGrid;
	}

	initGrid404() {
		let str = "404";
		let x = Math.floor((this.width - str.length * 6) / 2);
		let y = Math.floor((this.height - 10) / 2);
		for (let i = 0; i < str.length; i++) {
			let char = str.charAt(i);
			let index = this.CHAR_TO_INDEX[char];
			if (index !== undefined) {
				for (let j = 0; j < this.CHAR_GRID[index].length; j++) {
					let row = this.CHAR_GRID[index][j];
					for (let k = 0; k < row.length; k++) {
						let cell = row[k];
						this.grid[x + k][y + j] = cell;
					}
				}
				x += 7;
			}
		}
		this.clearCanvas();
		this.drawGrid();
		this.drawCells();
	}
	
	setCellState = function(x, y, state) {
		// Compute the row and column of the cell
		let row = Math.floor(y / this.cellSize);
		let col = Math.floor(x / this.cellSize);

		// Set the state of the cell
		this.grid[row][col] = state;

		// Redraw the grid cell
		this.renderCell(row, col);
	};

	toggleCell(x, y) {
		this.grid[x][y] = !this.grid[x][y];
	}
	
	renderCell(row, col) {
		let state = this.grid[row][col];
		let x = col * this.cellSize;
		let y = row * this.cellSize;

		this.ctx.fillStyle = state ? this.liveColor : this.deadColor;
		this.ctx.fillRect(x, y, this.cellSize, this.cellSize);
	}
	
	addClickListener() {
		let painting = false; // Flag to indicate whether the user is painting cells
		let self = this;

		this.canvas.addEventListener('mousedown', function(event) {
			painting = true;
			self.setCellState(event.offsetX, event.offsetY, true);
		});

		this.canvas.addEventListener('mousemove', function(event) {
			if (painting) {
				self.setCellState(event.offsetX, event.offsetY, true);
			}
		});

		this.canvas.addEventListener('mouseup', function(event) {
			painting = false;
		});
	}
}
