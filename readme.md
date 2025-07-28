# In-Memory Key Value Storage 

Key-value stores generally share the following interface:

1. Get( key ): Get the data stored for a particular key. loads data from data.csv(if available)

2. Set( key, value ): Stores "value" for a "key" so we can access it later

3. Delete( key ): Delete the data that was stored under the “key”.

4. Count(): Count number of Key-value Pairs Stored.

5. EXIT(): Exits from the CLI interactive mode, saves the data in a csv file.