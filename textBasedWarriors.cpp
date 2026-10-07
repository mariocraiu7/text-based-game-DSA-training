#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NUME 50
#define INF 1000000000


typedef struct NodTranzitie {
    int destinatie;
    int cost;
    struct NodTranzitie* urmator;
} NodTranzitie;


typedef struct {
    char nume[MAX_NUME];
    int bonus;
    char** creaturi;
    int nr_creaturi;
} Oras;


typedef struct {
    Oras* orase;
    int nr_orase;
    NodTranzitie** adiacenta;
} ReteaJoc;


int gaseste_id(ReteaJoc* retea, const char* nume) {
    for (int i = 0; i < retea->nr_orase; ++i) {
        if (strcmp(retea->orase[i].nume, nume) == 0) {
            return i;
        }
    }
    return -1;
}


void adauga_muchie(ReteaJoc* retea, int sursa, int dest, int cost) {
    NodTranzitie* nou = (NodTranzitie*)malloc(sizeof(NodTranzitie));
    nou->destinatie = dest;
    nou->cost = cost;
    nou->urmator = retea->adiacenta[sursa];
    retea->adiacenta[sursa] = nou;
}


void incarca_date(ReteaJoc* retea, const char* numeFisier) {
    FILE* fin = fopen(numeFisier, "r");
    if (!fin) {
        printf("Eroare la deschiderea fisierului!\n");
        exit(1);
    }

    fscanf(fin, "%d", &retea->nr_orase);
    retea->orase = (Oras*)malloc(retea->nr_orase * sizeof(Oras));
    retea->adiacenta = (NodTranzitie**)calloc(retea->nr_orase, sizeof(NodTranzitie*));

    for (int i = 0; i < retea->nr_orase; ++i) {
        fscanf(fin, "%s %d %d", retea->orase[i].nume, &retea->orase[i].bonus, &retea->orase[i].nr_creaturi);
        retea->orase[i].creaturi = (char**)malloc(retea->orase[i].nr_creaturi * sizeof(char*));

        for (int j = 0; j < retea->orase[i].nr_creaturi; ++j) {
            retea->orase[i].creaturi[j] = (char*)malloc(MAX_NUME * sizeof(char));
            fscanf(fin, "%s", retea->orase[i].creaturi[j]);
        }
    }

    int nr_tranzitii;
    fscanf(fin, "%d", &nr_tranzitii);
    for (int i = 0; i < nr_tranzitii; ++i) {
        char sursa[MAX_NUME], dest[MAX_NUME];
        int cost;
        fscanf(fin, "%s %s %d", sursa, dest, &cost);

        int idSursa = gaseste_id(retea, sursa);
        int idDest = gaseste_id(retea, dest);

        if (idSursa != -1 && idDest != -1) {
            adauga_muchie(retea, idSursa, idDest, cost);
            adauga_muchie(retea, idDest, idSursa, cost); /* Graf neorientat */
        }
    }
    fclose(fin);
}


void afiseaza_retea(ReteaJoc* retea) {
    printf("=== Reteaua de Orase (C) ===\n");
    for (int i = 0; i < retea->nr_orase; ++i) {
        printf("Oras: %s | Bonus: %d | Creaturi: ", retea->orase[i].nume, retea->orase[i].bonus);
        for (int j = 0; j < retea->orase[i].nr_creaturi; ++j) {
            printf("%s ", retea->orase[i].creaturi[j]);
        }
        printf("\n");
    }
    printf("============================\n\n");
}


void dfs(ReteaJoc* retea, int u, bool* vizitat, int* contor) {
    vizitat[u] = true;
    (*contor)++;
    NodTranzitie* t = retea->adiacenta[u];
    while (t != NULL) {
        if (!vizitat[t->destinatie]) {
            dfs(retea, t->destinatie, vizitat, contor);
        }
        t = t->urmator;
    }
}


bool poate_vizita_tot(ReteaJoc* retea, const char* numeStart) {
    int idStart = gaseste_id(retea, numeStart);
    if (idStart == -1) return false;

    bool* vizitat = (bool*)calloc(retea->nr_orase, sizeof(bool));
    int contor = 0;

    dfs(retea, idStart, vizitat, &contor);
    free(vizitat);

    return contor == retea->nr_orase;
}


void oras_cel_mai_important(ReteaJoc* retea) {
    int max_legaturi = -1;
    int id_max = -1;

    for (int i = 0; i < retea->nr_orase; ++i) {
        int muchii = 0;
        NodTranzitie* t = retea->adiacenta[i];
        while (t) {
            muchii++;
            t = t->urmator;
        }
        if (muchii > max_legaturi) {
            max_legaturi = muchii;
            id_max = i;
        }
    }

    if (id_max != -1) {
        printf("Cel mai important oras este: %s (conexiuni: %d)\n\n", retea->orase[id_max].nume, max_legaturi);
    }
}


void creaturi_comune(ReteaJoc* retea, const char* numeX, const char* numeY) {
    int idX = gaseste_id(retea, numeX);
    int idY = gaseste_id(retea, numeY);

    if (idX == -1 || idY == -1) return;

    printf("Creaturi comune intre %s si %s: ", numeX, numeY);
    bool gasit = false;

    for (int i = 0; i < retea->orase[idX].nr_creaturi; ++i) {
        for (int j = 0; j < retea->orase[idY].nr_creaturi; ++j) {
            if (strcmp(retea->orase[idX].creaturi[i], retea->orase[idY].creaturi[j]) == 0) {
                printf("%s ", retea->orase[idX].creaturi[i]);
                gasit = true;
            }
        }
    }
    if (!gasit) printf("Niciuna");
    printf("\n\n");
}


void bkt_orase(ReteaJoc* retea, int u, int energie_curenta, bool* vizitat, int vizitate_curent, int* max_vizitate) {
    if (vizitate_curent > *max_vizitate) {
        *max_vizitate = vizitate_curent;
    }

    NodTranzitie* t = retea->adiacenta[u];
    while (t != NULL) {
        int v = t->destinatie;
        if (!vizitat[v] && energie_curenta >= t->cost) {
            vizitat[v] = true;
            int energie_noua = energie_curenta - t->cost + retea->orase[v].bonus;

            bkt_orase(retea, v, energie_noua, vizitat, vizitate_curent + 1, max_vizitate);

            vizitat[v] = false; 
        }
        t = t->urmator;
    }
}


void determinare_maxim_orase(ReteaJoc* retea, const char* numeStart, int energieInitiala) {
    int idStart = gaseste_id(retea, numeStart);
    if (idStart == -1) return;

    bool* vizitat = (bool*)calloc(retea->nr_orase, sizeof(bool));
    vizitat[idStart] = true;

    int max_vizitate = 1;
    int energie_start = energieInitiala + retea->orase[idStart].bonus;

    bkt_orase(retea, idStart, energie_start, vizitat, 1, &max_vizitate);

    printf("Pornind din %s cu energia %d, se pot vizita maxim %d orase.\n\n", numeStart, energieInitiala, max_vizitate);
    free(vizitat);
}


void drum_minim_energie(ReteaJoc* retea, const char* numeStart) {
    int idStart = gaseste_id(retea, numeStart);
    if (idStart == -1) return;

    int* dist = (int*)malloc(retea->nr_orase * sizeof(int));
    bool* procesat = (bool*)calloc(retea->nr_orase, sizeof(bool));

    for (int i = 0; i < retea->nr_orase; ++i) {
        dist[i] = INF;
    }
    dist[idStart] = 0;

    for (int count = 0; count < retea->nr_orase - 1; ++count) {
        int min_dist = INF;
        int u = -1;

       
        for (int i = 0; i < retea->nr_orase; ++i) {
            if (!procesat[i] && dist[i] <= min_dist) {
                min_dist = dist[i];
                u = i;
            }
        }

        if (u == -1) break;
        procesat[u] = true;

       
        NodTranzitie* t = retea->adiacenta[u];
        while (t != NULL) {
            int v = t->destinatie;
            if (!procesat[v] && dist[u] != INF && dist[u] + t->cost < dist[v]) {
                dist[v] = dist[u] + t->cost;
            }
            t = t->urmator;
        }
    }

    printf("Energia minima cheltuita plecand din %s:\n", numeStart);
    for (int i = 0; i < retea->nr_orase; ++i) {
        if (dist[i] == INF) {
            printf(" -> %s: Inaccesibil\n", retea->orase[i].nume);
        }
        else {
            printf(" -> %s: %d puncte\n", retea->orase[i].nume, dist[i]);
        }
    }

    free(dist);
    free(procesat);
}

void elibereaza_memorie(ReteaJoc* retea) {
    for (int i = 0; i < retea->nr_orase; ++i) {
        for (int j = 0; j < retea->orase[i].nr_creaturi; ++j) {
            free(retea->orase[i].creaturi[j]);
        }
        free(retea->orase[i].creaturi);

        NodTranzitie* curent = retea->adiacenta[i];
        while (curent != NULL) {
            NodTranzitie* temp = curent;
            curent = curent->urmator;
            free(temp);
        }
    }
    free(retea->orase);
    free(retea->adiacenta);
}

int main() {
    ReteaJoc retea = { NULL, 0, NULL };

    incarca_date(&retea, "retea.txt");
    afiseaza_retea(&retea);

    bool conectat = poate_vizita_tot(&retea, "Castle");
    printf("Se pot vizita toate orasele plecand din Castle? %s\n\n", conectat ? "Da" : "Nu");

    oras_cel_mai_important(&retea);
    creaturi_comune(&retea, "Tower", "Inferno");
    determinare_maxim_orase(&retea, "Rampart", 15);
    drum_minim_energie(&retea, "Castle");

    elibereaza_memorie(&retea);
    return 0;
}