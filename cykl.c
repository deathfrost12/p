#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define VSTUP "cyklistika.txt"
#define VYSTUP "vysledkova_listina.txt"
#define BUFSIZE 200
#define DELIM ";:"

typedef struct CAS
{
    int hodiny;
    int minuty;
    float sekundy;
} CAS;

typedef struct ZAVODNIK
{
    int cislo;
    char prijmeni[20];
    char jmeno[20];
    char narodnost[5];
    int rocnik;
    char klub[35];
    struct CAS cas;
} ZAVODNIK;

int casNaSek(int hodiny, int minuty, int sekundy)
{
    return hodiny * 3600 + minuty * 60 + sekundy;
}

void sekNaCas(int sekundy, char *buf)
{
    sprintf(buf, "%02d:%02d:%02d", sekundy / 3600, (sekundy - (sekundy / 3600) * 3600) / 60, sekundy % 60);
}

void swap(ZAVODNIK *x, ZAVODNIK *y)
{
    ZAVODNIK pom = *x;
    *x = *y;
    *y = pom;
}

void bblSort(ZAVODNIK *zav, int pocet)
{
    for (int i = 0; i < pocet; i++)
    {
        for (int j = 0; j < pocet - i - 1; j++)
        {
            if (casNaSek(zav[j].cas.hodiny, zav[j].cas.minuty, zav[j].cas.sekundy) > casNaSek(zav[j + 1].cas.hodiny, zav[j + 1].cas.minuty, zav[j + 1].cas.sekundy))
            {
                swap(&zav[j], &zav[j + 1]);
            }
        }
    }
}

int prectiSoubor(ZAVODNIK *zav)
{
    FILE *fr;
    char buffer[BUFSIZE];
    char *data;
    int pocet = 0;
    int sloupec;

    if ((fr = fopen(VSTUP, "r")) == NULL)
    {
        printf("Soubor %s se nepodarilo otevrit.", VSTUP);
        return EXIT_FAILURE;
    }

    while (fgets(buffer, BUFSIZE, fr) != NULL)
    {
        if (pocet != 0)
        {
            data = strtok(buffer, DELIM);
            sloupec = 0;
            while (data != NULL)
            {
                sloupec++;
                switch (sloupec)
                {
                case 1:
                    zav[pocet - 1].cislo = atoi(data);
                    break;
                case 2:
                    strcpy(zav[pocet - 1].prijmeni, data);
                    break;
                case 3:
                    strcpy(zav[pocet - 1].jmeno, data);
                    break;
                case 4:
                    strcpy(zav[pocet - 1].narodnost, data);
                    break;
                case 5:
                    zav[pocet - 1].rocnik = atoi(data);
                    break;
                case 6:
                    strcpy(zav[pocet - 1].klub, data);
                    break;
                case 7:
                    zav[pocet - 1].cas.hodiny = atoi(data);
                    break;
                case 8:
                    zav[pocet - 1].cas.minuty = atoi(data);
                    break;
                case 9:
                    zav[pocet - 1].cas.sekundy = atof(data);
                    break;
                }
                data = strtok(NULL, DELIM);
            }
        }
        pocet++;
    }
    if (fclose(fr) == EOF)
    {
        printf("Soubor %s se nepodarilo zavrit.", VSTUP);
    }
    return (pocet - 1);
}

void vypisObr(ZAVODNIK *zav, int pocet)
{
    int cesi;
    int slovaci;
    int nejstarsi = 3000;
    char nejstarsijmeno[25];

    time_t sekundy = time(NULL);
    struct tm *current_time = localtime(&sekundy);

    for (int i = 0; i < pocet; ++i)
    {
        if (nejstarsi > zav[i].rocnik)
        {
            nejstarsi = zav[i].rocnik;
            strcpy(nejstarsijmeno, zav[i].prijmeni);
        }

        if (!strcmp(zav[i].narodnost, "CZE"))
        {
            cesi++;
        }

        if (!strcmp(zav[i].narodnost, "SVK"))
        {
            slovaci++;
        }
    }

    printf("S T A R T O V N I  L I S T I N A  -  O B R  D R A S A L\n");
    printf("----------------------------------------------------------------------------------------------------------------\n");
    printf(" startovni cislo|      prijmeni|      jmeno|  narodnost|  rocnik|                             klub|         cas|\n");
    printf("----------------------------------------------------------------------------------------------------------------\n");

    for (int j = 0; j < pocet; j++)
    {
        printf("%16d|%14s|%11s|%11s|%8d|%33s| %02d:%02d:%1.2f%s|\n",
               zav[j].cislo,
               zav[j].prijmeni,
               zav[j].jmeno,
               zav[j].narodnost,
               zav[j].rocnik,
               zav[j].klub,
               zav[j].cas.hodiny,
               zav[j].cas.minuty,
               zav[j].cas.sekundy,
               zav[j].cas.sekundy < 10 ? " " : "");
    }
    printf("________________________________________________________________________________________________________________\n");
    printf("\nNa startu je %d zavodniku.\n", pocet);
    printf("Nejstarsim zavodnikem je %s narozen v roce %d. je mu %d let.\n", nejstarsijmeno, nejstarsi, (current_time->tm_year + 1900) - nejstarsi);
    printf("Zavodnici dle narodnosti:\n");
    printf("   Cesi - %d\nSlovaci - %d\n   Jine - %d\n\n", cesi, slovaci, pocet - (cesi + slovaci));
    printf("Byl vytvoren soubor %s\n", VYSTUP);
}

void vypisSoubor(ZAVODNIK *zav, int pocet)
{
    char celkem[10];
    char ztrata[10];
    int prvset;
    FILE *fw;
    if ((fw = fopen(VYSTUP, "w")) == NULL)
    {
        printf("Soubor %s nebylo mozne otevrit.\n", VYSTUP);
    }

    fprintf(fw, "V Y S L E D K O V A  L I S T I N A  -  O B R  D R A S A L\n");
    fprintf(fw, "--------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(fw, "poradi|startovni cislo|      prijmeni|      jmeno|  narodnost|  rocnik|                             klub|         cas|   ztrata|\n");
    fprintf(fw, "--------------------------------------------------------------------------------------------------------------------------------\n");
    prvset = casNaSek(zav[0].cas.hodiny, zav[0].cas.minuty, zav[0].cas.sekundy);

    for (int i = 0; i < pocet; ++i)
    {
        sekNaCas(casNaSek(zav[i].cas.hodiny, zav[i].cas.minuty, zav[i].cas.sekundy) - prvset, ztrata);
        fprintf(fw, "%6d|%15d|%14s|%11s|%11s|%8d|%33s| %02d:%02d:%0.2f%s|%s%s|\n",
                i + 1,
                zav[i].cislo,
                zav[i].prijmeni,
                zav[i].jmeno,
                zav[i].narodnost,
                zav[i].rocnik,
                zav[i].klub,
                zav[i].cas.hodiny,
                zav[i].cas.minuty,
                zav[i].cas.sekundy,
                zav[i].cas.sekundy > 10 ? "" : " ",
                i == 0 ? " " : "+",
                i == 0 ? "        " : ztrata);
    }

    if (fclose(fw) == EOF)
    {
        printf("Soubor %s nebylo mozne zavrit.\n", VYSTUP);
    }
}

int main()
{
    ZAVODNIK zav[BUFSIZE];

    int pocet = 0;

    pocet = prectiSoubor(zav);
    vypisObr(zav, pocet);
    bblSort(zav, pocet);
    vypisSoubor(zav, pocet);

    return EXIT_SUCCESS;
}