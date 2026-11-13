
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_FRAMES 32

void handle_sigquit(int sig)
{
    void *frames[MAX_FRAMES];
    static const char header[] = "\n--- Stack Trace (SIGQUIT) ---\n";
    (void)sig;

    write(STDOUT_FILENO, header, sizeof(header) - 1);
    int n = backtrace(frames, MAX_FRAMES);
    backtrace_symbols_fd(frames, n, STDOUT_FILENO);
    write(STDOUT_FILENO, "\n", 1);
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigquit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Програму запущено. PID процесу: %d\n", getpid());
    printf("Команда для надсилання сигналу з іншого терміналу:\n");
    printf("  kill -3 %d\n\n", getpid());
    printf("Очікування сигналу... (Натисніть Ctrl+\\ у цьому терміналі для перевірки)\n");
    printf("Для завершення натисніть Ctrl+C.\n");
    fflush(stdout);

    for (;;)
        sleep(1);
}
