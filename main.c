#include "Projet.c"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{

    int fichier_charge_avec_succes=0;

    // On crée une base de connaissance vide
    liste_BC *base_connaissances = creer_base_vide();
    // On crée une base de faits vide
    liste_BF *base_faits = creer_base_vide_BF();

    // on charge les valeurs de la base de connaissances et de la base de faits et on les introduits dans les bases vides à l'aide des fonctions nécessaires
    fichier_charge_avec_succes = charger_base_de_connaissances_et_faits("Sauvegarde.txt", base_connaissances, base_faits);

    // // On affiche la base de faits (qui affichera donc la base de faits avec les nouveaux faits ajoutés après l'éxecution du moteur d'inference)
    // printf("on a %d faits et %d regles:\n", base_faits->nb_elem,base_connaissances->nb_elem);
    // proposition * liste_fait = base_faits->BF;
    // while (liste_fait!=NULL){
    //     printf("%s\n", liste_fait->value);
    //     liste_fait = liste_fait->next;
    // }

    if (fichier_charge_avec_succes == 1)
    {
        // Début du moteur d'inférence
        // on crée un pointeur sur la première proposition de la base de faits
        proposition *fait = base_faits->BF;
        // on crée un pointeur sur la première regle de la base de connaissances
        elem_BC *regle = acceder_regle_tete(base_connaissances);
        
        // tant qu'il y a un fait dans la base de faits
        while (fait != NULL)
        {
            printf("On regarde pour le fait %s\n", fait->value);
            // tant qu'il y a une regle dans la base de connaissances
            regle = acceder_regle_tete(base_connaissances);
            while (regle != NULL)
            {
                // si le fait est dans la premisse de la regle
                if (appartient_premisse(regle, fait->value))
                {
                    // on supprime le fait de la premisse de la regle
                    regle = supprimer_proposition(regle, fait->value);

                    // si la premisse de la regle est vide
                    if (premisse_est_vide(regle))
                    {
                        // on ajoute la conclusion à la base de faits
                        // on vérifie que la conclusion contient bien une valeur
                        if (acceder_conclusion(regle) != NULL)
                        {                           
                            ajouter_proposition_BF(base_faits, acceder_conclusion(regle)->value);
                            base_connaissances = supprimer_regle_vide(base_connaissances);
                        }
                    }
                }
                // on passe à la règle suivante
                regle = regle->prochain;
            }
        // On regarde si il y a un prochain fait dans la base de faits sinon on retourne null et on arrete le moteur d'inference
        fait = fait->next;
        regle = acceder_regle_tete(base_connaissances);
        }
    }

   afficherBCBF(base_faits,base_connaissances);


    // Libération de la mémoire allouée dynamiquement
    //  Libération de la base de faits
    proposition *facteur_actuel, *prochain_facteur;
    facteur_actuel = base_faits->BF;
    while (facteur_actuel != NULL)
    {
        prochain_facteur = facteur_actuel->next;
        free(facteur_actuel->value); // Si 'value' est alloué dynamiquement
        free(facteur_actuel);
        facteur_actuel = prochain_facteur;
    }
    free(base_faits);

    // Libération de la base de connaissances
    elem_BC *regle_actuelle, *prochaine_regle;
    regle_actuelle = base_connaissances->BC;
    while (regle_actuelle != NULL)
    {
        prochaine_regle = regle_actuelle->prochain;

        // Libération des propositions dans la prémisse
        proposition *prop_actuelle, *prochaine_prop;
        prop_actuelle = regle_actuelle->premisse;
        while (prop_actuelle != NULL && prop_actuelle != regle_actuelle->conclusion)
        {
            prochaine_prop = prop_actuelle->next;
            free(prop_actuelle->value);
            free(prop_actuelle);
            prop_actuelle = prochaine_prop;
        }

        // Libération de la conclusion si elle est différente de NULL
        if (regle_actuelle->conclusion != NULL)
        {
            free(regle_actuelle->conclusion->value);
            free(regle_actuelle->conclusion);
        }

        free(regle_actuelle);
        regle_actuelle = prochaine_regle;
    }
    free(base_connaissances);
    return 0;
}

// moteur d'inférence
// on a une base de faits, avec des propositions de faits certains
// on a une base de connaissances, avec des propositions etant la premisse et une conclusion

// on va prendre les propositions de faits certains un par un jusqu'il y en a plus
// puis regarder dans chaque regle si cette proposition est present, jusqu'a la dernière règle
// s'il est present on enleve cette propositon de la premisse de la regle en question
// si il n'y a plus de premisse alors on ajoute la conclusion a la base de faits
// besoin de fonction ajout proposition a la base de faits, et verifier si la proposition n'appartient pas déja a la base de faits
// si next du fait different de null alors le fait qu'on verifie, prend la valeur de next de fait
// sinon fin du programme on affiche la base de faits