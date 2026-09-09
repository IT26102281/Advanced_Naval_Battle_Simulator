#include "naval_sim.h"
#include <direct.h>

static void makeReportFolder(void)
{
    _mkdir(REPORT_FOLDER);
}

static void writeConditions(FILE *file, const Battlefield *field, const char *title)
{
    int i;
    fprintf(file, "%s\n", title);
    fprintf(file, "Canvas %.2f %.2f\n", field->canvasSize, field->canvasSize);
    fprintf(file, "Seed %u\n", field->seed);
    fprintf(file, "B %c %.2f %.2f Vmax %.2f Interval %.2f Damage %.2f Destroyed %d\n",
            field->battleship.type, field->battleship.x, field->battleship.y,
            field->battleship.maxVelocity, field->battleship.firingInterval,
            field->battleship.damage, field->battleship.destroyed);
    for (i = 0; i < field->escortCount; i++)
    {
        const EscortShip *escort = &field->escorts[i];
        fprintf(file, "E%d %s %.2f %.2f angle %.2f %.2f velocity %.2f %.2f impact %.2f interval %.2f destroyed %d shots %d\n",
                escort->id, escortInfo[escort->type].notation, escort->x, escort->y,
                escort->minAngle, escort->maxAngle, escort->minVelocity,
                escort->maxVelocity, escort->impactPower, escort->firingInterval,
                escort->destroyed, escort->shotsFired);
    }
}

void saveInitialConditions(const Battlefield *field, const char *fileName)
{
    FILE *file;
    makeReportFolder();
    file = fopen(fileName, "w");
    if (file == NULL)
    {
        printf("Could not save %s.\n", fileName);
        return;
    }
    writeConditions(file, field, "INITIAL CONDITIONS");
    fclose(file);
}

void saveFinalConditions(const Battlefield *field, const char *fileName)
{
    FILE *file;
    makeReportFolder();
    file = fopen(fileName, "a");
    if (file == NULL)
    {
        printf("Could not save %s.\n", fileName);
        return;
    }
    fprintf(file, "\n");
    writeConditions(file, field, "FINAL CONDITIONS");
    fclose(file);
}

void showPreviousReports(void)
{
    char command[100];
    printf("\nSaved report files:\n");
    snprintf(command, sizeof(command), "dir /b %s 2>nul", REPORT_FOLDER);
    system(command);
    printf("Open the files in the reports folder to see complete results.\n");
}
