#ifndef STRING_H
#define STRING_H

typedef unsigned int size_t;


#ifdef __cplusplus
extern "C" {
#endif

// Fonctions de comparaison
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

// Fonctions de recherche
char* strstr(const char* haystack, const char* needle);

// Fonctions de copie
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
void* memcpy(void* dest, const void* src, size_t n);

// Fonctions de remplissage
void* memset(void* s, int c, size_t n);

// Fonctions utilitaires
size_t strlen(const char* s);

#ifdef __cplusplus
}
#endif

#endif // STRING_H
