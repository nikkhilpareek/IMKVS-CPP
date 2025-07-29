# In-Memory Key-Value Store

A simple, persistent, in-memory key-value store built with C++.  
It features an interactive command-line interface (CLI) and supports basic database transactions.

---

## Features

- **CRUD Operations**: `SET`, `GET`, `REMOVE` for basic data manipulation.
- **In-Memory Speed**: All data is stored in memory for fast access using `std::unordered_map`.
- **JSON Persistence**: Data is saved to `data.json` on exit and reloaded on startup.
- **Transaction Support**: Atomic operations using `BEGIN`, `COMMIT`, and `ROLLBACK`.
- **Interactive CLI**: A user-friendly command line for interacting with the store.
- **Utility Commands**: Includes a `COUNT` command to check the number of stored keys.

---

## Getting Started

### Prerequisites

- A C++ compiler that supports the C++17 standard (e.g., `g++` or `clang++`)
- The [`nlohmann/json`](https://github.com/nlohmann/json) single-header library (`json.hpp`) placed in the project directory

---

### Building

To build the project, run the following command in your terminal:

```bash
g++ main.cpp KeyValueStore.cpp -o main -std=c++17
```

---

### Running

Execute the compiled program to start the interactive CLI:

```bash
./main
```

---

## Commands

The following commands are available in the CLI:

| Command          | Description                                                  | Example                        |
|------------------|--------------------------------------------------------------|--------------------------------|
| `SET key value`  | Sets a key to a specific value. Handles values with spaces. | `SET name "Alice Smith"`       |
| `GET key`        | Retrieves the value for a given key.                        | `GET name`                     |
| `REMOVE key`     | Deletes a key-value pair from the store.                    | `REMOVE name`                  |
| `COUNT`          | Returns the total number of keys in the store.             | `COUNT`                        |
| `BEGIN`          | Starts a new transaction.                                   | `BEGIN`                        |
| `COMMIT`         | Saves all changes made during the current transaction.      | `COMMIT`                       |
| `ROLLBACK`       | Discards all changes made during the current transaction.   | `ROLLBACK`                     |
| `EXIT`           | Saves the current state to `data.json` and closes the CLI.  | `EXIT`                         |

---

## Project Structure

```
.
├── main.cpp           // Contains the main application loop and CLI logic
├── KeyValueStore.h    // Class interface for the key-value store
├── KeyValueStore.cpp  // Implementation of the key-value store logic
├── json.hpp           // The nlohmann JSON single-header library
└── data.json          // File where data is persisted
```

---

## Dependencies

- [nlohmann/json](https://github.com/nlohmann/json) — for JSON parsing and persistence

---

## Notes

- Ensure `json.hpp` is in the same directory as your C++ source files.
- The application creates and updates `data.json` automatically when `EXIT` is used.

---


