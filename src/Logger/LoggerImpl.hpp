/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#if !defined(XCSOAR_LOGGER_IMPL_HPP)
#define XCSOAR_LOGGER_IMPL_HPP

#include "Sizes.h"
#include "DateTime.hpp"
#include "GPSClock.hpp"
#include "Poco/RWLock.h"
#include "Poco/RWLock.h"
#include "Navigation/GeoPoint.hpp"

#include <tchar.h>
#include <windef.h>

struct NMEA_INFO;
struct SETTINGS_COMPUTER;
struct Declaration;
class OrderedTask;

class LoggerImpl {
public:
  enum {
    LOGGER_PRETAKEOFF_BUFFER_MAX = 60,
    LOGGER_DISK_BUFFER_NUM_RECS = 10,
  };

  struct LoggerPreTakeoffBuffer {
    GEOPOINT Location;
    double Altitude;
    double BaroAltitude;
    BrokenDateTime DateTime;
    int SatelliteIDs[MAXSATELLITES];
    double Time;
    int NAVWarning;

    const struct LoggerPreTakeoffBuffer &operator=(const NMEA_INFO &src);
  };

public:
  LoggerImpl();

public:
  void LogPoint(const NMEA_INFO &gps_info);
  bool CheckDeclaration(void);
  bool isTaskDeclared() const;
  bool isLoggerActive() const;
  bool LoggerClearFreeSpace(const NMEA_INFO &gps_info);
  void guiStartLogger(const NMEA_INFO& gps_info,
                      const SETTINGS_COMPUTER& settings,
                      bool noAsk = false);
  void guiToggleLogger(const NMEA_INFO& gps_info,
                       const SETTINGS_COMPUTER& settings,
                       bool noAsk = false);
  void guiStopLogger(const NMEA_INFO &gps_info,
                     bool noAsk = false);
  void LoggerDeviceDeclare(const OrderedTask& task);
  void LoggerNote(const TCHAR *text);
  void clearBuffer();

private:
  void WriteLock() {
    lock.writeLock();
  }
  void Unlock() {
    lock.unlock();
  }
  Poco::RWLock lock;

private:
  void StartLogger(const NMEA_INFO &gps_info,
                   const SETTINGS_COMPUTER &settings,
                   const TCHAR *strAssetNumber);

  void AddDeclaration(const GEOPOINT &location, const TCHAR *ID);
  void StartDeclaration(const NMEA_INFO &gps_info,
                        const int numturnpoints);
  void EndDeclaration(void);
  void LoggerHeader(const NMEA_INFO &gps_info, const Declaration& decl);

  void StopLogger(const NMEA_INFO &gps_info);
  bool IGCWriteRecord(const char *szIn, const TCHAR *);

  bool LoggerDeclare(struct DeviceDescriptor *dev,
                     const Declaration &decl);
  void LoggerGInit();
  
private:
  void LogPointToFile(const NMEA_INFO& gps_info);
  void LogPointToBuffer(const NMEA_INFO &gps_info);
  void LoggerGStop(TCHAR* szLoggerFileName);
  
private:
  void LogFRecordToFile(const int SatelliteIDs[],
                        const BrokenTime broken_time, double Time,
                        int NAVWarning);
  void ResetFRecord(void);
  int LastFRecordValid;
  char szLastFRecord[MAX_IGC_BUFF];
  bool DetectFRecordChange;
  GPSClock frecord_clock;
  const char * GetHFFXARecord(void);
  const char * GetIRecord(void);
  double GetEPE(const NMEA_INFO& gps_info);
  int GetSIU(const NMEA_INFO& gps_info);

private:
  bool LoggerActive;
  bool DeclaredToDevice;

  /**
   * If at least one GPS fix came from the simulator
   * (NMEA_INFO.Simulator), the this flag is true, and signing is
   * disabled.
   */
  bool Simulator;

  TCHAR szLoggerFileName[MAX_PATH];
  char szLoggerFileName_c[MAX_PATH];
  int NumLoggerPreTakeoffBuffered;
  LoggerPreTakeoffBuffer FirstPoint;
  LoggerPreTakeoffBuffer LoggerPreTakeoffBuffer[LOGGER_PRETAKEOFF_BUFFER_MAX];

  /* stdio buffering is bad on wince3.0:
   * it appends up to 1024 NULLs at the end of the file if PDA power fails
   * This does not cause SeeYou to crash (today - but it may in the future)
   * NULLs are invalid IGC characters
   * So, we're creating our manual disk buffering system for the IGC files
   */
   
private:
  int LoggerDiskBufferCount;
  char LoggerDiskBuffer[LOGGER_DISK_BUFFER_NUM_RECS][MAX_IGC_BUFF];
  void DiskBufferFlush();
  bool DiskBufferAdd(char *sIn);
  void DiskBufferReset();
};

#endif
