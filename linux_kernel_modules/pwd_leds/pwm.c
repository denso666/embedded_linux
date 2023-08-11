/*
    Aplication developed to modify a led recognized in sys/class/leds
    Author: Felipe Alejandro Jimenez Castillo
    Date: 01/07/2023
*/
// #include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define LED_BRIGHTNESS "/sys/class/leds/external::led1/brightness"
#define LED_TRIGGER    "/sys/class/leds/external::led1/trigger"

static int fd_brightness;
static int fd_trigger;
static u_char led_power;
static char const* triggers[] = {"cpu", "none", "heartbeat", "timer"};
static const int triggers_size[] = {3, 4, 9, 5};

int menu(void);
int verify_integrity(void);
void trigger_menu(void);
void led_brightness(const int);
void brightness(void);

int main(int argc, const char *argv[])
{
    int opc, trigger_id;
    char updated_status[3];

    if (verify_integrity() == EXIT_FAILURE)
        return EXIT_FAILURE;

    while (1)
    {
        opc = menu();
        switch(opc)
        {
            // power toggle
            case 1:
            {
                led_power = !led_power;
                if (led_power)
                {
                    updated_status[0] = '2';
                    updated_status[1] = '5';
                    updated_status[2] = '5';
                }
                else
                {
                    updated_status[0] = '0';
                    updated_status[1] = '\0';
                    updated_status[2] = '\0';
                }
                write(fd_brightness, updated_status, 3);
                break;
            }
            // trigger change
            case 2: trigger_menu(); break;
            // dim led brightness
            // case 3:
            // {
            //     for (int i = 1; i<10; i++)
            //         led_brightness(i);

            //     break;
            // }
            case 3: brightness(); break;
            case 0: return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}


int menu(void)
{
    char *str;
    char buff[8];

    printf("\n=====================");
    printf("\n  MENU\n");
    printf("1. Toogle Power\n");
    printf("2. Change Trigger\n");
    printf("3. Change Brightness\n");
    printf("0. Exit\n");
    printf("  ? ");
    str = fgets(buff, 8, stdin);

    if (str == NULL)
        return EXIT_FAILURE;
    else if (buff[0] < '0' || buff[0] > '3')
    {
        printf("\tPlease try with a valid option\n");
        return EXIT_FAILURE;
    }
    else 
        return buff[0] - '0';
}

int verify_integrity(void)
{
    struct stat st;
    if (stat(LED_BRIGHTNESS, &st) != 0)
    {
        perror("led_brightness");
        return EXIT_FAILURE;
    }
    if (stat(LED_TRIGGER, &st) != 0)
    {
        perror("led_trigger");
        return EXIT_FAILURE;
    }

    // verify corrent file descriptor assign
    if ((fd_brightness = open(LED_BRIGHTNESS, O_WRONLY)) == -1)
    {
        perror("led_brightness");
        return EXIT_FAILURE;
    }
    if ((fd_trigger = open(LED_TRIGGER, O_WRONLY)) == -1)
    {
        perror("led_trigger");
        return EXIT_FAILURE;
    }

    write(fd_brightness, "0", 1);
    write(fd_trigger, "none", 4);
    led_power = 0;
    
    return EXIT_SUCCESS;
}
void trigger_menu()
{
    fflush(stdin);
    char *str;
    char buff[8];

    printf("\n\tTRIGGER\n");
    printf("\t1. cpu\n");
    printf("\t2. none\n");
    printf("\t3. heartbeat\n");
    printf("\t4. timer\n");
    printf("  ? ");
    str = fgets(buff, 8, stdin);

    if (buff[0] > '0' && buff[0] < '5')
    {
        int trigger_id = buff[0] - '1';
        write(fd_trigger, triggers[trigger_id], triggers_size[trigger_id]);
    }
    else
        printf("\tPlease try with a valid option\n");
}
void led_brightness(const int interval)
{
    long i = 6e5;
    while (i > 0)
    {
        if (!(i % interval))
            write(fd_brightness, "1", 1);
        else
            write(fd_brightness, "0", 1);
        i--;
    }
}
void brightness(void)
{
    char *str;
    char buff[8];

    printf("Percentage[0-255]? ");
    printf("  ? ");
    str = fgets(buff, 8, stdin);

    if (str == NULL) return;
    else
    {
        int size = 0;
        for (int i = 0; i<8; i++, size++)
        {
            if (buff[i] == '\n' && i > 0) break;
            else if (buff[i] < '0' || buff[i] > '9')
            {
                printf("\tPlease try with a valid number\n");
                return;
            }
        }
        write(fd_brightness, buff, size);
    }
}
