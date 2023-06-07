#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VSTUP "startovni_listina_kombinace_zeny.txt"
#define VYSTUP "vysledkova_listina_kombinace_zeny.txt"
#define BUFSIZE 200
#define DELIM " :,"

typedef struct CAS
{
    int minuty;
    int sekundy;
    int setiny;
} CAS;

typedef struct ZAVODNICE
{
    int startCis;
    char jmeno[20];
    char prijmeni[20];
    char narodnost[4];
    struct CAS sjezdy;
    struct CAS slalom;
} ZAVODNICE;

void setNaCas(int set, char *buf)
{
    sprintf(buf, "%d:%02d,%02d", set / 6000, (set - (set / 6000) * 6000) / 100, set % 100);
}

int casNaSet(int min, int sec, int set)
{
    return min * 6000 + sec * 100 + set;
}

int pocetCas(ZAVODNICE *zav, int i)
{
    return casNaSet(zav[i].sjezdy.minuty, zav[i].sjezdy.sekundy, zav[i].sjezdy.setiny) + casNaSet(zav[i].slalom.minuty, zav[i].slalom.sekundy, zav[i].slalom.setiny);
}

void swap(ZAVODNICE *x, ZAVODNICE *y)
{
    ZAVODNICE pom = *x;
    *x = *y;
    *y = pom;
}

void bblSort(ZAVODNICE *zav, int pocet)
{
    for (int i = 0; i < pocet; i++)
    {
        for (int j = 0; j < pocet - i - 1; j++)
        {
            if (pocetCas(zav, j) > pocetCas(zav, j + 1))
            {
                swap(&zav[j], &zav[j + 1]);
            }
        }
    }
}

int prectiSoubor(ZAVODNICE *zav)
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
        pocet++;
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
                    zav[pocet - 1].startCis = atoi(data);
                    break;
                case 2:
                    strcpy(zav[pocet - 1].jmeno, data);
                    break;
                case 3:
                    strcpy(zav[pocet - 1].prijmeni, data);
                    break;
                case 4:
                    strcpy(zav[pocet - 1].narodnost, data);
                    break;
                case 5:
                    zav[pocet - 1].sjezdy.minuty = atoi(data);
                    break;
                case 6:
                    zav[pocet - 1].sjezdy.sekundy = atoi(data);
                    break;
                case 7:
                    zav[pocet - 1].sjezdy.setiny = atoi(data);
                    break;
                case 8:
                    zav[pocet - 1].slalom.minuty = 0;
                    zav[pocet - 1].slalom.sekundy = atoi(data);
                    break;
                case 9:
                    zav[pocet - 1].slalom.setiny = atoi(data);
                    break;
                }
                data = strtok(NULL, DELIM);
            }
        }
    }
    if (fclose(fr) == EOF)
    {
        printf("Soubor %s se nepodarilo zavrit.", VSTUP);
    }
    return (pocet);
}

void vypisObr(ZAVODNICE *zav, int pocet)
{
    printf("Alpske lyzovani startovni listina K O M B I N A C E  Z E N Y\n");
    printf("-------------------------------------------------------------\n");
    printf("startovni cislo      jmeno        prijmeni stat    sjezd   slalom\n");
    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < pocet; i++)
    {
        printf("%15d %10s %15s %4s %02d:%02d,%02d %02d:%02d,%02d\n",
               zav[i].startCis,
               zav[i].jmeno,
               zav[i].prijmeni,
               zav[i].narodnost,
               zav[i].sjezdy.minuty,
               zav[i].sjezdy.sekundy,
               zav[i].sjezdy.setiny,
               zav[i].slalom.minuty,
               zav[i].slalom.sekundy,
               zav[i].slalom.setiny);
    }
    printf("\nSoubor %s byl vytvoren.\n\n", VYSTUP);
}

void vypisSubor(ZAVODNICE *zav, int pocet)
{
    FILE * fw;
    char buffer[20];
    char ztrata[20];
    int prvni;

    if ((fw = fopen(VYSTUP, "w")) == NULL)
    {
        printf("Soubor %s se nepodarilo otverit.", VYSTUP);
    }
    
    fprintf(fw, "A L P S K E  L Y Z O V A N I - K O M B I N A C E  Z E N Y\n");
    fprintf(fw, "---------------------------------------------------------\n");
    fprintf(fw, "poradi      jmeno        prijmeni stat    sjezd   slalom  celkem   ztrata\n");
    fprintf(fw, "-------------------------------------------------------------------------\n");

    prvni = pocetCas(zav, 0);
    for (int i = 0; i < pocet; i++)
    {
        setNaCas(pocetCas(zav, i) - prvni, ztrata);
        setNaCas(pocetCas(zav, i), buffer);
        fprintf(fw, "%6d %10s %15s %4s %02d:%02d,%02d %02d:%02d,%02d %s %s%s\n",
               i + 1,
               zav[i].jmeno,
               zav[i].prijmeni,
               zav[i].narodnost,
               zav[i].sjezdy.minuty,
               zav[i].sjezdy.sekundy,
               zav[i].sjezdy.setiny,
               zav[i].slalom.minuty,
               zav[i].slalom.sekundy,
               zav[i].slalom.setiny,
               buffer,
               i > 0 ? "+" : " ",
               i > 0 ? ztrata : "       "
               );
    }
    if (fclose(fw) == EOF)
    {
        printf("Soubor %s nebylo mozne zavrit.\n", VYSTUP);
    }
}

int main()
{
    ZAVODNICE zav[BUFSIZE];
    int pocet;

    pocet = prectiSoubor(zav);
    vypisObr(zav, pocet);
    bblSort(zav, pocet);
    vypisSubor(zav, pocet);

    return EXIT_SUCCESS;
}