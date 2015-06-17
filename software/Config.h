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
    int app0;
    int app1;
    int app2;
    int app3;
    const char *levels;
} Config;

Config config;

void Config_load(const char *inifilename);

int Config_getLevel0();
int Config_getApp0();
int Config_getApp1();
int Config_getApp2();
int Config_getApp3();
int Config_getApp(int i);
const char* Config_getLevels();
const char* Config_getEmail();
void Config_setBasePath(char *bp);

#endif
