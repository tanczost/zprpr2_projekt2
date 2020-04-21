// projekt2_Tanczos.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUBOR "filmy.txt"
#define MAXZNAK 100

typedef struct meno
{
	char krstne[MAXZNAK];
	char priezvisko[MAXZNAK];
}MENO;

typedef struct herec
{
	MENO meno;
	int rokNarodenia;
	struct herec* kolega;
}HEREC;

typedef struct film
{
	char nazov[MAXZNAK];
	int rokVyroby;
	MENO reziser; //meno rezisera
	HEREC* herci;
	film* dalsiFilm;
}FILM;

/*funkcie*/
FILM* nacitaj(void);
FILM* nacitajFilm(FILE *fr);
HEREC* nacitajHerca(FILE* fr);
void vypis(FILM* kino);
FILM* uvolni(FILM* kino);
void pridaj(FILM** kino);
FILM* vymaz(FILM* kino);
void filmy(FILM* kino);
void herci(FILM* kino);
void rok(FILM* kino);
HEREC* zoznamRok(HEREC* zac, HEREC zdroj);


int main()
{
	FILM* kino = NULL;
	while (1)
	{
		char riadok[MAXZNAK];
		char prikaz[MAXZNAK];
	fgets(riadok, MAXZNAK - 1, stdin);
		sscanf(riadok, "%s", prikaz);
		if (!strcmp(prikaz, "koniec")) //uvolni sa pamat a konci program
		{
			uvolni(kino);
			break;
		}
		else if (!strcmp(prikaz, "vypis")) vypis(kino); //vypis zoznam filmov
		else if (!strcmp(prikaz, "nacitaj")) //pamat sa uvolni a nacita sa znova zoznam filmov
		{
			kino = uvolni(kino);
			kino = nacitaj();
		}
		else if(!strcmp(prikaz, "pridaj")) pridaj(&kino);
		else if(!strcmp(prikaz, "vymaz")) kino = vymaz(kino);
		else if(!strcmp(prikaz, "filmy")) filmy(kino);
		else if(!strcmp(prikaz, "herci")) herci(kino);
		else if(!strcmp(prikaz, "rok")) rok(kino);

	}
	return 0;
}

FILM* nacitajFilm(FILE *fr)
{
	FILM *novy = (FILM*)malloc(sizeof(FILM));
	if (novy->dalsiFilm == NULL) { printf("Malo pamati\n"); exit(-1); }

	fgets(novy->nazov, MAXZNAK, fr);
	novy->nazov[strlen(novy->nazov) - 1] = '\0';
	fscanf(fr, "%d", &novy->rokVyroby);
	fscanf(fr, "%s %s", novy->reziser.krstne, novy->reziser.priezvisko);
	if (fr != stdin) fscanf(fr, "\n");
	else while (getchar() != '\n');
	novy->herci = NULL;
	novy->dalsiFilm = NULL;
}

HEREC* nacitajHerca(FILE* fr)
{
	HEREC *novy = (HEREC*)malloc(sizeof(HEREC));
	if (novy == NULL) { printf("Malo pamati\n"); exit(-1); }
	fscanf(fr, "%s %s %d", novy->meno.krstne, novy->meno.priezvisko, &novy->rokNarodenia);
	if (fr != stdin) fscanf(fr, "\n");
	else while (getchar() != '\n');
	novy->kolega = NULL;
	return novy;
}

FILM* nacitaj(void)
{
	/*premenne*/
	FILE* fr;
	char hviezda;

	if ((fr = fopen(SUBOR, "r")) == NULL)
	{
		printf("Subor sa nenasiel\n"); exit(-1);
	}
	int c;

	if ((c = getc(fr)) == EOF) return NULL; //koniec suboru
	else ungetc(c, fr);


	FILM* zac = nacitajFilm(fr); //vytvorenie prveho zaznamu
	FILM* kino = zac;


	if ((hviezda = getc(fr)) == '*')
	{
		kino->herci = nacitajHerca(fr);
	}
	HEREC* temp = kino->herci;

	while ((hviezda = getc(fr)) == '*')
	{
		temp->kolega = nacitajHerca(fr);
		temp = temp->kolega;
	}
	ungetc(hviezda, fr);

	while (!feof(fr))
	{
		kino->dalsiFilm = nacitajFilm(fr);
		kino = kino->dalsiFilm;
		if ((hviezda = getc(fr)) == '*')
		{
			kino->herci = nacitajHerca(fr);
		}
		HEREC* temp = kino->herci;

		while ((hviezda = getc(fr)) == '*')
		{
			temp->kolega = nacitajHerca(fr);
			temp = temp->kolega;
		}
		ungetc(hviezda, fr);
	}
	return zac;
}

void vypis(FILM* kino)
{
	while (kino != NULL)
	{
		printf("%s (%d) %s %s\n\tHraju:", kino->nazov, kino->rokVyroby, kino->reziser.krstne, kino->reziser.priezvisko);
		HEREC* ucinkujuci = kino->herci;
		while (ucinkujuci != NULL)
		{
			printf(" %s %s (%d),", ucinkujuci->meno.krstne, ucinkujuci->meno.priezvisko, ucinkujuci->rokNarodenia);
			ucinkujuci = ucinkujuci->kolega;
		}
		printf("\b \n");
		kino = kino->dalsiFilm;
	}
}

FILM* uvolni(FILM* kino)
{
	/*premenne*/
	FILM* pom1;
	HEREC *pom2;
	/*********/

	pom1 = kino;

	while (pom1 != NULL)
	{
		pom2 = pom1->herci;
		while (pom2 != NULL)
		{
			HEREC* pom = pom2;
			pom2 = pom2->kolega;
			free(pom); //uvolni sa herec
		}
		FILM* pom;
		pom = pom1;
		pom1 = pom1->dalsiFilm;
		free(pom); //uvolni sa film
	}
	return NULL;
}

void pridaj(FILM** kino)
{
	/*premenne*/
	char hviezda;
	HEREC* pom = NULL;
	FILM* temp = (*kino);
	/*********/
	if (*kino == NULL) //vstupujeme ked zoznam je prazdny
	{
		(*kino) = nacitajFilm(stdin); //nacitame prvy film
		temp = (*kino);
	}
	else //vstupujeme ked uz v zozname su filmy
	{
		while (temp->dalsiFilm != NULL) //najdeme posledny
				temp = temp->dalsiFilm;
		temp = temp->dalsiFilm = nacitajFilm(stdin); //nacitame posledny film a nastavime nanho ukazovatel
	}

	if ((hviezda = getchar()) != '*')
	{
		ungetc(hviezda, stdin);
		temp->herci = nacitajHerca(stdin); //zadavaju sa herci
	}
	else
	{
		ungetc(hviezda, stdin);
		temp->herci = NULL;  //nezadavaju sa herci
	}

	//while (getchar() != '\n');

	pom = temp->herci;
	while((hviezda = getchar()) != '*') //zadavanie dalsich hercov
	{
		ungetc(hviezda, stdin);
		pom->kolega = nacitajHerca(stdin);
		pom = pom->kolega;
		//while (getchar() != '\n');
	}

	while (getchar() != '\n');
}

FILM* vymaz(FILM* kino)
{
	/*premenne*/
	char nazov[MAXZNAK];
	FILM* pred = NULL;
	FILM* zac = kino;
	HEREC* pom1;
	int counter = 1;
	/**********/

	fgets(nazov, MAXZNAK, stdin);
	nazov[strlen(nazov) - 1] = '\0';
	while (kino != NULL && strcmp(nazov, kino->nazov)) //hladanie filmu
	{
		pred = kino;
		kino = kino->dalsiFilm;
		counter++;
	}

	if (kino == NULL) return zac; //film sa nenasiel, alebo nemame zoznam

	if (counter == 1) zac = kino->dalsiFilm;//vymazava sa prvy film, vratime iny ukazovatel
	else pred->dalsiFilm = kino->dalsiFilm;

	pom1 = kino->herci;
	while (pom1 != NULL)
	{
		HEREC* pomHerec = pom1;
		pom1 = pom1->kolega;
		free(pomHerec);
	}
	free(kino);
	return zac;
}

void filmy(FILM* kino)
{
	char meno1[MAXZNAK], meno2[MAXZNAK];
	scanf("%s %s", meno1, meno2);
	while (kino != NULL)
	{
		HEREC* pom = kino->herci;
		while (pom != NULL)
		{
			if (!strcmp(meno1, pom->meno.krstne) && !strcmp(meno2, pom->meno.priezvisko)) break;
			pom = pom->kolega;
		}
		if (pom != NULL) printf("%s (%d)\n", kino->nazov, kino->rokVyroby);
		kino = kino->dalsiFilm;
	}
	while (getchar() != '\n');

}

void herci(FILM* kino)
{
	char nazov1[MAXZNAK], nazov2[MAXZNAK];
	fgets(nazov1, MAXZNAK, stdin);
	nazov1[strlen(nazov1) - 1] = '\0';

	fgets(nazov2, MAXZNAK, stdin);
	nazov2[strlen(nazov2) - 1] = '\0';

	HEREC* pole = NULL;
	int pocetHercov = 0;
	while (kino != NULL)
	{
		if (strcmp(nazov1, kino->nazov) && strcmp(nazov2, kino->nazov)) { kino = kino->dalsiFilm; continue; }

		HEREC* pom = kino->herci;
		while (pom != NULL)
		{
			pole = (HEREC*)realloc(pole, (pocetHercov + 1) * sizeof(HEREC));
			pole[pocetHercov].meno = pom->meno;
			pole[pocetHercov].rokNarodenia = pom->rokNarodenia;
			pocetHercov++;
			pom = pom->kolega;
		}
		kino = kino->dalsiFilm;

	}
	for (int i = 0; i < pocetHercov; i++)
	{
		for (int j = i + 1; j < pocetHercov; j++)
		{
			if(!strcmp(pole[i].meno.priezvisko,pole[j].meno.priezvisko)) printf("%s %s\n", pole[i].meno.krstne, pole[i].meno.priezvisko);
		}
	}

	free(pole);
}

HEREC* zoznamRok(HEREC* zac, HEREC zdroj)
{
	if (zac == NULL) //vkladame prveho herca
	{
		zac = (HEREC*)malloc(sizeof(HEREC));
		*zac = zdroj;
		zac->kolega = NULL;
		return zac;
	}

	if (strcmp(zdroj.meno.priezvisko,zac->meno.priezvisko) < 0)
	{
		HEREC* novy = (HEREC*)malloc(sizeof(HEREC));
		*novy = zdroj;
		novy->kolega = zac;
		return novy;
	}


	HEREC* pom = zac;

	while (pom->kolega != NULL)
	{
		if (!strcmp(zdroj.meno.krstne, pom->meno.krstne) && !strcmp(zdroj.meno.priezvisko, pom->meno.priezvisko)
			&& zdroj.rokNarodenia == pom->rokNarodenia) return zac; //tohto herca uz mame v zozname


		pom = pom->kolega;
	}

	if (!strcmp(zdroj.meno.krstne, pom->meno.krstne) && !strcmp(zdroj.meno.priezvisko, pom->meno.priezvisko)
		&& zdroj.rokNarodenia == pom->rokNarodenia) return zac;
	HEREC *novy = (HEREC*)malloc(sizeof(HEREC));
	*novy = zdroj;
	novy->kolega = pom->kolega;
	pom->kolega = novy;
	return zac;


}

void rok(FILM* kino)
{
	int rok;
	scanf("%d", &rok);
	HEREC* zac = NULL;

	while (kino != NULL)
	{
		if (kino->rokVyroby == rok)
		{
			HEREC* temp = kino->herci;
			while (temp != NULL)
			{
				zac = zoznamRok(zac, *temp);
				temp = temp->kolega;
			}
		}
		kino = kino->dalsiFilm;
	}

	while (zac != NULL)
	{
		printf("%s %s\n", zac->meno.krstne, zac->meno.priezvisko);
		HEREC* pom = zac;
		zac = zac->kolega;
		free(pom);
	}
}
