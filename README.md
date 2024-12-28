<<<<<<< HEAD
# Projet_Fginal_SFSD
=======
# File Organization and Management System in C

## Overview
This project implements a file organization and management system in C, supporting both contiguous and linked file organizations with various internal organization methods. It provides functionality for file creation, record management, searching, and maintenance operations.

## Features
- **File Organization Types**
  - Contiguous organization
  - Linked organization
  - Support for both sorted and unsorted internal organizations

- **Core Operations**
  - File creation and initialization
  - Record insertion (sorted and unsorted)
  - Record searching (sequential and binary)
  - Record deletion (logical and physical)
  - File defragmentation
  - File renaming
  - Memory management and allocation

- **Memory Management**
  - Block-based storage system
  - Dynamic memory allocation
  - Defragmentation capabilities
  - Secondary memory simulation

## Technical Specifications
- Maximum data size per record: 512 bytes
- Maximum number of blocks: 10
- Block factor: 5
- Total number of available blocks: 1024
- Maximum filename length: 200 characters

## Data Structures
### Record (Enregistrement)
```c
typedef struct {
    int id;           // Unique identifier
    char data[MAX_DATA_SIZE]; 
    int is_deleted;   // Deletion flag
} Enregistrement;
```

### Block (Bloc)
```c
typedef struct {
    int is_free;    
    int next_bloc;  
    int record_count;
    Enregistrement enreg[FACTEUR_DE_BLOCAGE];
    char Data[MAX_DATA_SIZE];
} Bloc;
```

## Main Functions

### File Operations
- `create_file()`: Creates a new file with specified organization
- `chargerEnregistrement_fichier()`: Loads records into a file
- `LireFichier()`: Reads and displays file content
- `renommerFichier()`: Renames a file

### Record Management
- `insertion_enregNonTrie()`: Inserts records in unsorted mode
- `inserer_enregTrie()`: Inserts records in sorted mode
- `RhercheEnregistrementContNonOrd()`: Searches in unsorted contiguous files
- `RechercheEnregistrementContOrd()`: Binary search in sorted contiguous files
- `suplogiqueEnregistremen()`: Logical record deletion
- `supphy()`: Physical record deletion

### Memory Management
- `initialiser_disque()`: Initializes disk space
- `compactage()`: Performs memory compaction
- `defragment()`: Defragments file storage
- `gestionDespace()`: Manages disk space allocation

## Usage Example
```c
// Create and initialize a file
Fichier fichier;
create_file(&fichier);

// Load records
chargerEnregistrement_fichier(&fichier);

// Perform operations
// ... (specific operation examples)

// Clean up
defragment(&fichier);
```

## Compilation and Execution
To compile the program:
```bash
gcc -o sfsd_project main.c
./sfsd_project
```

## Notes
- Ensure proper initialization of the disk space before performing operations
- Regular defragmentation is recommended for optimal performance
- File operations are buffered for efficiency
- Memory management is handled automatically

## Error Handling
The system includes error handling for:
- Memory allocation failures
- File operation errors
- Invalid record operations
- Space management issues

## Contributing
To contribute to this project:
1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request
>>>>>>> origin/master
