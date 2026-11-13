
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <sys/stat.h>

#define PROG_NAME "pwd_79_22"
#define VERSION   "1.0.0"
#define AUTHOR    "Tykhonenko Yaroslav"

static void print_help(void)
{
    printf("Використання: %s [-L | -P] [ОПЦІЇ]\n", PROG_NAME);
    printf("Виводить ім'я поточної робочої директорії.\n\n");
    printf("  -L         Вивести логічний шлях (з $PWD, враховуючи символічні лінки)\n");
    printf("  -P         Вивести фізичний шлях (розкриваючи всі символічні лінки)\n");
    printf("  --help     Показати цю допомогу та завершити роботу\n");
    printf("  --version  Показати версію утиліти\n");
    printf("  --author   Показати ім'я та прізвище розробника\n");
}

/* Фізичний шлях: getcwd() повертає шлях без символічних лінків. */
static int print_physical(void)
{
    char *path = realpath(".", NULL);
    if (path == NULL) {
        perror("realpath");
        return 1;
    }
    printf("%s\n", path);
    free(path);
    return 0;
}

/* Перевіряє, що $PWD справді вказує на поточну директорію. */
static int pwd_is_valid(const char *pwd)
{
    struct stat st_pwd, st_dot;

    if (pwd == NULL || pwd[0] != '/')
        return 0;
    if (stat(pwd, &st_pwd) != 0 || stat(".", &st_dot) != 0)
        return 0;
    return st_pwd.st_dev == st_dot.st_dev && st_pwd.st_ino == st_dot.st_ino;
}

/* Логічний шлях: беремо $PWD; якщо він некоректний — повертаємось до фізичного. */
static int print_logical(void)
{
    const char *pwd = getenv("PWD");

    if (pwd_is_valid(pwd)) {
        printf("%s\n", pwd);
        return 0;
    }
    return print_physical();
}

int main(int argc, char *argv[])
{
    int physical = 0;  /* за замовчуванням -L */
    int opt;

    static const struct option long_opts[] = {
        {"help",    no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"author",  no_argument, NULL, 'a'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "LP", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'L': physical = 0; break;
        case 'P': physical = 1; break;
        case 'h': print_help(); return 0;
        case 'v': printf("%s версія %s\n", PROG_NAME, VERSION); return 0;
        case 'a': printf("Розробник: %s\n", AUTHOR); return 0;
        default:
            fprintf(stderr, "Спробуйте '%s --help' для отримання довідки.\n", PROG_NAME);
            return 1;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "%s: зайвий аргумент '%s'\n", PROG_NAME, argv[optind]);
        return 1;
    }

    return physical ? print_physical() : print_logical();
}
