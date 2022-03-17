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
#include "gui.h"
#include "IconsFontAwesome4.h"
#include "misc/cpp/imgui_stdlib.h"
#include "guiConst.h"
#include <imgui.h>

void FurnaceGUI::drawSampleEdit() {
  if (nextWindow==GUI_WINDOW_SAMPLE_EDIT) {
    sampleEditOpen=true;
    ImGui::SetNextWindowFocus();
    nextWindow=GUI_WINDOW_NOTHING;
  }
  if (!sampleEditOpen) return;
  if (ImGui::Begin("Sample Editor",&sampleEditOpen,settings.allowEditDocking?0:ImGuiWindowFlags_NoDocking)) {
    if (curSample<0 || curSample>=(int)e->song.sample.size()) {
      ImGui::Text("no sample selected");
    } else {
      DivSample* sample=e->song.sample[curSample];
      String sampleType="Invalid";
      if (sample->depth<17) {
        if (sampleDepths[sample->depth]!=NULL) {
          sampleType=sampleDepths[sample->depth];
        }
      }
      ImGui::InputText("Name",&sample->name);
      if (ImGui::BeginCombo("Type",sampleType.c_str())) {
        for (int i=0; i<17; i++) {
          if (sampleDepths[i]==NULL) continue;
          if (ImGui::Selectable(sampleDepths[i])) {
            sample->depth=i;
            e->renderSamplesP();
          }
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("no undo for sample type change operations!");
          }
        }
        ImGui::EndCombo();
      }
      if (ImGui::InputInt("Rate (Hz)",&sample->rate,10,200)) {
        if (sample->rate<100) sample->rate=100;
        if (sample->rate>96000) sample->rate=96000;
      }
      if (ImGui::InputInt("Pitch of C-4 (Hz)",&sample->centerRate,10,200)) {
        if (sample->centerRate<100) sample->centerRate=100;
        if (sample->centerRate>65535) sample->centerRate=65535;
      }
      ImGui::Text("effective rate: %dHz",e->getEffectiveSampleRate(sample->rate));
      bool doLoop=(sample->loopStart>=0);
      if (ImGui::Checkbox("Loop",&doLoop)) {
        if (doLoop) {
          sample->loopStart=0;
        } else {
          sample->loopStart=-1;
        }
      }
      if (doLoop) {
        ImGui::SameLine();
        if (ImGui::InputInt("##LoopPosition",&sample->loopStart,1,10)) {
          if (sample->loopStart<0 || sample->loopStart>=(int)sample->samples) {
            sample->loopStart=0;
          }
        }
      }
      if (ImGui::Button("Apply")) {
        e->renderSamplesP();
      }
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_VOLUME_UP "##PreviewSample")) {
        e->previewSample(curSample);
      }
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_VOLUME_OFF "##StopSample")) {
        e->stopSamplePreview();
      }
      ImGui::Separator();
      bool considerations=false;
      ImGui::Text("notes:");
      if (sample->loopStart>=0) {
        considerations=true;
        ImGui::Text("- sample won't loop on Neo Geo ADPCM-A and X1-010");
        if (sample->loopStart&1) {
          ImGui::Text("- sample loop start will be aligned to the nearest even sample on Amiga");
        }
        if (sample->loopStart>0) {
          ImGui::Text("- sample loop start will be ignored on Neo Geo ADPCM-B");
        }
      }
      if (sample->samples&1) {
        considerations=true;
        ImGui::Text("- sample length will be aligned to the nearest even sample on Amiga");
      }
      if (sample->samples&511) {
        considerations=true;
        ImGui::Text("- sample length will be aligned and padded to 512 sample units on Neo Geo ADPCM.");
      }
      if (sample->samples&4095) {
        considerations=true;
        ImGui::Text("- sample length will be aligned and padded to 4096 sample units on X1-010.");
      }
      if (sample->samples>65535) {
        considerations=true;
        ImGui::Text("- maximum sample length on Sega PCM and QSound is 65536 samples");
      }
      if (sample->samples>131071) {
        considerations=true;
        ImGui::Text("- maximum sample length on X1-010 is 131072 samples");
      }
      if (sample->samples>2097151) {
        considerations=true;
        ImGui::Text("- maximum sample length on Neo Geo ADPCM is 2097152 samples");
      }
      if (!considerations) {
        ImGui::Text("- none");
      }
    }
  }
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_SAMPLE_EDIT;
  ImGui::End();
}