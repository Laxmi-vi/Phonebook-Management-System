#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the maximum number of contacts and string lengths
#define MAX_CONTACTS 100
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15
#define FILENAME "phonebook.txt"

// Structure to hold a single contact
typedef struct {
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
} Contact;

// Global array to store contacts and the current count
Contact phonebook[MAX_CONTACTS];
int contactCount = 0;

// Function Prototypes
void loadContacts();
void saveContacts();
void addContact();
void viewContacts();
void searchContact();
void deleteContact();
void displayMenu();
void runApplication();

// --- File Handling Functions ---

// Load contacts from file
void loadContacts() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        // File does not exist or cannot be opened, starting with 0 contacts
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
    printf("\nContacts loaded successfully (%d contacts).\n", contactCount);
}

// Save contacts to file
void saveContacts() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        fprintf(stderr, "\nError: Could not open file for saving!\n");
        return;
    }

    for (int i = 0; i < contactCount; i++) {
        fprintf(file, "%s,%s\n", phonebook[i].name, phonebook[i].phone);
    }
    fclose(file);
}

// --- Application Features ---

// 1. Add Contact
void addContact() {
    if (contactCount >= MAX_CONTACTS) {
        printf("\nError: Phonebook is full. Cannot add more contacts.\n");
        return;
    }

    printf("\n--- Add New Contact ---\n");
    
    // Get Name
    printf("Enter name (max %d chars): ", MAX_NAME_LEN - 1);
    // Use fgets to safely read string with spaces
    if (fgets(phonebook[contactCount].name, MAX_NAME_LEN, stdin) == NULL) return;
    // Remove newline character if present
    phonebook[contactCount].name[strcspn(phonebook[contactCount].name, "\n")] = 0;
    
    // Get Phone Number
    printf("Enter phone number (max %d chars): ", MAX_PHONE_LEN - 1);
    if (fgets(phonebook[contactCount].phone, MAX_PHONE_LEN, stdin) == NULL) return;
    phonebook[contactCount].phone[strcspn(phonebook[contactCount].phone, "\n")] = 0;
    
    // Validation: basic check that input is not empty
    if (strlen(phonebook[contactCount].name) == 0 || strlen(phonebook[contactCount].phone) == 0) {
        printf("\nContact not added: Name or phone number cannot be empty.\n");
        // Decrement contactCount to discard the partially added contact (optional, but good practice)
        return;
    }
    
    contactCount++;
    saveContacts();
    printf("\nContact added successfully! Total contacts: %d\n", contactCount);
}

// 2. View All Contacts
void viewContacts() {
    printf("\n--- All Saved Contacts (%d total) ---\n", contactCount);
    if (contactCount == 0) {
        printf("The phonebook is empty.\n");
        return;
    }

    printf("--------------------------------------------------\n");
    printf("| %-20s | %-15s |\n", "NAME", "PHONE NUMBER");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < contactCount; i++) {
        printf("| %-20s | %-15s |\n", phonebook[i].name, phonebook[i].phone);
    }
    printf("--------------------------------------------------\n");
}

// 3. Search Contact by First Letter
void searchContact() {
    char searchChar;
    int found = 0;
    
    printf("\n--- Search Contacts ---\n");
    printf("Enter the first letter of the name to search: ");
    
    // Read a single character
    char buffer[2];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
    
    // Get the first character and convert to uppercase for case-insensitive search
    searchChar = toupper(buffer[0]); 
    
    if (searchChar == '\0') {
        printf("No letter entered.\n");
        return;
    }

    printf("\nSearch results for names starting with '%c':\n", searchChar);
    printf("--------------------------------------------------\n");
    printf("| %-20s | %-15s |\n", "NAME", "PHONE NUMBER");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < contactCount; i++) {
        // Check if the first letter matches (case-insensitive)
        if (toupper(phonebook[i].name[0]) == searchChar) {
            printf("| %-20s | %-15s |\n", phonebook[i].name, phonebook[i].phone);
            found = 1;
        }
    }
    printf("--------------------------------------------------\n");

    if (!found) {
        printf("No contacts found starting with '%c'.\n", searchChar);
    }
}

// 4. Delete Contact by First Letter
void deleteContact() {
    char deleteChar;
    int deletedCount = 0;
    int i, j;
    
    printf("\n--- Delete Contacts ---\n");
    printf("Enter the first letter of the name to delete (all matching names will be deleted): ");
    
    // Read a single character
    char buffer[2];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
    
    // Get the first character and convert to uppercase for case-insensitive delete
    deleteChar = toupper(buffer[0]); 

    if (deleteChar == '\0') {
        printf("No letter entered.\n");
        return;
    }

    // Confirmation step
    printf("Are you sure you want to delete ALL contacts starting with '%c'? (y/n): ", deleteChar);
    char confirm[2];
    if (fgets(confirm, sizeof(confirm), stdin) == NULL) return;
    if (toupper(confirm[0]) != 'Y') {
        printf("Deletion cancelled.\n");
        return;
    }

    // i is the contact to check, j is the position to move the non-deleted contacts to
    for (i = 0, j = 0; i < contactCount; i++) {
        // Check if the first letter DOES NOT match the delete criteria
        if (toupper(phonebook[i].name[0]) != deleteChar) {
            // Keep the contact by moving it to the 'j' position (only if i != j)
            if (i != j) {
                phonebook[j] = phonebook[i]; 
            }
            j++; // Increment the position for the next contact to keep
        } else {
            // This contact will be deleted
            deletedCount++;
        }
    }

    // Update the contact count
    contactCount = j; 

    if (deletedCount > 0) {
        saveContacts(); // Save the updated list
        printf("\nSuccessfully deleted %d contact(s) starting with '%c'. Total contacts remaining: %d\n", 
               deletedCount, deleteChar, contactCount);
    } else {
        printf("\nNo contacts found starting with '%c' to delete.\n", deleteChar);
    }
}

// --- Menu and Main Loop ---

void displayMenu() {
    printf("\n====================================\n");
    printf("       C CONSOLE PHONEBOOK APP      \n");
    printf("====================================\n");
    printf("1. Add New Contact\n");
    printf("2. View All Contacts\n");
    printf("3. Search Contact (by first letter)\n");
    printf("4. Delete Contact (by first letter)\n");
    printf("5. Exit and Save\n");
    printf("------------------------------------\n");
    printf("Enter your choice: ");
}

// Clear the input buffer after reading a number
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void runApplication() {
    int choice;
    // Load existing data at startup
    loadContacts(); 
    
    do {
        displayMenu();
        
        // Read choice
        if (scanf("%d", &choice) != 1) {
            // Handle non-integer input
            printf("\nInvalid input. Please enter a number from the menu.\n");
            clearInputBuffer();
            choice = 0; // Reset choice to loop again
            continue;
        }
        clearInputBuffer(); // Clear the rest of the line after reading the number

        switch (choice) {
            case 1:
                addContact();
                break;
            case 2:
                viewContacts();
                break;
            case 3:
                searchContact();
                break;
            case 4:
                deleteContact();
                break;
            case 5:
                printf("\nExiting application. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice. Please select an option between 1 and 5.\n");
                break;
        }
    } while (choice != 5);
}

int main() {
    runApplication();
    return 0;
}