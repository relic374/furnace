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

#ifndef _YM2610_H
#define _YM2610_H
#include "fmshared_OPN.h"
#include "../macroInt.h"
#include "ay.h"
#include "sound/ymfm/ymfm_opn.h"

class DivYM2610Interface: public ymfm::ymfm_interface {
  public:
    unsigned char* adpcmAMem;
    unsigned char* adpcmBMem;
    int sampleBank;
    uint8_t ymfm_external_read(ymfm::access_class type, uint32_t address);
    void ymfm_external_write(ymfm::access_class type, uint32_t address, uint8_t data);
    DivYM2610Interface(): adpcmAMem(NULL), adpcmBMem(NULL), sampleBank(0) {}
};

class DivPlatformYM2610Base: public DivPlatformOPN {
  protected:
    unsigned char* adpcmAMem;
    size_t adpcmAMemLen;
    unsigned char* adpcmBMem;
    size_t adpcmBMemLen;
    DivYM2610Interface iface;

  public:
    const void* getSampleMem(int index);
    size_t getSampleMemCapacity(int index);
    size_t getSampleMemUsage(int index);
    void renderSamples();
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void quit();
    DivPlatformYM2610Base():
      DivPlatformOPN(9440540.0, 72, 32) {}
};

class DivPlatformYM2610: public DivPlatformYM2610Base {
  protected:
    const unsigned short chanOffs[4]={
      0x01, 0x02, 0x101, 0x102
    };

    const unsigned char konOffs[4]={
      1, 2, 5, 6
    };

    const unsigned char bchOffs[4]={
      1, 2, 4, 5
    };

    struct Channel {
      DivInstrumentFM state;
      unsigned char freqH, freqL;
      int freq, baseFreq, pitch, pitch2, portaPauseFreq, note, ins;
      unsigned char psgMode, autoEnvNum, autoEnvDen;
      signed char konCycles;
      bool active, insChanged, freqChanged, keyOn, keyOff, portaPause, inPorta, furnacePCM, hardReset;
      int vol, outVol;
      int sample;
      unsigned char pan;
      DivMacroInt std;
      void macroInit(DivInstrument* which) {
        std.init(which);
        pitch2=0;
      }
      Channel():
        freqH(0),
        freqL(0),
        freq(0),
        baseFreq(0),
        pitch(0),
        pitch2(0),
        portaPauseFreq(0),
        note(0),
        ins(-1),
        psgMode(1),
        autoEnvNum(0),
        autoEnvDen(0),
        active(false),
        insChanged(true),
        freqChanged(false),
        keyOn(false),
        keyOff(false),
        portaPause(false),
        inPorta(false),
        furnacePCM(false),
        hardReset(false),
        vol(0),
        outVol(15),
        sample(-1),
        pan(3) {}
    };
    Channel chan[14];
    DivDispatchOscBuffer* oscBuf[14];
    bool isMuted[14];
    ymfm::ym2610* fm;
    ymfm::ym2610::output_data fmout;

    DivPlatformAY8910* ay;
  
    unsigned char sampleBank;

    bool extMode;
  
    double NOTE_OPNB(int ch, int note);
    double NOTE_ADPCMB(int note);
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
    bool isStereo();
    bool keyOffAffectsArp(int ch);
    void notifyInsChange(int ins);
    void notifyInsDeletion(void* ins);
    void setSkipRegisterWrites(bool val);
    void poke(unsigned int addr, unsigned short val);
    void poke(std::vector<DivRegWrite>& wlist);
    const char** getRegisterSheet();
    void setFlags(unsigned int flags);
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void quit();
    ~DivPlatformYM2610();
};
#endif
