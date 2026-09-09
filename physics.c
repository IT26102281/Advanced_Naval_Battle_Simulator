#include "naval_sim.h"

static double degreesToRadians(double degrees)
{
    return degrees * 3.141592653589793 / 180.0;
}

double distanceBetween(double x1, double y1, double x2, double y2)
{
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

int isBattleshipType(char type)
{
    return type == 'U' || type == 'M' || type == 'R' || type == 'S';
}

int findShot(double distance, double minAngle, double maxAngle,
             double minVelocity, double maxVelocity, Shot *shot)
{
    int angle;
    double bestTime = 1000000.0;
    int found = 0;

    if (distance <= 0.0)
    {
        return 0;
    }

    for (angle = (int)ceil(minAngle); angle <= (int)floor(maxAngle); angle++)
    {
        double radians = degreesToRadians((double)angle);
        double sine = sin(2.0 * radians);
        double requiredVelocity;
        double flightTime;

        if (sine <= 0.0)
        {
            continue;
        }
        requiredVelocity = sqrt(distance * GRAVITY / sine);
        if (requiredVelocity >= minVelocity && requiredVelocity <= maxVelocity)
        {
            flightTime = distance / (requiredVelocity * cos(radians));
            if (flightTime < bestTime)
            {
                bestTime = flightTime;
                shot->distance = distance;
                shot->angle = (double)angle;
                shot->velocity = requiredVelocity;
                shot->time = flightTime;
                found = 1;
            }
        }
    }
    return found;
}

int escortCanHitBattleship(const Battlefield *field, const EscortShip *escort)
{
    double distance = distanceBetween(escort->x, escort->y,
                                      field->battleship.x, field->battleship.y);
    Shot shot;
    return findShot(distance, escort->minAngle, escort->maxAngle,
                    escort->minVelocity, escort->maxVelocity, &shot);
}

int battleshipCanHitEscort(const Battlefield *field, const EscortShip *escort,
                           int jammed, double minimumBattleshipAngle,
                           Shot *shot)
{
    double distance;
    double minimumAngle = jammed ? minimumBattleshipAngle : 1.0;

    distance = distanceBetween(field->battleship.x, field->battleship.y,
                               escort->x, escort->y);
    return findShot(distance, minimumAngle, 89.0, 1.0,
                    field->battleship.maxVelocity, shot);
}
