# Proposal: Integrated Mouse/Touch Event Support for linenoise

## 1. Context & Purpose
In terminal-based interactive applications (like the *Serial Experiments Lain* interactive fiction), players in mobile environments (such as Termux) frequently interact via touchscreens.
The traditional `linenoise` library is a minimalist line editor that only handles standard keyboard input. When a terminal is in mouse-tracking mode, user taps generate SGR escape sequences starting with `\x1b[<`. Because `linenoise` does not recognize these, it leads to:
*   **Garbage Leakage**: Mouse sequences are treated as plain text and displayed directly in the prompt.
*   **Logic Blocking**: Mouse events cannot be captured in real-time by the upper-level engine without waiting for a newline.

**Goal:** Identify and handle mouse escape sequences within the `linenoise` core loop to ensure a clean UI and provide an event feedback mechanism.

## 2. Description of Changes

### A. Core Interception Mechanism
Deeply parse the `\x1b` (ESC) sequences within the internal `linenoise` read loop:
*   **State Machine Parsing**: When an `\x1b[` sequence is detected, check if the subsequent character is `<` (SGR mouse protocol identifier).
*   **Sequence Consumption**: Once identified as a mouse sequence, continue reading until the terminator `M` (press) or `m` (release) is encountered, ensuring the entire multi-byte sequence is consumed.

### B. Event Return Mechanism
*   **Introduce Special Constants**: Define a new return value or special key code (e.g., `LINENOISE_MOUSE_EVENT`) in `linenoise.h`.
*   **Interruptible Return**: When a valid click is identified, `linenoise` can optionally return immediately with this constant, allowing the game engine to determine the click position.

### C. Interface Updates
*   Add an optional configuration (e.g., `linenoiseSetMouseSupport(int enable)`) to allow users to toggle this feature based on terminal capabilities.

## 3. Technical Requirements

1.  **Silent Handling**: Mouse sequences MUST NOT appear in `l.buf` (the line buffer) and MUST NOT be echoed to the terminal.
2.  **Atomicity**: The entire escape sequence must be consumed atomically. If interrupted, the state must be cleared to prevent input corruption.
3.  **Zero Dependencies**: Maintain the minimalist philosophy of `linenoise` by avoiding heavy dependencies like `ncurses`.
4.  **SGR Compatibility**: Specifically optimize for the `\x1b[<Cb;Cx;CyM` format used by Termux and modern xterm-compatible terminals.

## 4. Expected Outcome

*   The prompt remains clean even if the user taps the screen while inputting text.
*   The application can identify the clicked row/column and translate it into game logic (e.g., selecting a choice).
*   Elimination of input desynchronization issues during rapid interaction.
