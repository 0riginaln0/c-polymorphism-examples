// gcc -std=c99 -Wall -Wextra -Wpedantic -pedantic-errors -Werror -Wconversion multimethods.c -o multi.exe && ./multi.exe
/////////////////////////////////////
// MULTIMETHODS /////////////////////
/////////////////////////////////////

#define MULTIMETHODS_IMPLEMENTATION
#include "multimethods_intrusive_list.h" // or "multimethods_hash_table.h"

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
