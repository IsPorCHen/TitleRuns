#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <locale>
#include <iostream>
#include <ctime>

#define TRACK_LENGTH 50

using namespace std;

struct Turtle {
    int id;
    int position;
};

HANDLE* hThreads = nullptr;
vector<Turtle> turtles;
FILE* logFile;
volatile bool raceFinished = false;
HANDLE raceFinishedEvent;

DWORD WINAPI TurtleRace(LPVOID param) {
    Turtle* turtle = (Turtle*)param;

    while (!raceFinished) {
        srand(time(0) + turtle->id);

        int step = rand() % 3;
        turtle->position += step;

        if (turtle->position >= TRACK_LENGTH) {
            turtle->position = TRACK_LENGTH;
            raceFinished = true;
            SetEvent(raceFinishedEvent);
        }

        cout << "Черепаха " << turtle->id << " -> Позиция: " << turtle->position << endl;
        fprintf(logFile, "Черепаха %d -> Позиция: %d\n", turtle->id, turtle->position);
        if (raceFinished) {
            cout << "Черепаха " << turtle->id << " выиграла гонку!" << endl;
            fprintf(logFile, "Черепаха %d выиграла гонку!\n", turtle->id);
        }

        Sleep(2000);
    }

    return 0;
}

int main() {
    setlocale(0, "rus");

    srand(time(0));

    int countTurtles = 0;
    while (true) {
        cout << "Введите кол-во черепашек (5-10): ";
        cin >> countTurtles;

        if (countTurtles >= 5 && countTurtles <= 10)
            break;

        cout << "Столько черепашек нельзя вводить" << endl;
    }

    turtles.resize(countTurtles);
    hThreads = new HANDLE[countTurtles];
    logFile = fopen("race_log.txt", "w");

    if (logFile == NULL) {
        printf("Не удалось открыть файл для записи.\n");
        return 1;
    }

    cout << "Старт черепашьих бегов!" << endl;
    fprintf(logFile, "Старт черепашьих бегов!\n");

    raceFinishedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < countTurtles; i++) {
        turtles[i] = { i, 0 };
        hThreads[i] = CreateThread(NULL, 0, TurtleRace, &turtles[i], 0, NULL);
    }

    WaitForSingleObject(raceFinishedEvent, INFINITE);

    for (int i = 0; i < countTurtles; i++) {
        CloseHandle(hThreads[i]);
    }

    cout << "Гонка завершена!" << endl;
    fprintf(logFile, "Гонка завершена!\n");
    fclose(logFile);

    delete[] hThreads;
    CloseHandle(raceFinishedEvent);

    return 0;
}
