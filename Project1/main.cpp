#include <iostream>
#include <math.h>
#include <future>
#include <stdio.h>
#include <chrono>
#include <mutex>
#include <fstream>
#include <string>
#include <vector>

/// @brief Mutex pour protéger l'accès aux ressources partagées.
std::mutex mt;

/**
 * @brief Vérifie si un nombre est premier.
 *
 * Cette fonction détermine si un nombre donné est premier en testant sa divisibilité.
 *
 * @param value Le nombre entier à vérifier.
 * @return true Si le nombre est premier.
 * @return false Si le nombre n'est pas premier.
 */
bool isPremier(int value)
{
    if (value == 2)
    {
        return true;
    }
    else if (value == 1 || value % 2 == 0)
    {
        return false;
    }
    for (int i = 3; i <= round(sqrt(value)); i += 2)
    {
        if (value % i == 0)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Recherche les nombres premiers dans une plage donnée et les ajoute à une liste.
 *
 * Cette fonction parcourt une plage de nombres, identifie ceux qui sont premiers,
 * puis les ajoute à une liste partagée de nombres premiers.
 *
 * @param debut Le début de la plage à analyser.
 * @param fin La fin de la plage à analyser.
 * @param pas L'incrément entre chaque nombre à vérifier (utilisé pour distribuer le travail entre threads).
 * @param listePremier Pointeur partagé vers un vecteur où seront stockés les nombres premiers trouvés.
 */
void premiers(int debut, int fin, int pas, std::shared_ptr<std::vector<int>> listePremier)
{
    for (int i = debut; i < fin; i += pas)
    {
        if (isPremier(i))
        {
            mt.lock();
            listePremier->push_back(i);
            mt.unlock();
        }
    }
}

/**
 * @brief Lance le calcul des nombres premiers dans une plage, en utilisant plusieurs threads.
 *
 * Cette fonction crée un nombre spécifié de threads, chacun calculant les nombres premiers
 * dans des sous-intervalles de la plage donnée. Les résultats sont ensuite enregistrés dans un fichier.
 *
 * @param threadsUsed Le nombre de threads à utiliser pour le calcul.
 * @param fin La limite supérieure de la plage de nombres à analyser.
 */
void calculNombre(int threadsUsed, int fin)
{
    // Liste des threads pour le calcul
    std::vector<std::thread> listeAsync;
    std::shared_ptr<std::vector<int>> listePremiers = std::make_shared<std::vector<int>>();

    // Mesurer le temps de calcul
    auto begin = std::chrono::high_resolution_clock::now();

    // Créer et lancer les threads
    for (int i = 0; i < threadsUsed; i++)
    {
        listeAsync.push_back(std::thread(premiers, i, fin, threadsUsed, listePremiers));
    }

    // Attendre la fin de tous les threads
    for (int i = 0; i < threadsUsed; i++)
    {
        listeAsync[i].join();
    }

    // Écrire les résultats dans un fichier
    std::ofstream fichier("data" + std::to_string(threadsUsed) + "Thread" + std::to_string(fin) + "Value.txt");
    if (fichier)
    {
        fichier.clear();
        for (auto& value : *listePremiers)
        {
            fichier << value << std::endl;
        }
    }

    // Calculer le temps écoulé et afficher le résultat
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

    std::cout << "Temps pour " << threadsUsed << " Thread(s)\t     pour  " << fin << " nombres \t: " << elapsed.count() << " microsecondes\n";
}

/**
 * @brief Point d'entrée du programme.
 *
 * Le programme calcule et enregistre les nombres premiers pour plusieurs intervalles et différents nombres de threads.
 *
 * @return int Code de retour du programme.
 */
int main()
{
    unsigned int numThreads = std::thread::hardware_concurrency();

    // Calcul des nombres premiers avec différents nombres de threads et tailles d'intervalle
    calculNombre(1, 10000);
    calculNombre(numThreads, 10000);
    calculNombre(1, 100000);
    calculNombre(numThreads, 100000);
    calculNombre(1, 1000000);
    calculNombre(numThreads, 1000000);

    return 0;
}
