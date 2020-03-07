#include <tcl8.5/expect.h>
#include <unistd.h>

char cmdbuf[256];
char localbuf[256];

int retryies = 1000;

char* opt1 = "UserKnownHostsFile=/dev/null";
char* opt2 = "StrictHostKeyChecking=no";

int
upload_file(char* host, char* pass, char* file)
{
	printf("Uploading %s\n", file);
	snprintf(cmdbuf, sizeof(cmdbuf), "%s:/tmp", host);
	int fd = exp_spawnl("scp", "scp", "-r", "-o",
				opt1, "-o", opt2,
				file, cmdbuf, NULL);
	FILE* out = fdopen(fd, "w");
	int t = 0;
	enum {SSH_PASS, SSH_PROGRESS};
	retry:
	switch(exp_expectl(fd,
					exp_glob, "assword", SSH_PASS,
					exp_glob, "%", SSH_PROGRESS,
					exp_end)) {
			case SSH_PASS:
					fprintf(out, "%s\n", pass);
					goto retry;
			case EXP_TIMEOUT:
					printf("Timeout on first step of scp (not an error)\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			case SSH_PROGRESS:
					goto retry;
			default:
					printf("Expect error\n");
					return 0;
	}
	close(fd);
	return 0;
}

int
ssh_channel(char* host, char* pass)
{
	printf("Opening channel to %s\n", host);
	int fd = exp_spawnl("ssh", "ssh", "-o",
				opt1, "-o", opt2,
				host, NULL);
	enum {SSH_PASS, SSH_PROMPT, SSH_COMPLETE};
	int t = 0;
	FILE* out = fdopen(fd, "w");
	int st = 0;
	retry:
	switch(exp_expectl(fd,
					exp_glob, "assword", SSH_PASS,
					exp_glob, "\\$", SSH_PROMPT,
					exp_glob, "# $", SSH_PROMPT,
					exp_glob, "rhelper_on", SSH_COMPLETE,
					exp_end)) {
			case SSH_PASS:
					fprintf(out, "%s\n", pass);
					goto retry;
			case EXP_TIMEOUT:
					printf("Timeout (wait for establishing): wait more...\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			case SSH_PROMPT:
					switch (st) {
					case 0:
						fprintf(out, "chmod +x tmp/rhelper\n");
						st ++;
						goto retry;
					case 1:
						fprintf(out, "/tmp/rhelper\n");
						st ++;
						goto retry;
					}
					break;
			
			case SSH_COMPLETE:
					break;
			default:
					printf("Expect error\n");
					return fd;
	}
    printf("Channel estableshed\n");
    return fd;
}

int
call_rhelper(int channel, char* cmdname, char* host, char* pass)
{
	snprintf(localbuf, sizeof(localbuf), "scripts/%s.sh", cmdname);
	upload_file(host, pass, localbuf);
	FILE* out = fdopen(channel, "w");
	printf("Startign script ...\n");
	enum {RHELPER_SUCC, RHELPER_FAIL};
	fprintf(out, "script\n%s.sh\n\n", cmdname);
	int t = 0;
	retry:
	switch(exp_expectl(channel,
					exp_glob, "resultcode0", RHELPER_SUCC,
					exp_glob, "resultcode1", RHELPER_FAIL,
					exp_end)) {
			case RHELPER_SUCC:
					printf("Remote succ\n");
					return 0;
			case RHELPER_FAIL:
					printf("Remote fail\n");
					return 1;
			case EXP_TIMEOUT:
					printf("Timeout on first step of ssh (not an error)\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			default:
					printf("Expect error\n");
					return 1;
	}
	return 1;
}

int
spawn_helper(int channel, char* cmdname, char* host, char* pass)
{
	FILE* out = fdopen(channel, "w");
	fprintf(out, "spawn\n%s\n\n", cmdname);
	return 0;
}

int
send_helper(int channel, char* cmdname, char* host, char* pass)
{
	FILE* out = fdopen(channel, "w");
	fprintf(out, "%s", cmdname);
	fflush(out);
	return 0;
}

int
wait_helper(int channel, char* cmdname, char* host, char* pass)
{
	enum {RHELPER_DONE};
	int t = 0;
	retry:
	switch(exp_expectl(channel,
					exp_glob, cmdname, RHELPER_DONE,
					exp_end)) {
			case RHELPER_DONE:
					printf("Remote expected\n");
					return 0;
			case EXP_TIMEOUT:
					printf("Timeout on first step of ssh (not an error)\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			default:
					printf("Expect error\n");
	}
	return 0;
}

int
dnld_helper(int channel, char* cmdname, char* host, char* pass)
{
	snprintf(cmdbuf, sizeof(cmdbuf), "%s:/%s", host, cmdname);
	int fd = exp_spawnl("scp", "scp", "-o",
				opt1, "-o", opt2,
				"-r", cmdbuf, ".", NULL);
	FILE* out = fdopen(fd, "w");
	int t = 0;
	enum {SSH_PASS, SSH_PROGRESS};
	retry:
	switch(exp_expectl(fd,
					exp_glob, "assword", SSH_PASS,
					exp_glob, "%", SSH_PROGRESS,
					exp_end)) {
			case SSH_PASS:
					fprintf(out, "%s\n", pass);
					goto retry;
			case EXP_TIMEOUT:
					printf("Timeout on first step of scp (not an error)\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			case SSH_PROGRESS:
					goto retry;
			default:
					printf("Expect error\n");
					return 0;
	}
	close(fd);
	return 0;
}

int
regexp_helper(int channel, char* expr, char** result)
{
	enum {REGEXP_DONE};
	regexp* re = TclRegComp(expr);
	int t = 0;
	retry:
	switch(exp_expectl(channel,
					exp_compiled, expr, re, REGEXP_DONE,
					exp_end)) {
			case REGEXP_DONE:
					printf("Remote expected: %s\n", re->startp[0]);
					return 0;
			case EXP_TIMEOUT:
					printf("Timeout on first step of ssh (not an error)\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			default:
					printf("Expect error\n");
	}
	return 0;
}

int
ssh_connect(char* host, char* pass, char* cmd)
{
	printf("Connecting ssh to %s\n", host);
	int fd = exp_spawnl("ssh", "ssh", "-o",
				opt1, "-o", opt2,
				host, cmd, NULL);
	enum {SSH_PASS, SSH_PROMPT, SSH_COMPLETE};
	FILE* out = fdopen(fd, "w");
	int t = 0;
	retry:
	switch(exp_expectl(fd,
					exp_glob, "assword", SSH_PASS,
					exp_glob, "\\$", SSH_COMPLETE,
					exp_glob, "# $", SSH_COMPLETE,
					exp_glob, "helper_on", SSH_COMPLETE,
					exp_end)) {
			case SSH_PASS:
					fprintf(out, "%s\n", pass);
					printf("Pass sent\n");
					goto retry;
			case EXP_TIMEOUT:
					printf("Timeout (wait for establishing): wait more...\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			case SSH_COMPLETE:
					break;
			default:
					printf("Expect error\n");
					return fd;
	}
    printf("Channel estableshed\n");
    return fd;
}

/*
 * Dangerous names: connect and others... system conflicts
 * */
int
exconnect(char* host, char* pass, char* name, char* cmd, char* connected)
{
	printf("Connecting to %s\n", host);
	int fd = exp_spawnl(cmd, cmd, host, NULL);
	enum {SSH_PASS, SSH_NAME, SSH_COMPLETE};
	FILE* out = fdopen(fd, "w");
	int t = 0;
	retry:
	switch(exp_expectl(fd,
					exp_glob, "Name", SSH_NAME,
					exp_glob, "assword", SSH_PASS,
					exp_glob, connected, SSH_COMPLETE,
					exp_end)) {
			case SSH_NAME:
					fprintf(out, "%s\n", name);
					printf("Name sent\n");
					goto retry;
			case SSH_PASS:
					fprintf(out, "%s\n", pass);
					printf("Pass sent\n");
					goto retry;
			case EXP_TIMEOUT:
					printf("Timeout (wait for establishing): wait more...\n");
					t ++;
					if (t < retryies)
						goto retry;
					break;
			case SSH_COMPLETE:
					break;
			default:
					printf("Expect error\n");
					return fd;
	}
    printf("Channel estableshed\n");
    return fd;
}
