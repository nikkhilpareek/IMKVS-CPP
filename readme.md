# In-Memory Key-Value Store

<p align="center"><img src="https://socialify.git.ci/nikkhilpareek/IMKVS-CPP/image?custom_description=A+Simple+in-memory+Key-Value+Storage&amp;description=1&amp;language=1&amp;name=1&amp;owner=1&amp;stargazers=1&amp;theme=Auto" alt="project-image"></p>

A simple, persistent, in-memory key-value store built with C++.
It features an interactive command-line interface (CLI) and supports basic database transactions, multiple data types, and key expirations.

---

## Features

-   **CRUD Operations**: `SET`, `GET`, `REMOVE` for basic data manipulation.
-   **Multiple Data Types**: Natively supports both **strings** and **integers**.
-   **Atomic Operations**: `INCR` and `DECR` commands for safe, atomic modification of integer values.
-   **Time-To-Live (TTL)**: Keys can be set with an automatic expiration time, making it suitable for caching.
-   **In-Memory Speed**: All data is stored in memory for fast access using `std::unordered_map`.
-   **JSON Persistence**: Data is saved to `data.json` on exit and reloaded on startup.
-   **Transaction Support**: Atomic operations using `BEGIN`, `COMMIT`, and `ROLLBACK`. All commands, including `INCR`/`DECR`, are fully transactional.
-   **Interactive CLI**: A user-friendly command line for interacting with the store.

---

## Getting Started

### Prerequisites

-   A C++ compiler that supports the C++17 standard (e.g., `g++` or `clang++`)
-   The [`nlohmann/json`](https://github.com/nlohmann/json) single-header library (`json.hpp`) placed in the project directory

---

### Building

To build the project, run the following command in your terminal:

```
bash
g++ main.cpp KeyValueStore.cpp -o main -std=c++17
```

---

### Running

Execute the compiled program to start the interactive CLI:

```bash
./main
```

---

## CLI Commands

The following commands are available in the CLI:

| Command                    | Description                                                                 | Example                  |
|---------------------------|-----------------------------------------------------------------------------|--------------------------|
| `SET key value [ttl_ms]`  | Sets a key to a value with an optional TTL (in milliseconds).               | `SET name "Nikhil" 60000` |
| `GET key`                 | Retrieves the value for a given key.                                        | `GET name`               |
| `REMOVE key`              | Deletes a key-value pair from the store.                                    | `REMOVE name`            |
| `INCR key`                | Atomically increments an integer key. Creates it if non-existent.           | `INCR counter`           |
| `DECR key`                | Atomically decrements an integer key. Creates it if non-existent.           | `DECR counter`           |
| `COUNT`                   | Returns the total number of keys in the store.                              | `COUNT`                  |
| `BEGIN`                   | Starts a new transaction.                                                   | `BEGIN`                  |
| `COMMIT`                  | Saves all changes made during the current transaction.                      | `COMMIT`                 |
| `ROLLBACK`                | Discards all changes made during the current transaction.                   | `ROLLBACK`               |
| `EXIT`                    | Saves the current state to `data.json` and closes the CLI.                  | `EXIT`                   |

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

### To be Added in Future

#### Core Feature Enhancements

- Namespaces: Allow for multiple, isolated databases within a single running instance, accessible via a SELECT command.

- REST API: Expose the key-value store over HTTP, allowing other applications to interact with it via a web API.

#### Robustness & Performance

- Thread Safety: Add mutexes to the data structures to allow for safe concurrent access from multiple threads.

- Performance Benchmarking: Create a test suite to measure the performance of SET and GET operations and compare them to other in-memory stores.

#### Developer Experience & Tooling
- Unit Testing: Integrate a testing framework like Google Test or Catch2 to create automated tests for the KeyValueStore class.

- CMake Build System: Replace the manual g++ command with a CMakeLists.txt file for a standard, cross-platform build system.

- Continuous Integration (CI): Set up a GitHub Actions workflow to automatically build the project and run tests every time you push new code.
