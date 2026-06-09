/////////////////////////////////////
// MULTIMETHODS /////////////////////
/////////////////////////////////////

// #ifndef MULTIMETHODS_H
// #define MULTIMETHODS_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static char* my_strdup(const char* src) {
    if (src == NULL) return NULL;
    size_t len = strlen(src) + 1;
    char* dst = (char*)malloc(len);
    if (dst) {
        memcpy(dst, src, len);
    }
    return dst;
}

typedef void* (*Multimethod_Fn)(void*);

int create_multimethod(const char* name);

int add_method(const char* multimethod_name, const char* dispatch_value, Multimethod_Fn fn);

void* call_multimethod(const char* multimethod_name, const char* dispatch_value, void* arg);

void multimethod_cleanup(void);

typedef struct Method_Entry {
    char* dispatch_value;
    Multimethod_Fn fn;
    struct Method_Entry* next;
} Method_Entry;

typedef struct Multimethod {
    char* name;
    Method_Entry* methods;
    struct Multimethod* next;
} Multimethod;

// #ifdef MULTIMETHODS_IMPLEMENTATION

static Multimethod* multimethods_head = NULL;

static Multimethod* find_multimethod(const char* name) {
    Multimethod* cur = multimethods_head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

static Method_Entry* find_method(Multimethod* mm, const char* dispatch_value) {
    Method_Entry* cur = mm->methods;
    while (cur) {
        if (strcmp(cur->dispatch_value, dispatch_value) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

int create_multimethod(const char* name) {
    if (name == NULL) {
        return -1;
    }
    
    if (find_multimethod(name) != NULL) {
        return -1;
    }

    Multimethod* new_mm = (Multimethod*)malloc(sizeof(Multimethod));
    if (new_mm == NULL) {
        return -1;
    }

    new_mm->name = my_strdup(name);
    if (new_mm->name == NULL) {
        free(new_mm);
        return -1;
    }

    new_mm->methods = NULL;
    new_mm->next = multimethods_head;
    multimethods_head = new_mm;

    return 0;
}

int add_method(const char* multimethod_name, const char* dispatch_value, Multimethod_Fn fn) {
    if (multimethod_name == NULL || dispatch_value == NULL) {
        return -1;
    }

    Multimethod* mm = find_multimethod(multimethod_name);
    if (mm == NULL) {
        return -1;
    }

    if (find_method(mm, dispatch_value) != NULL) {
        return -1;
    }

    Method_Entry* new_entry = (Method_Entry*)malloc(sizeof(Method_Entry));
    if (new_entry == NULL) {
        return -1;
    }

    new_entry->dispatch_value = my_strdup(dispatch_value);
    if (new_entry->dispatch_value == NULL) {
        free(new_entry);
        return -1;
    }

    new_entry->fn = fn;
    new_entry->next = mm->methods;
    mm->methods = new_entry;

    return 0;
}

void* call_multimethod(const char* multimethod_name, const char* dispatch_value, void* arg) {
    if (multimethod_name == NULL || dispatch_value == NULL) {
        return NULL;
    }

    Multimethod* mm = find_multimethod(multimethod_name);
    if (mm == NULL) {
        return NULL;
    }

    Method_Entry* entry = find_method(mm, dispatch_value);
    if (entry == NULL) {
        return NULL;
    }

    return entry->fn(arg);
}

void multimethod_cleanup(void) {
    Multimethod* cur_mm = multimethods_head;

    while (cur_mm) {
        Multimethod* next_mm = cur_mm->next;

        Method_Entry* cur_me = cur_mm->methods;
        while (cur_me) {
            Method_Entry* next_me = cur_me->next;
            free(cur_me->dispatch_value);
            free(cur_me);
            cur_me = next_me;
        }
        free(cur_mm->name);
        free(cur_mm);
        cur_mm = next_mm;
    }

    multimethods_head = NULL;
}

// #endif
// #endif

#include <stdio.h>

typedef struct { double side; } Square;
typedef struct { double width, length; } Rectangle;
typedef struct { double radius; } Circle;

void* square_area(void* args) {
    Square* s = (Square*)args;
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) return NULL;
    *result = s->side * s->side;
    return result;
}

void* square_perimeter(void* args) {
    Square* s = (Square*)args;
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) return NULL;
    *result = s->side * 4;
    return result;
}

void* rectangle_area(void* args) {
    Rectangle* r = (Rectangle*)args;
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) return NULL;
    *result = r->width * r->length;
    return result;
}

void* rectangle_perimeter(void* args) {
    Rectangle* r = (Rectangle*)args;
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) return NULL;
    *result = 2 * (r->width + r->length);
    return result;
}

void* circle_area(void* args) {
    Circle* c = (Circle*)args;
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) return NULL;
    *result = 3.14 * c->radius * c->radius;
    return result;
}

void* circle_perimeter(void* args) {
    Circle* c = (Circle*)args;
    double* result = (double*)malloc(sizeof(double));
    if (result == NULL) return NULL;
    *result = 2 * 3.14 * c->radius;
    return result;
}

int main(void) {
    if (create_multimethod("area") != 0) {
        fprintf(stderr, "Failed to create multimethod 'area'\n");
        return 1;
    }
    if (create_multimethod("perimeter") != 0) {
        fprintf(stderr, "Failed to create multimethod 'perimeter'\n");
        multimethod_cleanup();
        return 1;
    }

    if (add_method("area",      "square",    square_area) != 0 ||
        add_method("perimeter", "square",    square_perimeter) != 0 ||
        add_method("area",      "rectangle", rectangle_area) != 0 ||
        add_method("perimeter", "rectangle", rectangle_perimeter) != 0 ||
        add_method("area",      "circle",    circle_area) != 0 ||
        add_method("perimeter", "circle",    circle_perimeter) != 0) {
        fprintf(stderr, "Failed to add one or more methods\n");
        multimethod_cleanup();
        return 1;
    }

    Square sq = {.side = 5.0};
    Rectangle rc = {.width = 4.0, .length = 6.0};
    Circle ci = {.radius = 3.0};
    
    double* area_sq = (double*)call_multimethod("area", "square", &sq);
    double* perim_sq = (double*)call_multimethod("perimeter", "square", &sq);
    if (area_sq == NULL || perim_sq == NULL) {
        fprintf(stderr, "Error calling multimethod for square\n");
        free(area_sq); free(perim_sq);
        multimethod_cleanup();
        return 1;
    }
    printf("Square area: %.2f, perimeter: %.2f\n", *area_sq, *perim_sq);
    free(area_sq); free(perim_sq);

    double* area_rc = (double*)call_multimethod("area", "rectangle", &rc);
    double* perim_rc = (double*)call_multimethod("perimeter", "rectangle", &rc);
    if (area_rc == NULL || perim_rc == NULL) {
        fprintf(stderr, "Error calling multimethod for rectangle\n");
        free(area_rc); free(perim_rc);
        multimethod_cleanup();
        return 1;
    }
    printf("Rectangle area: %.2f, perimeter: %.2f\n", *area_rc, *perim_rc);
    free(area_rc); free(perim_rc);

    double* area_ci = (double*)call_multimethod("area", "circle", &ci);
    double* perim_ci = (double*)call_multimethod("perimeter", "circle", &ci);
    if (area_ci == NULL || perim_ci == NULL) {
        fprintf(stderr, "Error calling multimethod for circle\n");
        free(area_ci); free(perim_ci);
        multimethod_cleanup();
        return 1;
    }
    printf("Circle area: %.2f, perimeter: %.2f\n", *area_ci, *perim_ci);
    free(area_ci); free(perim_ci);
    
    struct { const char* type; void* data; } shapes[] = {
        {"square", &sq},
        {"rectangle", &rc},
        {"circle", &ci}
    };
    int num_shapes = sizeof(shapes) / sizeof(shapes[0]);

    for (int i = 0; i < num_shapes; i++) {
        double* area = (double*)call_multimethod("area", shapes[i].type, shapes[i].data);
        double* perimeter = (double*)call_multimethod("perimeter", shapes[i].type, shapes[i].data);
        if (area == NULL || perimeter == NULL) {
            fprintf(stderr, "Error calling multimethod for shape %d\n", i + 1);
            free(area);
            free(perimeter);
            multimethod_cleanup();
            return 1;
        }
        printf("Shape %d area: %.2f, perimeter: %.2f\n", i + 1, *area, *perimeter);
        free(area);
        free(perimeter);
    }

    multimethod_cleanup();
    return 0;
}