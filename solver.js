const boardElement = document.getElementById("sudoku-board");
const solveBtn = document.getElementById("solve-btn");
const clearBtn = document.getElementById("clear-btn");
const exampleBtn = document.getElementById("example-btn");
const messageElement = document.getElementById("message");
const boardOutput = document.getElementById("board-output");

function createBoard() {
  boardElement.innerHTML = "";

  for (let row = 0; row < 9; row++) {
    for (let col = 0; col < 9; col++) {
      const input = document.createElement("input");
      input.type = "text";
      input.maxLength = 1;
      input.classList.add("cell");

      if (col === 2 || col === 5) input.classList.add("right-border");
      if (row === 2 || row === 5) input.classList.add("bottom-border");

      input.addEventListener("input", () => {
        input.value = input.value.replace(/[^1-9]/g, "");
      });

      boardElement.appendChild(input);
    }
  }
}

function getBoard() {
  const cells = document.querySelectorAll(".cell");
  const board = [];

  for (let row = 0; row < 9; row++) {
    const currentRow = [];
    for (let col = 0; col < 9; col++) {
      const value = cells[row * 9 + col].value;
      currentRow.push(value === "" ? 0 : Number(value));
    }
    board.push(currentRow);
  }

  return board;
}

function setBoard(board) {
  const cells = document.querySelectorAll(".cell");

  for (let row = 0; row < 9; row++) {
    for (let col = 0; col < 9; col++) {
      cells[row * 9 + col].value = board[row][col] === 0 ? "" : board[row][col];
    }
  }
}

function clearBoard() {
  document.querySelectorAll(".cell").forEach(cell => {
    cell.value = "";
  });
  messageElement.textContent = "";
  boardOutput.textContent = "";
}

function loadExample() {
  const example = [
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 3, 0, 8, 5],
    [0, 0, 1, 0, 2, 0, 0, 0, 0],

    [0, 0, 0, 5, 0, 7, 0, 0, 0],
    [0, 0, 4, 0, 0, 0, 1, 0, 0],
    [0, 9, 0, 0, 0, 0, 0, 0, 0],

    [5, 0, 0, 0, 0, 0, 0, 7, 3],
    [0, 0, 2, 0, 1, 0, 0, 0, 0],
    [0, 0, 0, 0, 4, 0, 0, 0, 9]
  ];

  setBoard(example);
  messageElement.textContent = "Example puzzle loaded.";
  boardOutput.textContent = "";
}

function boardToText(board) {
  return board.map(row => row.join("")).join("\n");
}

function textToBoard(text) {
  return text.trim().split("\n").map(line => line.trim().split("").map(Number));
}

async function solveSudoku() {
  const board = getBoard();
  const puzzleText = boardToText(board);

  messageElement.textContent = "Solving...";
  boardOutput.textContent = "";

  try {
    const response = await fetch("http://localhost:8080/solve", {
      method: "POST",
      headers: {
        "Content-Type": "text/plain"
      },
      body: puzzleText
    });

    const resultText = await response.text();

    if (!response.ok) {
      messageElement.textContent = resultText;
      return;
    }

    const solvedBoard = textToBoard(resultText);
    setBoard(solvedBoard);
    messageElement.textContent = "Solved successfully.";
    boardOutput.textContent = resultText;
  } catch (error) {
    messageElement.textContent = "Could not reach backend. Start the C++ server first.";
  }
}

solveBtn.addEventListener("click", solveSudoku);
clearBtn.addEventListener("click", clearBoard);
exampleBtn.addEventListener("click", loadExample);

createBoard();