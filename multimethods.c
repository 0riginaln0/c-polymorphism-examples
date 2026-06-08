/////////////////////////////////////
// MULTIMETHODS /////////////////////
/////////////////////////////////////

/*
 * ;; Clojure version
 *
 * (defmulti area :type)
 * (defmulti perimeter :type)
 *
 * (defmethod area :square [{:keys [side]}] (* side side))
 * (defmethod perimeter :square [{:keys [side]}] (* side 4))
 *
 * (defmethod area :rectangle [{:keys [width length]}] (* width length))
 * (defmethod perimeter :rectangle [{:keys [width length]}] (* 2 (+ width length)))
 *
 * (defmethod area :circle [{:keys [radius]}] (* 3.14 radius radius))
 * (defmethod perimeter :circle [{:keys [radius]}] (* 2 3.14 radius))
 *
 * (def square   {:type :square, :center-x 0.0, :center-y 0.0, :side 5.0})
 * (def rectangle {:type :rectangle, :center-x 2.0, :center-y 3.0, :width 4.0, :length 6.0})
 * (def circle    {:type :circle, :center-x 1.0, :center-y 1.0, :radius 3.0})
 *
 * (defn -main []
 *   (doseq [shape [square rectangle circle]]
 *     (println (format "Shape area: %.2f, perimeter: %.2f"
 *                      (area shape) (perimeter shape))))
 *
 *   (let [shapes [square rectangle circle]]
 *     (doseq [[idx shape] (map-indexed vector shapes)]
 *       (println (format "Shape %d area: %.2f, perimeter: %.2f"
 *                        (inc idx) (area shape) (perimeter shape))))))
 *
 * (-main)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function pointer type for method implementations.
// Takes a void* argument that can be cast to a user‑defined struct.
typedef void* (*multimethod_fn)(void*);

// Create a new multimethod with the given name.
// Returns 0 on success, -1 if a multimethod with that name already exists or on allocation error.
int create_multimethod(const char* name);

// Add a method to an existing multimethod.
// dispatch_value: string key used for dispatch (e.g. "square", "circle").
// fn: function pointer to the method implementation.
// Returns 0 on success, -1 if the multimethod doesn't exist or the key already exists.
int add_method(const char* multimethod_name, const char* dispatch_value, multimethod_fn fn);

// Call the multimethod: lookup the function for the given dispatch_value,
// and invoke it with the provided argument.
// Returns whatever the method returns (void*). Returns NULL if multimethod or dispatch_value is not found.
void* call_multimethod(const char* multimethod_name, const char* dispatch_value, void* arg);

// Clean up all allocated memory.
void multimethod_cleanup(void);

typedef struct MethodEntry {
    char* dispatch_value;
    multimethod_fn fn;
    struct MethodEntry* next;
} MethodEntry;

typedef struct Multimethod {
    char* name;
    MethodEntry* methods;
    struct Multimethod* next;
} Multimethod;

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

static MethodEntry* find_method(Multimethod* mm, const char* dispatch_value) {
    MethodEntry* cur = mm->methods;
    while (cur) {
        if (strcmp(cur->dispatch_value, dispatch_value) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

int create_multimethod(const char* name) {
    if (find_multimethod(name) != NULL) {
        return -1;
    }

    Multimethod* new_mm = (Multimethod*)malloc(sizeof(Multimethod));
    if (new_mm == NULL) {
        return -1;
    }

    new_mm->name = strdup(name);
    if (new_mm->name == NULL) {
        return -1;
    }

    new_mm->methods = NULL;
    new_mm->next = multimethods_head;
    multimethods_head = new_mm;

    return 0;
}

int add_method(const char* multimethod_name, const char* dispatch_value, multimethod_fn fn) {
    Multimethod* mm = find_multimethod(multimethod_name);
    if (mm == NULL) {
        return -1;
    }

    if (find_method(mm, dispatch_value) != NULL) {
        return -1;
    }

    MethodEntry* new_entry = (MethodEntry*)malloc(sizeof(MethodEntry));
    if (new_entry == NULL) {
        return -1;
    }

    new_entry->dispatch_value = strdup(dispatch_value);
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
    Multimethod* mm = find_multimethod(multimethod_name);
    if (mm == NULL) {
        return NULL;
    }

    MethodEntry* entry = find_method(mm, dispatch_value);
    if (entry == NULL) {
        return NULL;
    }

    return entry->fn(arg);
}

void multimethod_cleanup(void) {
    Multimethod* cur_mm = multimethods_head;

    while (cur_mm) {
        Multimethod* next_mm = cur_mm->next;

        MethodEntry* cur_me = cur_mm->methods;
        while (cur_me) {
            MethodEntry* next_me = cur_me->next;
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

typedef struct { double side; } Square;
typedef struct { double width, length; } Rectangle;
typedef struct { double radius; } Circle;

void* square_area(void* args) {
    Square* s = (Square*)args;
    double* result = malloc(sizeof(double));
    *result = s->side * s->side;
    return result;
}
void* square_perimeter(void* args) {
    Square* s = (Square*)args;
    double* result = malloc(sizeof(double));
    *result = s->side * 4;
    return result;
}
void* rectangle_area(void* args) {
    Rectangle* r = (Rectangle*)args;
    double* result = malloc(sizeof(double));
    *result = r->width * r->length;
    return result;
}
void* rectangle_perimeter(void* args) {
    Rectangle* r = (Rectangle*)args;
    double* result = malloc(sizeof(double));
    *result = 2 * (r->width + r->length);
    return result;
}
void* circle_area(void* args) {
    Circle* c = (Circle*)args;
    double* result = malloc(sizeof(double));
    *result = 3.14 * c->radius * c->radius;
    return result;
}
void* circle_perimeter(void* args) {
    Circle* c = (Circle*)args;
    double* result = malloc(sizeof(double));
    *result = 2 * 3.14 * c->radius;
    return result;
}

int main(void) {
    create_multimethod("area");
    create_multimethod("perimeter");

    add_method("area",      "square", square_area);
    add_method("perimeter", "square", square_perimeter);
    add_method("area",      "rectangle", rectangle_area);
    add_method("perimeter", "rectangle", rectangle_perimeter);
    add_method("area",      "circle", circle_area);
    add_method("perimeter", "circle", circle_perimeter);

    Square sq = {.side = 5.0};
    Rectangle rc = {.width = 4.0, .length = 6.0};
    Circle ci = {.radius = 3.0};

    double* area_sq = (double*)call_multimethod("area", "square", &sq);
    double* perim_sq = (double*)call_multimethod("perimeter", "square", &sq);
    printf("Square area: %.2f, perimeter: %.2f\n", *area_sq, *perim_sq);
    free(area_sq); free(perim_sq);

    double* area_rc = (double*)call_multimethod("area", "rectangle", &rc);
    double* perim_rc = (double*)call_multimethod("perimeter", "rectangle", &rc);
    printf("Rectangle area: %.2f, perimeter: %.2f\n", *area_rc, *perim_rc);
    free(area_rc); free(perim_rc);

    double* area_ci = (double*)call_multimethod("area", "circle", &ci);
    double* perim_ci = (double*)call_multimethod("perimeter", "circle", &ci);
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
        printf("Shape %d area: %.2f, perimeter: %.2f\n", i + 1, *area, *perimeter);
        free(area);
        free(perimeter);
    }

    multimethod_cleanup();
    return 0;
}
