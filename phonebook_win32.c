#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- CORE PHONEBOOK LOGIC ---
#define MAX_CONTACTS 100
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15
#define FILENAME "phonebook.txt"

typedef struct {
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
} Contact;

Contact phonebook[MAX_CONTACTS];
int contactCount = 0;

// --- Win32 GUI IDs and State ---
#define ID_ADD_BUTTON    101
#define ID_VIEW_BUTTON   102
#define ID_SEARCH_BUTTON 103
#define ID_DELETE_BUTTON 104
#define ID_EXIT_BUTTON   105
#define ID_OK_BUTTON     106
#define ID_MAIN_DISPLAY  107
#define ID_USER_INPUT    108

// Application States for handling multi-step input
typedef enum {
    STATE_IDLE,
    STATE_ADD_NAME,
    STATE_ADD_PHONE,
    STATE_SEARCH,
    STATE_DELETE
} AppState;

// Global control handles and state variable
HWND hDisplay, hInput, hwnd;
AppState currentState = STATE_IDLE;
char tempName[MAX_NAME_LEN] = {0}; 

// Comparison function for qsort to sort contacts by name (ascending, case-insensitive)
int compareContacts(const void *a, const void *b) {
    const Contact *contactA = (const Contact *)a;
    const Contact *contactB = (const Contact *)b;
    
    // Use _stricmp (case-insensitive string compare) for alphabetical order
    return _stricmp(contactA->name, contactB->name);
}

// Sorts the entire phonebook array by name
void sortPhonebook() {
    if (contactCount > 0) {
        qsort(phonebook, contactCount, sizeof(Contact), compareContacts);
    }
}

// --- Helper function to check for duplicate name (case-insensitive) ---
// Returns 1 (true) if a contact with the given name already exists, 0 (false) otherwise.
int isContactNameDuplicate(const char *name) {
    for (int i = 0; i < contactCount; i++) {
        // Use _stricmp for case-insensitive comparison
        if (_stricmp(phonebook[i].name, name) == 0) {
            return 1; // Duplicate found
        }
    }
    return 0; // No duplicate found
}


// --- Data & Display Functions (Modified for GUI) ---

void saveToFile();
void loadFromFile() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        contactCount = 0;
        return;
    }
    contactCount = 0;
    while (contactCount < MAX_CONTACTS && 
           fscanf(file, "%49[^,],%14[^\n]\n", 
                       phonebook[contactCount].name, 
                       phonebook[contactCount].phone) == 2) {
        contactCount++;
    }
    fclose(file);
    
    // Ensure contacts are sorted after loading from file
    sortPhonebook(); 
}

void saveToFile() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) return;
    for (int i = 0; i < contactCount; i++) {
        fprintf(file, "%s,%s\n", phonebook[i].name, phonebook[i].phone);
    }
    fclose(file);
}

void setDisplay(const char* message) {
    SetWindowText(hDisplay, message);
}

void viewContacts() {
    char output[4096] = {0}; 
    char line[100];
    
    if (contactCount == 0) {
        strcpy(output, "The phonebook is empty.");
    } else {
        // Contacts are already sorted
        sprintf(output, "--- All Saved Contacts (%d total) ---\r\n\r\n", contactCount);
        for (int i = 0; i < contactCount; i++) {
            sprintf(line, "%-20s : %s\r\n", phonebook[i].name, phonebook[i].phone);
            strcat(output, line);
        }
    }
    setDisplay(output);
}

void searchContactByCriteria(const char* searchCriteria) {
    char output[4096] = {0};
    char line[100];
    int found = 0;
    int criteriaLength = strlen(searchCriteria);
    
    // Determine the search mode
    int searchMode = 0; // 0: Partial Match (First Letter)
    if (criteriaLength > 1) {
        searchMode = 1; // 1: Exact Match (Full Name)
    }

    if (searchMode == 1) {
        sprintf(output, "Search results for exact name '%s' (case-insensitive):\r\n\r\n", searchCriteria);
    } else {
        sprintf(output, "Search results for names starting with '%c' (case-insensitive):\r\n\r\n", (char)toupper(searchCriteria[0]));
    }

    for (int i = 0; i < contactCount; i++) {
        int match = 0;
        
        if (searchMode == 1) { // Exact Match (Full Name)
            // Use _stricmp for case-insensitive comparison
            if (_stricmp(phonebook[i].name, searchCriteria) == 0) {
                match = 1;
            }
        } else { // Partial Match (First Letter)
            // Check the first character case-insensitively
            if (toupper(phonebook[i].name[0]) == toupper(searchCriteria[0])) {
                match = 1;
            }
        }

        if (match) {
            sprintf(line, "%-20s : %s\r\n", phonebook[i].name, phonebook[i].phone);
            strcat(output, line);
            found = 1;
        }
        
        if (searchMode == 1 && found) {
            break; 
        }
    }
    
    if (!found) {
        strcat(output, "No contacts found matching the criteria.");
    }
    setDisplay(output);
}

// MODIFIED FUNCTION: Captures and displays the actual contact name on successful single-delete
// mode: 0 for Partial (first letter), 1 for Exact (full name)
void deleteContactByCriteria(const char* searchCriteria, int mode) {
    int deletedCount = 0;
    int i, j;
    char deletedName[MAX_NAME_LEN] = {0}; // NEW: Store the name of the first exact match deleted
    
    for (i = 0, j = 0; i < contactCount; i++) {
        int shouldDelete = 0;
        
        if (mode == 1) { // Exact Name Match (Full name provided)
            // Use _stricmp for guaranteed case-insensitive match
            if (_stricmp(phonebook[i].name, searchCriteria) == 0) {
                shouldDelete = 1;
                // Capture the actual name before it is overwritten/removed
                if (deletedCount == 0) {
                    strncpy(deletedName, phonebook[i].name, MAX_NAME_LEN - 1);
                    deletedName[MAX_NAME_LEN - 1] = '\0';
                }
            }
        } else { // Partial Match (First letter provided)
            // Checks if the first character matches case-insensitively
            if (toupper(phonebook[i].name[0]) == toupper(searchCriteria[0])) {
                shouldDelete = 1;
            }
        }

        if (shouldDelete) {
            deletedCount++;
        } else {
            if (i != j) {
                phonebook[j] = phonebook[i]; 
            }
            j++; 
        }
    }

    contactCount = j; 

    if (deletedCount > 0) {
        saveToFile();
        char resultMsg[100];
        
        // NEW LOGIC: If it was an exact match and only one contact was deleted, use the actual name.
        if (mode == 1 && deletedCount == 1) {
             sprintf(resultMsg, "Successfully deleted contact '%s'.", deletedName);
        } else {
             // For partial/multi-delete, use the generic message.
             sprintf(resultMsg, "Successfully deleted %d contact(s) matching '%s'.", deletedCount, searchCriteria);
        }
        
        setDisplay(resultMsg);
    } else {
        char resultMsg[100];
        sprintf(resultMsg, "No contacts found matching '%s' to delete.", searchCriteria);
        setDisplay(resultMsg);
    }
}


// --- Window Procedure and Message Handling ---
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    const char CLASS_NAME[]  = "PhonebookWindowClass";
    
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE);

    if (!RegisterClass(&wc)) return 0;

    hwnd = CreateWindowEx(
        0, CLASS_NAME, "Phonebook GUI", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 500, NULL, NULL, hInstance, NULL
        );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    
    switch (uMsg) {
        
        case WM_CREATE: {
            loadFromFile(); 

            int btnWidth = 100, btnHeight = 30;
            int xCenter = 400 / 2;

            // --- Vertical Action Buttons ---
            CreateWindow("BUTTON", "Add", WS_VISIBLE | WS_CHILD, xCenter - btnWidth/2, 20, btnWidth, btnHeight, hwnd, (HMENU)ID_ADD_BUTTON, NULL, NULL);
            CreateWindow("BUTTON", "View", WS_VISIBLE | WS_CHILD, xCenter - btnWidth/2, 55, btnWidth, btnHeight, hwnd, (HMENU)ID_VIEW_BUTTON, NULL, NULL);
            CreateWindow("BUTTON", "Search", WS_VISIBLE | WS_CHILD, xCenter - btnWidth/2, 90, btnWidth, btnHeight, hwnd, (HMENU)ID_SEARCH_BUTTON, NULL, NULL);
            CreateWindow("BUTTON", "Delete", WS_VISIBLE | WS_CHILD, xCenter - btnWidth/2, 125, btnWidth, btnHeight, hwnd, (HMENU)ID_DELETE_BUTTON, NULL, NULL);
            CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, xCenter - btnWidth/2, 160, btnWidth, btnHeight, hwnd, (HMENU)ID_EXIT_BUTTON, NULL, NULL);

            // --- Main Display (Multiline, Read-Only Edit Control) ---
            hDisplay = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "Welcome to the Phonebook contacts!",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                10, 200, 364, 150, hwnd, (HMENU)ID_MAIN_DISPLAY, NULL, NULL);

            // --- User Input Box (Single-line Edit Control) ---
            hInput = CreateWindow("EDIT", "", 
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                10, 370, 364, 25, hwnd, (HMENU)ID_USER_INPUT, NULL, NULL);

            // --- OK Button ---
            CreateWindow("BUTTON", "OK", WS_VISIBLE | WS_CHILD, xCenter - btnWidth/2, 410, btnWidth, btnHeight, hwnd, (HMENU)ID_OK_BUTTON, NULL, NULL);

            break;
        }

        case WM_COMMAND: {
            char inputBuffer[MAX_NAME_LEN + 15] = {0}; 
            int inputLength;
            
            switch (LOWORD(wParam)) {
                case ID_ADD_BUTTON:
                    currentState = STATE_ADD_NAME;
                    setDisplay("Enter Name:");
                    SetWindowText(hInput, "");
                    SetFocus(hInput);
                    break;
                
                case ID_VIEW_BUTTON:
                    currentState = STATE_IDLE;
                    SetWindowText(hInput, "");
                    viewContacts();
                    break;

                case ID_SEARCH_BUTTON:
                    currentState = STATE_SEARCH;
                    // Prompt for dual-mode search
                    setDisplay("Enter a full name for exact search, OR enter a single letter for partial search:");
                    SetWindowText(hInput, "");
                    SetFocus(hInput);
                    break;
                
                case ID_DELETE_BUTTON:
                    currentState = STATE_DELETE;
                    // Prompt for dual-mode deletion
                    setDisplay("Enter a full name for exact delete, OR enter a single letter to delete all contacts starting with that letter:"); 
                    SetWindowText(hInput, "");
                    SetFocus(hInput);
                    break;

                case ID_EXIT_BUTTON:
                    saveToFile();
                    PostQuitMessage(0);
                    break;

                case ID_OK_BUTTON:
                    inputLength = GetWindowText(hInput, inputBuffer, sizeof(inputBuffer) - 1);
                    SetWindowText(hInput, "");

                    if (inputLength == 0 && currentState != STATE_IDLE) {
                        setDisplay("Input cannot be empty. Please try again.");
                        break;
                    }
                    
                    if (currentState == STATE_ADD_NAME) {
                        strncpy(tempName, inputBuffer, MAX_NAME_LEN - 1);
                        tempName[MAX_NAME_LEN - 1] = '\0';
                        
                        // --- CHECK FOR DUPLICATE NAME AFTER GETTING NAME ---
                        if (isContactNameDuplicate(tempName)) {
                            char duplicateMsg[100];
                            sprintf(duplicateMsg, "Error: Contact name '%s' already exists. Please enter a unique name.", tempName);
                            setDisplay(duplicateMsg);
                            currentState = STATE_IDLE;
                            tempName[0] = '\0';
                            SetWindowText(hInput, "");
                            break; 
                        }
                        // --- END DUPLICATE CHECK ---

                        currentState = STATE_ADD_PHONE;
                        setDisplay("enter phone:");
                        SetFocus(hInput);
                    
                    } else if (currentState == STATE_ADD_PHONE) {
                        if (strlen(inputBuffer) != 10) {
                            // If invalid, re-display the rule and stay in the state
                            setDisplay("Invalid input. Phone must be 10 digits. Please re-enter.");
                            currentState = STATE_ADD_PHONE;
                            SetFocus(hInput);
                        } else {
                            // Add and Save the contact
                            if (contactCount < MAX_CONTACTS) {
                                strncpy(phonebook[contactCount].name, tempName, MAX_NAME_LEN - 1);
                                phonebook[contactCount].name[MAX_NAME_LEN - 1] = '\0'; // Ensure termination
                                strncpy(phonebook[contactCount].phone, inputBuffer, MAX_PHONE_LEN - 1);
                                phonebook[contactCount].phone[MAX_PHONE_LEN - 1] = '\0'; // Ensure termination
                                contactCount++;
                                
                                // Sort the phonebook array after adding the new contact
                                sortPhonebook();
                                
                                saveToFile(); 
                                
                                char successMsg[100];
                                sprintf(successMsg, "Contact '%s' added successfully!", tempName);
                                setDisplay(successMsg);
                            } else {
                                setDisplay("Error: Phonebook is full.");
                            }
                            currentState = STATE_IDLE;
                            tempName[0] = '\0';
                        }
                    
                    } else if (currentState == STATE_SEARCH) {
                        // Use the corrected dual-mode search function
                        searchContactByCriteria(inputBuffer);
                        currentState = STATE_IDLE;
                    
                    } else if (currentState == STATE_DELETE) {
                        // Determine the deletion mode based on input length
                        int deleteMode = 0; // Default: Partial (First Letter)
                        if (inputLength > 1) {
                            deleteMode = 1; // Exact Match (Full Name)
                        }

                        // Call the corrected deletion function
                        deleteContactByCriteria(inputBuffer, deleteMode); 
                        currentState = STATE_IDLE;
                    }

                    break;
            }
            break;
        }

        case WM_CLOSE:
            saveToFile();
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}