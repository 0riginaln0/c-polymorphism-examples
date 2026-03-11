//////////////////////////////////
// PROTOCOLS /////////////////////
//////////////////////////////////
// MULTIPLE EXTENSION
// Dispatch is a pregenerated switch case instead of struct layout pointer casting trick

#include <stdio.h>
#include <stdlib.h>

#define PI 3.14

typedef struct Shape
{
    double (*area)(void *self);
    double (*perimeter)(void *self);
} Shape;

typedef struct Colorable
{
    void (*color)(void *self);
} Colorable;

typedef struct
{
    Shape shape;
    Colorable colorable;
    double length;
    double width;
} Rectangle;

double rectangle_area(void *self)
{
    Rectangle *rect = (Rectangle *)self;
    return rect->length * rect->width;
}

double rectangle_perimeter(void *self)
{
    Rectangle *rect = (Rectangle *)self;
    return 2 * (rect->length + rect->width);
}

void rectangle_color(void *self)
{
    printf("This rectangle is blue.\n");
}

void init_rectangle(Rectangle *rect, double length, double width)
{
    rect->length = length;
    rect->width = width;
    rect->shape.area = rectangle_area;
    rect->shape.perimeter = rectangle_perimeter;
    rect->colorable.color = rectangle_color;
}

typedef struct
{
    Shape shape;
    Colorable colorable;
    double radius;
} Circle;

double circle_area(void *self)
{
    Circle *circle = (Circle *)self;
    return PI * circle->radius * circle->radius;
}

double circle_perimeter(void *self)
{
    Circle *circle = (Circle *)self;
    return 2 * PI * circle->radius;
}

void circle_color(void *self)
{
    printf("This circle is red.\n");
}

void init_circle(Circle *circle, double radius)
{
    circle->radius = radius;
    circle->shape.area = circle_area;
    circle->shape.perimeter = circle_perimeter;
    circle->colorable.color = circle_color;
}

typedef enum
{
    ShapeExtenderKindRectangle,
    ShapeExtenderKindCircle,
} ShapeExtenderKind;

typedef struct
{
    ShapeExtenderKind kind;
    void *self;
} ShapeExtender;

typedef enum
{
    ColorableExtenderKindRectangle,
    ColorableExtenderKindCircle,
} ColorableExtenderKind;

typedef struct
{
    ColorableExtenderKind kind;
    void *self;
} ColorableExtender;

void colorable_protocol_dispatch_color(ColorableExtender extender)
{
    switch (extender.kind)
    {
    case ColorableExtenderKindRectangle:
        return rectangle_color((Rectangle *)extender.self);
    case ColorableExtenderKindCircle:
        return circle_color((Circle *)extender.self);
    }
}

double shape_protocol_dispatch_area(ShapeExtender extender)
{
    switch (extender.kind)
    {
    case ShapeExtenderKindRectangle:
        return rectangle_area((Rectangle *)extender.self);
    case ShapeExtenderKindCircle:
        return circle_area((Circle *)extender.self);
    }
}

double shape_protocol_dispatch_perimeter(ShapeExtender extender)
{
    switch (extender.kind)
    {
    case ShapeExtenderKindRectangle:
        return rectangle_perimeter((Rectangle *)extender.self);
    case ShapeExtenderKindCircle:
        return circle_perimeter((Circle *)extender.self);
    }
}

int main()
{
    Rectangle my_rectangle;
    init_rectangle(&my_rectangle, 334, 25);

    Circle my_circle;
    init_circle(&my_circle, 5);

    printf("Rectangle Area: %.2f\n", my_rectangle.shape.area(&my_rectangle));
    printf("Rectangle Perimeter: %.2f\n", my_rectangle.shape.perimeter(&my_rectangle));

    printf("Circle Area: %.2f\n", my_circle.shape.area(&my_circle));
    printf("Circle Perimeter: %.2f\n", my_circle.shape.perimeter(&my_circle));

    ColorableExtender colorables[2];
    colorables[0] = (ColorableExtender){.kind = ColorableExtenderKindRectangle, .self = (void *)&my_rectangle};
    colorables[1] = (ColorableExtender){.kind = ColorableExtenderKindCircle, .self = (void *)&my_circle};
    for (int i = 0; i < 2; i++)
    {
        colorable_protocol_dispatch_color(colorables[i]);
    }

    ShapeExtender shapes[2];
    shapes[0] = (ShapeExtender){.kind = ShapeExtenderKindRectangle, .self = (void *)&my_rectangle};
    shapes[1] = (ShapeExtender){.kind = ShapeExtenderKindCircle, .self = (void *)&my_circle};
    for (int i = 0; i < 2; i++)
    {
        printf("AREA: %.2f\n", shape_protocol_dispatch_area(shapes[i]));
        printf("PERIMETER: %.2f\n", shape_protocol_dispatch_perimeter(shapes[i]));
    }

    return 0;
}
