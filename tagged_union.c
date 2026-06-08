/////////////////////////////////////
// TAGGED UNION /////////////////////
/////////////////////////////////////
#include <stdio.h>

typedef enum
{
    SHAPE_SQUARE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE
} ShapeKind;

typedef struct
{
    ShapeKind kind;
    union
    {
        struct
        {
            double side;
        } square;
        struct
        {
            double width;
            double length;
        } rectangle;
        struct
        {
            double radius;
        } circle;
    };
} Shape;

double area(Shape *s)
{
    switch (s->kind)
    {
    case SHAPE_SQUARE:
        return s->square.side * s->square.side;
    case SHAPE_RECTANGLE:
        return s->rectangle.width * s->rectangle.length;
    case SHAPE_CIRCLE:
        return 3.14 * s->circle.radius * s->circle.radius;
    default:
        return 0;
    }
}

double perimeter(Shape *s)
{
    switch (s->kind)
    {
    case SHAPE_SQUARE:
        return s->square.side * 4;
    case SHAPE_RECTANGLE:
        return 2 * (s->rectangle.width + s->rectangle.length);
    case SHAPE_CIRCLE:
        return 2 * 3.14 * s->circle.radius;
    default:
        return 0;
    }
}

int main()
{
    Shape square;
    square.kind = SHAPE_SQUARE;
    square.square.side = 5.0;

    Shape rectangle;
    rectangle.kind = SHAPE_RECTANGLE;
    rectangle.rectangle.width = 4.0;
    rectangle.rectangle.length = 6.0;

    Shape circle;
    circle.kind = SHAPE_CIRCLE;
    circle.circle.radius = 3.0;

    printf("Square area: %.2f, perimeter: %.2f\n", area(&square), perimeter(&square));
    printf("Rectangle area: %.2f, perimeter: %.2f\n", area(&rectangle), perimeter(&rectangle));
    printf("Circle area: %.2f, perimeter: %.2f\n", area(&circle), perimeter(&circle));

    Shape shapes[3];
    shapes[0] = square;
    shapes[1] = rectangle;
    shapes[2] = circle;

    for (int i = 0; i < 3; i++)
    {
        printf("Shape %d area: %.2f, perimeter: %.2f\n", i + 1, area(&shapes[i]), perimeter(&shapes[i]));
    }

    return 0;
}
