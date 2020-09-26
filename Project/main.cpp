#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include <stdio.h>
#include <windows.h>

void CreateConsole()
{
	FreeConsole();

	// create a separate new console window
	AllocConsole();

	// attach the new console to this application's process
	AttachConsole(GetCurrentProcessId());

	// reopen the std I/O streams to redirect I/O to the new console
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);

	puts("hello, world\n");
}

#include <deque>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CMainWindow w;
	w.show();

	CreateConsole();
	return a.exec();
}
