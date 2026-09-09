#include "naval_sim.h"
#include <ctype.h>

static void printInstructions(void)
{
    printf("\n--- Instructions ---\n");
    printf("The simulator has one battleship and several stationary escort ships.\n");
    printf("A shot is possible when a projectile angle and velocity can reach the target.\n");
    printf("Parts 1-A to 1-C use one-shot attacks. Part 1-B also moves B through path points.\n");
    printf("Parts 2-A and 2-B add firing intervals and a simple attack strategy.\n");
    printf("Part 2-C is intentionally not included in this version.\n");
    printf("All detailed output is saved in the reports folder.\n");
}

static int readMenuChoice(void)
{
    int choice;
    printf("\nChoice: ");
    if (scanf("%d", &choice) != 1)
    {
        while (getchar() != '\n')
        {
        }
        return -1;
    }
    return choice;
}

int main(void)
{
    Battlefield field;
    int hasSetup = 0;
    int choice;

    printf("==========================================\n");
    printf("       ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("       SE1012 Programming Methodology\n");
    printf("==========================================\n");

    do
    {
        printf("\n1. Setup battlefield\n");
        printf("2. Show battlefield\n");
        printf("3. Run Part 1-A\n");
        printf("4. Run Part 1-B\n");
        printf("5. Run Part 1-C\n");
        printf("6. Run Part 2-A\n");
        printf("7. Run Part 2-B\n");
        printf("8. Run all implemented parts\n");
        printf("9. Instructions\n");
        printf("10. Previous reports\n");
        printf("0. Exit\n");
        choice = readMenuChoice();

        if (choice == 1)
        {
            setupBattlefield(&field);
            saveInitialConditions(&field, REPORT_FOLDER "/initial_conditions.txt");
            hasSetup = 1;
        }
        else if (choice == 2)
        {
            if (hasSetup)
            {
                printBattlefield(&field);
            }
            else
            {
                printf("Please setup the battlefield first.\n");
            }
        }
        else if (choice >= 3 && choice <= 8)
        {
            if (!hasSetup)
            {
                printf("Please setup the battlefield first.\n");
            }
            else if (choice == 3)
            {
                runPart1A(&field);
            }
            else if (choice == 4)
            {
                runPart1B(&field);
            }
            else if (choice == 5)
            {
                runPart1C(&field);
            }
            else if (choice == 6)
            {
                runPart2A(&field);
            }
            else if (choice == 7)
            {
                runPart2B(&field);
            }
            else
            {
                runAllParts(&field);
            }
        }
        else if (choice == 9)
        {
            printInstructions();
        }
        else if (choice == 10)
        {
            showPreviousReports();
        }
        else if (choice != 0)
        {
            printf("Invalid menu choice.\n");
        }
    } while (choice != 0);

    printf("Goodbye.\n");
    return 0;
}
