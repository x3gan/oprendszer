#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

void handler(int sign)
{
  if (sign == SIGUSR1){printf("Várom a feldolgozandó szállítmányt!\n");}

  if (sign == SIGUSR2){printf("A feldolgozást megkezdtem...\n");}
}

struct OneShipment
{
  char area[100];
  char name[100];
  int quantity;
  char type[100];
};

FILE *createFile(struct OneShipment shipments[], int size);
void showAllData(FILE *file, struct OneShipment shipments[], int size);
void showAllAreaData(FILE *file, char area[100]);
void addNewOrder(FILE *file, struct OneShipment shipments[], int *length, int *size, char area[100], char name[100], int quantity, char type[100], int *feher, int *piros, int *kek);
void modifyData(FILE *file, struct OneShipment shipments[], int size, int *feher, int *piros, int *kek);
void deleteData(FILE *file, int *size, struct OneShipment shipments[], int *feher, int *piros, int *kek);
void processing(int feher, int piros, int kek, int pipefd[]);

int main()
{
  srand(time(NULL));
  

  signal(SIGUSR1, handler);
  signal(SIGUSR2, handler);
  char sz[10000];

  int pipefd[2];
  pipe(pipefd);

  struct OneShipment shipments[100] = {
      {"Balatoni borvidék", "Villányi Andrea", 10, "fehér"},
      {"Egri borvidék", "Dózsa Gábor", 20, "piros"},
      {"Tokaji borvidék", "Ivány Sándor", 30, "kék"}};

  int size = 3;
  int length = 3;

  int feher = 10;
  int piros = 20;
  int kek = 30;

  FILE *f = createFile(shipments, size);

  char but;

  do
  {

    printf("1: Összes adat listázása.\n");
    printf("2: Adatok listázása borvidékek szerint.\n");
    printf("3: Új szállítmány hozzáadása.\n");
    printf("4: Rögzített adat módosítása.\n");
    printf("5: Adat törlése.\n");
    printf("6: Quit.\n");

    scanf(" %c", &but);
    

    switch (but)
    {
    case '1':
    {
      printf("\n");
      showAllData(f, shipments, size);
      printf("\n");
      break;
    }
    case '2':
    {
      printf("\n");
      char area[100];
      printf("Add meg melyik borvidék szerint :");
      scanf(" %s", area);
      showAllAreaData(f, area);
      printf("\n");

      break;
    }
    case '3':
    {
      printf("\n");
      printf("Borvidék: ");
      int c;
      while ((c = getchar()) != '\n' && c != EOF);

      char area[100];
      scanf("%[^\n]s", area);

      printf("Termelő neve: ");
      while ((c = getchar()) != '\n' && c != EOF);

      char name[100];
      scanf("%[^\n]s", name);

      printf("Mennyiség: ");
      int quantity;
      scanf("%d", &quantity);

      printf("Fajta: ");
      while ((c = getchar()) != '\n' && c != EOF);

      char type[100];
      scanf("%[^\n]s", type);

      addNewOrder(f, shipments, &length, &size, area, name, quantity, type, &feher, &piros, &kek);

      processing(feher, piros, kek, pipefd);
      printf("\n");


      break;
    }
    case '4':
    {
      printf("\n");
      modifyData(f, shipments, size, &feher, &piros, &kek);
      processing(feher, piros, kek, pipefd);
      printf("\n");

      break;
    }
    case '5':
    {
      printf("\n");
      deleteData(f, &size, shipments, &feher, &piros, &kek);
      processing(feher, piros, kek, pipefd);
      printf("\n");

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

  return 0;
}

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
  }
  else if (strcmp(addNewShipment.type, "piros") == 0)
  {
    (*piros) += addNewShipment.quantity;
  }
  else if (strcmp(addNewShipment.type, "kék") == 0)
  {
    (*kek) += addNewShipment.quantity;
  }

  file = fopen("szolo.dat", "a");
  if (file == NULL)
  {
    perror("Hiba a fájl megnyitásában!.\n");
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
    perror("Hiba a fájl megnyitásában!\n");
    exit(1);
  }

  fread(shipments, sizeof(struct OneShipment), size, file);

  int lineNum;
  printf("Add meg a módosítani kíván sor sorszámát: ");
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

  printf("Borvidék: ");
  scanf("%s", shipments[lineNum - 1].area);
  printf("Termelő neve: ");
  scanf("%s", shipments[lineNum - 1].name);
  printf("Mennyiség: ");
  scanf("%d", &shipments[lineNum - 1].quantity);
  printf("Fajta: ");
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
    perror("Hiba a fájl megnyitásában!\n");
    exit(1);
  }

  fseek(file, 0, SEEK_SET);
  fread(shipments, sizeof(struct OneShipment), *size, file);

  int lineNum;
  printf("Add meg a törölni kívánt sor sorszámát: ");
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

  fseek(file, 0, SEEK_SET);
  fwrite(newShipments, sizeof(struct OneShipment), *size, file);

  fclose(file);
  free(newShipments);
}

void processing(int feher, int piros, int kek, int pipefd[])
{
  double l = 0.6, u = 0.8;

  double randNum = rand() / (double)(RAND_MAX) * (u - l) + l;

  char buffer[100];
  int tipusok[] = {feher, piros, kek};

  struct CountShipment
      {
        char countType[50];
        int countQuantity;
      };

struct DoneShipment
  {
    char doneType[50];
    double doneQuantity;
  };

  for (int i = 0; i < 3; i++)
  {
    if (tipusok[i] >= 50)
    {

      pid_t child = fork();

      if (child < 0)
      {
        perror("Fork hiba!");
        exit(EXIT_FAILURE);
      }
      else if (child == 0)
      {
        struct CountShipment recievedShipment;
        struct DoneShipment doneShipment;

        kill(getppid(), SIGUSR1);
        read(pipefd[0], &recievedShipment, sizeof(recievedShipment));

        kill(getppid(), SIGUSR2);
        strcpy(doneShipment.doneType, recievedShipment.countType);
        doneShipment.doneQuantity = recievedShipment.countQuantity * randNum;  

        sleep(5);
        write(pipefd[1], &doneShipment, sizeof(doneShipment));
        
        
        exit(EXIT_SUCCESS);
      }
      else
      {
        pause();

        struct CountShipment countShipment;
        struct DoneShipment recievedDoneShipment;
        
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
        pause();
        read(pipefd[0], &recievedDoneShipment, sizeof(recievedDoneShipment));
        printf("A %s típusú szőlőből vizsgálat után várhatóan: %.2f liter bor lesz.\n", recievedDoneShipment.doneType, recievedDoneShipment.doneQuantity);

        wait(NULL);

      }
    }
  }
}
