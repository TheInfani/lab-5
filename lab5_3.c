
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_FILE "config.txt"

static char logLevel[16] = "info";
static int  logInterval  = 5;
static volatile sig_atomic_t reload_requested = 0;

/* Зчитує "<рівень> <інтервал>" з config.txt і виводить нові значення. */
static void loadConfig(void)
{
    FILE *f = fopen(CONFIG_FILE, "r");
    if (f == NULL) {
        perror("Не вдалося відкрити " CONFIG_FILE);
        return;
    }

    char level[16];
    int interval;
    if (fscanf(f, "%15s %d", level, &interval) == 2 && interval > 0) {
        strcpy(logLevel, level);
        logInterval = interval;
        printf("\n[CONFIG LOADED] logLevel: \"%s\", logInterval: %d\n\n",
               logLevel, logInterval);
    } else {
        fprintf(stderr, "\nПомилка формату %s. Залишено попередні значення.\n\n",
                CONFIG_FILE);
    }
    fclose(f);
    fflush(stdout);
}

/* У обробнику лише виставляємо прапорець (async-signal-safe). */
static void handle_sigusr1(int sig)
{
    (void)sig;
    reload_requested = 1;
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  /* без SA_RESTART, щоб sleep() переривався сигналом */

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    loadConfig();

    printf("PID процесу: %d\n", getpid());
    printf("Команда для перезавантаження конфігурації:\n");
    printf("  kill -USR1 %d\n\n", getpid());
    fflush(stdout);

    unsigned int left = 0;
    for (;;) {
        if (reload_requested) {
            reload_requested = 0;
            loadConfig();
        }
        printf("[%s] Повідомлення системи (інтервал %d сек)...\n",
               logLevel, logInterval);
        fflush(stdout);

        left = sleep(logInterval);
        (void)left;
    }
}
