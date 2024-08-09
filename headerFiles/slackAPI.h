#ifndef SLACKAPI_H
#define SLACKAPI_H

//send a message to _wiki-toolbox
int sendMessageToSlack(char *message);

//looks at the last message on _wiki-toolbox
void checkLastSlackMessage();

// gets the last message on _wiki-toolbox and parses it to a slackMsg struct
slackMessage getSlackMessage(slackMessage slackMsg);

//add a slack member
int addSlackMember(char *channels, char *email);
#endif