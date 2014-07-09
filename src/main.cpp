#include "dialog.h"
#include <QApplication>
#include <cassert>
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

static QString defaultHexFileName () {
  QString hexfilename("linkbot_v3.0.9.hex");
#ifndef _WIN32
// ----------------------------------------------------------------------------
// This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
#ifdef __APPLE__    
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) 
    {
    }
    CFRelease(resourcesURL); 

    chdir(path); 
  return QString::fromStdString(std::string(path)) + QString("/hexfiles/") + hexfilename;
#else
  return QString("hexfiles/linkbot_latest.hex");
#endif
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

  auto ret = QString(path) + "\\hexfiles\\" + hexfilename; 
  delete [] path;
  path = NULL;

  return ret;
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Dialog w { nullptr, argc > 1 ? argv[1] : defaultHexFileName() };
    Dialog w ( NULL, argc > 1 ? argv[1] : defaultHexFileName() );
    w.show();

    return a.exec();
}
