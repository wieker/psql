#ifndef EXPCORE_H
#define EXPCORE_H

int upload_file(char* host, char* pass, char* file);

int ssh_channel(char* host, char* pass);

int call_rhelper(int channel, char* cmdname, char* host, char* pass);
int spawn_helper(int channel, char* cmdname, char* host, char* pass);
int send_helper(int channel, char* cmdname, char* host, char* pass);
int wait_helper(int channel, char* cmdname, char* host, char* pass);
int dnld_helper(int channel, char* cmdname, char* host, char* pass);

int regexp_helper(int channel, char* expr, char** result);

int ssh_connect(char* host, char* pass, char* cmd);
int exconnect(char* host, char* pass, char* name, char* cmd, char* connected);

#endif
