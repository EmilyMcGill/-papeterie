#include "notificator.h"

#include <QMetaType>
#include <QVariant>
#include <QIcon>
#include <QApplication>
#include <QStyle>
#include <QByteArray>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QImageWriter>

#ifdef USE_DBUS
#include <QtDBus/QtDBus>
#include <stdint.h>
#endif

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
extern bool qt_mac_execute_apple_script(const QString &script, AEDesc *ret);
#endif

#ifdef USE_DBUS
// https://wiki.ubuntu.com/NotificationDevelopmentGuidelines recommends at least 128
const int FREEDESKTOP_NOTIFICATION_ICON_SIZE = 128;
#endif

Notificator::Notificator(const QString &programName, QSystemTrayIcon *trayicon, QWidget *parent):
    QObject(parent),
    parent(parent),
    programName(programName),
    mode(None),
    trayIcon(trayicon)
#ifdef USE_DBUS
    ,interface(0)
#endif
{
    if(trayicon && trayicon->supportsMessages())
    {
        mode = QSystemTray;
    }
#ifdef USE_DBUS
    interface = new QDBusInterface("org.freedesktop.Notifications",
          "/org/freedesktop/Notifications", "org.freedesktop.Notifications");
    if(interface->isValid())
    {
        mode = Freedesktop;
    }
#endif
#ifdef Q_OS_MAC
    // Check if Growl is installed (based on Qt's tray icon implementation)
    CFURLRef cfurl;
    OSStatus status = LSGetApplicationForInfo(kLSUnknownType, kLSUnknownCreator, CFSTR("growlTicket"), kLSRolesAll, 0, &cfurl);
    if (status != kLSApplicationNotFoundErr) {
        CFBundleRef bundle = CFBundleCreate(0, cfurl);
        if (CFStringCompare(CFBundleGetIdentifier(bundle), CFSTR("com.Growl.GrowlHelperApp"), kCFCompareCaseInsensitive | kCFCompareBackwards) == kCFCompareEqualTo) {
            if (CFStringHasSuffix(CFURLGetString(cfurl), CFSTR("/Growl.app/")))
                mode = Growl13;
            else
                mode = Growl12;
        }
        CFRelease(cfurl);
        CFRelease(bundle);
    }
#endif
}

Notificator::~Notificator()
{
#ifdef USE_DBUS
    delete interface;
#endif
}

#ifdef USE_DBUS

// Loosely based on 