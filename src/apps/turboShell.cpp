#include <drivers/display/terminal/terminal.hpp>
#include <system/CPU/scheduling/ohMyTime/omtime.hpp>
#include <system/CPU/scheduling/PIT/pit.hpp>
#include <system/CPU/scheduling/RTC/rtc.hpp>
#include <drivers/devices/keyboard.hpp>
#include <lib/string.hpp>
#include <drivers/display/serial/serial.hpp>
#include <system/ACPI/acpi.hpp>
#include <lib/panic.hpp>
#include <drivers/fs/vfs/turboVFS.hpp>
#include <system/CPU/scheduling/scheduler/scheduler.hpp>
#include <lib/color.h>
#include <kernel/kernel.hpp>
#include <drivers/display/ssfn/ssfn.hpp>
#include <system/memory/pMemory/pMemory.hpp>
#include <apps/turboShell.hpp>

void myTime(){
	while(true){
		size_t size = 0;
		for(size_t i = 0; i < STACK_SIZE; ++i){
			if(kernelStack[i] != 'A'){
				break;
			}
			size++;
		}
		ssfn::setColor(ssfn::fgcolor, 0x001f52); // red
		ssfn::printfAt(0,0,"\r%s", rtc::getTime());
		ssfn::printfAt(0, 1, "\rFREE RAM: %zu KB", (pMemory::getFreeRam() / 1024 / 1024));
	}
}

void nano(){
	while(true){
		char* tmp = keyboard::getLine();
	}
}

namespace turbo::shell{
    
    turbo::vfs::tfs_node_t *currentNode;
    char *path;

    void parse(char* cmd,char *arg){
        switch(hash(cmd)){
            case hash("nano"):
				terminal::clear();
                scheduler::createProc("Init", reinterpret_cast<uint64_t>(nano), 0);
				scheduler::createThread((uint64_t)myTime, 0, scheduler::initproc);
				scheduler::init();
                return;

            case hash("help"):
                printf("All the listed command are not totally implement !\n");
                printf("-turbo --Display a string into the terminal !\n");
                printf("-help --Display all the implement commands and future implemented command !\n");
				printf("-panic-- create a kernel panic \n");
				printf("-clear --clear the terminal\n");
				printf("-ls --list every directory and files\n");
				printf("-cd --change directory\n");
				printf("-mkdir --create a directory\n");
				printf("-touch --create and empty file\n");
				printf("-nano --a sort of nano to prouve Turb-OS is multi tasking\n");
                break;

            case hash(""):
                break;
            case hash("turbo"):
	     	printf("Get rick rolled\n");
		break;

            case hash("panic"):
                PANIC("WHAT IS THE PPROBLEM ?");
                break;

            case hash("clear"):
                turbo::terminal::clear();
                break;

            case hash("ls"):
            {
                turbo::vfs::tfs_node_t *node;
                if(!strcmp(arg,"../") || !strcmp(arg,"..")){ node = currentNode->parent; }
                else if(!strcmp(arg,"./") || !strcmp(arg,".") || !strcmp(arg,"")){ node = currentNode; }
                else if(!strcmp(arg,"/")){ node = turbo::vfs::open(nullptr,arg); }
                else { node = turbo::vfs::open(currentNode,arg); }

                if(!node){
                    printf("[!]-Error, no such file or directory ");
                    break;
                }         
                if ((node->flags & 0x07) != turbo::vfs::TFS_FOLDER)
                {
                    printf("%s\n",arg);
                    break;
                }
                for (size_t i = 0; i < node->childs.getLength(); i++)
                {
                    switch(node->childs.at(i)->flags & 0x07){
                        case turbo::vfs::TFS_FOLDER:
                            printf("%s[D]-%s%s",BRED,node->childs.at(i)->name,RESET);
                            printf(" ");
                            break;
                        case turbo::vfs::TFS_PATHDEVICE:
                            printf("%s[P]-%s%s",BGREEN,node->childs.at(i)->name,RESET);
                            printf(" ");
                            break;
                        case turbo::vfs::TFS_SYMLINK:
                            printf("%s[S]-%s%s",BYELLOW,node->childs.at(i)->name,RESET);
                            printf(" ");
                            break;
                        case turbo::vfs::TFS_FILE:
                            printf("%s[F]-%s%s",BMAGENTA,node->childs.at(i)->name,RESET);
                            printf(" ");
                            break;
                        default:
                            printf("%s%s%s",BWHITE,node->childs.at(i)->name,RESET);
                            printf(" ");
                            break;
                    }
                }
                printf("\n");
                break;
            }
            case hash("mkdir"):
                if(arg != nullptr){
                    turbo::vfs::addDir(currentNode,arg);
                }
                break;
            case hash("touch"):
                if(arg !=nullptr){
                    turbo::vfs::addFile(currentNode,arg);
                }
                break;
            case hash("cd"):
            {
                if(!strcmp(arg,"")){
                    currentNode = turbo::vfs::getChild(0,"/");
                    return;
                }
                if(!strcmp(arg,"..") || !strcmp(arg,"../")){
                    //int length = sizeof(path);
                    currentNode = currentNode->parent;
                    return;
                }
                if (!strcmp(arg, ".") || !strcmp(arg, "./")) {
                    return;
                }
                if (!strcmp(arg, "/"))
                {
                    currentNode = vfs::tfs_root->ptr;
                    return;
                }
                turbo::vfs::tfs_node_t *node;
                if(!strcmp(arg,"/")){
                    node = turbo::vfs::open(nullptr,arg);
                    currentNode = node;
                }else{
                    node = turbo::vfs::open(currentNode,arg);
                    currentNode = node;
                    strcat(path,"/");
                    strcat(path,currentNode->name);
                }

                if(!node){
                    printf("%s[!]-Error%s No suche file or directory \n",BRED,RESET);
                    return;
                }
                if((node->flags & 0x07) != turbo::vfs::TFS_FOLDER){
                    printf("%s[!]-Error%s The specified file is not a directory \n",BRED,RESET);
                    return;
                }
                break;
            }

            default:
                printf("NEVER GONNA GIVE YOU UP, NEVER GONNA LET YOU DOWN !\n");
                printf("Unknown command\n");
                break;
        }
    }

    void run(){
        turbo::serial::log("Starting the TurboShell ! \n");

        path = (char*) malloc(sizeof(char)* 128);
        while(true){
            if(!currentNode){
                currentNode = (turbo::vfs::tfs_node_t*)malloc(sizeof(turbo::vfs::tfs_node_t));
                currentNode->flags = turbo::vfs::TFS_FOLDER;
                strcpy(currentNode->path,"~");
            }
            printf("%sroot%s@%s%s%s%s%s",BRED,RESET,BRED,currentNode->path,RESET,":",RESET);
            char *command = turbo::keyboard::getLine();
            char tmp[10] = "\0";

            for(size_t i = 0; i < strlen(command);i++){
                if(command[i] != ' ' && command[i] != '\0'){
                    char c[2] = "\0";
                    c[0] = command[i];
                    strcat(tmp,c); 
                }
				else break;
            }
            char *arguments = strrm(command, tmp);
            arguments = strrm(arguments," ");
            parse(tmp,arguments);
        }
    }
}
