/*
A "Nemzeti Szőlő Társaság" kft (NSZT) szeretné segíteni a minőségi szőlő termesztést szerte az országban. A nevezetes szőlőtermelő vidékek gazdáit segítve átveszik a felesleges minőségi szőlőket méltányos áron. 

 A szüreti időszak kellős közepén járunk, a termelők látják, hogy mennyi termést tudnak betakarítani. Ennek ismeretében minden gazda látja, hogy a saját igényén túl mekkora felesleges szőlőmennyiséget tudott betakarítani. Ezt a felesleget átadja az NSZT részére.

Az adatokat a szolo.dat fájlban tároljuk, az alkalmazás indulásakor inicializáljuk ezt az adatfájlt, néhány konstans adatot (forrásszövegből) írjunk be a fájlba. Ezután a program adjon lehetőséget az adatok listázására (teljes vagy szőlőtermelő vidékek szerint), 
új szőlő szállítmány fogadására (egy gazda többször is szállíthat), egy rögzített adat módosítására, vagy miután a belső vizsgálat kideríti, hogy egy szállítmány nem minőségi, annak törlésére. A szőlő vidékeket fix adatként kezelheti.(Balatoni borvidék, Egri borvidék, Tokaji borvidék, stb) 
Egy szállítmányról az alábbi adatokat tároljuk: Melyik vidékről származik a szőlő, termelő neve, átvett mennyiség, szőlő fajta. 

Az NSZT mint szülő folyamat kezeli az átvételeket. Ha összegyűlik egy adott ideális mennyiség a szőlőtípusokból(szőlő fajtákat nem keverünk, NSZT mondja meg mennyi az ideális mennyiség, hány kg), akkor szülő folyamat elindítja a szőlő feldolgozást.(gyerek folyamat) A feldolgozó ha készen áll, 
jelzést küld vissza NSZT-hez, hogy várja a feldolgozandó szőlőt. NSZT a jelzés fogadása után csövön átküldi a feldolgozó folyamatnak, hogy milyen típusú szőlőt, és mekkora mennyiséget küldött. Gyerekfolyamat válaszában jelzést küld vissza, hogy fogadta a szőlő szállítmányt, 
és elkezdte a borkészítést. Ezután 5 és 10 másodperc közötti várakozás után, csövön visszaírja, hogy a szőlőt megvizsgálta és a minősége alapján kilogrammonként 0.6 és 0.8 liter(véletlenszerűen) közötti bor várható, így a kapott mennyiségből a kész termék mennyi lesz.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>


double random_int(double min, double max)
{
    return min + (double)rand() / (double)RAND_MAX * (max - min);
}

void signalHandler(int signum) {
   printf("Signal received: %d\n", signum);
}

#pragma region Defining structs
struct OneShipment{
  char area[100];
  char name[100];
  int quantity;
  char type[100];
};
#pragma endregion

#pragma region Defining functions
FILE* createFile(struct OneShipment shipments[], int size);
void showAllData(FILE* file, struct OneShipment shipments[], int size);
void showAllAreaData(FILE* file, char area[100]);
void addNewOrder(FILE* file, struct OneShipment shipments[], int* length, int* size, char area[100], char name[100], int quantity, char type[100], int* feher, int* piros, int* kek);
void modifyData(FILE* file, struct OneShipment shipments[], int size);
void deleteData(FILE* file, int* size, struct OneShipment shipments[]);
void processing(struct OneShipment shipments[], int length, int* size);
#pragma endregion

int main() 
{ 
  #pragma region Initializing data
  struct OneShipment* shipments = malloc(3 * sizeof(struct OneShipment));
  shipments[0] = (struct OneShipment){"Balatoni borvidék", "Termelő neve 1", 100, "fehér"};
  shipments[1] = (struct OneShipment){"Egri borvidék", "Termelő neve 2", 200, "piros"};
  shipments[2] = (struct OneShipment){"Tokaji borvidék", "Termelő neve 3", 300, "kék"};  
  
  int size = 3;
  int length = 3;

  int feher = 10; //ideal: 50 (for all 3)
  int piros = 20;
  int kek = 30;

  FILE* f = createFile(shipments, size);
  #pragma endregion

  char but;

    #pragma region Menu
    do {
        printf("1: Összes adat listázása.\n");
        printf("2: Adatok listázása borvidékek szerint.\n");
        printf("3: Új szállítmány hozzáadása.\n");
        printf("4: Rögzített adat módosítása.\n");
        printf("5: Adat törlése.\n");
        printf("6: Quit.\n");

        scanf(" %c", &but);
    #pragma endregion

        switch (but) {
            case '1': {
              #pragma region Case1
                showAllData(f, shipments, size);
                break;
              #pragma endregion
            }
            case '2': {
              #pragma region Case2
                char area[100];
                printf("Enter the area:");
                scanf(" %s", area);
                showAllAreaData(f, area);
                break;
              #pragma endregion
            }
            case '3': {
              #pragma region Case3
                printf("Area: ");
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                char area[100];
                scanf("%[^\n]s", area);

                printf("Name: ");
                while ((c = getchar()) != '\n' && c != EOF);
                char name[100];
                scanf("%[^\n]s", name);

                printf("Quantity: ");
                int quantity;
                scanf("%d", &quantity);

                printf("Type: ");
                while ((c = getchar()) != '\n' && c != EOF);
                char type[100];
                scanf("%[^\n]s", type);

                addNewOrder(f, shipments, &length, &size, area, name, quantity, type, &feher, &piros, &kek);
                //processing(shipments, *length, size);
                break;
              #pragma endregion
            }
            case '4': {
                modifyData(f, shipments, size);
                break;
            }
            case '5':{
                deleteData(f, &size, shipments);
                break;
            }
            case '6': {
                free(shipments);
                return 0;
            }
            default:
                printf("Hibás bemenet!\n");
                break;
            }
    } while (1);

  free(shipments);
  return 0;
}


FILE* createFile(struct OneShipment shipments[], int size){
  FILE *f = fopen("szolo.dat", "w");
  for(int i = 0; i < size; i++) {
    fwrite(&shipments[i], sizeof(struct OneShipment), 1, f);
  }
  fclose(f);
  return f;
}

void showAllData(FILE* file, struct OneShipment shipments[], int size){
  struct OneShipment shipment;
  file = fopen("szolo.dat", "r");
  printf("Size: %d\n", size);
  for(int i = 0; i < size; i++) {
    fread(&shipment, sizeof(struct OneShipment), 1, file);
    int lineNum = i + 1;
    printf("%d. %s, %s, %d, %s\n", lineNum, shipment.area, shipment.name, shipment.quantity, shipment.type);
  }
  fclose(file);
}

void showAllAreaData(FILE* file, char area[100]){

  area[strcspn(area, "\n")] = 0;

  struct OneShipment shipment;

  FILE *g = fopen("szolo.dat", "r");
  while(fread(&shipment, sizeof(struct OneShipment), 1, g)) {
    char shipmentArea[50];  
    strcpy(shipmentArea, shipment.area);
    shipmentArea[strcspn(shipmentArea, " ")] = 0;

    if(strcmp(shipmentArea, area) == 0) {
        printf("%s, %s, %d, %s\n", shipment.area, shipment.name, shipment.quantity, shipment.type);
    }
  }
  fclose(g);

}

void addNewOrder(FILE* file, struct OneShipment shipments[], int* length, int* size, char area[100], char name[100], int quantity, char type[100], int* feher, int* piros, int* kek){
  struct OneShipment addNewShipment;
  area[strcspn(area, "\n")] = 0;

  strcpy(addNewShipment.area, area);
  strcpy(addNewShipment.name, name);
  addNewShipment.quantity = quantity;
  strcpy(addNewShipment.type, type);
  if (strcmp(addNewShipment.type, "fehér") == 0)
  {
    (*feher) += addNewShipment.quantity;
    printf("Fehér: %d\n", *feher);
  }
  else if (strcmp(addNewShipment.type, "piros") == 0)
  {
    (*piros) += addNewShipment.quantity;
    printf("Piros: %d\n", *piros);
  }
  else if (strcmp(addNewShipment.type, "kék") == 0)
  {
    (*kek) += addNewShipment.quantity;
    printf("Kék: %d\n", *kek);
  }

  file = fopen("szolo.dat", "a");
  if (file == NULL) {
    perror("Error at opening the file.\n");
    exit(1);
  }

  fwrite(&addNewShipment, sizeof(struct OneShipment), 1, file);

  fclose(file);

  (*length)++;
  shipments[*size] = addNewShipment;
 (*size)++;

}

void modifyData(FILE* file, struct OneShipment shipments[], int size){
  file = fopen("szolo.dat", "r+");
  if (file == NULL) {
    perror("Error at opening the file.\n");
    exit(1);
  }
  
  fread(shipments, sizeof(struct OneShipment), size, file);

  int lineNum;
  printf("Enter the line number to modify: ");
  scanf("%d", &lineNum);

  printf("Area: ");
  scanf("%s", shipments[lineNum - 1].area);
  printf("Name: ");
  scanf("%s", shipments[lineNum - 1].name);
  printf("Quantity: ");
  scanf("%d", &shipments[lineNum - 1].quantity);
  printf("Type: ");
  scanf("%s", shipments[lineNum - 1].type);

  fseek(file, 0, SEEK_SET);
  fwrite(shipments, sizeof(struct OneShipment), size, file);
  
  fclose(file);
}

void deleteData(FILE* file, int* size, struct OneShipment shipments[]){
  file = fopen("szolo.dat", "r+");
  if (file == NULL) { perror("Error at opening the file.\n"); exit(1);}

  printf("Size: %d\n", *size);
  fseek(file, 0, SEEK_SET);
  fread(shipments, sizeof(struct OneShipment), *size, file);

  int lineNum;
  printf("Enter the line number to delete: ");
  scanf("%d", &lineNum);

  struct OneShipment* newShipments = malloc((*size - 1) * sizeof(struct OneShipment));
  int c = 0;
  for (int i = 0; i < *size; i++)
  {
    if (i != lineNum - 1)
    {
      newShipments[c] = shipments[i];
      c++;
    }
  }

  (*size)--;
  printf("Size: %d\n", *size);
  fseek(file, 0, SEEK_SET);
  fwrite(newShipments, sizeof(struct OneShipment), *size, file);

  fclose(file);
  free(newShipments);
  
}

/*
void processing(struct OneShipment shipments[], int length, int* size){
  signal(SIGUSR1, signalHandler);
  
  int totalQuantity[50] = {0};

   for(int i = 0; i < length; i++) {
       totalQuantity[i] += shipments[i].quantity;

       if(totalQuantity[i] >= 150) {
           pid_t child = fork();
           if(child == 0) {
               childProcess(shipments, i);
               exit(0);
           }
       }
   }
}
*/

/*
void childProcess(struct OneShipment *shipments, int index) {
   printf("Child process started for wine type: %s\n", shipments[index].type);
   sleep(3); // Simulate processing time
   printf("Child process ended for wine type: %s\n", shipments[index].type);
}
*/
