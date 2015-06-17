#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"
#include "inih/ini.h"
#include "defaults.h"

static char *basePath;

static int Config_handler(void* user, const char* section, const char* name,
                   const char* value)
{
    
    Config* pconfig = (Config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("ledmirror_settings", "version")) {

        pconfig->versionString = strdup(value);

    } else if (MATCH("ledmirror_settings", "level0")) {

        pconfig->level0 = atoi(value);

    } else if (MATCH("ledmirror_settings", "level1")) {

        pconfig->level1 = atoi(value);

    }else if (MATCH("ledmirror_settings", "level2")) {

        pconfig->level2 = atoi(value);

    }else if (MATCH("apps", "app0")) {

        pconfig->app0 = atoi(value);

    }else if (MATCH("apps", "app1")) {

        pconfig->app1 = atoi(value);

    }else if (MATCH("apps", "app2")) {

        pconfig->app2 = atoi(value);

    }else if (MATCH("apps", "app3")) {

        pconfig->app3 = atoi(value);

    } else if (MATCH("user", "name")) {

        pconfig->name = strdup(value);

    } else if (MATCH("user", "email")) {

        pconfig->email = strdup(value);

    } else {

        return 0;  /* unknown section/name, error */
        
    }
    return 1;
}

void Config_setBasePath(char *bp){
    basePath = strdup(bp);
}

int Config_getLevel0(){
    if (!config.level0){
        return LEVEL0;
    }
    return config.level0;
}
int Config_getLevel1(){
    if (!config.level1){
        return LEVEL1;
    }
    return config.level1;
}
int Config_getLevel2(){
    if (!config.level2){
        return LEVEL2;
    }
    return config.level2;
}

const char* Config_getLevels(){
    char levels[3] = { Config_getLevel0(), Config_getLevel1(), Config_getLevel2()};
    config.levels = levels;
    return config.levels;
}

int Config_getApp(int i){
    int apps[4] ={Config_getApp0(),Config_getApp1(),Config_getApp2(),Config_getApp3()};
    return apps[i];
}
const char* Config_getEmail(){
    return config.email;
}
int Config_getApp0(){
    return config.app0;
}
int Config_getApp1(){
    return config.app1;
}
int Config_getApp2(){
    return config.app2;
}
int Config_getApp3(){
    return config.app3;
}

void Config_load(const char *inifilename){
    
    if (ini_parse(inifilename, Config_handler, &config) < 0) {
        printf("Can't load '%s'\n",inifilename);
        exit(1);
    }

    printf("Config loaded from '%s': version=%s, name=%s, email=%s level0=%i level1=%i level2=%i app0=%i \n", 
        inifilename,
        config.versionString, 
        config.name, 
        config.email,
        config.level0,
        config.level1,
        config.level2,
        config.app0
        );

    // printf("BASE %s\n", basePath);
    // printf("svg %s\n", Config_getSVGName());
    // printf("script %s\n", Config_getScriptName());
    // printf("image %s\n", Config_getImageName());

    // float xnull = CENTER - Config_getCanvasWidth()/2.0;
    // float ynull = SHOULDER_HEIGHT+CANVAS_Y + (500.0 - Config_getCanvasHeight()/2.0);

    // printf("origin x:%f y:%f \n",xnull,ynull);

}
