// gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic-errors -Werror -Wconversion multi.c -o multi.exe && ./multi.exe
/////////////////////////////////////
// MULTIMETHODS /////////////////////
/////////////////////////////////////

// #ifndef MULTIMETHODS_H
// #define MULTIMETHODS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define MULTIMETHODS_HASH_CAPACITY 32
#define METHODS_HASH_CAPACITY 16

typedef void (*Multimethod_Fn)(void* args, void* out);

bool defmulti(char const * const name);
bool defmethod(char const * const multimethod_name, char const * const dispatch_value, Multimethod_Fn fn);
bool dispatch(char const * const multimethod_name, char const * const dispatch_value, void* args, void* out);
void multimethod_cleanup(void);

typedef struct Method_Entry {
    char* dispatch_value;
    Multimethod_Fn fn;
} Method_Entry;

typedef struct Multimethod {
    char* name;
    Method_Entry methods[METHODS_HASH_CAPACITY];
} Multimethod;

// #ifdef MULTIMETHODS_IMPLEMENTATION

#define IS_NULL(x) ((x) == NULL)
#define ANY_NULL_1(a)           (IS_NULL(a))
#define ANY_NULL_2(a, b)        (IS_NULL(a) || IS_NULL(b))
#define ANY_NULL_3(a, b, c)     (IS_NULL(a) || IS_NULL(b) || IS_NULL(c))
#define ANY_NULL_4(a, b, c, d)  (IS_NULL(a) || IS_NULL(b) || IS_NULL(c) || IS_NULL(d))

#define GET_ANY_NULL_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define any_is_null(...) GET_ANY_NULL_MACRO(__VA_ARGS__, ANY_NULL_4, ANY_NULL_3, ANY_NULL_2, ANY_NULL_1)(__VA_ARGS__)

static char* my_strdup(char const * const src) {
    if (src == NULL) return NULL;
    size_t len = strlen(src) + 1;
    char* dst = malloc(len);
    if (dst) memcpy(dst, src, len);
    return dst;
}

static uint32_t hash_djb2(char const * str, uint32_t capacity) {
    if (src == NULL) return 0;

    uint32_t hash = 5381;
    unsigned char const * u_str = (unsigned char const *)str;
    int c;
    while (true) {
        c = *u_str;
        if (c == 0) break;
        u_str += 1;
        hash = ((hash << 5) + hash) + (uint32_t)c;
    }
    return hash % capacity;
}

static Multimethod multimethods_table[MULTIMETHODS_HASH_CAPACITY] = {0};

static Multimethod* find_multimethod(char const * const name) {
    uint32_t index = hash_djb2(name, MULTIMETHODS_HASH_CAPACITY);
    uint32_t start_index = index;

    while (multimethods_table[index].name != NULL) {
        if (strcmp(multimethods_table[index].name, name) == 0) {
            return &multimethods_table[index];
        }
        index = (index + 1) % MULTIMETHODS_HASH_CAPACITY;
        if (index == start_index) break;
    }

    return NULL;
}

static Method_Entry* find_method(Multimethod * const mm, char const * const dispatch_value) {
    uint32_t index = hash_djb2(dispatch_value, METHODS_HASH_CAPACITY);
    uint32_t start_index = index;

    while (mm->methods[index].dispatch_value != NULL) {
        if (strcmp(mm->methods[index].dispatch_value, dispatch_value) == 0) {
            return &mm->methods[index];
        }
        index = (index + 1) % METHODS_HASH_CAPACITY;
        if (index == start_index) break;
    }

    return NULL;
}

bool defmulti(char const * const name) {
    if (name == NULL) return false;

    if (find_multimethod(name)) return false;

    uint32_t index = hash_djb2(name, MULTIMETHODS_HASH_CAPACITY);
    uint32_t start_index = index;

    while (multimethods_table[index].name != NULL) {
        index = (index + 1) % MULTIMETHODS_HASH_CAPACITY;
        if (index == start_index) return false;
    }

    multimethods_table[index].name = my_strdup(name);
    if (multimethods_table[index].name == NULL) return false;

    memset(multimethods_table[index].methods, 0, sizeof(multimethods_table[index].methods));
    return true;
}

bool defmethod(char const * const multimethod_name, char const * const dispatch_value, Multimethod_Fn fn) {
    if (any_is_null(multimethod_name, dispatch_value, fn)) return false;

    Multimethod* mm = find_multimethod(multimethod_name);
    if (mm == NULL) return false;

    if (find_method(mm, dispatch_value)) return false;

    uint32_t index = hash_djb2(dispatch_value, METHODS_HASH_CAPACITY);
    uint32_t start_index = index;

    while (mm->methods[index].dispatch_value != NULL) {
        index = (index + 1) % METHODS_HASH_CAPACITY;
        if (index == start_index) return false;
    }

    mm->methods[index].dispatch_value = my_strdup(dispatch_value);
    if (mm->methods[index].dispatch_value == NULL) return false;

    mm->methods[index].fn = fn;
    return true;
}

bool dispatch(char const * const multimethod_name, char const * const dispatch_value, void* arg, void* out) {
    if (any_is_null(multimethod_name, dispatch_value)) return false;

    Multimethod* mm = find_multimethod(multimethod_name);
    if (mm == NULL) return false;

    Method_Entry* entry = find_method(mm, dispatch_value);
    if (entry == NULL) return false;

    entry->fn(arg, out);
    return true;
}

void multimethod_cleanup(void) {
    for (size_t i = 0; i < MULTIMETHODS_HASH_CAPACITY; i++) {
        if (multimethods_table[i].name == NULL) continue;
        free(multimethods_table[i].name);
        multimethods_table[i].name = NULL;

        for (size_t j = 0; j < METHODS_HASH_CAPACITY; j++) {
            if (multimethods_table[i].methods[j].dispatch_value == NULL) continue;
            free(multimethods_table[i].methods[j].dispatch_value);
            multimethods_table[i].methods[j].dispatch_value = NULL;
        }
    }
}

#undef IS_NULL
#undef ANY_NULL_1
#undef ANY_NULL_2
#undef ANY_NULL_3
#undef ANY_NULL_4
#undef GET_ANY_NULL_MACRO
#undef any_is_null

// #endif
// #endif

#include <stdio.h>

typedef struct { double side; } Square;
typedef struct { double width, length; } Rectangle;
typedef struct { double radius; } Circle;

void square_area(void* args, void* out) {
    Square* s = (Square*)args;
    *(double*)out = s->side * s->side;
}

void square_perimeter(void* args, void* out) {
    Square* s = (Square*)args;
    *(double*)out = s->side * 4.0;
}

void rectangle_area(void* args, void* out) {
    Rectangle* r = (Rectangle*)args;
    *(double*)out = r->width * r->length;
}

void rectangle_perimeter(void* args, void* out) {
    Rectangle* r = (Rectangle*)args;
    *(double*)out = 2.0 * (r->width + r->length);
}

void circle_area(void* args, void* out) {
    Circle* c = (Circle*)args;
    *(double*)out = 3.14 * c->radius * c->radius;
}

void circle_perimeter(void* args, void* out) {
    Circle* c = (Circle*)args;
    *(double*)out = 2.0 * 3.14 * c->radius;
}

int main(void) {
    if (!defmulti("area")) {
        fprintf(stderr, "Failed to create multimethod 'area'\n");
        return 1;
    }
    if (!defmulti("perimeter")) {
        fprintf(stderr, "Failed to create multimethod 'perimeter'\n");
        goto error_cleanup;
    }

    if (!defmethod("area",      "square",    square_area)         ||
        !defmethod("perimeter", "square",    square_perimeter)    ||
        !defmethod("area",      "rectangle", rectangle_area)      ||
        !defmethod("perimeter", "rectangle", rectangle_perimeter) ||
        !defmethod("area",      "circle",    circle_area)         ||
        !defmethod("perimeter", "circle",    circle_perimeter)) {
        fprintf(stderr, "Failed to add one or more methods\n");
        goto error_cleanup;
    }

    Square sq = {.side = 5.0};
    Rectangle rc = {.width = 4.0, .length = 6.0};
    Circle ci = {.radius = 3.0};

    double area;
    double perim;
    bool ok = true;
    ok &= dispatch("area", "square", &sq, &area);
    ok &= dispatch("perimeter", "square", &sq, &perim);
    if (!ok) {
        fprintf(stderr, "Error calling multimethod for square\n");
        goto error_cleanup;
    }
    printf("Square area: %.2f, perimeter: %.2f\n", area, perim);

    ok &= dispatch("area", "rectangle", &rc, &area);
    ok &= dispatch("perimeter", "rectangle", &rc, &perim);
    if (!ok) {
        fprintf(stderr, "Error calling multimethod for rectangle\n");
        goto error_cleanup;
    }
    printf("Rectangle area: %.2f, perimeter: %.2f\n", area, perim);

    ok &= dispatch("area", "circle", &ci, &area);
    ok &= dispatch("perimeter", "circle", &ci, &perim);
    if (!ok) {
        fprintf(stderr, "Error calling multimethod for circle\n");
        goto error_cleanup;
    }
    printf("Circle area: %.2f, perimeter: %.2f\n", area, perim);
    
    struct { const char* type; void* data; } shapes[] = {
        {"square", &sq},
        {"rectangle", &rc},
        {"circle", &ci}
    };
    size_t num_shapes = sizeof(shapes) / sizeof(shapes[0]);

    for (size_t i = 0; i < num_shapes; i++) {
        ok &= dispatch("area", shapes[i].type, shapes[i].data, &area);
        ok &= dispatch("perimeter", shapes[i].type, shapes[i].data, &perim);
        if (!ok) {
            fprintf(stderr, "Error calling multimethod for shape %zu\n", i + 1);
            goto error_cleanup;
        }
        printf("Shape %zu area: %.2f, perimeter: %.2f\n", i + 1, area, perim);
    }

    multimethod_cleanup();
    return 0;

  error_cleanup:
    multimethod_cleanup();
    return 1;
}
