# 📂 Segregate By Date

A lightweight Linux command-line utility written in C that automatically organizes the files in a directory into date-named folders (`YYYY-MM-DD`) based on each file's **creation (birth) time**.

Built using low-level Linux system calls, with no external dependencies.

---

## ✨ Features

- Organizes files by their **real creation time** (via `statx()` and `STATX_BTIME`), not just modification time
- Automatically creates `YYYY-MM-DD` folders when they do not exist
- Reuses existing date folders
- Skips hidden files (names starting with `.`) and non-regular files (directories, symlinks, etc.)
- Zero dependencies, only the C standard library and Linux system calls

---

## 🧠 How It Works

1. Opens the target directory with `opendir()` and walks through it using `readdir()`.
2. Filters out hidden files and anything that is not a regular file (`DT_REG`).
3. For each file, calls `statx()` to read its birth time (`stx_btime`).
4. Converts the timestamp to UTC with `gmtime_r()` and formats it using `strftime()` as `YYYY-MM-DD`.
5. Creates the folder with `mkdir()` (ignored if it already exists, `EEXIST`).
6. Moves the file into that folder with `rename()`.

### Example

**Before**

```
Downloads/
├── report.pdf        (created 2026-09-28)
├── photo.jpg         (created 2026-09-28)
├── notes.txt         (created 2026-09-30)
└── .hidden           (skipped)
```

**After**

```
Downloads/
├── 2026-09-28/
│   ├── report.pdf
│   └── photo.jpg
├── 2026-09-30/
│   └── notes.txt
└── .hidden
```

---

## 🛠️ Requirements

| Requirement | Details |
|-------------|---------|
| OS | Linux |
| Kernel | 4.11 or newer (for `statx`) |
| glibc | 2.28 or newer (for the `statx()` wrapper) |
| Compiler | GCC or Clang |
| Filesystem | Must support birth time (ext4, btrfs, xfs, etc.) |

---

## 🚀 Build & Run

### Clone the repository

```bash
git clone https://github.com/Mangesh03/<repository-name>.git
cd <repository-name>
```

### Compile

```bash
gcc -Wall -Wextra -o segregate_by_date segregate_by_date.c
```

### Run

```bash
./segregate_by_date /absolute/path/to/directory
```

### Sample output

```
Open directory
report.pdf
File name : report.pdf
Folder name : 2026-09-28
Directory Path to mkdir : /home/user/Downloads/2026-09-28
Moved file : report.pdf successfully
Success
```

---

## 📁 Project Structure

```
.
├── segregate_by_date.c   # Source code
└── README.md             # Documentation
```

### Functions

| Function | Description |
|----------|-------------|
| `main()` | Validates arguments and starts the process |
| `Segregate_By_Date(char *directory_name)` | Iterates over the directory and filters eligible files |
| `GetCreationTime(char *directory_name, char *file_name)` | Reads the creation time, creates the date folder and moves the file |

---

## ⚠️ Notes & Limitations

- **Files are moved, not copied.** Test on a sample directory first.
- Folder names use **UTC**, so a file created late at night local time may land in the next day's folder.
- If the filesystem does not expose birth time, the file is left untouched and a message is printed.
- Only the top level of the given directory is processed (no recursion).
- If two files share the same name in the same date folder, `rename()` will overwrite the existing one.

---

## 🔮 Future Improvements

- Dry-run mode (`--dry-run`) to preview changes
- Fallback to modification time when birth time is unavailable
- Local time support via `localtime_r()`
- Recursive directory processing
- Collision handling for duplicate file names
- Custom date format option (e.g. `YYYY/MM`)

---

## 👤 Author

**Mangesh Ashok Bedre**

- GitHub: [@Mangesh03](https://github.com/Mangesh03)

---

