//////////////////////////////////
// BASE EMBEDDING ////////////////
//////////////////////////////////

#include <stdio.h>


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
    return 3.14 * circle->radius * circle->radius;
}

double circle_perimeter(void *self) {
    Circle *circle = (Circle *)self;
    return 2 * 3.14 * circle->radius;
}

void init_circle(Circle *circle, double radius) {
    circle->radius = radius;
    circle->shape.area = circle_area;
    circle->shape.perimeter = circle_perimeter;
}

typedef struct {
    Shape shape;
    double side;
} Square;

double square_area(void *self) {
    Square *s = (Square *)self;
    return s->side * s->side;
}

double square_perimeter(void *self) {
    Square *s = (Square *)self;
    return 4 * s->side;
}

void init_square(Square *square, double side) {
    square->side = side;
    square->shape.area = square_area;
    square->shape.perimeter = square_perimeter;
}

int main() {
    Rectangle rect;
    init_rectangle(&rect, 4, 6);

    Circle circle;
    init_circle(&circle, 3.0);

    Square square;
    init_square(&square, 5);

    printf("Square area: %.2f, perimeter: %.2f\n",
            square.shape.area(&square), square.shape.perimeter(&square));
    printf("Rectangle area: %.2f, perimeter: %.2f\n",
            rect.shape.area(&rect), rect.shape.perimeter(&rect));
    printf("Circle area: %.2f, perimeter: %.2f\n",
            circle.shape.area(&circle), circle.shape.perimeter(&circle));

    Shape *shapes[3];
    shapes[0] = (Shape *)&square;
    shapes[1] = (Shape *)&rect;
    shapes[2] = (Shape *)&circle;

    for (int i = 0; i < 3; i++) {
        printf("Shape %d area: %.2f, perimeter: %.2f\n", i + 1,
                shapes[i]->area(shapes[i]),
                shapes[i]->perimeter(shapes[i]));
    }

    return 0;
}
