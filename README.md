# meeting-scheduler
Command-line program in C that manages a local database of one-hour meetings. Supports adding, deleting, listing, saving, and loading meetings. Uses dynamic memory allocation for flexible storage of meeting data.

-----

# Features
- Add a meeting with a description, month, day, and hour.
- Delete a meeting by date and time.
- List all meetings in chronological order.
- Save the schedule to a file.
- Load a schedule from a file (replaces current database).
- Quit gracefully while freeing all allocated memory.
