#include <jni.h>
#include <string>
#include <algorithm>
#include <climits>
#include <android/log.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <errno.h>

/**
 * Configuration des Logs pour le débogage et la surveillance de sécurité.
 * On utilise un tag spécifique pour filtrer facilement dans Logcat.
 */
#define TAG_SEC "JNI_SECURITY"
#define LOG_SEC(...) __android_log_print(ANDROID_LOG_WARN, TAG_SEC, __VA_ARGS__)

/**
 * 🛠 MÉTHODE 1 : Analyse du TracerPid
 * Sur Android (système Linux), l'état de chaque processus est stocké dans /proc/self/status.
 * Le champ 'TracerPid' indique l'identifiant du processus qui "trace" (débogage) l'application.
 * Valeur 0 = Pas de debugger.
 * Valeur > 0 = Un outil (GDB, LLDB, etc.) est attaché au processus.
 */
bool isTracerPidDetected() {
    // Tentative d'ouverture du fichier de statut du processus actuel
    FILE* f = fopen("/proc/self/status", "r");
    if (!f) return false; // En cas d'échec d'ouverture, on assume que c'est ok

    char buffer[256];
    int tracerPidValue = 0;

    // Lecture ligne par ligne pour trouver le champ spécifique
    while (fgets(buffer, sizeof(buffer), f)) {
        if (strncmp(buffer, "TracerPid:", 10) == 0) {
            // Conversion de la valeur trouvée après le label "TracerPid:"
            tracerPidValue = atoi(&buffer[10]);
            break;
        }
    }
    fclose(f);

    // Si le PID du traceur n'est pas nul, une menace est détectée
    if (tracerPidValue != 0) {
        LOG_SEC("ALERTE : Un processus de traçage a été identifié (PID: %d)", tracerPidValue);
        return true;
    }
    return false;
}

/**
 * 🛠 MÉTHODE 2 : Test d'attachement via ptrace
 * Le mécanisme ptrace ne permet qu'à un seul processus "parent" de tracer un processus "enfant".
 * En essayant de s'auto-tracer (PTRACE_TRACEME), on peut savoir si quelqu'un d'autre occupe déjà la place.
 */
bool isPtraceDetected() {
    // Si ptrace renvoie -1, une erreur est survenue
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
        // L'erreur EPERM signifie que l'attachement est refusé car déjà occupé
        if (errno == EPERM) {
            LOG_SEC("ALERTE : ptrace a été bloqué par un debugger déjà présent");
            return true;
        }
        // Les autres erreurs (EACCES) peuvent être liées aux politiques SELinux sur émulateur
        return false;
    }
    // Si l'attachement réussit, on libère immédiatement le lien
    ptrace(PTRACE_DETACH, 0, NULL, NULL);
    return false;
}

/**
 * 🛠 MÉTHODE 3 : Scan de la cartographie mémoire (/proc/self/maps)
 * Cette méthode permet de voir quelles bibliothèques dynamiques (.so) sont chargées.
 * On recherche des noms caractéristiques d'outils d'instrumentation ou de "hooking".
 */
bool isSuspiciousEnvironment() {
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) return false;

    char buffer[512];
    bool isSuspect = false;

    // On parcourt la liste des segments mémoire chargés
    while (fgets(buffer, sizeof(buffer), f)) {
        // Détection de Frida ou Xposed, outils majeurs de modification d'applis
        if (strstr(buffer, "frida") || strstr(buffer, "xposed")) {
            LOG_SEC("MENACE MÉMOIRE : Signature d'outil détectée dans maps");
            isSuspect = true;
            break;
        }
    }
    fclose(f);
    return isSuspect;
}

// =============================================================================
// FONCTIONS MÉTIER (LOGIQUE DE L'APPLICATION)
// =============================================================================

/**
 * Retourne un message de bienvenue stocké dans le code natif.
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_MainActivity_getNativeMessage(JNIEnv* env, jobject) {
    return env->NewStringUTF("Accès autorisé : Bienvenue dans la zone sécurisée C++");
}

/**
 * Calcule le produit cumulé (factoriel) de manière itérative.
 * Gère les cas d'erreur comme les nombres négatifs ou le dépassement de capacité.
 */
extern "C" JNIEXPORT jint JNICALL
Java_com_example_jnidemo_MainActivity_computeProduct(JNIEnv*, jobject, jint n) {
    if (n < 0) return -1; // Code d'erreur pour entrée invalide

    long long accumulator = 1;
    for (int i = 1; i <= n; i++) {
        accumulator *= i;
        // Protection contre l'overflow d'un entier 32 bits
        if (accumulator > INT_MAX) return -2;
    }
    return (jint) accumulator;
}

/**
 * Inverse une chaîne de caractères reçue depuis Java.
 * Utilise la bibliothèque standard C++ pour la manipulation.
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_MainActivity_flipText(JNIEnv* env, jobject, jstring input) {
    if (input == nullptr) return env->NewStringUTF("");

    // Conversion de jstring (UTF-16) en C-string (UTF-8)
    const char* nativeString = env->GetStringUTFChars(input, nullptr);
    std::string workingCopy(nativeString);

    // Libération immédiate pour éviter les fuites de mémoire JNI
    env->ReleaseStringUTFChars(input, nativeString);

    // Utilisation de l'algorithme d'inversion
    std::reverse(workingCopy.begin(), workingCopy.end());

    return env->NewStringUTF(workingCopy.c_str());
}

/**
 * Calcule la somme de tous les entiers d'un tableau Java.
 */
extern "C" JNIEXPORT jint JNICALL
Java_com_example_jnidemo_MainActivity_calculateSum(JNIEnv* env, jobject, jintArray data) {
    if (data == nullptr) return 0;

    jsize arrayLen = env->GetArrayLength(data);
    jint* elements = env->GetIntArrayElements(data, nullptr);

    long long totalSum = 0;
    for (int i = 0; i < arrayLen; i++) {
        totalSum += elements[i];
    }

    // Libération du pointeur vers le tableau
    env->ReleaseIntArrayElements(data, elements, 0);

    // Vérification de dépassement avant retour
    if (totalSum > INT_MAX) return -2;
    return (jint) totalSum;
}

/**
 * 🔴 POINT D'ENTRÉE DE SÉCURITÉ
 * Cette fonction est appelée par le code Java pour décider de l'activation des fonctions.
 * Elle combine plusieurs techniques de détection pour une robustesse accrue.
 */
extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_jnidemo_MainActivity_isDebugDetected(JNIEnv* env, jobject thiz) {
    // On agrège les résultats des trois analyses de sécurité
    bool result = isTracerPidDetected() || isPtraceDetected() || isSuspiciousEnvironment();
    return result ? JNI_TRUE : JNI_FALSE;
}