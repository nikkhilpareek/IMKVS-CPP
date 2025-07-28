# In-Memory Key Value Storage

A simple key-value store with a command-line interface.

### Core Commands

1.  **`Get(key)`**: Get the data stored for a particular key. Loads data from `data.csv` on startup.
2.  **`Set(key, value)`**: Store a value for a key so we can access it later.
3.  **`Delete(key)`**: Delete the data stored under a key.
4.  **`Count()`**: Count the number of key-value pairs stored.
5.  **`EXIT()`**: Exit the interactive mode and save all data to a CSV file.

### Future Updates to add:

* **Transactions**: Group commands together with `BEGIN`, `COMMIT`, and `ROLLBACK`.
* **Time-To-Live (TTL)**: Add an expiration time to keys so they delete automatically.
* **More Data Types**: Support numbers and commands like `INCR` (increment).
* **Enhanced Commands**: Add more helpful utilities like `KEYS` or `CLEAR`.
* **Better File Format**: Switch from CSV to a more robust format like JSON.