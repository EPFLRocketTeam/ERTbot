#ifndef COMMANDS_H
#define COMMANDS_H

command* checkForCommand(command** commandQueue);

PeriodicCommand* initalizePeriodicCommands();

command* executeCommand(command** commandQueue);

#endif