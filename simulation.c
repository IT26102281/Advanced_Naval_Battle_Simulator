#include "naval_sim.h"

static Battlefield copyField(const Battlefield *original)
{
    return *original;
}

static int aliveEscortCount(const Battlefield *field)
{
    int i;
    int count = 0;
    for (i = 0; i < field->escortCount; i++)
    {
        if (!field->escorts[i].destroyed)
        {
            count++;
        }
    }
    return count;
}

static void destroyEscortsInRange(Battlefield *field, FILE *report,
                                  int jammed, double minimumAngle,
                                  double currentTime)
{
    int i;
    for (i = 0; i < field->escortCount; i++)
    {
        EscortShip *escort = &field->escorts[i];
        Shot shot;
        if (!escort->destroyed && battleshipCanHitEscort(field, escort, jammed,
                                                         minimumAngle, &shot))
        {
            escort->destroyed = 1;
            escort->shotsFired = 0;
            fprintf(report, "B hit E%d at time %.2f s, angle %.1f, velocity %.2f\n",
                    escort->id, currentTime, shot.angle, shot.velocity);
        }
    }
}

static void escortsFireOnce(Battlefield *field, FILE *report, int cumulative)
{
    int i;
    for (i = 0; i < field->escortCount; i++)
    {
        EscortShip *escort = &field->escorts[i];
        if (!escort->destroyed && !escort->hasFired)
        {
            escort->hasFired = 1;
            escort->shotsFired++;
            if (escortCanHitBattleship(field, escort))
            {
                if (cumulative)
                {
                    field->battleship.damage += escort->impactPower;
                    fprintf(report, "E%d hit B: +%.2f damage, total %.2f\n",
                            escort->id, escort->impactPower,
                            field->battleship.damage);
                }
                else
                {
                    field->battleship.destroyed = 1;
                    fprintf(report, "E%d hit B and destroyed it.\n", escort->id);
                    return;
                }
                if (field->battleship.damage >= 1.0)
                {
                    field->battleship.destroyed = 1;
                    fprintf(report, "B was destroyed by cumulative damage.\n");
                    return;
                }
            }
        }
    }
}

static void resetShotFlags(Battlefield *field)
{
    int i;
    field->battleship.destroyed = 0;
    field->battleship.damage = 0.0;
    field->battleship.shotsFired = 0;
    for (i = 0; i < field->escortCount; i++)
    {
        field->escorts[i].destroyed = 0;
        field->escorts[i].hasFired = 0;
        field->escorts[i].shotsFired = 0;
    }
}

static void printResult(const Battlefield *field)
{
    printf("B status: %s | Damage: %.2f | Escorts remaining: %d\n",
           field->battleship.destroyed ? "DESTROYED" : "ALIVE",
           field->battleship.damage, aliveEscortCount(field));
}

void runPart1A(const Battlefield *original)
{
    Battlefield field = copyField(original);
    FILE *report;

    resetShotFlags(&field);
    report = fopen(REPORT_FOLDER "/part1a.txt", "w");
    if (report == NULL)
    {
        printf("Could not create Part 1-A report.\n");
        return;
    }
    saveInitialConditions(&field, REPORT_FOLDER "/part1a.txt");
    report = fopen(REPORT_FOLDER "/part1a.txt", "a");
    fprintf(report, "\nPART 1-A\nEscort ships fire once before the battleship fires.\n");
    escortsFireOnce(&field, report, 0);
    if (!field.battleship.destroyed)
    {
        destroyEscortsInRange(&field, report, 0, 1.0, 0.0);
    }
    fprintf(report, "B destroyed: %d\n", field.battleship.destroyed);
    saveFinalConditions(&field, REPORT_FOLDER "/part1a.txt");
    fclose(report);

    printf("Part 1-A complete. ");
    printResult(&field);
}

static void runPathSimulation(const Battlefield *original, int jammed,
                              int jamPoint, const char *fileName)
{
    Battlefield field = copyField(original);
    FILE *report;
    double oldX = field.battleship.x;
    double oldY = field.battleship.y;
    int point;
    int totalPoints = 8;

    resetShotFlags(&field);
    saveInitialConditions(&field, fileName);
    report = fopen(fileName, "a");
    if (report == NULL)
    {
        printf("Could not create path report.\n");
        return;
    }
    fprintf(report, "\nPART 1-B %s\n", jammed ? "Simulation 2 (jammed angle)" : "Simulation 1");
    fprintf(report, "PART 1-B %s\n", jammed ? "Simulation 2 (jammed angle)" : "Simulation 1");
    fprintf(report, "Generated path points: %d\n", totalPoints);
    for (point = 0; point < totalPoints && !field.battleship.destroyed; point++)
    {
        field.battleship.x = fmod(oldX + point * field.canvasSize / totalPoints,
                                  field.canvasSize);
        field.battleship.y = fmod(oldY + point * field.canvasSize * 0.63 / totalPoints,
                                  field.canvasSize);
        fprintf(report, "\nIteration %d: B at (%.2f, %.2f)\n",
                point + 1, field.battleship.x, field.battleship.y);
        escortsFireOnce(&field, report, 0);
        if (!field.battleship.destroyed)
        {
            destroyEscortsInRange(&field, report, jammed && point >= jamPoint,
                                  jammed && point >= jamPoint ? 15.0 : 1.0,
                                  (double)point);
        }
    }
    fprintf(report, "\nB destroyed: %d\n", field.battleship.destroyed);
    saveFinalConditions(&field, fileName);
    fclose(report);

    printf("%s complete. ", jammed ? "Part 1-B Simulation 2" : "Part 1-B Simulation 1");
    printResult(&field);
}

void runPart1B(const Battlefield *original)
{
    runPathSimulation(original, 0, 99, REPORT_FOLDER "/part1b_simulation1.txt");
    runPathSimulation(original, 1, 4, REPORT_FOLDER "/part1b_simulation2.txt");
}

void runPart1C(const Battlefield *original)
{
    Battlefield field = copyField(original);
    FILE *report;

    resetShotFlags(&field);
    saveInitialConditions(&field, REPORT_FOLDER "/part1c.txt");
    report = fopen(REPORT_FOLDER "/part1c.txt", "a");
    if (report == NULL)
    {
        return;
    }
    fprintf(report, "\nPART 1-C\nCumulative impact is used for attacks on B.\n");
    escortsFireOnce(&field, report, 1);
    if (!field.battleship.destroyed)
    {
        destroyEscortsInRange(&field, report, 0, 1.0, 0.0);
    }
    fprintf(report, "Cumulative B damage: %.2f\n", field.battleship.damage);
    saveFinalConditions(&field, REPORT_FOLDER "/part1c.txt");
    fclose(report);

    printf("Part 1-C complete. ");
    printResult(&field);
}

static int chooseBestTarget(const Battlefield *field, int *alreadyChosen)
{
    int i;
    int selected = -1;
    double bestScore = -1.0;
    for (i = 0; i < field->escortCount; i++)
    {
        Shot shot;
        double distance;
        double score;
        if (field->escorts[i].destroyed || alreadyChosen[i])
        {
            continue;
        }
        if (!battleshipCanHitEscort(field, &field->escorts[i], 0, 1.0, &shot))
        {
            continue;
        }
        distance = shot.distance;
        score = field->escorts[i].impactPower * 100.0 - distance;
        if (score > bestScore)
        {
            bestScore = score;
            selected = i;
        }
    }
    return selected;
}

void runPart2A(const Battlefield *original)
{
    Battlefield field = copyField(original);
    int selected[MAX_ESCORTS] = {0};
    int attackNumber;
    double currentTime = 0.0;
    FILE *report;

    resetShotFlags(&field);
    saveInitialConditions(&field, REPORT_FOLDER "/part2a.txt");
    report = fopen(REPORT_FOLDER "/part2a.txt", "a");
    if (report == NULL)
    {
        return;
    }
    fprintf(report, "\nPART 2-A\nStrategy: highest impact power adjusted by distance.\n");
    escortsFireOnce(&field, report, 1);
    for (attackNumber = 0; attackNumber < field.escortCount && !field.battleship.destroyed; attackNumber++)
    {
        int selectedIndex = chooseBestTarget(&field, selected);
        Shot shot;
        if (selectedIndex < 0)
        {
            break;
        }
        selected[selectedIndex] = 1;
        field.battleship.shotsFired++;
        currentTime += field.battleship.firingInterval;
        battleshipCanHitEscort(&field, &field.escorts[selectedIndex], 0, 1.0, &shot);
        field.escorts[selectedIndex].destroyed = 1;
        fprintf(report, "Attack %d at %.2f s: B attacked E%d, angle %.1f, velocity %.2f\n",
                attackNumber + 1, currentTime, field.escorts[selectedIndex].id,
                shot.angle, shot.velocity);
    }
    fprintf(report, "Battle ended at %.2f seconds.\n", currentTime);
    saveFinalConditions(&field, REPORT_FOLDER "/part2a.txt");
    fclose(report);

    printf("Part 2-A complete. ");
    printResult(&field);
}

void runPart2B(const Battlefield *original)
{
    Battlefield field = copyField(original);
    FILE *report;
    double nextBattleshipShot = 0.0;
    double nextEscortShot[MAX_ESCORTS];
    double currentTime = 0.0;
    int i;
    int events = 0;

    resetShotFlags(&field);
    saveInitialConditions(&field, REPORT_FOLDER "/part2b.txt");
    report = fopen(REPORT_FOLDER "/part2b.txt", "a");
    if (report == NULL)
    {
        return;
    }
    for (i = 0; i < field.escortCount; i++)
    {
        nextEscortShot[i] = 0.0;
    }
    fprintf(report, "\nPART 2-B\nEscort ships can fire continuously.\n");

    while (!field.battleship.destroyed && aliveEscortCount(&field) > 0 && events < 500)
    {
        double nextEvent = nextBattleshipShot;
        int eventEscort = -1;
        Shot shot;

        for (i = 0; i < field.escortCount; i++)
        {
            if (!field.escorts[i].destroyed && nextEscortShot[i] < nextEvent)
            {
                nextEvent = nextEscortShot[i];
                eventEscort = i;
            }
        }
        currentTime = nextEvent;
        if (eventEscort >= 0)
        {
            EscortShip *escort = &field.escorts[eventEscort];
            escort->shotsFired++;
            nextEscortShot[eventEscort] += escort->firingInterval;
            if (escortCanHitBattleship(&field, escort))
            {
                field.battleship.damage += escort->impactPower;
                fprintf(report, "%.2f s: E%d fired, B damage %.2f\n",
                        currentTime, escort->id, field.battleship.damage);
                if (field.battleship.damage >= 1.0)
                {
                    field.battleship.destroyed = 1;
                }
            }
        }
        else
        {
            int target = chooseBestTarget(&field, (int[MAX_ESCORTS]){0});
            nextBattleshipShot += field.battleship.firingInterval;
            field.battleship.shotsFired++;
            if (target >= 0 && battleshipCanHitEscort(&field, &field.escorts[target], 0, 1.0, &shot))
            {
                field.escorts[target].destroyed = 1;
                fprintf(report, "%.2f s: B fired at E%d, angle %.1f, velocity %.2f\n",
                        currentTime, field.escorts[target].id, shot.angle, shot.velocity);
            }
        }
        events++;
    }
    fprintf(report, "Battle ended at %.2f seconds after %d events.\n", currentTime, events);
    saveFinalConditions(&field, REPORT_FOLDER "/part2b.txt");
    fclose(report);

    printf("Part 2-B complete. ");
    printResult(&field);
}

void runAllParts(const Battlefield *original)
{
    runPart1A(original);
    runPart1B(original);
    runPart1C(original);
    runPart2A(original);
    runPart2B(original);
}
