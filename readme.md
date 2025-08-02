# In-Memory Key-Value Store

<p align="center"><img src="https://socialify.git.ci/nikkhilpareek/IMKVS-CPP/image?custom_description=A+Simple+in-memory+Key-Value+Storage&amp;description=1&amp;language=1&amp;name=1&amp;owner=1&amp;stargazers=1&amp;theme=Auto" alt="project-image"></p>

A simple, persistent, thread-safe, in-memory key-value store built with C++.
It features a professional build system, automated CI/CD pipeline, unit tests, and robust data integrity checks.

---

## Features

-   **CRUD Operations**: `SET`, `GET`, `REMOVE` for basic data manipulation.
-   **Multiple Data Types**: Natively supports both **strings** and **integers**.
-   **Atomic Operations**: `INCR` and `DECR` commands for safe modification of integer values.
-   **Time-To-Live (TTL)**: Keys can be set with an automatic expiration time.
-   **Transaction Support**: Atomic operations using `BEGIN`, `COMMIT`, and `ROLLBACK`.
-   **Thread Safety**: All data operations are thread-safe using `std::recursive_mutex`, allowing for safe concurrent access.
-   **Data Integrity**: Each key-value pair is individually hashed with SHA-256 to detect tampering or corruption in the persisted file.
-   **Robust Persistence**: On startup, the store gracefully handles corrupted entries in the `data.json` file without crashing, loading all valid data.
-   **Professional Build System**: Uses **CMake** for a standardized, cross-platform build process.
-   **Automated Testing**: Integrated with the **Google Test** framework for unit testing.
-   **Continuous Integration**: A **GitHub Actions** workflow automatically builds and tests the project on every push and pull request.

---

## Getting Started

### Prerequisites

-   A C++ compiler that supports the C++17 standard (e.g., `g++`, `clang++`, or MSVC).
-   **CMake** (version 3.14 or higher).
-   **Git**.

---

### Building

The project uses a standard CMake workflow. The `nlohmann/json`, `picosha2`, and `googletest` libraries are handled automatically by CMake's `FetchContent` module.

```bash
# 1. Clone the repository
git clone [https://github.com/nikkhilpareek/IMKVS-CPP.git](https://github.com/nikkhilpareek/IMKVS-CPP.git)
cd IMKVS-CPP

# 2. Create a build directory
mkdir build
cd build

# 3. Configure the project
cmake ..

# 4. Compile the code
cmake --build .
```

This will create two executables inside the build directory: `imkvs` (the main application) and `kv_tests` (the test runner).

---

### Running

To start the interactive CLI, run the `imkvs` executable from the build directory:
```bash
./imkvs
```

---
### Running the Tests

To run the automated unit tests, execute the kv_tests executable from the build directory:
```bash
./kv_tests
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
├── .github/workflows/ci.yml # GitHub Actions workflow for CI
├── .gitignore               # Specifies files for Git to ignore
├── CMakeLists.txt           # The main CMake build script
├── KeyValueStore.cpp        # Implementation of the key-value store logic
├── KeyValueStore.h          # Class interface for the key-value store
├── main.cpp                 # Contains the main application loop and CLI logic
├── tests.cpp                # Unit tests using the Google Test framework
└── readme.md                # Project documentation
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
1. REST API: Expose the key-value store over HTTP, allowing other applications to interact with it via a web API.

2. Namespaces: Allow for multiple, isolated databases within a single running instance.

3. Encryption: Encrypt the data.json file to protect data confidentiality.

4. Performance Benchmarking: Create a test suite to measure the performance of operations.
