#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

 // Maximum size of data in each record
 #define MAX_DATA_SIZE 512
 #define MAX_BLOCKS 10
 #define MODE_CONTIGUE 0
 #define MODE_CHAINEE 1
 #define FACTEUR_DE_BLOCAGE 5
 #define MAX_LINE_LENGTH 100
 #define NBR_BLOCS 1024     // Nombre total de blocs
 #define MAX_FILENAME 200

 // Structure to represent a record (Enregistrement)
typedef struct {
    int id;           // Identifiant unique
    char data[MAX_DATA_SIZE]; // Donn�es de l'enregistrement
    int is_deleted;   // 1 si l'enregistrement est supprim�, 0 sinon
} Enregistrement;

 // Structure to represent a block (Bloc)
 
typedef struct {
    int is_free;    // Indique si le bloc est libre (1 = libre, 0 = occup�)
    int next_bloc;  // Indice du bloc suivant (ou -1 si aucun)
    int record_count;                // Nombre d'enregistrements dans le bloc

    Enregistrement enreg[FACTEUR_DE_BLOCAGE];         // Record stored in this block
    char Data[MAX_DATA_SIZE];
 } Bloc;
 
 Bloc buffer[MAX_DATA_SIZE];
 Bloc *memoire = NULL; // Global declaration

 // Structure to represent a file (Fichier)
 typedef struct {
     char nom[50];                 // Name of the file
     int nb_enregistrements;       // Number of records
int organisation_globale;     //mode d'organisation :chain� ou contigue
int organisation_interne;
     Bloc *bloc[MAX_DATA_SIZE];    // Array of blocks
     int block_count;    // Nombre de blocs
 } Fichier;
 
 
 
 // Pointer to memory for managing blocks


 // Structure de la table d'allocation pour chaque bloc
typedef struct {
    int is_free;   // 1 = libre, 0 = occup�
} TableAllocation;



// D�claration de la table d'allocation comme un tableau de structures TableAllocation
TableAllocation table_allocation[NBR_BLOCS];
 typedef struct {
  char NomDuFichier [20];
  int NbrBloc;
  int Nbren;
  int Adress;
  char Modeglob[20];
  char Modeint [20];
}Meta;

Bloc disque[NBR_BLOCS];// D�claration de la m�moire secondaire simul�e

// Fonction pour initialiser la table d'allocation et la stocker au d�but du disque
void initialiser_table_allocation() {
int i;
    // Initialisation de la table d�allocation
    for ( i = 0; i < NBR_BLOCS; i++) {
        if (i == 0) {
            // Bloc 0 r�serv� pour la table d�allocation
            table_allocation[i].is_free = 0;
        } else {
            // Les autres blocs sont marqu�s comme libres
            table_allocation[i].is_free = 1;
        }
    }

    // Copier la table d�allocation dans le bloc 0 du disque
    memcpy(disque[0].Data, table_allocation, NBR_BLOCS * sizeof(TableAllocation));

    printf("Table d'allocation initialis�e et stock�e dans le bloc 0.\n");
}

// Fonction pour afficher le statut des blocs (debug)
void afficher_table_allocation() {
    printf("Statut des blocs (0 = occup�, 1 = libre):\n");
    int i;
    for ( i = 0; i < NBR_BLOCS; i++) {
        printf("Bloc %d: %d\n", i, table_allocation[i].is_free);
    }
}




void miseAjourAllocation(FILE *f, int bloc_index, int statut) {//pour l'insertion
    fseek(f, bloc_index * sizeof(int), SEEK_SET);  // Se d�placer � la position du bloc � mettre � jour
    fwrite(&statut, sizeof(int), 1, f);  // Mettre � jour le statut du bloc (libre ou occup�)
}


void miseAJourAllocation(FILE* allocationFile, int blockNumber, int status) { // pour suppression
    fseek(allocationFile, blockNumber * sizeof(int), SEEK_SET);  // Se d�placer au bloc sp�cifi�
    fwrite(&status, sizeof(int), 1, allocationFile);  // �crire le nouveau statut (0 pour libre, 1 pour occup�)
    fflush(allocationFile);  // S'assurer que la mise � jour soit bien enregistr�e imm�diatement
}


void mise_a_jour_table_allocation_apres_creation(Fichier* fichier) {
int i;
    for (i = 0; i < fichier->nb_enregistrements; i++) {
        if (table_allocation[i + 1].is_free == 1) {
            table_allocation[i + 1].is_free = 0;  // Marquer comme occup�
            printf("Bloc %d occup� pour le fichier %s.\n", i + 1, fichier->nom);
        }
    }

// Copier la table d'allocation mise � jour dans le disque
    memcpy(disque[0].Data, table_allocation, NBR_BLOCS * sizeof(TableAllocation));
    printf("Table d'allocation mise � jour apr�s cr�ation du fichier.\n");
}




void miseAJourAllocationCompacte(FILE *allocationFile) {
    // R�initialiser la table d'allocation
    fseek(allocationFile, 0, SEEK_SET); // Positionner le curseur au d�but du fichier de la table d'allocation
    int totalBloc = MAX_DATA_SIZE;  // Le nombre total de blocs � suivre, ajustez cette variable si n�cessaire

    // Parcourir chaque entr�e de la table d'allocation (chaque bloc)
    int i;
    for ( i = 0; i < totalBloc; i++) {
        if (memoire[i].is_free == 1) {
            // Bloc lib�r� lors du compactage, donc on marque son allocation dans la table comme libre
            miseAJourAllocation(allocationFile, i, 0);  // Marquer le bloc i comme libre dans la table d'allocation
        } else {
            // Bloc occup� apr�s compactage
            miseAJourAllocation(allocationFile, i, 1);  // Marquer le bloc i comme occup� dans la table d'allocation
        }
    }

    printf("Table d'allocation mise � jour apr�s compactage.\n");
}




 void initialiser_disque(int nb_blocs, int taille_bloc) {
     memoire = (Bloc *)malloc(nb_blocs * sizeof(Bloc)); // Allocate memory for the blocks
     if (memoire == NULL) {
         printf("Erreur de m�moire !\n");
         exit(1);
     }
 }
 
 
 void compactage() {
    int i = 0;
    int j ;
    for ( j = 0; j < MAX_DATA_SIZE; j++) {
        if (memoire[j].is_free == 0) {
            if (i != j) {
                memoire[i] = memoire[j];  
                memoire[j].is_free = 1;  
            }
            i++;
        }
    }
    printf("Compactage effectue.\n");
}

void vider_MS() {
int i;
    for ( i = 0; i < MAX_DATA_SIZE; i++) {
        memoire[i].is_free = 1;
        memoire[i].next_bloc = -1;
    }
}



void gestionDespace(int *disque, int tailleDisque, Enregistrement enregistrement, int tailleBloc) {
    int libres = 0;
    int i ;
    // Calculer le nombres de blocs libres
    for (i = 0; i < tailleDisque; i++) {
        if (disque[i] == 0) {
            libres++;
        }
    }

    // Calcul des blocs n�cessaires pour l'enregistrement
    int blocsNecessaires = (sizeof(enregistrement) + tailleBloc - 1) / tailleBloc;

    // V�rification de l'espace libre
    if (blocsNecessaires <= libres) {
        compactageDisque(disque, tailleDisque);
    } else {
        printf("M�moire pleine !\n");
    }
}



void create_file(Fichier *fichier) {
    printf("Enter file name: ");
    scanf("%s",  fichier->nom);

    printf("Enter number of records: ");
    scanf("%d", &fichier->nb_enregistrements);

    if (fichier->nb_enregistrements <= 0) {
        printf("Number of records must be greater than 0!\n");
        exit(EXIT_FAILURE);
    }

    printf("Select global organization mode (0 for contiguous, 1 for linked): ");
    scanf("%d", &fichier->organisation_globale);

    printf("Select internal organization mode (0 for unsorted, 1 for sorted): ");
    scanf("%d", &fichier->organisation_interne);

    // Allocate memory for file blocks
   

// ????? ?? ???? ?????? ????? ?????????? ???
int i ;
for ( i = 0; i < fichier->nb_enregistrements; i++) {
    fichier->bloc[i] = malloc(sizeof(Bloc));  // ????? ???? ?????
    if (fichier->bloc[i] == NULL) {  // ?????? ?? ???? ???????
        printf("Error allocating memory for block %d\n", i);
        exit(EXIT_FAILURE);
    }
    fichier->bloc[i]->is_free = 1;       // ????? ?????? ??? "??"
    fichier->bloc[i]->next_bloc = -1;   // ????? ?????? ??????? ??? -1 (?? ????)


     // Allocation dynamique du tableau de blocs
  fichier->nb_enregistrements = MAX_DATA_SIZE ;
     if (fichier->bloc == NULL) {
         printf("Erreur d'allocation de memoire pour les blocs\n");
         exit(1);
     }

     // Initialisation des blocs
     int i ;
     for (  i = 0; i < fichier->nb_enregistrements ; i++) {
        fichier->bloc[i]->is_free = 1;
        fichier->bloc[i]->next_bloc = -1;
    }
 
 }

//insertion si le fichier est non trie
void insertion_enregNonTrie(FILE *f , FILE * fm){
Bloc buffer,buffer1;
rewind(f);
Enregistrement E;
int a1 = Lirecar(fm,2);// le nbr de bloc
int a2 = Lirecar(fm,3);// le nbr denregistrement
int a = Lirecar(fm,4);// ladress du 1er bloc
int DB = (a+a1)-1;// ladress du dernier bloc

printf("Veuillez donnez le ID de ce nouveau enregistrement \n :");
scanf("%d",&E.id);
printf("Veuillez donnez le ID de ce nouveau enregistrement \n :");
scanf("%[^\n]", E.data);
fseek(f,DB*sizeof(buffer),SEEK_SET);// le cureseur dans le dernier bloc
fread(&buffer,sizeof(buffer),1,f);// lire le continu

if(buffer.next_bloc<FACTEUR_DE_BLOCAGE){
    buffer.enreg[buffer.next_bloc].id = E.id;
    strcpy(buffer.enreg[buffer.next_bloc].data, E.data);//insere le nouveau enregistrement
    buffer.next_bloc++;
    fseek(f,sizeof(buffer),SEEK_CUR);// revient en arriere poue reecrire le bloc modifie
    fwrite(&buffer,sizeof(buffer),1,f);
    updateMeta(fm,3,a2+1);
}else{
// si le dernier bloc est plein on creer un nouveau
    buffer.enreg[0].id = E.id;
    strcpy(buffer.enreg[0].data,E.data);
    buffer.next_bloc = 1;
    fseek(f,(DB+1)*sizeof(buffer),SEEK_SET);
    fwrite(&buffer, sizeof(buffer),1,f);
    updateMeta(fm,3,a2+1);
    updateMeta(fm,2,a1+1);
}
}


 
// insertion dans un  fichier trie
void inserer_enregTrie(FILE *f, FILE *temp, Enregistrement nouvel_enr,int nb_enregistrements) {
    Enregistrement buffer[nb_enregistrements];
    int insere = 0;  // 0 si l'enregistrement nest pas insere 1 snn
    int n; // Nombre d'enregistrements lus

    rewind(f);
    rewind(temp);

    // Lire le fichier par blocs (taille du buffer)
    int i;
    while ((n = fread(buffer, sizeof(Enregistrement), nb_enregistrements, f)) > 0) {
        for ( i = 0; i < n; i++) {
            if (!insere && nouvel_enr.id < buffer[i].id) {// Insere le nouvel enregistrement s'il est plus petit que le prochain
                fwrite(&nouvel_enr, sizeof(Enregistrement), 1, temp);
                insere = 1;
            }
            fwrite(&buffer[i], sizeof(Enregistrement), 1, temp); // ecrire l'enregistrement actuel
           
        }
    }
  if (!insere) {// insertion a la fin
        fwrite(&nouvel_enr, sizeof(Enregistrement), 1, temp);
    }

    printf("Insertion reussie.\n");
}


 
 


void chargerEnregistrement_fichier(Fichier *fichier) {
    printf("Chargement des enregistrements dans le fichier :\n");
   int i ;
    for (i = 0; i < fichier->nb_enregistrements; i++) {
        if (fichier->organisation_globale == 0) {  // Organisation contigu�
           fichier->bloc[i]->is_free = 0;
            fichier->bloc[i]->enreg->id = i + 1;
            snprintf(fichier->bloc[i]->enreg->data, sizeof(fichier->bloc[i]->enreg->data),
                    "Data %d", i + 1);
        } else {  // Organisation cha�n�e
            fichier->bloc[i]->is_free = 0;
            fichier->bloc[i]->enreg->id = i + 1;
            snprintf(fichier->bloc[i]->enreg->data, sizeof(fichier->bloc[i]->enreg->data),
                    "Data %d", i + 1);
            fichier->bloc[i]->next_bloc = (i < fichier->nb_enregistrements - 1) ? i + 1 : -1;
        }
    }



// Fonction de recherche s�quentielle dans un fichier contigu non ordonn�
void RhercheEnregistrementContNonOrd(FILE *ms, FILE *f, int p[], int idRecherche) {
if (ms == NULL) {
        printf("Erreur : le fichier principal (ms) est invalide ou n'a pas pu �tre ouvert.\n");
        return;
    }

    if (f == NULL) {
        printf("Erreur : le fichier des m�tadonn�es (f) est invalide ou n'a pas pu �tre ouvert.\n");
        return;
    }

Bloc buffer; // Variable pour stocker un bloc
int nbBlocs; // Nombre total de blocs dans le fichier
// Lire le nombre total de blocs depuis le fichier des caract�ristiques
 fread(&nbBlocs, sizeof(int), 1, f);

int i = 0; // Num�ro du bloc (index� � partir de 0)

// Parcourir tous les blocs
while (i < nbBlocs) {

// Aller au bloc i dans le fichier gr�ce � fseek
 fseek(ms, i * sizeof(buffer), SEEK_SET);
 // Lire le bloc courant dans le buffer
fread(&buffer, sizeof(buffer), 1, ms);

 int j = 0; // Parcourir les enregistrements dans le bloc
while (j < buffer.next_bloc) {
if (buffer.enreg[j].id == idRecherche) { // Si l'ID correspond
p[0] = i + 1; // Num�ro du bloc (1-index�)
p[1] = j + 1; // Position dans le bloc (1-index�)
printf("Enregistrement trouv� ! Bloc : %d, Position : %d\n", p[0], p[1]);
        return;
      }
       j++;  
}
      i++; // Passer au bloc suivant
    }
// Si l'enregistrement n'a pas �t� trouv�
        printf("Enregistrement avec ID %d non trouv�.\n", idRecherche);
}


// Fonction de recherche dichotomique dans un fichier contigu ordonn�
void RechercheEnregistrementContOrd(FILE *f,bool Trouv, int *i, int *j, int c) {
    Bloc buffer; // Structure pour lire un bloc depuis le fichier
int bi, bs, mid, inf, sup, stop;

// Lecture des bornes du fichier (supposant que le fichier est ouvert)
   fseek(f, 0, SEEK_END);      // Aller � la fin du fichier
bs = ftell(f) / sizeof(buffer);  // D�terminer le nombre total de blocs (borne sup�rieure)
bi = 1;            // Premi�re borne (bloc initial)

    stop = 0;    // Variable de contr�le pour arr�ter la recherche
Trouv = 0;   // Initialiser le r�sultat � "non trouv�"
// Boucle pour la recherche externe (dichotomique sur les blocs)
    while ((bi <= bs) && (!stop) && (!(Trouv))) {
mid = (bi + bs) / 2; // Calculer le bloc m�dian

  // Lire le bloc m�dian dans le buffer
  fseek(f, (mid - 1) * sizeof(buffer), SEEK_SET);
  fread(&buffer, sizeof(buffer), 1, f);

       // V�rifier si la cl� se trouve dans ce bloc
  if (c >= buffer.enreg[0].id && c <= buffer.enreg[buffer.next_bloc - 1].id) {
// La cl� est dans ce bloc, effectuer une recherche interne
            inf = 0;
            sup = buffer.next_bloc - 1;

      while (inf <= sup && !(Trouv)) {
                *j = (inf + sup) / 2; // Calculer la position m�diane dans le bloc

               if (buffer.enreg[*j].id == c) {
                    Trouv = true; // La cl� est trouv�
           *i = mid;  // Sauvegarder le num�ro du bloc
} else if (buffer.enreg[*j].id > c) {
sup = *j - 1; // Continuer la recherche dans la partie gauche
} else {
inf = *j + 1; // Continuer la recherche dans la partie droite
}
}
stop = 1; // Arr�ter la recherche externe
} else if (c < buffer.enreg[0].id) {
// La cl� est plus petite que la plus petite cl� du bloc
bs = mid - 1; // Continuer dans les blocs de gauche
} else {
// La cl� est plus grande que la plus grande cl� du bloc
bi = mid + 1; // Continuer dans les blocs de droite
}
}

    // Si la cl� n'a pas �t� trouv�e, indiquer sa position th�orique
if (!(Trouv)) {
  *i = bi; // Bloc o� la cl� devrait se trouver
  *j = -1; // Cl� non trouv�e
}
}


// Fonction de recherche dans un fichier cha�n� non ordonn�
void RechercheEnregistrementChaineeNonOrd(FILE *f, bool *Trouv, int *i, int *j, int c) {
Bloc buffer; // Structure pour lire un bloc depuis le fichier
    Trouv = false;
    *i = 0; // Num�ro du bloc courant
    *j = -1; // Position de l'enregistrement
    // Lire le premier bloc
fseek(f, 0, SEEK_SET);
fread(&buffer, sizeof(buffer), 1, f);

  while (*i != -1 && !(Trouv)) {
        int k ;
// Parcourir les enregistrements dans le bloc courant
for ( k = 0; k < buffer.next_bloc; k++) {
          if (buffer.enreg[k].id == c) {
                Trouv = true;
*j = k + 1; // Position dans le bloc
return;
 }
}

        // Passer au bloc suivant
        *i = buffer.next_bloc;
        if (*i != -1) {
fseek(f, (*i - 1) * sizeof(buffer), SEEK_SET);
fread(&buffer, sizeof(buffer), 1, f);
}
    }
}


// Fonction de recherche dans un fichier cha�n� ordonn�
void RechercheEnregistrementChaineeOrd(FILE *f, bool *Trouv, int *i, int *j, int c) {
      Bloc buffer; // Structure pour lire un bloc depuis le fichier
 Trouv = false;
 *i = 0; // Num�ro du bloc courant
 *j = -1; // Position de l'enregistrement

     // Lire le premier bloc
fseek(f, 0, SEEK_SET);
fread(&buffer, sizeof(buffer), 1, f);

while (*i != -1 && !(Trouv)) {
           // Si la cl� est potentiellement dans ce bloc
    if (c >= buffer.enreg[0].id && c <= buffer.enreg[buffer.next_bloc - 1].id) {
           // Recherche interne (dichotomique dans le bloc)
          int inf = 0, sup = buffer.next_bloc - 1, mid;

          while (inf <= sup) {
  mid = (inf + sup) / 2;
  if (buffer.enreg[mid].id == c) {
  Trouv = true;
  *j = mid + 1; // Position dans le bloc
  return;
  } else if (buffer.enreg[mid].id > c) {
  sup = mid - 1;
  } else {
    inf = mid + 1;
}
}
// Si la cl� n'est pas dans ce bloc, arr�ter la recherche
return;
   }
       // Passer au bloc suivant
   *i = buffer.next_bloc;
         if (*i != -1) {
fseek(f, (*i - 1) * sizeof(buffer), SEEK_SET);
    fread(&buffer, sizeof(buffer), 1, f);
}
}
}


// Suppression logique d'un enregistrement
void suplogiqueEnregistremen(const char* F, int enrgAsupp) {
    FILE* file = fopen(F, "rb+" ); // Ouvre le fichier
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        return;
    }

    Enregistrement enreg;
    int found = 0;

    // Parcours des enregistrements
    while (fread(&enreg, sizeof(enreg), 1, file) == 1) {
        if (enreg.id == enrgAsupp && enreg.is_deleted == 0) {
            enreg.is_deleted = 1; // Marquer comme supprim�
            fseek(file, -sizeof(enreg), SEEK_CUR); // Revenir en arri�re
            fwrite(&enreg, sizeof(enreg), 1, file); // �crire l'enregistrement mis � jour
            found = 1;
            printf("Enregistrement avec ID %d marqu� comme supprim�.\n", enrgAsupp);
            break;
        }
    }

    if (!found) {
        printf("Enregistrement avec ID %d non trouv� ou d�j� supprim�.\n", enrgAsupp);
    }

    fclose(file); // Fermer le fichier
}


// Function to physically delete a record from a file
int supphy(const char *F, const char *enregistrement) {
    FILE *original, *temp;
    char line[MAX_LINE_LENGTH];
    char temp_filename[] = "temp.txt"; // Temporary file name

    // Open the original file for reading
    original = fopen(F, "r");
    if (original == NULL) {
        perror("Error opening the original file");
        return 1;
    }

    // Open the temporary file for writing
    temp = fopen(temp_filename, "w");
    if (temp == NULL) {
        perror("Error opening the temporary file");
        fclose(original);
        return 1;
    }

    // Read the file in chunks and process each line
    while (fread(line, sizeof(char), MAX_LINE_LENGTH, original) > 0) {
        // If the record is not found in the line, write it to the temporary file
        if (strstr(line, enregistrement) == NULL) {
            fputs(line, temp);
        }
    }

    // Close both files
    fclose(original);
    fclose(temp);

    // Delete the original file
    if (remove(F) != 0) {
        perror("Error deleting the original file");
        return 1;
    }

    // Rename the temporary file to replace the original
    if (rename(temp_filename, F) != 0) {
        perror("Error renaming the temporary file");
        return 1;
    }

    printf("Record \"%s\" successfully deleted.\n", enregistrement);
    return 0; // Indicate success
}


// Fonction de d�fragmentation
void defragment(Fichier *file) {
    int current_block = 0;  // Bloc actuel o� les enregistrements seront d�plac�s
    int current_position = 0;  // Position actuelle dans le bloc
   int i;
    for ( i = 0; i < fichier->bloc; i++) {
        Bloc *bloc = &fichier->bloc[i];
int j ;
        for ( j = 0; j <fichier->nb_enregistrements ; j++) {
            if (!bloc->enreg[j].is_deleted) {
                // Si l'enregistrement n'est pas supprim�, le d�placer
                bloc->enreg[current_position] = bloc->enreg[j];
                current_position++;

                // Si le bloc actuel est plein, passer au bloc suivant
                if (current_position >= fichier->nb_enregistrements) {
                    current_block++;
                    current_position = 0;
                }
            }
        }
    }

// Lib�rer l'espace inutilis� dans les blocs
   
    for ( i = current_block; i < fichier->block_count; i++) {
       fichier->bloc[i]->record_count = 0; // R�initialiser le nombre d'enregistrements
    }

    printf("D�fragmentation termin�e.\n");
}


// Fonction pour renommer un fichier
void renommerFichier(const char *ancienNom, const char *nouveauNom) {
    // V�rifier si les noms ne d�passent pas la taille maximale
    if (strlen(ancienNom) > MAX_FILENAME || strlen(nouveauNom) > MAX_FILENAME) {
        printf("Erreur : Le nom du fichier est trop long.\n");
        return;
    }

    // Utilisation de la fonction rename pour renommer le fichier
    if (rename(ancienNom, nouveauNom) == 0) {
        printf("Le fichier '%s' a �t� renomm� en '%s' avec succ�s.\n", ancienNom, nouveauNom);
    } else {
        perror("Erreur lors du renommage du fichier");
    }
}



// Fonction pour afficher le fichier (pour v�rifier l'�tat des enregistrements)
void print_file(Fichier *file) {
int i ;
    for ( i = 0; i <fichier->block_count; i++) {
        printf("Bloc %d:\n", i);
        int j ;
        for (j = 0; j < fichier->bloc[i]->record_count; j++) {
            printf("  ID: %d, Data: %s\n", fichier->bloc[i]->enreg[j].id, fichier->bloc[i]->enreg[j].data);
        }
    }
}



int Lirecar(FILE *f, int n1) {
   rewind(f);
   Meta car;
   if (fread(&car, sizeof(Meta), 1, f) != 1) {
       printf("Erreur de lecture du fichier.\n");
       return -1;  // Erreur de lecture
   }
   switch (n1) {
   case 1:
      printf("%s", car.NomDuFichier);  // Si NomDuFichier est une cha�ne
      break;
   case 2:
      return car.NbrBloc;
   case 3:
      return car.Nbren;
   case 4:
      return car.Adress;
   case 5:
      printf("%c", car.Modeglob);  // Assurez-vous que Modeglob est un caract�re
      break;
   case 6:
      printf("%c", car.Modeint);  // Assurez-vous que Modeint est un caract�re
      break;
   default:
      printf("Option invalide.\n");
      break;
   }
   return 0;
}
 void updateMeta(FILE *f, int n1, int s) {
   rewind(f);
   switch (n1) {
   case 2:
      fseek(f, sizeof(char) * 20, SEEK_SET);  // Supposons que NomDuFichier soit une cha�ne de 20 caract�res
      fwrite(&s, sizeof(int), 1, f);
      break;
   case 3:
      fseek(f, sizeof(char) * 20 + sizeof(int), SEEK_SET);  // NomDuFichier + NbrBloc
      fwrite(&s, sizeof(int), 1, f);
      break;
   case 4:
      fseek(f, sizeof(char) * 20 + sizeof(int) * 2, SEEK_SET);  // NomDuFichier + NbrBloc + Nbren
      fwrite(&s, sizeof(int), 1, f);
      break;
   default:
      printf("Option invalide.\n");
      break;
   }
}

 

// lire le contenu de le fichier
void LireFichier(const char *nomfich) {
    FILE *fichier;
    Bloc buffer;
    int i = 1; // Numero  du bloc

    fichier = fopen(nomfich, "rb");
    if (fichier == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", nomfich);
        return;
    }

    printf("\n--- Contenu du fichier %s ---\n", nomfich);


while (fread(&buffer, sizeof(Bloc), 1, fichier) == 1) {
        if (buffer. record_count > 0) {
            printf("Bloc %d :\n", i);
            int j ;
            for ( j = 0; j < buffer.record_count; j++) {
                printf("  Cle : %d, Donnees : %s\n", buffer.enreg[j].id, buffer.enreg[j].data);
            }
        } else {
            printf("Bloc %d vide.\n", i);
        }
        i++;
    }

    fclose(fichier); // Fermeture du fichier
}
// Fonction pour charger un fichier dans la m�moire secondaire
void ChargerFichierDansMemoire(char *nomFich, Bloc **memoire, int *nbBlocs) {
    FILE *F = fopen(nomFich, "rb");
    if (!F) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Lire le nombre total de blocs
    fseek(F, 0, SEEK_END);
    *nbBlocs = ftell(F) / sizeof(Bloc); // Calcul du nombre de blocs
    rewind(F);

// Allouer la m�moire secondaire pour tous les blocs
    *memoire = (Bloc *)malloc((*nbBlocs) * sizeof(Bloc));
    if (!(*memoire)) {
        perror("Erreur d'allocation de m�moire");
        fclose(F);
        exit(EXIT_FAILURE);
    }

    // Charger chaque bloc dans la m�moire secondaire
    int i ;
    for ( i = 0; i < *nbBlocs; i++) {
        fread(&((*memoire)[i]), sizeof(Bloc), 1, F);
    }

   
    printf("Fichier charg� dans la m�moire secondaire avec succ�s !\n");
    fclose(F);
}
int main() {
    // Initialize structures and memory
    initialiser_disque(1024, sizeof(Bloc));
    Fichier *fichier = (Fichier *)malloc(sizeof(Fichier));
    if (fichier == NULL) {
        printf("Erreur d'allocation de mémoire pour le fichier\n");
        return EXIT_FAILURE;
    }

    // Initialize allocation file
    allocationFile = fopen("allocation.dat", "wb+");
    if (allocationFile == NULL) {
        printf("Erreur lors de la création du fichier d'allocation\n");
        free(fichier);
        return EXIT_FAILURE;
    }

    // Initialize table allocation
    initialiser_table_allocation();

    int choix;
    char nouveauNom[MAX_FILENAME];
    int idRecherche;
    bool trouv;
    int i, j, c;
    int enrgAsupp;

    do {
        printf("\n=== Menu Principal ===\n");
        printf("1. Créer un nouveau fichier\n");
        printf("2. Afficher l'état de la mémoire secondaire\n");
        printf("3. Afficher les métadonnées des fichiers\n");
        printf("4. Rechercher un enregistrement par ID\n");
        printf("5. Insérer un nouvel enregistrement\n");
        printf("6. Supprimer un enregistrement\n");
        printf("7. Supprimer un fichier\n");
        printf("8. Renommer un fichier\n");
        printf("9. Quitter\n");
        printf("Choisissez une option : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: {
                create_file(fichier);
                mise_a_jour_table_allocation_apres_creation(fichier);
                miseAJourAllocationCompacte(allocationFile);
                char nom = Lirecar(fichier, 1);
                int nbrB = Lirecar(fichier, 2);
                ChargerFichierDansMemoire(&nom, &memoire, nbrB);
                compactage();
                miseAJourAllocationCompacte(allocationFile);
                break;
            }
            case 2:
                afficher_table_allocation();
                break;
            case 3: {
                printf("Les metadonnees des fichiers sont :\n");
                for (int i = 1; i <= 6; i++) {
                    Lirecar(fichier, i);
                }
                break;
            }
            case 4: {
                printf("Veuillez saisir l'ID de l'enregistrement à rechercher : ");
                scanf("%d", &idRecherche);

                if (fichier->organisation_globale == MODE_CONTIGUE) {
                    if (fichier->organisation_interne == 0) {
                        int p[2];
                        RhercheEnregistrementContNonOrd(fichier->bloc, *fichier, p, idRecherche);
                    } else {
                        trouv = false;
                        i = j = c = 0;
                        RechercheEnregistrementContOrd(fichier, trouv, &i, &j, c);
                    }
                } else {
                    if (fichier->organisation_interne == 0) {
                        trouv = false;
                        i = j = c = 0;
                        RechercheEnregistrementChaineeNonOrd(fichier, &trouv, &i, &j, c);
                    } else {
                        trouv = false;
                        i = j = c = 0;
                        RechercheEnregistrementChaineeOrd(fichier, &trouv, &i, &j, c);
                    }
                }
                break;
            }
            case 5: {
                gestionDespace((int *)disque, NBR_BLOCS, (Enregistrement){0}, sizeof(Bloc));
                Meta fm;
                if (fichier->organisation_interne == 0) {
                    insertion_enregNonTrie(fichier, &fm);
                } else {
                    Enregistrement nouvel_enr;
                    printf("Veuillez saisir l'ID de l'enregistrement : ");
                    scanf("%d", &nouvel_enr.id);
                    printf("Veuillez saisir les données de l'enregistrement : ");
                    scanf("%s", nouvel_enr.data);
                    Fichier temp;
                    inserer_enregTrie(fichier, &temp, nouvel_enr, fichier->nb_enregistrements);
                }
                miseAjourAllocation(fichier, 0, 1);
                updateMeta(fichier, 1, 0);
                LireFichier(fichier->nom);
                break;
            }
            case 6: {
                printf("Veuillez saisir l'ID de l'enregistrement à supprimer : ");
                scanf("%d", &enrgAsupp);
                suplogiqueEnregistremen(fichier->nom, enrgAsupp);
                supphy(fichier->nom, "");  // Empty string as we're using ID-based deletion
                defragment(fichier);
                miseAJourAllocation(allocationFile, 0, 0);
                updateMeta(fichier, 1, 0);
                LireFichier(fichier->nom);
                break;
            }
            case 7: {
                remove(fichier->nom);
                compactage();
                miseAJourAllocationCompacte(allocationFile);
                break;
            }
            case 8: {
                printf("Veuillez saisir le nouveau nom du fichier : ");
                scanf("%s", nouveauNom);
                renommerFichier(fichier->nom, nouveauNom);
                updateMeta(fichier, 1, 0);
                Lirecar(fichier, 1);
                break;
            }
            case 9: {
                vider_MS();
                choix = 10;  // Exit the loop
                break;
            }
            default:
                printf("Option invalide. Veuillez réessayer.\n");
        }
    } while (choix < 10);

    // Cleanup
    if (memoire != NULL) {
        free(memoire);
    }
    if (fichier != NULL) {
        // Free individual blocks
        for (int i = 0; i < fichier->nb_enregistrements; i++) {
            if (fichier->bloc[i] != NULL) {
                free(fichier->bloc[i]);
            }
        }
        free(fichier);
    }
    if (allocationFile != NULL) {
        fclose(allocationFile);
    }

    return EXIT_SUCCESS;
}