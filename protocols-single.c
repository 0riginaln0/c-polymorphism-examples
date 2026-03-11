//////////////////////////////////
// PROTOCOLS /////////////////////
//////////////////////////////////
// SINGLE EXTENSION
// This implementation works when all structs that extend this protocol don't extend any other protocols.

#include <stdio.h>
#include <stdlib.h>

#define PI 3.14


typedef struct Shape {
    double (*area)(void *self);
    double (*perimeter)(void *self);
} Shape;


typedef struct {
    Shape shape;
    double length;
    double width;
} Rectangle;

double rectangle_area(void *self) {
    Rectangle *rect = (Rectangle *)self;
    return rect->length * rect->width;
}

double rectangle_perimeter(void *self) {
    Rectangle *rect = (Rectangle *)self;
    return 2 * (rect->length + rect->width);
}

void init_rectangle(Rectangle *rect, double length, double width) {
    rect->length = length;
    rect->width = width;
    rect->shape.area = rectangle_area;
    rect->shape.perimeter = rectangle_perimeter;
}


typedef struct {
    Shape shape;
    double radius;
} Circle;

double circle_area(void *self) {
    Circle *circle = (Circle *)self;
    return PI * circle->radius * circle->radius;
}


double circle_perimeter(void *self) {
    Circle *circle = (Circle *)self;
    return 2 * PI * circle->radius;
}

void init_circle(Circle *circle, double radius) {
    circle->radius = radius;
    circle->shape.area = circle_area;
    circle->shape.perimeter = circle_perimeter;
}

int main() {
    Rectangle my_rectangle;
    init_rectangle(&my_rectangle, 4, 25);


    Circle my_circle;
    init_circle(&my_circle, 5);


    printf("Rectangle Area: %.2f\n", my_rectangle.shape.area(&my_rectangle));
    printf("Rectangle Perimeter: %.2f\n", my_rectangle.shape.perimeter(&my_rectangle));


    printf("Circle Area: %.2f\n", my_circle.shape.area(&my_circle));
    printf("Circle Perimeter: %.2f\n", my_circle.shape.perimeter(&my_circle));


    Shape *shapes[2];
    shapes[0] = (Shape *)&my_rectangle;
    shapes[1] = (Shape *)&my_circle;

    for (int i = 0; i < 2; i++) {
        printf("Shape Area: %.2f\n", shapes[i]->area(shapes[i]));
    }

    return 0;
}