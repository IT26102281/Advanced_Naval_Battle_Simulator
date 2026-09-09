#ifndef NAVAL_SIM_H
#define NAVAL_SIM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_ESCORTS 100
#define MAX_PATH_POINTS 30
#define REPORT_FOLDER "reports"
#define GRAVITY 9.81

typedef enum
{
    EA = 0,
    EB,
    EC,
    ED,
    EE,
    ESCORT_TYPE_COUNT
} EscortType;

typedef struct
{
    char notation[3];
    char name[40];
    char gun[40];
    double impactPower;
    double angleRange;
    double minVelocity;
    double maxVelocity;
    double firingInterval;
} EscortInfo;

typedef struct
{
    char notation;
    char name[30];
    char gun[40];
} BattleshipInfo;

typedef struct
{
    int id;
    EscortType type;
    double x;
    double y;
    double minAngle;
    double maxAngle;
    double minVelocity;
    double maxVelocity;
    double impactPower;
    double firingInterval;
    int destroyed;
    int hasFired;
    int shotsFired;
} EscortShip;

typedef struct
{
    char type;
    double x;
    double y;
    double maxVelocity;
    double firingInterval;
    int destroyed;
    double damage;
    int shotsFired;
} Battleship;

typedef struct
{
    double distance;
    double angle;
    double velocity;
    double time;
} Shot;

typedef struct
{
    double canvasSize;
    int escortCount;
    unsigned int seed;
    Battleship battleship;
    EscortShip escorts[MAX_ESCORTS];
} Battlefield;

extern const EscortInfo escortInfo[ESCORT_TYPE_COUNT];
extern const BattleshipInfo battleshipInfo[4];

void setupBattlefield(Battlefield *field);
void printBattlefield(const Battlefield *field);
void saveInitialConditions(const Battlefield *field, const char *fileName);
void saveFinalConditions(const Battlefield *field, const char *fileName);
void showPreviousReports(void);

int findShot(double distance, double minAngle, double maxAngle,
             double minVelocity, double maxVelocity, Shot *shot);
double distanceBetween(double x1, double y1, double x2, double y2);
int isBattleshipType(char type);
int escortCanHitBattleship(const Battlefield *field, const EscortShip *escort);
int battleshipCanHitEscort(const Battlefield *field, const EscortShip *escort,
                           int jammed, double minimumBattleshipAngle,
                           Shot *shot);

void runPart1A(const Battlefield *original);
void runPart1B(const Battlefield *original);
void runPart1C(const Battlefield *original);
void runPart2A(const Battlefield *original);
void runPart2B(const Battlefield *original);
void runAllParts(const Battlefield *original);

#endif
