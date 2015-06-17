#ifndef CONFIG_H
#define CONFIG_H

typedef struct
{
    const char* versionString;
    const char* name;
    const char* email;
    int level0;
    int level1;
   	int level2;
   	int level3;
    const char *app0;
    const char *app1;
    const char *app2;
    const char *app3;
    const char *levels;
} Config;

Config config;

void Config_load(char *inifilename);

int Config_getLevel0();
const char* Config_getLevels();
const char* Config_getEmail();
void Config_setBasePath(char *bp);

#endif
