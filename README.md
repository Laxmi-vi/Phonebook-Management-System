# 📞 Phonebook Management System

A GUI-based Phonebook Management System developed using **C Programming** and the **Win32 API**.

This project provides a simple Windows graphical interface for managing contacts, including adding, viewing, searching, and deleting contacts.

## 🚀 Features

- ➕ Add new contacts
- 📋 View all saved contacts
- 🔍 Search contacts
- 🗑️ Delete contacts
- 🔤 Alphabetical sorting of contacts
- 🚫 Duplicate name validation
- 📱 Phone number validation
- 💾 File handling for persistent contact storage
- 🖥️ Windows GUI using Win32 API

## 🛠️ Technologies Used

- **C Programming**
- **Win32 API**
- **File Handling**
- **Structures**
- **Arrays**
- **qsort()**
- **GCC Compiler**

## 📂 Project Structure

```text
Phonebook-Management-System/
│
├── phonebook.c
├── phonebook_win32.c
├── phonebook.txt
└── README.md
```

### 📄 File Description

| File | Description |
|---|---|
| `phonebook.c` | Console-based Phonebook application |
| `phonebook_win32.c` | Windows GUI-based Phonebook application |
| `phonebook.txt` | Stores phonebook contact data |
| `README.md` | Project documentation |

## 🖥️ GUI Application

The GUI version is developed using the Windows **Win32 API**.

It provides:

- Add
- View
- Search
- Delete
- Exit

The application also stores contacts in `phonebook.txt`.

## 🔎 Search Function

The GUI supports:

- Searching by the **first letter** of a contact name
- Searching by the **full contact name**

Search is performed without considering letter case.

## 🗑️ Delete Function

The application supports:

- Deleting a contact by full name
- Deleting contacts by first letter

A confirmation step is included before deletion.

## 💾 Data Storage

Contact information is stored in:

```text
phonebook.txt
```

The application loads existing contacts when it starts and saves changes when contacts are added or deleted.

## ▶️ How to Run

### Requirements

- Windows
- GCC / MinGW
- Command Prompt or PowerShell

### Run the GUI Version

Open the project folder in Command Prompt or PowerShell and run:

```bash
gcc phonebook_win32.c -o phonebook_gui.exe -mwindows
```

Then run:

```bash
phonebook_gui.exe
```

### Run the Console Version

To compile the console version:

```bash
gcc phonebook.c -o phonebook.exe
```

Then run:

```bash
phonebook.exe
```

## 🎯 Learning Outcomes

This project helped me strengthen my understanding of:

- C Programming
- Data Structures
- Structures and Arrays
- File Handling
- Searching and Sorting
- Windows GUI Development
- Event-driven Programming
- Problem Solving

## 👨‍💻 Author

**Laxmi Kounte**

Computer Science Engineering Student

## ⭐ Future Improvements

- Edit existing contacts
- Add contact groups
- Improve GUI design
- Add additional validation
- Add database support
- Cross-platform GUI support

---

⭐ If you find this project useful, feel free to explore the source code.
