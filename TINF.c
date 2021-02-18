#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXLEN 1048576  // gornja granica za duljine svih polja

int izracunaj_k(char g[], int n) {
    char *found;
    found = strstr(g, "1");             // vraca pokazivac na prvu jedinicu u stringu jer je moguce da je g(x) unesen s vodecim nulama
    if(!found)
        return -1;

    int lok = (int)(found - g);         // racuna lokaciju prve jedinice u stringu
    int k = n - (strlen(g) - lok - 1);  // n - duljina g(x)-1 jer broji \0 - lokacija jedinice - 1 jer krece od nulte potencije pa moramo umanjiti za nju

    return k;
}

char h[MAXLEN], nn[MAXLEN];

void polinom_h(char g[], int n, char *matricaH, int *potencija_h) {
    for(int i = 0; i < n; i++)  // polinom od h(x) koji je duljine n
        h[i] = '0';

    for(int i = 0; i < n + 1; i++)  // polinom od n+1 duljine n
        nn[i] = '0';
    nn[0] = '1'; // jedinica na n-toj i 0-toj poziciji
    nn[n] = '1';

    char *found;
    found = strstr(nn, "1");
    int lok_nn = (int)(found - nn); // lokacija prve jedinice u nn polju kako bio zadovoljen uvjer while petlje

    found = strstr(g, "1");
    int lok_g = (int)(found - g);      // lokacija prve jedinice u g(x) polinomu
    int duljina_g = strlen(g) - lok_g; // duljina g(x) polinoma s kojim dijelimo (n+1) polinom kako bi dobili h(x)

    while(lok_nn >= 0 && lok_nn <= n) {
        h[n - lok_nn - duljina_g + 1] = '1'; // h(x) se puni jedinicama na lokacijama koje odgovaraju potenciji x^lokaciju
        for(int i = 0; i < duljina_g; i++) {
            // detaljno objasniti u dokumentaciji
            if(nn[lok_nn + i] == g[lok_g + i])
                nn[lok_nn + i] = '0'; // dijeli kao na papiru kad potpisujemo s tim da oduzimanje razlicitih znamenki zamijeni s 1, a istih s 0
            else
                nn[lok_nn + i] = '1'; // jer se radi o aritmetici mod 2
        }

        found = strstr(nn, "1"); // while petlju je potrebno vrtiti sve dok nn polje nije 0...0
        lok_nn = (int)(found - nn);
    }

    for(int i = 0; i < n; i++)
        if(h[i] == '1')
            *potencija_h = i; // zadnja jedinica u h je najveca potencija od h

//  for(int i = 0; i < n; i++)
//      printf("%c", h[i]);            // provjera h(x)
//  printf("\n");                      // pazi h(0) je x^0 pa je potrebno okrenuti ispis h(x)

    int brojac = 0;
    for(int x = 0; x < n - *potencija_h; x++) {
        for(int y = 0; y < n; y++)
            *(matricaH + n * x + y) = '0';
    }

    for(int x = 0; x < n - *potencija_h; x++) {
        // punjenje h(x) u matricuH
        for(int y = 0; y < n; y++) {
            // broj redaka matriceH dobijemo ciklicnom rotacijom h(x) sve dok 1 na mjestu najvece potencije ne dode na n-to mjesto
            if(y + brojac < n)
                *(matricaH + n * x + y + brojac) = h[y];
        }
        brojac++;
    }

//  for(int x = 0; x < n - k; x++)
//      for(int y = 0; y < n; y++)
//          printf("%c", *(matricaH + n * x + y));   // provjera matrice H
//      printf("\n");
//  }

    return;
}

void sindrom(char c[], char *matricaH, int n, int potencija_h, char *s) {
    int brojac; // mnozenje matriceH i kodne rijeci c kako bi se dobio sindrom duljine n-najveca_potencija_od_h(x)
    for(int x = 0; x < n - potencija_h; x++) {
        brojac = 0;
        for(int y = 0; y < n; y++)
            if(c[y] == '1' && *(matricaH + x * n + y) == '1')
                brojac++;    // broje se jedinice u brojacu prilikom mozenja matriceH i c
        if(brojac % 2 == 1)  // nakon sto smo izmnozili cijeli redak radimo mod 2 iliti xor
            *(s + x) = '1';  // ako je neparan broj jedinica sindrom za taj redak je 1
        else
            *(s + x) = '0';
    }
    return;
}

int mjesto_greske(char *matricaH, char s[], int potencija_h, int n) {
    // potrebno je usporediti sindrom s matricomH i naci na kojem mjestu se dogodila greska
    int brojac;
    for(int x = 0; x < n; x++) {
        brojac = 0;
        for(int y = 0; y < n - potencija_h; y++)
            if(*(matricaH + n * y + x) == s[y])
                brojac++; // kada nademo stupac u matriciH koji je jednak sindromu saljemo vrijednos stupca pomocu f-je
        if(brojac == n - potencija_h)
            return x;
    }
    return -1; // vrati -1 ako nema greske, ovo je potrebno zbog upozorenja prevoditelja da nije vracena vrijednost
}

void ispravi_gresku(char *c, int mjestopogreske) {
    // ispravljenje greske na lokaciji -> mjesto_greske
    if(*(c + mjestopogreske) == '0')
        *(c + mjestopogreske) = '1';
    else
        *(c + mjestopogreske) = '0';
}

char g[MAXLEN], c[MAXLEN], matricaH[MAXLEN], s[MAXLEN];

int main(void) {
    int n;
    int potencija_h;

    printf("Unesite duljinu kodne rijeci ciklicnog koda K: ");
    scanf("%d", &n);
    printf("\n");
    if(n <= 0) {
        printf("Neispravan unos. Duljina kodne rijeci mora biti veca od nule.\n");
        return 0;
    }

    printf("Unesite g(x) u vektorskom obliku (niz nula i jedinica): ");
    scanf("%*[\r\n]%[^\r\n]", g);
    printf("\n");
    int brojac=0;
    for(int i = 0; g[i] != '\0'; i++) { 
        brojac++;
        if(g[i] != '0' && g[i] != '1') {
            printf("Neispravan unos. g(x) mora biti niz nula i jedinica.\n");
            return 0;
        }
    }
     if(brojac > n) {
        printf("Neispravan unos. Duljina g(x) mora biti manja ili jednaka n.\n");
        return 0;
    }

    int k = izracunaj_k(g, n); // izracunata vrijednost k i vracena preko funkcije
    if(k <= 0) {
        printf("Neispravan unos. g(x) mora biti polinom stupnja manjega od n.\n");
        return 0;
    }
    printf("K[n,k] = K[%d,%d]\n\n", n, k);

    polinom_h(g, n, matricaH, &potencija_h);

    printf("Polinom h(x) zadanog koda u vektorskom obliku je: ");
    for(int i = potencija_h; i >= 0; i--)  // obrnuti poredak h(x) kako bi h[0] bilo x^najvecu_potenciju_od_h
        printf("%c", matricaH[i]);
    printf("\n\n");

//  for(int x = 0; x < n - k; x++) {
//      for(int y = 0; y < n; y++)                      // provjera matrice H
//          printf("%c", matricaH[x * (n - k) + y]);
//      printf("\n");
//  }

    printf("Unesite kodnu rijec c duljine n: ");
    scanf("%*[\r\n]%[^\r\n]", c);
    printf("\n");
//  for(int i = 0; i < n; i++)                          // provjera c
//      printf("%c", c[i]);
//  printf("%s", c);
    if((int)strlen(c) != n) {
        printf("Neispravan unos. Duljina kodne rijeci c mora biti n.\n");
        return 0;
    }
    for(int i = 0; c[i] != '\0'; i++) {
        if(c[i] != '0' && c[i] != '1' && c[i] != '\n' && c[i] != '\0') {
            printf("Neispravan unos. Kodna rijec c mora biti niz nula i jedinica.\n");
            return 0;
        }
    }
    sindrom(c, matricaH, n, potencija_h, s); // f-ja mijenja polje s koje predstavlja sindrom

    int provjera_sindroma = 0;
    for(int i = 0; i < n - potencija_h; i++) {
//      printf("%c", s[i]);        // provjera s(x)
        if(s[i] == '1')
            provjera_sindroma = 1; // ako je sindrom razlicit od 00..0 potrebno je ispraviti gresku
    }
//  printf("\n");

    if(provjera_sindroma == 1) {
        printf("Sindrom je razlicit od 00..0 te je potrebno ispraviti kodnu rijec.\n\n");
        int mjestogreske = mjesto_greske(matricaH, s, potencija_h, n);
//      printf("%d", mjestogreske);

        ispravi_gresku(c, mjestogreske);

        sindrom(c, matricaH, n, potencija_h, s); // izracunaj sindrom ispravljene kodne rijeci
        for(int i = 0; i < n - potencija_h; i++)
            if(s[i] == '1') { // ako je novi sindrom razlicit od 00..0, dogodila se visestruka pogreska
                printf("Otkriveno je vise pogresaka nego sto je moguce ispraviti.\n");
                return 0;
            }
        printf("Ispravljena kodna rijec je: %s\n", c);
    }
    else
        printf("Sindrom je jednak 00..0 i nije potrebno ispravljati kodnu rijec.\n");

    return 0;
}