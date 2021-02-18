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

void generirajuca_matrica(char g[], int n, int k, char *matricaG) {
    int duljina_g = strlen(g);
    memset(matricaG, '0', k * n);                               // inicijaliziraj matricu na 00..0
    for(int i = 0; i < k; i++)                                  // za svaki redak
        for(int j = 0; j <= n - k; j++)                         // kopiraj koeficijente polinoma g(x) obrnutim redom, pocevsi od glavne dijagonale
            *(matricaG + i * n + i + j) = g[duljina_g - j - 1];

//  for(int i = 0; i < k; i++) {                                // provjera matrice
//      for(int j = 0; j < n; j++)
//          printf("%c", *(matricaG + i * n + j));
//      printf("\n");
//  }
}

void xor_niz(char *niz1, char *niz2, int n) {
    for(int i = 0; i < n; i++)
        if(*(niz1 + i) == *(niz2 + i)) // ako a == b
            *(niz1 + i) = '0';         // onda a XOR b = 0
        else
            *(niz1 + i) = '1';         // inace a XOR b = 1
}

void kodne_rijeci(char *matricaG, int m, int n, int k, char *matricaC) {
    memset(matricaC, '0', m * n);                               // inicijaliziraj sve kodne rijeci na 00..0
    for(int i = 0; i < m; i++)                                  // za svaku kodnu rijec
        for(int j = 0; j < k; j++)                              // za svaki bit u binarnom zapisu
            if(i & (1 << j))                                    // ako je j-ti bit 1
                xor_niz(matricaC + i * n, matricaG + j * n, n); // pribroji (XOR) j-ti redak matrice G

//  for(int i = 0; i < m; i++) {                                // provjera kodnih rijeci
//      for(int j = 0; j < n; j++)
//          printf("%c", *(matricaC + i * n + j));
//      printf("\n");
//  }
}

int hammingova_udaljenost(char *niz1, char *niz2, int n) {
    int rjesenje = 0;                  // pocni od 0
    for(int i = 0; i < n; i++)
        if(*(niz1 + i) != *(niz2 + i)) // za svaki par bitova na istom mjestu koji se razlikuju
            rjesenje++;                // pribroji 1
    return rjesenje;
}

int minimalna_udaljenost(char *matricaC, int m, int n) {
    int rjesenje = n;                    // najveca moguca udaljenost je n (ako kod ima samo dvije rijeci i one su komplementarne, npr. 00..0 i 11..1)
    for(int i = 0; i < m; i++)
        for(int j = i + 1; j < m; j++) { // za svaki par kodnih rijeci
            int udaljenost2 = hammingova_udaljenost(matricaC + i * n, matricaC + j * n, n);
            if(rjesenje > udaljenost2)   // ako je njihova udaljenost manja od trenutnog rjesenja
                rjesenje = udaljenost2;  // popravi rjesenje
        }
    return rjesenje;
}

void vektori_pogresaka_rek(int n, int t, int *p, char *matricaVP, int i, int v) {
    // i je pozicija trenutnog bita, v je trenutni vektor; funkciju pozvati s pocetnim vrijednostima 0, 0
    if(i == n || !t) {                         // ako smo dosli do kraja vektora ili smo odabrali sve pogresne bitove
        if(!v)                                 // ako je vektor 00..0, ne radi nista
            return;
        for(int j = 0; j < n; j++)             // zapisi binarni zapis vektora v u matricu
            if(v & (1 << j))
                matricaVP[(*p) * n + j] = '1';
            else
                matricaVP[(*p) * n + j] = '0';
        (*p)++;                                // uvecaj brojac
        return;                                // izlaz iz rekurzije
    }
    vektori_pogresaka_rek(n, t, p, matricaVP, i + 1, v);                // pomakni se na sljedeci bit uz i-ti bit = 0
    vektori_pogresaka_rek(n, t - 1, p, matricaVP, i + 1, v | (1 << i)); // pomakni se na sljedeci bit uz i-ti bit = 1
}

void vektori_pogresaka(int n, int t, int *p, char *matricaVP) {
    (*p) = 0;                                        // postavi brojac vektora p na 0
    vektori_pogresaka_rek(n, t, p, matricaVP, 0, 0); // pozovi rekurziju

//  for(int i = 0; i < (*p); i++) {                  // provjera vektora pogresaka
//      for(int j = 0; j < n; j++)
//          printf("%c", *(matricaVP + i * n + j));
//      printf("\n");
//  }
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

char g[MAXLEN], c[MAXLEN], matricaH[MAXLEN], matricaG[MAXLEN], matricaC[MAXLEN], matricaVP[MAXLEN], s[MAXLEN], s2[MAXLEN];

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
    for(int i = 0; g[i] != '\0'; i++) {
        if(g[i] != '0' && g[i] != '1') {
            printf("Neispravan unos. g(x) mora biti niz nula i jedinica.\n");
            return 0;
        }
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

    generirajuca_matrica(g, n, k, matricaG);

    int m = (1 << k);
    kodne_rijeci(matricaG, m, n, k, matricaC);

    int d = minimalna_udaljenost(matricaC, m, n);
    int t = (d - 1) / 2;
//  printf("%d %d\n", d, t);

    int p;
    vektori_pogresaka(n, t, &p, matricaVP);

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
        for(int i = 0; i < p; i++) {                        // za svaki vektor pogreske
            sindrom(matricaVP + i * n, matricaH, n, potencija_h, s2);
            if(!strncmp(s, s2, n - potencija_h)) {          // ako je sindrom vektora pogreske jednak sindromu kodne rijeci, ispravi
//              for(int j = 0; j < n; j++)
//                  printf("%c", *(matricaVP + i * n + j));
//              printf("\n");
                xor_niz(c, matricaVP + i * n, n);           // ispravljanje po vektoru pogreske je XOR po bitovima
                printf("Ispravljena kodna rijec je: %s\n", c);
                return 0;
            }
        }
        printf("Otkriveno je vise pogresaka nego sto je moguce ispraviti.\n"); // ako smo dosli ovdje, onda ne mozemo ispraviti pogresku
    }
    else
        printf("Sindrom je jednak 00..0 i nije potrebno ispravljati kodnu rijec.\n");

    return 0;
}