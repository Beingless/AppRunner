// app_runner.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <time.h>

#include <Windows.h>
#include <shellapi.h>
#include <winuser.h>

void writeLine(FILE* f, const char* line)
{
    if (NULL == f || NULL == line)
    {
        return;
    }

    time_t rawtime;
    struct tm * timeinfo;
    static const int TIME_BUFFER_SIZE = 128;
    char timeBuffer[TIME_BUFFER_SIZE] = "";

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(timeBuffer, TIME_BUFFER_SIZE, "%F %X", timeinfo);

    fprintf(f, "[%s] %s\n", timeBuffer, line);
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    char* app = strtok(lpCmdLine, ",");
    if (NULL == app)
    {
        return 0;
    }

    char* logFile = strtok(NULL, ",");
    if (NULL == logFile)
    {
        logFile = "C:\\wosp_crash.log";
    }

    FILE* f = fopen(logFile, "a+");
    if (NULL == f)
    {
        return 0;
    }

    static const int LINE_SIZE = 128;
    char lineBuffer[LINE_SIZE] = "";

    writeLine(f, "=============================");
    memset(lineBuffer, 0, LINE_SIZE);
    sprintf(lineBuffer, "app runner started. cmd line: %s", lpCmdLine);
    writeLine(f, lineBuffer);

    SHELLEXECUTEINFOA ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = app;
    ShExecInfo.lpParameters = "";
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteExA(&ShExecInfo);
    DWORD dwWaitResult = WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

    memset(lineBuffer, 0, LINE_SIZE);
    sprintf(lineBuffer, "WaitForSingleObject returns: 0x%X", dwWaitResult);
    writeLine(f, lineBuffer);

    DWORD exit_code;
    if (FALSE == GetExitCodeProcess(ShExecInfo.hProcess, &exit_code))
    {
        memset(lineBuffer, 0, LINE_SIZE);
        sprintf(lineBuffer, "GetExitCodeProcess() failure: 0x%X", GetLastError());
        writeLine(f, lineBuffer);
    }
    else if (STILL_ACTIVE == exit_code)
    {
        writeLine(f, "Still running");
    }
    else
    {
        memset(lineBuffer, 0, LINE_SIZE);
        sprintf(lineBuffer, "exit code: 0x%X", exit_code);
        writeLine(f, lineBuffer);
    }

    CloseHandle(ShExecInfo.hProcess);

    writeLine(f, "app runner finished.");
    writeLine(f, "=============================\n");

    return 0;
}


