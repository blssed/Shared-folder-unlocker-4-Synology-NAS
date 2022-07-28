/*
    Copyright (C) 2021, Tom-Niklas Metz
    This file is part of Shared folder unlocker 4 synology NAS.
    Shared folder unlocker 4 synology NAS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Shared folder unlocker 4 synology NAS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Shared folder unlocker 4 synology NAS.  If not, see <https://www.gnu.org/licenses/>.
    Shared folder unlocker 4 synology NAS  Copyright (C) 2021, Tom-Niklas Metz
    This program comes with ABSOLUTELY NO WARRANTY.
    This is free software, and you are welcome to redistribute it
    under certain conditions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <signal.h>

void readPassword(char*, char*);

int main() {

    char *pathToKPScript = "C:\\\"Program Files (x86)\"\\\"KeePass Password Safe 2\"\\KPScript";
    char *pathToPasswordDB = "\\\\network\\example\\path\\passwordsafe.kdbx";
    char *keepassPrefix = "prefix-in-keepass-title";

    FILE *fp;
    char databasePassword[128], nasIpAdress[16], nasPort[6], nasUsername[36], nasPassword[36], temp[100], titles[10000], passwords[10000];
    char listTitles[100][10000], listPassword[100][10000];

    readPassword("Please enter the KeePass-Password: ", databasePassword);

    char keepassTitleStr[1000];
    sprintf(keepassTitleStr, "%s %s %s %s%s %s%s%s", pathToKPScript, "-c:GetEntryString", pathToPasswordDB, "-pw:", databasePassword,  "-Field:Title -ref-Title:\"//", keepassPrefix, "//\"");
    
    fp = popen(keepassTitleStr, "r");
    if(fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }
    
    while(fgets(temp, 100, fp))
        strcat(titles, temp);

    int j = 0, h = 0;
    for(int i = 0; titles[i] != '\0'; i++) {
        if(titles[i] == '\n') {
            i++;
            strncpy(listTitles[j], titles+h+7, i-h-7);
            listTitles[j][strcspn(listTitles[j], "\n")] = 0;
            h = i;
            j++;
        }
    }

    pclose(fp);
    memset(temp, 0, strlen(temp));

    char keepassPasswordStr[1000];
    sprintf(keepassPasswordStr, "%s %s %s %s%s %s%s%s", pathToKPScript, "-c:GetEntryString", pathToPasswordDB, "-pw:", databasePassword, "-Field:Password -ref-Title:\"//", keepassPrefix, "//\"");
    fp = popen(keepassPasswordStr, "r");
    if(fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }
    
    while(fgets(temp, 100, fp))
        strcat(passwords, temp);

    j = 0, h = 0;
    for(int i = 0; passwords[i] != '\0'; i++) {
        if(passwords[i] == '\n') {
            i++;
            strncpy(listPassword[j], passwords+h, i-h);
            listPassword[j][strcspn(listPassword[j], "\n")] = 0;
            h = i;
            j++;
        }
    }

    pclose(fp);

    printf("Please enter the NAS-IP: ");
    scanf("%15s", nasIpAdress);
    printf("Please enter the SSH Port: ");
    scanf("%5s", nasPort);
    printf("Please enter the NAS-Username: ");
    scanf("%35s", nasUsername);
    readPassword("Please enter the NAS-Password: ", nasPassword);

    char nasPlinkCommand[10000];
    for(int i = 1; listTitles[i+1][0] != '\0'; i++) {
        memset(nasPlinkCommand, 0, strlen(nasPlinkCommand));
        sprintf(nasPlinkCommand, "%s %s %s%s%s %s %s %s%s%s%s%s%s%s", "plink.exe -batch -P", nasPort, nasUsername, "@", nasIpAdress, "-pw", nasPassword, "\"echo ", nasPassword, " | sudo -S /usr/syno/sbin/synoshare --enc_mount \'", listTitles[i], "\' \'", listPassword[i], "\'\"");
        printf("\nUnlocking folder: %s\n", listTitles[i]);
        system(nasPlinkCommand);
    }
    
    return 0;
}

void readPassword(char* outputText, char* password) {
    char c;
    int index = 0;

    printf(outputText);
    /* 13 is ASCII value of Enter key */
    while((c = getch()) != 13) {
        if(c == 3)
            exit(0);
        if(index < 0) {
            index = 0;
            continue;
        }
        /* 8 is ASCII value of BACKSPACE character */
        else if(c == 8 && index > 0) {
            putch('\b');
            putch(' ');
            putch('\b');
            index--;
            continue;
        }
        else if(c == 8 && index == 0)
            continue;
        password[index++] = c;
        putch('*');
    }
    password[index] = '\0';
    printf("\n");
}