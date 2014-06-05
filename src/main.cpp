#include "dialog.h"
#include <QApplication>
#include <cassert>

static QString defaultHexFileName () {
#ifndef _WIN32
  return "hexfiles/linkbot_latest.hex";
#else
  /* Get the install path of BaroboLink from the registry */
  DWORD size;
  HKEY key;
  int rc;

  rc = RegOpenKeyExA(
      HKEY_LOCAL_MACHINE,
      "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\BaroboLink.exe",
      0,
      KEY_QUERY_VALUE,
      &key);

  if (ERROR_SUCCESS != rc) {
    // FIXME?
    return "hexfiles\\linkbot_latest.hex";
  }

  /* Find out how much memory to allocate. */
  rc = RegQueryValueExA(
      key,
      "PATH",
      NULL,
      NULL,
      NULL,
      &size);
  assert(ERROR_SUCCESS == rc);

  /* hlh: FIXME this should probably be TCHAR instead, and we should support
   * unicode or whatever */
  char* path = new char [size + 1];

  rc = RegQueryValueExA(
      key,
      "PATH",
      NULL,
      NULL,
      (LPBYTE)path,
      &size);
  assert(ERROR_SUCCESS == rc);

  path[size] = '\0';

  auto ret = QString(path) + "\\hexfiles\\linkbot_v3.0.6.hex";
  delete [] path;
  path = NULL;

  return ret;
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w { nullptr, argc > 1 ? argv[1] : defaultHexFileName() };
    w.show();

    return a.exec();
}
