#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#pragma warning(disable : 4996)
// Énumération pour les constantes
enum Const {
    NB_ETU_MAX = 100, NB_ABS_MAX = 800,
    NOM_MAX = 31, MAX_DATE = 40, MAX_CARAC_JUSTIFICATION = 100,
    MAX_JOUR_POUR_JUSTIF = 3, DEMI_JOURNEE = 3, ABS_DEFAILLANT = 5
};

// Énumération pour les demi-journées
enum demiJournee {
    am,
    pm
};

// Structure pour stocker les informations des étudiants
typedef struct {
    char nomEtu[NOM_MAX];
    int noGroupe;
    int idEtu;
    int nbAbsencesEtu; //Nombres d'absences de l'étudiant
} Etudiant;

// Structure pour les absences
typedef struct {
    int idAbs;
    int idEtu;
    int date;  // Date à laquelle votre absence est enregistré
    int noGroupe; // Numéro de groupe de l'étudiant
    int dateJustif; // Date à laquelle la justification a été enregistrer
    enum demiJournee demiJournee; //Demi journée de l'absence
    char justification[MAX_CARAC_JUSTIFICATION];
    char nomEtu[NOM_MAX + 1]; // Nom de l'étudiant
    bool estJustifiee; // Si l'absence a été justifiée
    bool enTraitement;// Si l'absence est en cours de traitement
    bool estValide; //Si l'absence est validé ou non

    bool euValidation; // Si l'absence a deja eu un état de validation
    bool recuJustif; // Si l'absence a déja reçu un justificatif
    bool retard; // Si l'absence a été invalidé par un retard de justificatif

    bool enValidation; // Si l'absence est en attente de validation
} Absences;

// Prototypes des fonctions
void inscription(const char* nom, int groupe, Etudiant etudiant[], int* nbEtuInscrits);
void absence(int idEtu, int nbJour, const char* demiJournee, Absences absences[], Etudiant etudiant[], int* nbAbsences, const int* nbEtuInscrits);
void justifications(int idAbs, int jourAbs, const char* justification, Absences absences[], int nbAbsences, int* nbAbsencesAttente);
void enValidations(Absences absences[], const int* nbAbsencesAttente);
void listeEtudiants(int jourCourant, Etudiant etudiants[], int nbEtuInscrits, Absences absences[], int nbAbsences);
void etudiantInfo(Etudiant* etudiants, int nbEtuInscrits, Absences* absences, int nbAbsences, int idEtu, int jourCourant);
int calculerAbsences(int idEtu, int jourCourant, Absences absences[], int nbAbsences);
void trierEtudiants(Etudiant etudiants[], int nbEtuInscrits);
void validationJustification(int idAbs, const char* validation, Absences absences[], const int* nbAbsences);
void defaillants(int jourCourant, int nbEtuInscrits, int nbAbsences, Absences absences[], Etudiant etudiant[]);
int comparerAbsences(const void* a, const void* b);



int main(void) {
    Etudiant etudiant[NB_ETU_MAX]; // Tableau pour stocker les étudiants
    Absences absences[NB_ABS_MAX];  // Tableau pour stocker les absences
    int nbEtuInscrits = 0;  // Nombre d'étudiants inscrits
    int nbAbsences = 0;  // Nombre d'absences enregistrées
    int nbAbsencesAttente = 0; //nombre d'absences en attente

    char nomEtu[NOM_MAX + 1];
    int groupeEtu;
    int idEtu;
    int nbJour; // Jour au quel l'absence va etre enregistrer
    int idAbs;
    int jourAbs; // Jour au quel le justificatif est enregistrer
    char demiJournee[100];//mettre une constante
    char justification[MAX_CARAC_JUSTIFICATION + 1];
    char validation[3];//mettre une constante // Validation "ok" ou "ko"
    while (1) {
        char commande[100];

        if (scanf("%s", commande) != 1) {
            break; // Fin de l'entrée
        }

        if (strcmp(commande, "inscription") == 0) {
            scanf("%s %d", nomEtu, &groupeEtu);
            inscription(nomEtu, groupeEtu, etudiant, &nbEtuInscrits);
        }
        else if (strcmp(commande, "absence") == 0) {
            scanf("%d %d %s", &idEtu, &nbJour, demiJournee);
            absence(idEtu, nbJour, demiJournee, absences, etudiant, &nbAbsences, &nbEtuInscrits);
        }
        else if (strcmp(commande, "justificatif") == 0) {
            scanf("%d %d", &idAbs, &jourAbs);
            getchar();  // Consommer le '\n' après scanf
            fgets(justification, MAX_CARAC_JUSTIFICATION, stdin);
            justification[strcspn(justification, "\n")] = '\0';  // Supprime le '\n' à la fin
            justifications(idAbs, jourAbs, justification, absences, nbAbsences, &nbAbsencesAttente);
        }
        else if (strcmp(commande, "etudiants") == 0) {
            int jour_courant;
            scanf("%d", &jour_courant);
            listeEtudiants(jour_courant, etudiant, nbEtuInscrits, absences, nbAbsences);
        }
        else if (strcmp(commande, "etudiant") == 0) {
            int jour_courant;
            scanf("%d %d", &idEtu, &jour_courant);
            etudiantInfo(etudiant, nbEtuInscrits, absences, nbAbsences, idEtu, jour_courant);
        }
        else if (strcmp(commande, "validations") == 0) {
            enValidations(absences, &nbAbsences);
        }
        else if (strcmp(commande, "validation") == 0) {
            scanf("%d %s", &idAbs, validation);
            validationJustification(idAbs, validation, absences, &nbAbsences);
        }
        else if (strcmp(commande, "defaillants") == 0) {
            int jourCourant;
            scanf("%d", &jourCourant);
            defaillants(jourCourant, nbEtuInscrits, nbAbsences, absences, etudiant);
        }
        else if (strcmp(commande, "exit") == 0) {
            break;
        }
        else {
            printf("Commande non reconnue.\n");
        }
    }

    return 0;
}


int comparerAbsences(const void* a, const void* b) { //Focntion qui compare les étudiants afin de pouvoir les trier
    Absences* absA = (Absences*)a;
    Absences* absB = (Absences*)b;

    // Comparaison par date
    if (absA->date != absB->date) {
        return absA->date - absB->date;
    }

    // Si les dates sont les mêmes, on compare par demi-journée
    if (absA->demiJournee == am && absB->demiJournee == pm) {
        return -1; // "am" doit précéder "pm"
    }
    else if (absA->demiJournee == pm && absB->demiJournee == am) {
        return 1;  // "pm" doit suivre "am"
    }

    // Si demi-journée est identique
    return 0;
}

// Fonction permettant l'inscription d'un étudiant
void inscription(const char* nom, int groupe, Etudiant etudiant[], int* nbEtuInscrits) {
    // Vérification de l'existence de l'étudiant
    for (int i = 0; i < *nbEtuInscrits; i++) {
        if (strcmp(etudiant[i].nomEtu, nom) == 0 && etudiant[i].noGroupe == groupe) {
            printf("Nom incorrect\n");
            return;
        }
    }

    // Inscription du nouvel étudiant
    strcpy(etudiant[*nbEtuInscrits].nomEtu, nom);
    etudiant[*nbEtuInscrits].noGroupe = groupe;
    etudiant[*nbEtuInscrits].idEtu = *nbEtuInscrits + 1;
    etudiant[*nbEtuInscrits].nbAbsencesEtu = 0;

    // Affichage du message de confirmation
    printf("Inscription enregistree (%d)\n", etudiant[*nbEtuInscrits].idEtu);

    // Incrémentation du nombre d'étudiants inscrits
    (*nbEtuInscrits)++;
}

// Fonction pour enregistrer une absence
void absence(int idEtu, int nbJour, const char* demiJournee, Absences absences[], Etudiant etudiant[], int* nbAbsences, const int* nbEtuInscrits) {
    // Vérification de l'identifiant de l'étudiant
    int etuTrouve = -1;
    for (int i = 0; i < *nbEtuInscrits; i++) {
        if (etudiant[i].idEtu == idEtu) {
            etuTrouve = i;
            break;
        }
    }
    if (etuTrouve == -1) { // Si l'étudiant n'est pas trouvé
        printf("Identifiant incorrect\n");
        return;
    }

    // Vérification du numéro de jour (compris entre 1 et 40)
    if (nbJour < 1 || nbJour > MAX_DATE) {
        printf("Date incorrecte\n");
        return;
    }

    // Vérification de la demi-journée (doit être "am" ou "pm")
    enum demiJournee dj;
    if (strcmp(demiJournee, "am") == 0) {
        dj = am;
    }
    else if (strcmp(demiJournee, "pm") == 0) {
        dj = pm;
    }
    else {
        printf("Demi-journee incorrecte\n");
        return;
    }

    // Vérification si l'absence a deja été enregistré
    for (int i = 0; i < *nbAbsences; i++) {
        if (absences[i].idEtu == idEtu && absences[i].date == nbJour && absences[i].demiJournee == dj) {
            printf("Absence deja connue\n");
            return;
        }
    }

    // Enregistrement de l'absence
    Absences* nouvelleAbsence = &absences[*nbAbsences];
    nouvelleAbsence->idAbs = *nbAbsences + 1;  // Identifiant de l'absence
    nouvelleAbsence->idEtu = idEtu;
    nouvelleAbsence->date = nbJour;
    nouvelleAbsence->dateJustif = 0;//date mise a 0 car pas de justificatif
    nouvelleAbsence->demiJournee = dj;
    nouvelleAbsence->estJustifiee = false;  // Non justifiée au début
    nouvelleAbsence->enTraitement = true;  // Absences en cours de traitement
    nouvelleAbsence->estValide = false;  // Absence pas encore validée
    nouvelleAbsence->euValidation = false; // Absences pas en attente de validation
    nouvelleAbsence->recuJustif = false;
    nouvelleAbsence->retard = false;
    nouvelleAbsence->justification[0] = '\0';  // Chaîne vide pour la justification
    nouvelleAbsence->enValidation = false;
    // Copier le nom et le groupe de l'étudiant
    strcpy(nouvelleAbsence->nomEtu, etudiant[etuTrouve].nomEtu);
    nouvelleAbsence->noGroupe = etudiant[etuTrouve].noGroupe;

    // Message de confirmation
    printf("Absence enregistree [%d]\n", nouvelleAbsence->idAbs);

    // Incrémenter le nombre d'absences
    (*nbAbsences)++;
}

// Fonction pour enregistrer les justifications
void justifications(int idAbs, int jourAbs, const char* justification, Absences absences[], int nbAbsences, int* nbAbsencesAttente) {
    int found = 0;

    // Rechercher l'absence correspondant à l'idAbs
    for (int i = 0; i < nbAbsences; i++) {
        if (absences[i].idAbs == idAbs) {
            found = 1;

            // Vérifier si la date du justificatif précède la date de l'absence
            if (jourAbs < absences[i].date) {
                printf("Date incorrecte\n");
                return;
            }

            // Vérifier si un justificatif est déjà enregistré
            if (absences[i].estJustifiee || absences[i].euValidation || absences[i].recuJustif) {
                printf("Justificatif deja connu\n");
                return;
            }

            // Vérifier si le justificatif est déposé dans l'intervalle de temps légal
            if (jourAbs >= absences[i].date && jourAbs <= absences[i].date + MAX_JOUR_POUR_JUSTIF) {
                // Enregistrer le justificatif et marquer l'absence en attente de validation
                strncpy(absences[i].justification, justification, MAX_CARAC_JUSTIFICATION - 1);
                absences[i].justification[MAX_CARAC_JUSTIFICATION - 1] = '\0'; // S'assurer que la chaîne est bien terminée
                absences[i].estJustifiee = true;
                absences[i].enTraitement = true; // Absence en attente de validation
                absences[i].estValide = false;
                absences[i].dateJustif = jourAbs;
                absences[i].recuJustif = true;

                absences[i].enValidation = true;
                (*nbAbsencesAttente)++;  // Incrémenter le nombre d'absences en attente
                printf("Justificatif enregistre\n");
            }
            else {
                // Si le justificatif est hors délai, on le marque comme non validable
                strncpy(absences[i].justification, justification, MAX_CARAC_JUSTIFICATION - 1);
                absences[i].justification[MAX_CARAC_JUSTIFICATION - 1] = '\0'; // S'assurer que la chaîne est bien terminée
                absences[i].estJustifiee = false;
                absences[i].enTraitement = false; // Non valide
                absences[i].estValide = false; // Marquer comme non valide
                absences[i].dateJustif = jourAbs;
                absences[i].recuJustif = true;
                absences[i].retard = true;
                printf("Justificatif enregistre\n");
            }
            return;
        }
    }

    if (!found) {
        printf("Identifiant incorrect\n");
    }
}

// Fonction pour calculer le nombre d'absences jusqu'au jour courant pour un étudiant donné
int calculerAbsences(int idEtu, int jourCourant, Absences absences[], int nbAbsences) {
    int totalAbsences = 0;
    for (int i = 0; i < nbAbsences; i++) {
        if (absences[i].idEtu == idEtu && absences[i].date <= jourCourant) {
            totalAbsences++;
        }
    }
    return totalAbsences;
}

// Fonction pour trier les étudiants par groupe puis par ordre alphabétique
void trierEtudiants(Etudiant etudiants[], int nbEtuInscrits) {
    Etudiant temporaire;

    for (int i = 0; i < nbEtuInscrits - 1; i++) {
        for (int j = 0; j < nbEtuInscrits - i - 1; j++) {
            // Comparer les groupes
            if (etudiants[j].noGroupe > etudiants[j + 1].noGroupe) {
                temporaire = etudiants[j];
                etudiants[j] = etudiants[j + 1];
                etudiants[j + 1] = temporaire;
            }
            // Si les groupes sont égaux, comparer les noms
            else if (etudiants[j].noGroupe == etudiants[j + 1].noGroupe &&
                strcmp(etudiants[j].nomEtu, etudiants[j + 1].nomEtu) > 0) {
                temporaire = etudiants[j];
                etudiants[j] = etudiants[j + 1];
                etudiants[j + 1] = temporaire;
            }
        }
    }
}

// Fonction pour lister les étudiants avec le nombre d'absences jusqu'à un jour donné
void listeEtudiants(int jourCourant, Etudiant etudiants[], int nbEtuInscrits, Absences absences[], int nbAbsences) {
    if (jourCourant < 1) {
        printf("Date incorrecte\n");
        return;
    }

    if (nbEtuInscrits == 0) {
        printf("Aucun inscrit\n");
        return;
    }

    // Trie des étudiants
    trierEtudiants(etudiants, nbEtuInscrits);

    // Calculer le nombre d'absences pour chaque étudiant jusqu'au jour courant
    for (int i = 0; i < nbEtuInscrits; i++) {
        etudiants[i].nbAbsencesEtu = calculerAbsences(etudiants[i].idEtu, jourCourant, absences, nbAbsences);
    }

    // Afficher la liste des étudiants
    for (int i = 0; i < nbEtuInscrits; i++) {
        printf("(%d) %-10s %1d %2d\n", etudiants[i].idEtu, etudiants[i].nomEtu, etudiants[i].noGroupe, etudiants[i].nbAbsencesEtu);
    }
}

void etudiantInfo(Etudiant* etudiants, int nbEtuInscrits, Absences* absences, int nbAbsences, int idEtu, int jourCourant) {
    int etuTrouve = -1;

    for (int i = 0; i < nbEtuInscrits; i++) {
        if (etudiants[i].idEtu == idEtu) {
            etuTrouve = i;
            break;
        }
    }
    // Si l'étudiant n'est pas trouvé
    if (etuTrouve == -1) {
        printf("Identifiant incorrect\n");
        return;
    }
    // Si la date est incorrecte
    if (jourCourant < 1) {
        printf("Date incorrecte\n");
        return;
    }

    // Mise à jour du nombre d'absences pour l'étudiant concerné
    etudiants[etuTrouve].nbAbsencesEtu = calculerAbsences(etudiants[etuTrouve].idEtu, jourCourant, absences, nbAbsences);

    // Mise à jour de l'état des absences pour l'étudiant concerné en fonction du jour courant
    for (int i = 0; i < nbAbsences; i++) {
        if (((absences[i].idEtu == idEtu) && (jourCourant - absences[i].date > 3) && (absences[i].enTraitement)
            && (!absences[i].estJustifiee) && (!absences[i].recuJustif)) ||
            ((absences[i].idEtu == idEtu) && (jourCourant - absences[i].date > 3) && (!absences[i].enTraitement || absences[i].enTraitement)
                && (!absences[i].estJustifiee) && (absences[i].recuJustif) && (absences[i].retard)) == 1) {
            absences[i].estValide = false;  // Absence considérée comme non justifiée après 3 jours sans justificatif
            absences[i].enTraitement = false;
        }
        if (((absences[i].dateJustif > jourCourant) && (absences[i].estJustifiee)//attente de justif
            && (absences[i].enTraitement) && (!absences[i].euValidation)) == 1) {
            absences[i].estJustifiee = false;
        }
        if (((absences[i].dateJustif <= jourCourant && absences[i].dateJustif > 0) && (!absences[i].estJustifiee)
            && (absences[i].enTraitement) && (!absences[i].euValidation)) == 1) {
            absences[i].estJustifiee = true;
        }
        if (((absences[i].date + 3 >= jourCourant) && (!absences[i].estJustifiee) &&
            (!absences[i].enTraitement) && (!absences[i].estValide) && (!absences[i].euValidation) && (!absences[i].retard)) == 1) {
            absences[i].enTraitement = true;
        }
        if (((absences[i].dateJustif > jourCourant) && (!absences[i].estJustifiee)//attente de justif
            && (!absences[i].enTraitement) && (absences[i].euValidation)) == 1) {
            absences[i].enTraitement = true;
        }
        if (((absences[i].dateJustif <= jourCourant) && (!absences[i].estJustifiee)//attente de justif
            && (absences[i].enTraitement) && (absences[i].euValidation)) == 1) {
            absences[i].enTraitement = false;
        }
        if (((absences[i].dateJustif > jourCourant && jourCourant - absences[i].date <= 3) && (absences[i].retard))) {
            absences[i].enTraitement = true;
            absences[i].estJustifiee = false;
        }
    }

    // Affichage des informations de l'étudiant trouvé
    printf("(%d) %s %d %d\n", etudiants[etuTrouve].idEtu, etudiants[etuTrouve].nomEtu, etudiants[etuTrouve].noGroupe, etudiants[etuTrouve].nbAbsencesEtu);

    // Variables pour vérifier s'il y a des absences dans chaque catégorie
    bool hasAttenteJustificatif = false;
    bool hasAttenteValidation = false;
    bool hasJustifiees = false;
    bool hasNonJustifiees = false;

    qsort(absences, nbAbsences, sizeof(Absences), comparerAbsences);//trier les etudiants par dates puis par demi-journée

    // Absences en attente de justificatif
    for (int i = 0; i < nbAbsences; ++i) {
        if (((absences[i].idEtu == idEtu) && (!absences[i].estJustifiee) && (absences[i].enTraitement) &&
            (absences[i].date <= jourCourant)) ||
            (((absences[i].idEtu == idEtu) && (absences[i].estJustifiee) && (!absences[i].enTraitement) &&
                (absences[i].date <= jourCourant && absences[i].dateJustif > jourCourant))) == 1) {
            if (!hasAttenteJustificatif) {
                printf("-En attente justificatif\n");
                hasAttenteJustificatif = true;
            }
            char dj[DEMI_JOURNEE];
            strcpy(dj, absences[i].demiJournee == am ? "am" : "pm");
            printf("[%d] %d/%s \n", absences[i].idAbs, absences[i].date, dj);
        }
    }

    // Absences en attente de validation
    for (int i = 0; i < nbAbsences; ++i) {
        if (absences[i].idEtu == idEtu && absences[i].estJustifiee && absences[i].enTraitement && absences[i].date <= jourCourant) {
            if (!hasAttenteValidation) {
                printf("-En attente validation\n");
                hasAttenteValidation = true;
            }
            char dj[DEMI_JOURNEE];
            strcpy(dj, absences[i].demiJournee == am ? "am" : "pm");
            if (absences[i].recuJustif == true) {
                printf("[%d] %d/%s (%s)\n", absences[i].idAbs, absences[i].date, dj, absences[i].justification);
            }
            else {
                printf("[%d] %d/%s \n", absences[i].idAbs, absences[i].date, dj);
            }
        }
    }
    // Absences justifiées
    for (int i = 0; i < nbAbsences; ++i) {
        if (((absences[i].idEtu == idEtu) && (absences[i].estValide) && (absences[i].date <= jourCourant && absences[i].dateJustif <= jourCourant)) == 1) {
            if (!hasJustifiees) {
                printf("-Justifiees\n");
                hasJustifiees = true;
            }
            char dj[DEMI_JOURNEE];
            strcpy(dj, absences[i].demiJournee == am ? "am" : "pm");
            if (absences[i].recuJustif == true) {
                printf("[%d] %d/%s (%s)\n", absences[i].idAbs, absences[i].date, dj, absences[i].justification);
            }
            else {
                printf("[%d] %d/%s \n", absences[i].idAbs, absences[i].date, dj);
            }
        }
    }
    // Absences non-justifiées
    for (int i = 0; i < nbAbsences; ++i) {
        if (((absences[i].idEtu == idEtu) && (!absences[i].estValide) && (!absences[i].enTraitement)
            && (!absences[i].estJustifiee) && (absences[i].date <= jourCourant)) == 1) {
            if (!hasNonJustifiees) {
                printf("-Non-justifiees\n");
                hasNonJustifiees = true;
            }
            char dj[DEMI_JOURNEE];
            strcpy(dj, absences[i].demiJournee == am ? "am" : "pm");
            if (absences[i].recuJustif == true) {
                printf("[%d] %d/%s (%s)\n", absences[i].idAbs, absences[i].date, dj, absences[i].justification);
            }
            else {
                printf("[%d] %d/%s \n", absences[i].idAbs, absences[i].date, dj);
            }
        }
    }
}

void enValidations(Absences absences[], const int* nbAbsences) {
    bool validationExistante = false; // pour voir si il y'a des absences qui attend validation

    enum demiJournee demijournee;

    short int num_dj = 1;

    // tri identifiant d'étudiant puis par date
    for (int i = 0; i < *nbAbsences; i++) {
        for (int j = i + 1; j < *nbAbsences; j++) {
            // Si identifiant étudiant est le même et que la date de l'absence j est antérieure à celle de i on echange
            if (absences[i].idEtu > absences[j].idEtu ||
                (absences[i].idEtu == absences[j].idEtu && absences[i].date > absences[j].date)) {
                Absences temp = absences[i];
                absences[i] = absences[j];
                absences[j] = temp;
            }
        }
    }

    // absences en attente de validation
    for (int i = 0; i < *nbAbsences; i++) {
        if (absences[i].enValidation) {
            char dj[DEMI_JOURNEE];
            validationExistante = true;
            // détails absences
            if (absences[i].demiJournee == am) {
                strcpy(dj, "am");
            }
            if (absences[i].demiJournee == pm) {
                strcpy(dj, "pm");
            }

            printf("[%d] (%d) %-8s %3d %2d/%s (%s)\n",
                absences[i].idAbs, absences[i].idEtu, absences[i].nomEtu,
                absences[i].noGroupe, absences[i].date, dj,
                absences[i].justification);
        }
    }

    // si aucune validation trouvée
    if (!validationExistante) {
        printf("Aucune validation en attente\n");
    }
}

void validationJustification(int idAbs, const char* validation, Absences absences[], const int* nbAbsences) {
    int absenceTrouvee = -1;

    // Vérifier l'existence de l'absence
    for (int i = 0; i < *nbAbsences; i++) {
        if (absences[i].idAbs == idAbs) {
            absenceTrouvee = i;

            // Vérifier si l'absence est en attente de validation
            if ((absences[absenceTrouvee].euValidation) || (absences[absenceTrouvee].retard)) {
                printf("Validation deja connue\n");
                return;
            }
            if (!absences[absenceTrouvee].recuJustif) {
                printf("Identifiant incorrect\n");
                return;
            }
            // Traiter la validation
            if (strcmp(validation, "ok") == 0) {
                absences[absenceTrouvee].estValide = true;  // L'absence est validée
                absences[absenceTrouvee].enTraitement = false; // L'absence n'est plus en traitement
                absences[absenceTrouvee].euValidation = true;

                absences[absenceTrouvee].enValidation = false;
                printf("Validation enregistree\n");
            }
            else if (strcmp(validation, "ko") == 0) {
                absences[absenceTrouvee].estValide = false; // L'absence est rejetée
                absences[absenceTrouvee].enTraitement = false; // L'absence n'est plus en traitement
                absences[absenceTrouvee].estJustifiee = false; // Absence considéré comme non justifiée
                absences[absenceTrouvee].euValidation = true;

                absences[absenceTrouvee].enValidation = false;
                printf("Validation enregistree\n");
            }
            else {
                printf("Code incorrect\n");
            }
            return;
        }
    }

    if (absenceTrouvee == -1) {
        printf("Identifiant incorrect\n");
    }
}

void defaillants(int jourCourant, int nbEtuInscrits, int nbAbsences, Absences absences[], Etudiant etudiant[]) {

    if (jourCourant < 1) {
        printf("Date Incorrect\n");
        return;
    }

    bool defaillantTrouve = false;

    // Trie des étudiants
    trierEtudiants(etudiant, nbEtuInscrits);

    // Parcourir les étudiants pour calculer leurs absences non justifiées et non en attente de validation
    for (int i = 0; i < nbEtuInscrits; i++) {
        int absencesNonValide = 0;

        // Parcourir les absences pour cet étudiant
        for (int j = 0; j < nbAbsences; j++) {
            if (absences[j].idEtu == etudiant[i].idEtu && absences[j].date <= jourCourant) {
                // Condition pour savoir si l'absence est non justifiée
                bool absenceNonJustifiee = //demander à Matteo
                    // Cas 1: Pas de justificatif et le délai de 3 jours est écoulé
                    (!absences[j].retard && jourCourant > absences[j].date + MAX_JOUR_POUR_JUSTIF) ||

                    // Cas 2: Un justificatif a été fourni mais il est invalidé
                    (absences[j].retard && !absences[j].estValide) ||

                    // Cas 3: Un justificatif est en attente de validation, et il n'est pas encore validé
                    (absences[j].euValidation && !absences[j].estValide);

                // Si l'une des conditions est vraie, on incrémente le nombre d'absences non justifiées
                if (absenceNonJustifiee) {
                    absencesNonValide++;
                }
            }

            // Si l'étudiant a plus d'une absence non justifiée, il est défaillant
            if (absencesNonValide >= ABS_DEFAILLANT) {
                defaillantTrouve = true;
                printf("(%d) %-10s %3d %2d\n", etudiant[i].idEtu, etudiant[i].nomEtu, etudiant[i].noGroupe, etudiant[i].nbAbsencesEtu);
                break; // Inutile de compter plus d'absences pour cet étudiant
            }
        }
    }
    // Si aucun étudiant n'est trouvé comme défaillant
    if (!defaillantTrouve) {
        printf("Aucun defaillant\n");
    }
}
