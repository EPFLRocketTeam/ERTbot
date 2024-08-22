#ifndef COMMANDS_H
#define COMMANDS_H

command** checkForCommand(command** commandQueue, PeriodicCommand** headOfPeriodicCommands);

PeriodicCommand** initalizePeriodicCommands(PeriodicCommand** headOfPeriodicCommands);

command** executeCommand(command** commandQueue);

#endif