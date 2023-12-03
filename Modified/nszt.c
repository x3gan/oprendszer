#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

void Handler(int sign)
{

  if (sign == SIGUSR1)
  {
    printf("A gyerek folyamat várja a feldolgozandó szállítmányt!\n");
  }

  if (sign == SIGUSR2)
  {
    printf("A gyerek folyamat fogadta a szőlő szállítmányt, és elkezdte a feldolgozást!\n");
  }
}

#pragma region Defining structs
struct OneShipment
{
  char area[100];
  char name[100];
  int quantity;
  char type[100];
};
#pragma endregion

#pragma region Defining BASIC functions
FILE *createFile(struct OneShipment shipments[], int size);
void showAllData(FILE *file, struct OneShipment shipments[], int size);
void showAllAreaData(FILE *file, char area[100]);
void addNewOrder(FILE *file, struct OneShipment shipments[], int *length, int *size, char area[100], char name[100], int quantity, char type[100], int *feher, int *piros, int *kek);
void modifyData(FILE *file, struct OneShipment shipments[], int size, int *feher, int *piros, int *kek);
void deleteData(FILE *file, int *size, struct OneShipment shipments[], int *feher, int *piros, int *kek);
void processing(int feher, int piros, int kek, int pipefd[], int pipefd2[]);
#pragma endregion

int main()
{
  int lower = 5, upper = 7;
  srand(time(0));


  int randNum = (rand() % (upper - lower + 1)) + lower; // could use the rand.c one

  signal(SIGUSR1, Handler);
  signal(SIGUSR2, Handler);
  char sz[10000];

  int pipefd[2];
  int pipefd2[2];
  if (pipe(pipefd) == -1)
  {
    perror("Pipe hibás!");
    return 1;
  }

  if (pipe(pipefd2) == -1)
  {
    perror("Pipe hibás!");
    return 1;
  }

#pragma region BASIC
#pragma region Initializing data
  struct OneShipment shipments[100] = {
      {"Balatoni borvidék", "Termelő neve 1", 10, "fehér"},
      {"Egri borvidék", "Termelő neve 2", 20, "piros"},
      {"Tokaji borvidék", "Termelő neve 3", 30, "kék"}};

  int size = 3;
  int length = 3;

  int feher = 10; // ideal: 50 (for all 3)
  int piros = 20;
  int kek = 30;


  FILE *f = createFile(shipments, size);
#pragma endregion

  char but;

#pragma region Menu
  do
  {
    printf("1: Összes adat listázása.\n");
    printf("2: Adatok listázása borvidékek szerint.\n");
    printf("3: Új szállítmány hozzáadása.\n");
    printf("4: Rögzített adat módosítása.\n");
    printf("5: Adat törlése.\n");
    printf("6: Quit.\n");

    scanf(" %c", &but);
#pragma endregion

    switch (but)
    {
    case '1':
    {
#pragma region Case1
      showAllData(f, shipments, size);
      break;
#pragma endregion
    }
    case '2':
    {
#pragma region Case2
      char area[100];
      printf("Enter the area:");
      scanf(" %s", area);
      showAllAreaData(f, area);
      break;
#pragma endregion
    }
    case '3':
    {
#pragma region Case3
      printf("Area: ");
      int c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      char area[100];
      scanf("%[^\n]s", area);

      printf("Name: ");
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      char name[100];
      scanf("%[^\n]s", name);

      printf("Quantity: ");
      int quantity;
      scanf("%d", &quantity);

      printf("Type: ");
      while ((c = getchar()) != '\n' && c != EOF)
        ;
      char type[100];
      scanf("%[^\n]s", type);

      addNewOrder(f, shipments, &length, &size, area, name, quantity, type, &feher, &piros, &kek);

      processing(feher, piros, kek, pipefd, pipefd2);


      break;
#pragma endregion
    }
    case '4':
    {
      modifyData(f, shipments, size, &feher, &piros, &kek);
      processing(feher, piros, kek, pipefd, pipefd2);

      break;
    }
    case '5':
    {
      deleteData(f, &size, shipments, &feher, &piros, &kek);
      processing(feher, piros, kek, pipefd, pipefd2);

      break;
    }
    case '6':
    {
      return 0;
    }
    default:
      printf("Hibás bemenet!\n");
      break;
    }
    
  } while (1);

#pragma endregion

  // free(shipments);

  return 0;
}

#pragma region BASIC PROCESSES
FILE *createFile(struct OneShipment shipments[], int size)
{
  FILE *f = fopen("szolo.dat", "w");
  for (int i = 0; i < size; i++)
  {
    fwrite(&shipments[i], sizeof(struct OneShipment), 1, f);
  }
  fclose(f);
  return f;
}

void showAllData(FILE *file, struct OneShipment shipments[], int size)
{
  struct OneShipment shipment;
  file = fopen("szolo.dat", "r");
  printf("Size: %d\n", size);
  for (int i = 0; i < size; i++)
  {
    fread(&shipment, sizeof(struct OneShipment), 1, file);
    int lineNum = i + 1;
    printf("%d. %s, %s, %d, %s\n", lineNum, shipment.area, shipment.name, shipment.quantity, shipment.type);
  }
  fclose(file);
}

void showAllAreaData(FILE *file, char area[100])
{

  area[strcspn(area, "\n")] = 0;

  struct OneShipment shipment;

  FILE *g = fopen("szolo.dat", "r");
  while (fread(&shipment, sizeof(struct OneShipment), 1, g))
  {
    char shipmentArea[50];
    strcpy(shipmentArea, shipment.area);
    shipmentArea[strcspn(shipmentArea, " ")] = 0;

    if (strcmp(shipmentArea, area) == 0)
    {
      printf("%s, %s, %d, %s\n", shipment.area, shipment.name, shipment.quantity, shipment.type);
    }
  }
  fclose(g);
}

void addNewOrder(FILE *file, struct OneShipment shipments[], int *length, int *size, char area[100], char name[100], int quantity, char type[100], int *feher, int *piros, int *kek)
{
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
  if (file == NULL)
  {
    perror("Error at opening the file.\n");
    exit(1);
  }

  fwrite(&addNewShipment, sizeof(struct OneShipment), 1, file);

  fclose(file);

  (*length)++;
  shipments[*size] = addNewShipment;
  (*size)++;
}

void modifyData(FILE *file, struct OneShipment shipments[], int size, int *feher, int *piros, int *kek)
{
  file = fopen("szolo.dat", "r+");
  if (file == NULL)
  {
    perror("Error at opening the file.\n");
    exit(1);
  }

  fread(shipments, sizeof(struct OneShipment), size, file);

  int lineNum;
  printf("Enter the line number to modify: ");
  scanf("%d", &lineNum);

  if (strcmp(shipments[lineNum - 1].type, "fehér") == 0)
  {
    (*feher) -= shipments[lineNum - 1].quantity;
    printf("Fehér: %d\n", *feher);
  }
  else if (strcmp(shipments[lineNum - 1].type, "piros") == 0)
  {
    (*piros) -= shipments[lineNum - 1].quantity;
  }
  else if (strcmp(shipments[lineNum - 1].type, "kék") == 0)
  {
    (*kek) -= shipments[lineNum - 1].quantity;
  }

  printf("Area: ");
  scanf("%s", shipments[lineNum - 1].area);
  printf("Name: ");
  scanf("%s", shipments[lineNum - 1].name);
  printf("Quantity: ");
  scanf("%d", &shipments[lineNum - 1].quantity);
  printf("Type: ");
  scanf("%s", shipments[lineNum - 1].type);
  
  if (strcmp(shipments[lineNum - 1].type, "fehér") == 0)
  {
    (*feher) += shipments[lineNum - 1].quantity;
    printf("Fehér: %d\n", *feher);
  }
  else if (strcmp(shipments[lineNum - 1].type, "piros") == 0)
  {
    (*piros) += shipments[lineNum - 1].quantity;
  }
  else if (strcmp(shipments[lineNum - 1].type, "kék") == 0)
  {
    (*kek) += shipments[lineNum - 1].quantity;
  }

  fseek(file, 0, SEEK_SET);
  fwrite(shipments, sizeof(struct OneShipment), size, file);

  fclose(file);
}

void deleteData(FILE *file, int *size, struct OneShipment shipments[], int *feher, int *piros, int *kek)
{
  file = fopen("szolo.dat", "r+");
  if (file == NULL)
  {
    perror("Error at opening the file.\n");
    exit(1);
  }

  printf("Size: %d\n", *size);
  fseek(file, 0, SEEK_SET);
  fread(shipments, sizeof(struct OneShipment), *size, file);

  int lineNum;
  printf("Enter the line number to delete: ");
  scanf("%d", &lineNum);
  if (strcmp(shipments[lineNum - 1].type, "fehér") == 0)
  {
    (*feher) -= shipments[lineNum - 1].quantity;
  }
  else if (strcmp(shipments[lineNum - 1].type, "piros") == 0)
  {
    (*piros) -= shipments[lineNum - 1].quantity;
  }
  else if (strcmp(shipments[lineNum - 1].type, "kék") == 0)
  {
    (*kek) -= shipments[lineNum - 1].quantity;
  }
  

  struct OneShipment *newShipments = malloc((*size - 1) * sizeof(struct OneShipment));
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
#pragma endregion

void processing(int feher, int piros, int kek, int pipefd[], int pipefd2[])
{
  char buffer[100];
  int tipusok[] = {feher, piros, kek};


  for (int i = 0; i < 3; i++)
  {
    if (tipusok[i] >= 50)
    {
      struct CountShipment
      {
        char countType[50];
        int countQuantity;
      };

      pid_t child = fork();

      if (child < 0)
      {
        perror("Fork hiba!");
        exit(EXIT_FAILURE);
      }
      else if (child == 0)
      {
        kill(getppid(), SIGUSR1);
        struct CountShipment recievedShipment;
        read(pipefd[0], &recievedShipment, sizeof(recievedShipment));

        kill(getppid(), SIGUSR2);

        sleep(5);
        printf("A %s típusú szőlőt a gyerek folyamat megvizsgálta és a mennyisége várhatóan: %d liter lesz.\n", recievedShipment.countType, recievedShipment.countQuantity);

        if (write(pipefd2[1], buffer, sizeof(buffer)) == -1)
        {
          perror("Hiba történt íráskor!\n");
          exit(EXIT_FAILURE);
          free(buffer);
        }
        exit(EXIT_SUCCESS);
      }
      else
      {
        pause();

        struct CountShipment countShipment;
        switch (i)
        {
        case 0:
          strcpy(countShipment.countType, "fehér");
          countShipment.countQuantity = feher;
          break;
        case 1:
          strcpy(countShipment.countType, "piros");
          countShipment.countQuantity = piros;
          break;
        case 2:
          strcpy(countShipment.countType, "kék");
          countShipment.countQuantity = kek;
          break;

        default:
          break;
        }

        write(pipefd[1], &countShipment, sizeof(countShipment));
        wait(NULL);
      }
    }
  }
}
