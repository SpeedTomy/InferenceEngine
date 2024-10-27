#include "Projet.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>





// Fonctions sur les regles

elem_BC* creer_regle_vide() {
    elem_BC *nouvelle_regle = (elem_BC*)malloc(sizeof(elem_BC));
    if (nouvelle_regle) {
        nouvelle_regle->premisse = NULL;
        nouvelle_regle->conclusion = NULL;
        nouvelle_regle->prochain = NULL;
    }
    return nouvelle_regle;
}

elem_BC* ajouter_proposition(elem_BC *regle, char *valeur) {
    proposition *nouvelle_prop = (proposition*)malloc(sizeof(proposition));
    nouvelle_prop->value = strdup(valeur);
    nouvelle_prop->next = regle->conclusion; // Pointe directement vers la conclusion existante

    if (regle->premisse == NULL) {
        // Si la prémisse est vide, la nouvelle proposition devient la première de la liste
        regle->premisse = nouvelle_prop;
        nouvelle_prop->prev = NULL;
    } else {
        // Sinon, insérer la nouvelle proposition juste avant la conclusion
        if (regle->conclusion && regle->conclusion->prev) {
            regle->conclusion->prev->next = nouvelle_prop;
            nouvelle_prop->prev = regle->conclusion->prev;
            regle->conclusion->prev = nouvelle_prop;
        } else {
            // Dans le cas où il n'y a pas encore de conclusion
            proposition *derniere = regle->premisse;
            while (derniere->next != NULL) {
                derniere = derniere->next;
            }
            derniere->next = nouvelle_prop;
            nouvelle_prop->prev = derniere;
        }
    }
    return regle;
}

elem_BC* creer_conclusion(elem_BC *regle, char *valeur) {
    // Si une conclusion existe déjà, remplacez simplement sa valeur.
    if (regle->conclusion != NULL) {
        free(regle->conclusion->value);
        regle->conclusion->value = strdup(valeur);
    } else {
        // Création de la nouvelle conclusion
        proposition *concl = (proposition*)malloc(sizeof(proposition));
        concl->value = strdup(valeur);
        concl->next = NULL; // La conclusion est le dernier élément, donc next est NULL
        concl->prev = NULL; // Initialisé à NULL, sera mis à jour si nécessaire

        if (regle->premisse != NULL) {
             // insérer la conclusion à la fin de la liste
            proposition *tmp = regle->premisse;
            while (tmp->next != NULL) {
                tmp = tmp->next;
            }
            tmp->next = concl;
            concl->prev = tmp;
        } 

        // Mettre à jour le pointeur de conclusion de la règle
        regle->conclusion = concl;
    }

    return regle;
}


int appartient_premisse_recursif(proposition *courante, proposition *conclusion, char *valeur) {
    // Vérifier si on est arrivé à la fin de la prémisse ou à la conclusion
    if (courante == NULL || courante == conclusion) {
        return 0;
    }

    // Vérifier si la valeur courante correspond
    if (strcmp(courante->value, valeur) == 0) {
        return 1;
    }

    // Appel récursif avec la proposition suivante
    return appartient_premisse_recursif(courante->next, conclusion, valeur);
}
int appartient_premisse(elem_BC *regle, char *valeur) {
    // Vérifier si la règle ou la valeur est NULL
    if (regle == NULL || valeur == NULL) {
        return 0;
    }

    // Appel initial de la fonction récursive
    return appartient_premisse_recursif(regle->premisse, regle->conclusion, valeur);
}

elem_BC* supprimer_proposition(elem_BC *regle, char *valeur) {
    if (regle == NULL || valeur == NULL || regle->premisse == NULL) {
        return regle; // Rien à faire si la règle est vide ou la valeur est NULL
    }

    proposition *courante = regle->premisse;
    proposition *precedente = NULL;

    while (courante != NULL && courante != regle->conclusion) {
        if (strcmp(courante->value, valeur) == 0) {
            if (precedente == NULL) {
                // La proposition à supprimer est la première de la prémisse
                regle->premisse = courante->next;
            } else {
                // Relier le précédent au suivant, en sautant la proposition actuelle
                precedente->next = courante->next;
                if (courante->next != NULL) {
                    courante->next->prev = precedente;
                }
            }
            // Libérer la mémoire de la proposition supprimée
            free(courante->value);
            free(courante);
            break;
        }
        precedente = courante;
        courante = courante->next;
    }

    return regle;
}

int premisse_est_vide(elem_BC *regle) {
    if (regle == NULL) {
        // Si la règle est NULL, la prémisse est considérée comme vide.
        return 1;
    }

    // Vérifie si la prémisse pointe directement sur la conclusion.
    // Si c'est le cas, cela signifie que la prémisse est vide.
    if (regle->premisse == regle->conclusion){
        return 1;
    } else {
        return 0;
    }
}

proposition* premiere_proposition(elem_BC *regle) {
    if (regle == NULL || regle->premisse == NULL) {
        return NULL; // Retourner NULL si la règle est NULL ou si la prémisse est vide
    }

    return regle->premisse;
}

proposition* acceder_conclusion(elem_BC *regle) {
    if (regle == NULL || regle->conclusion == NULL) {
        return NULL; // Retourner NULL si la règle est NULL
    }

    return regle->conclusion;
}





// Fonctions sur la base de connaissances

liste_BC* creer_base_vide() {
    liste_BC *base = (liste_BC*)malloc(sizeof(liste_BC));
    if (base) {
        base->BC = NULL;
        base->nb_elem = 0;
    }
    return base;
}

liste_BC* ajouter_regle(liste_BC *base, elem_BC *nouvelle_regle) {
    if (base == NULL) {
        return NULL; // Renvoie NULL si la base est NULL
    }

    if (nouvelle_regle == NULL) {
        return base; // Renvoie la base telle qu'elle est si la nouvelle règle est NULL
    }

    // Ajouter la nouvelle règle
    if (base->BC == NULL) {
        base->BC = nouvelle_regle;
    } else {
        elem_BC *tmp = base->BC;
        while (tmp->prochain != NULL) {
            tmp = tmp->prochain;
        }
        tmp->prochain = nouvelle_regle;
    }

    base->nb_elem++;
    return base;
}

elem_BC* acceder_regle_tete(liste_BC *base) {
    if (base == NULL || base->BC == NULL) {
        return NULL; // Retourner NULL si la base est NULL ou vide
    }

    return base->BC;
}

liste_BC * supprimer_regle_vide(liste_BC *base) {
    if (base == NULL || base->BC == NULL) {
        return base; // Rien à faire si la base est vide
    }

    elem_BC *regle_actuelle = base->BC;
    elem_BC *regle_precedente = NULL;

    while (regle_actuelle != NULL) {
        if (premisse_est_vide(regle_actuelle)) {
            // Si c'est la première règle
            if (regle_precedente == NULL) {
                base->BC = regle_actuelle->prochain;
            } else {
                regle_precedente->prochain = regle_actuelle->prochain;
            }

            // Libérer la mémoire de la règle actuelle
            if (regle_actuelle->conclusion != NULL) {
                free(regle_actuelle->conclusion->value);
                free(regle_actuelle->conclusion);
            }
            elem_BC *regle_a_supprimer = regle_actuelle;
            regle_actuelle = regle_actuelle->prochain; // Avant de libérer la mémoire
            free(regle_a_supprimer);

            base->nb_elem--;
        } else {
            // Passer à la règle suivante
            regle_precedente = regle_actuelle;
            regle_actuelle = regle_actuelle->prochain;
        }
    }

    return base;
}





// Fonctions Base de faits

liste_BF* creer_base_vide_BF() {
    liste_BF *base = (liste_BF*)malloc(sizeof(liste_BF));
    if (base) {
        base->BF = NULL;
        base->nb_elem = 0;
    }
    return base;
}

void ajouter_proposition_BF(liste_BF *base, char *valeur) {

    if (base == NULL || valeur == NULL) {
        return; // Ne rien faire si la base est NULL ou si la valeur est NULL
    }

    // Vérifier si la proposition existe déjà dans la base de faits
    proposition *courante = base->BF;
    while (courante != NULL) {
        if (strcmp(courante->value, valeur) == 0) {
            // La proposition existe déjà, donc on ne l'ajoute pas
            return;
        }
        courante = courante->next;
    }

    proposition *nouvelle_prop = (proposition*)malloc(sizeof(proposition));
    nouvelle_prop->value = strdup(valeur);
    nouvelle_prop->next = NULL;

    if (base != NULL){
        if (base->BF == NULL) {
            // Si la base est vide, la nouvelle proposition devient la première de la liste de faits
            base->BF = nouvelle_prop;
            nouvelle_prop->prev = NULL;
        } else {
            // Sinon, trouvez la fin de la liste et ajoutez la proposition
            proposition *tmp = base->BF;
            while (tmp->next != NULL) {
                tmp = tmp->next;
            }
            nouvelle_prop->prev = tmp;
            tmp->next = nouvelle_prop;
        }

        base->nb_elem++; // Mettre à jour le compteur de faits
    }
}


//Fonction fichier
int charger_base_de_connaissances_et_faits(const char *nom_fichier, liste_BC *base_connaissances, liste_BF *base_faits) {
    FILE *fichier = fopen(nom_fichier, "r");
    printf("Chargement du fichier %s...\n", nom_fichier);
    char ligne[1024];
    char *token;

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return 0; //indique que le chargement s'est mal passé
    }

    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Enlever le saut de ligne à la fin si présent
        ligne[strcspn(ligne, "\n")] = 0;

        // Vérifier si la ligne contient une règle ou un fait
        if (strstr(ligne, "=>")) { // C'est une règle
            elem_BC *nouvelle_regle = creer_regle_vide();

            // Extraire les prémisses
            token = strtok(ligne, " ,"); // Séparer par des espaces et des virgules
            while (token != NULL && strstr(token, "=>") == NULL) {
                nouvelle_regle = ajouter_proposition(nouvelle_regle, token);
                token = strtok(NULL, " ,");
            }

            // Sauter le "=>" et extraire la conclusion
            if (token != NULL && strstr(token, "=>") != NULL) {
                token = strtok(NULL, " ,"); // Passez à la conclusion
                if (token != NULL) {
                    nouvelle_regle = creer_conclusion(nouvelle_regle, token);
                    // Ajouter la règle à la base de connaissances
                    base_connaissances = ajouter_regle(base_connaissances, nouvelle_regle);
                }
            }

        } else { // C'est un fait
            // Ajouter le fait à la base de faits
            ajouter_proposition_BF(base_faits, ligne);
        }
    }

    fclose(fichier);
    printf("Chargement termine.\n");
    return 1; //indique que le chargement s'est bien passé
}

void afficherBCBF(liste_BF* base_faits, liste_BC* base_connaissances){

     // On affiche la base de faits (qui affichera donc la base de faits avec les nouveaux faits ajoutés après l'éxecution du moteur d'inference)
    printf("\n\non a %d faits et %d regles:\n", base_faits->nb_elem, base_connaissances->nb_elem);
    proposition *liste_fait = base_faits->BF;
    while (liste_fait != NULL)
    {
        printf("%s\n", liste_fait->value);
        liste_fait = liste_fait->next;
    }

    // On affiche la base de connaissances (les règles)
    printf("Liste des regles:\n");
    elem_BC *liste_regle = base_connaissances->BC;
    while (liste_regle != NULL)
    {
        proposition *courante = liste_regle->premisse;
        while (courante != NULL && courante != liste_regle->conclusion)
        {
            printf("%s, ", courante->value);
            courante = courante->next;
        }
        if (liste_regle->conclusion != NULL)
        {
            printf("=> %s\n", liste_regle->conclusion->value);
        }
        else
        {
            printf("=> NULL\n");
        }
        liste_regle = liste_regle->prochain;
    }

}