#include <drivers/display/terminal/terminal.hpp>
#include <system/CPU/scheduling/HPET/hpet.hpp>
#include <drivers/devices/keyboard.hpp>
#include <drivers/display/terminal/printf.h>
#include <lib/string.hpp>
#include <drivers/display/serial/serial.hpp>
#include <apps/turboShell.hpp>
#include <system/ACPI/acpi.hpp>
#include <stddef.h>
#include <lib/panic.hpp>
#include <system/CPU/scheduling/RTC/rtc.hpp>


namespace turbo::shell{

    void parse(char* cmd){
        switch(hash(cmd)){
            case hash("turbo"):
                printf("2FAST4U\n");
                break;

            case hash("help"):
                printf("All the listed command are not totally implement !\n");
                printf("-turbo --Display the link to access to the Instagram account of the DoggyBoys !\n");
                printf("-time --Display the actual date and hour\n");
                printf("-help --Display all the implement commands and future implemented command !\n");
                printf("-BIGG --Display an important message\n");
                break;

            case hash(""):
                break;
                
            case hash("panic"):
                PANIC("WHAT IS THE PPROBLEM ?");
                break;
            
            case hash("BIGG"):
                printf("ALL VOTE FOR THE MIGHTY BIGG BDE\n");
                break;

            case hash("Surprise"):
                printf("http://vm.tiktok.com/ZMLR3yp4B/\n");
                break;

            case hash("time"):
                turbo::rtc::getTime();
                break;

            default:
                printf("NEVER GONNA GIVE YOU UP, NEVER GONNA LET YOU DOWN !\n");
                printf("Unknown command\n");
                break;
        }
    }

    void run(){
        turbo::serial::log("Starting the TurboShell ! \n");

        printf("Press enter to begin...");
        turbo::keyboard::getLine();
        parse(nullptr);
        
        while (true){
            printf("root@turboShell: ");
            char *command = turbo::keyboard::getLine();
            parse(command);
        }
        
    }
}