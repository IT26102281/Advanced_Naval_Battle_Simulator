#include "naval_sim.h"
#include <ctype.h>

const EscortInfo escortInfo[ESCORT_TYPE_COUNT] = {
    {"EA", "1936A-class Destroyer", "SK C/34 naval gun", 0.08, 20.0, 15.0, 120.0, 8.0},
    {"EB", "Gabbiano-class Corvette", "L/47 dual-purpose gun", 0.06, 30.0, 12.0, 105.0, 7.0},
    {"EC", "Matsu-class Destroyer", "Type 89 dual-purpose gun", 0.07, 25.0, 10.0, 110.0, 6.0},
    {"ED", "F-class Escort Ship", "SK C/32 naval gun", 0.05, 50.0, 8.0, 95.0, 5.0},
    {"EE", "Japanese Kaibokan", "4.7 inch naval guns", 0.04, 70.0, 6.0, 90.0, 4.0}};

const BattleshipInfo battleshipInfo[4] = {
    {'U', "USS Iowa (BB-61)", "50-caliber Mark 7 gun"},
    {'M', "MS King George V", "356 mm Mark VII gun"},
    {'R', "Richelieu", "15 inch Mle 1935 gun"},
    {'S', "Sovetsky Soyuz-class", "16 inch B-37 gun"}};

static int readInteger(const char *message, int minimum, int maximum)
{
    int value;
    do
    {
        printf("%s", message);
        if (scanf("%d", &value) != 1)
        {
            while (getchar() != '\n')
            {
            }
            value = minimum - 1;
        }
        if (value < minimum || value > maximum)
        {
            printf("Please enter a value from %d to %d.\n", minimum, maximum);
        }
    } while (value < minimum || value > maximum);
    return value;
}

static double readDouble(const char *message, double minimum, double maximum)
{
    double value;
    do
    {
        printf("%s", message);
        if (scanf("%lf", &value) != 1)
        {
            while (getchar() != '\n')
            {
            }
            value = minimum - 1.0;
        }
        if (value < minimum || value > maximum)
        {
            printf("Please enter a value from %.2f to %.2f.\n", minimum, maximum);
        }
    } while (value < minimum || value > maximum);
    return value;
}

static char readBattleshipType(void)
{
    char type;
    int valid = 0;
    do
    {
        printf("Battleship type (U/M/R/S): ");
        scanf(" %c", &type);
        type = (char)toupper((unsigned char)type);
        valid = isBattleshipType(type);
        if (!valid)
        {
            printf("Please select U, M, R or S.\n");
        }
    } while (!valid);
    return type;
}

static int randomInteger(int minimum, int maximum)
{
    return minimum + rand() % (maximum - minimum + 1);
}

void setupBattlefield(Battlefield *field)
{
    int i;
    char positionChoice;

    printf("\n--- Battlefield Setup ---\n");
    field->canvasSize = readDouble("Canvas size D (50 - 1000): ", 50.0, 1000.0);
    field->escortCount = readInteger("Number of escort ships (1 - 100): ", 1, MAX_ESCORTS);
    field->battleship.type = readBattleshipType();
    field->battleship.maxVelocity = readDouble("Battleship maximum shell velocity (30 - 250): ", 30.0, 250.0);
    field->battleship.firingInterval = readDouble("Battleship firing interval in seconds (1 - 60): ", 1.0, 60.0);

    printf("Use random starting positions? (y/n): ");
    scanf(" %c", &positionChoice);
    positionChoice = (char)tolower((unsigned char)positionChoice);

    printf("Random seed (0 uses current time): ");
    scanf("%u", &field->seed);
    if (field->seed == 0)
    {
        field->seed = (unsigned int)time(NULL);
    }
    srand(field->seed);

    if (positionChoice == 'y')
    {
        field->battleship.x = (double)randomInteger(0, (int)field->canvasSize);
        field->battleship.y = (double)randomInteger(0, (int)field->canvasSize);
    }
    else
    {
        field->battleship.x = readDouble("Battleship x coordinate: ", 0.0, field->canvasSize);
        field->battleship.y = readDouble("Battleship y coordinate: ", 0.0, field->canvasSize);
    }

    field->battleship.destroyed = 0;
    field->battleship.damage = 0.0;
    field->battleship.shotsFired = 0;

    for (i = 0; i < field->escortCount; i++)
    {
        EscortShip *escort = &field->escorts[i];
        EscortType type = (EscortType)randomInteger(0, ESCORT_TYPE_COUNT - 1);
        const EscortInfo *info = &escortInfo[type];

        escort->id = i + 1;
        escort->type = type;
        escort->x = (double)randomInteger(0, (int)field->canvasSize);
        escort->y = (double)randomInteger(0, (int)field->canvasSize);
        escort->minAngle = 5.0 + randomInteger(0, 20);
        escort->maxAngle = escort->minAngle + info->angleRange;
        if (escort->maxAngle > 89.0)
        {
            escort->maxAngle = 89.0;
        }
        escort->minVelocity = info->minVelocity;
        escort->maxVelocity = info->maxVelocity;
        escort->impactPower = info->impactPower;
        escort->firingInterval = info->firingInterval;
        escort->destroyed = 0;
        escort->hasFired = 0;
        escort->shotsFired = 0;
    }

    printf("Battlefield created using seed %u.\n", field->seed);
}

void printBattlefield(const Battlefield *field)
{
    int i;
    printf("\nCanvas: %.1f x %.1f | Seed: %u\n", field->canvasSize, field->canvasSize, field->seed);
    printf("Battleship %c at (%.1f, %.1f), Vmax %.1f\n",
           field->battleship.type, field->battleship.x, field->battleship.y,
           field->battleship.maxVelocity);
    for (i = 0; i < field->escortCount; i++)
    {
        const EscortShip *escort = &field->escorts[i];
        printf("E%-3d %-2s at (%6.1f, %6.1f) angle %.1f-%.1f V %.1f-%.1f\n",
               escort->id, escortInfo[escort->type].notation, escort->x, escort->y,
               escort->minAngle, escort->maxAngle,
               escort->minVelocity, escort->maxVelocity);
    }
}
