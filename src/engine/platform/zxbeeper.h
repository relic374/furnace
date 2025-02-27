/**
 * Furnace Tracker - multi-system chiptune tracker
 * Copyright (C) 2021-2022 tildearrow and contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _ZXBEEPER_H
#define _ZXBEEPER_H

#include "../dispatch.h"
#include <queue>
#include "../macroInt.h"

class DivPlatformZXBeeper: public DivDispatch {
  struct Channel {
    int freq, baseFreq, pitch, pitch2, note;
    int ins;
    bool active, insChanged, freqChanged, keyOn, keyOff, inPorta;
    signed char vol, outVol;
    unsigned short sPosition;
    unsigned char duty;
    DivMacroInt std;
    void macroInit(DivInstrument* which) {
      std.init(which);
      pitch2=0;
    }
    Channel():
      freq(0),
      baseFreq(0),
      pitch(0),
      pitch2(0),
      note(0),
      ins(-1),
      active(false),
      insChanged(true),
      freqChanged(false),
      keyOn(false),
      keyOff(false),
      inPorta(false),
      vol(1),
      outVol(1),
      sPosition(0),
      duty(64) {}
  };
  Channel chan[6];
  DivDispatchOscBuffer* oscBuf[6];
  bool isMuted[6];
  struct QueuedWrite {
      unsigned char addr;
      unsigned char val;
      QueuedWrite(unsigned char a, unsigned char v): addr(a), val(v) {}
  };
  std::queue<QueuedWrite> writes;
  unsigned char lastPan, ulaOut;

  int cycles, curChan, sOffTimer, delay, curSample, curSamplePeriod;
  unsigned int curSamplePos;
  int tempL[32];
  int tempR[32];
  unsigned char regPool[128];
  bool sampleOut;
  friend void putDispatchChan(void*,int,int);
  public:
    void acquire(short* bufL, short* bufR, size_t start, size_t len);
    int dispatch(DivCommand c);
    void* getChanState(int chan);
    DivMacroInt* getChanMacroInt(int ch);
    DivDispatchOscBuffer* getOscBuffer(int chan);
    unsigned char* getRegisterPool();
    int getRegisterPoolSize();
    void reset();
    void forceIns();
    void tick(bool sysTick=true);
    void muteChannel(int ch, bool mute);
    bool keyOffAffectsArp(int ch);
    void setFlags(unsigned int flags);
    void notifyWaveChange(int wave);
    void notifyInsDeletion(void* ins);
    void poke(unsigned int addr, unsigned short val);
    void poke(std::vector<DivRegWrite>& wlist);
    const char** getRegisterSheet();
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void quit();
    ~DivPlatformZXBeeper();
};

#endif
