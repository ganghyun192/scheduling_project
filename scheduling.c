#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 100 // 프로세스의 수는 최대 100임

typedef struct
{
    int pid;          // 프로세스아이디인데 p1,p2,p3로서 출력용으로 사용하는 속성임
    int arrival;      // 도착시간 속성
    int burst;        // 실행할때까지 걸리는 시간임
    int remaining;    //.라운드로빈은 순환이기때문에 남은 실행시간을 담아줘야함
    int completed;    // 완료==1,미완료==0 걍 불대수처럼 쓰는거임 ㅇㅇ
    float waiting;    // 종료시간-도착시간
    float turnaround; // 반환시간
} Process;            // 타입변환임

void printResults(Process p[], int n)
{                                                                     // 프로세스 이름,도착시간,실행시간등등 간트차트 뺀 나머지 출력하는 함수
    float total_waiting = 0, total_turnaround = 0;                    // 평균 대기,반환시간 출력을위한 실수형태의 누적합 변수임
    printf("\n프로세스ID\t도착시간\t실행시간\t대기시간\t반환시간\n"); // 출력하는 칸 나누기
    for (int i = 0; i < n; i++)
    {                                                                                                                   // 프로세스이름,도착시간,실행시간,대기시간,반환시간 출력을 위한 반복문
        printf("P%d\t\t%d\t\t%d\t\t%.2f\t\t%.2f\n", p[i].pid, p[i].arrival, p[i].burst, p[i].waiting, p[i].turnaround); // 프로세스개수만큼 반복하는데 각각의 정보들을 순차적으로 출력함
        total_waiting += p[i].waiting;                                                                                  // 평균대기시간 계산을위한 각 프로세스의 대기시간 누적합을 함
        total_turnaround += p[i].turnaround;                                                                            // 평균실행시간 계산을위한 각 프로세스의 실행시간 누적합을 함
    }
    printf("\n평균 대기시간: %.2f", total_waiting / n);      // 평균대기시간 출력을위한 각 프로세스의 대기시간 누적합을 프로세스의 개수로 나눠서 출력함
    printf("\n평균 반환시간: %.2f\n", total_turnaround / n); // 평균출력시간 출력을위한 각 프로세스의 실행시간 누적합을 프로세스 개수로 나눠서 출력함
}

// gantt[] : 시간 순서대로 실행된 프로세스 ID 배열
// time[]  : 각 프로세스가 시작된 시간 배열 (time[0]은 첫 시작 시간, 마지막은 종료 시간)
// count   : 간트 차트에 표시할 실행 횟수
void printGanttChart(int gantt[], int time[], int count)
{
    printf("\nGantt 차트:\n");

    // 위쪽 테두리 출력
    for (int i = 0; i < count; i++)
        printf("----");
    printf("-\n");

    // 프로세스 ID 출력
    for (int i = 0; i < count; i++)
        printf("|P%d ", gantt[i]);
    printf("|\n");

    // 아래쪽 테두리 출력
    for (int i = 0; i < count; i++)
        printf("----");
    printf("-\n");

    // 시간 출력 (시작 시간 ~ 종료 시간)
    for (int i = 0; i <= count; i++)
        printf("%-4d", time[i]);
    printf("\n");
}

void fcfs(Process p[], int n)
{
    printf("\n--- FCFS 스케줄링 ---\n");
    int time = 0, g[MAX], t[MAX], gc = 0;
    for (int i = 0; i < n; i++)
    {
        if (time < p[i].arrival)
            time = p[i].arrival;
        p[i].waiting = time - p[i].arrival;
        time += p[i].burst;
        p[i].turnaround = p[i].waiting + p[i].burst; //
        g[gc] = p[i].pid;
        t[gc++] = time - p[i].burst;
    }
    t[gc] = time;
    printGanttChart(g, t, gc);
    printResults(p, n);
}

void sjf(Process p[], int n)
{
    printf("\n--- SJF 스케줄링 ---\n");
    int completed = 0, time = 0, g[MAX], t[MAX], gc = 0;
    while (completed < n)
    {
        int idx = -1, min_burst = 1e9;
        for (int i = 0; i < n; i++)
        {
            if (!p[i].completed && p[i].arrival <= time && p[i].burst < min_burst)
            {
                min_burst = p[i].burst;
                idx = i;
            }
        }
        if (idx == -1)
        {
            time++;
            continue;
        }
        p[idx].waiting = time - p[idx].arrival;
        time += p[idx].burst;
        p[idx].turnaround = p[idx].waiting + p[idx].burst;
        p[idx].completed = 1;
        g[gc] = p[idx].pid;
        t[gc++] = time - p[idx].burst;
        completed++;
    }
    t[gc] = time;
    printGanttChart(g, t, gc);
    printResults(p, n);
}

void roundRobin(Process p[], int n, int q)
{
    printf("\n--- 라운드 로빈 스케줄링 ---\n");
    int time = 0, g[MAX], t[MAX], gc = 0, remain = n;
    for (int i = 0; i < n; i++)
        p[i].remaining = p[i].burst;
    while (remain > 0)
    {
        int progress = 0;
        for (int i = 0; i < n; i++)
        {
            if (p[i].arrival <= time && p[i].remaining > 0)
            {
                g[gc] = p[i].pid;
                t[gc++] = time;
                if (p[i].remaining > q)
                {
                    p[i].remaining -= q;
                    time += q;
                }
                else
                {
                    time += p[i].remaining;
                    p[i].waiting = time - p[i].arrival - p[i].burst;
                    p[i].turnaround = p[i].waiting + p[i].burst;
                    p[i].remaining = 0;
                    remain--;
                }
                progress = 1;
            }
        }
        if (!progress)
            time++;
    }
    t[gc] = time;
    printGanttChart(g, t, gc);
    printResults(p, n);
}

void reset(Process p[], Process original[], int n)
{
    for (int i = 0; i < n; i++)
    {
        p[i] = original[i];
        p[i].completed = 0;
    }
}

int main()
{
    int n, q;
    Process original[MAX], p[MAX];

    printf("프로세스 수 입력: ");
    scanf("%d", &n);
    for (int i = 0; i < n; i++)
    { // 프로세스의 도착시간 실행시간 입력받기위한 루프
        original[i].pid = i + 1;
        printf("P%d 도착시간, 실행시간 입력: ", i + 1);
        scanf("%d %d", &original[i].arrival, &original[i].burst); //
    }

    printf("Time Quantum (RR): ");
    scanf("%d", &q);

    reset(p, original, n);
    fcfs(p, n);
    reset(p, original, n);
    sjf(p, n);
    reset(p, original, n);
    roundRobin(p, n, q);

    return 0;
}
