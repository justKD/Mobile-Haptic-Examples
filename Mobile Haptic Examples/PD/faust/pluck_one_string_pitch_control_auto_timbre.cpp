//-----------------------------------------------------
//
// Code generated with Faust 0.9.67 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline float faustpower(float x)          { return powf(x,N); } 
template <int N> inline double faustpower(double x)        { return pow(x,N); }
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
#endif
/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2006-2011 Albert Graef <Dr.Graef@t-online.de>
    ---------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as 
	published by the Free Software Foundation; either version 2.1 of the 
	License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
 	License along with the GNU C Library; if not, write to the Free
  	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  	02111-1307 USA. 
 ************************************************************************
 ************************************************************************/

/* Pd architecture file, written by Albert Graef <Dr.Graef@t-online.de>.
   This was derived from minimal.cpp included in the Faust distribution.
   Please note that this is to be compiled as a shared library, which is
   then loaded dynamically by Pd as an external. */

#include <stdlib.h>
#include <math.h>
#include <string>

#include "misc.h"
#include "UI.h"
#include "meta.h"
#include "dsp.h"

//using namespace std;

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/


/***************************************************************************
   Pd UI interface
 ***************************************************************************/

enum ui_elem_type_t {
  UI_BUTTON, UI_CHECK_BUTTON,
  UI_V_SLIDER, UI_H_SLIDER, UI_NUM_ENTRY,
  UI_V_BARGRAPH, UI_H_BARGRAPH,
  UI_END_GROUP, UI_V_GROUP, UI_H_GROUP, UI_T_GROUP
};

struct ui_elem_t {
  ui_elem_type_t type;
  char *label;
  float *zone;
  float init, min, max, step;
};

class PdUI2 : public UI
{
public:
  int nelems;
  ui_elem_t *elems;
		
  PdUI2();
  PdUI2(const char *s);
  virtual ~PdUI2();

protected:
  std::string path;
  void add_elem(ui_elem_type_t type, const char *label = NULL);
  void add_elem(ui_elem_type_t type, const char *label, float *zone);
  void add_elem(ui_elem_type_t type, const char *label, float *zone,
		float init, float min, float max, float step);
  void add_elem(ui_elem_type_t type, const char *label, float *zone,
		float min, float max);

public:
  virtual void addButton(const char* label, float* zone);
  virtual void addCheckButton(const char* label, float* zone);
  virtual void addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step);
  virtual void addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step);
  virtual void addNumEntry(const char* label, float* zone, float init, float min, float max, float step);

  virtual void addHorizontalBargraph(const char* label, float* zone, float min, float max);
  virtual void addVerticalBargraph(const char* label, float* zone, float min, float max);
  
  virtual void openTabBox(const char* label);
  virtual void openHorizontalBox(const char* label);
  virtual void openVerticalBox(const char* label);
  virtual void closeBox();
	
  virtual void run();
};

static std::string mangle(const char *s)
{
  const char *s0 = s;
  std::string t = "";
  if (!s) return t;
  while (*s)
    if (isalnum(*s))
      t += *(s++);
    else {
      const char *s1 = s;
      while (*s && !isalnum(*s)) ++s;
      if (s1 != s0 && *s) t += "-";
    }
  return t;
}

static std::string normpath(std::string path)
{
  path = std::string("/")+path;
  int pos = path.find("//");
  while (pos >= 0) {
    path.erase(pos, 1);
    pos = path.find("//");
  }
  return path;
}

static std::string pathcat(std::string path, std::string label)
{
  if (path.empty())
    return normpath(label);
  else if (label.empty())
    return normpath(path);
  else
    return normpath(path+"/"+label);
}

PdUI2::PdUI2()
{
  nelems = 0;
  elems = NULL;
  path = "";
}

PdUI2::PdUI2(const char *s)
{
  nelems = 0;
  elems = NULL;
  path = s?s:"";
}

PdUI2::~PdUI2()
{
  if (elems) {
    for (int i = 0; i < nelems; i++)
      if (elems[i].label)
	free(elems[i].label);
    free(elems);
  }
}

inline void PdUI2::add_elem(ui_elem_type_t type, const char *label)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  std::string s = pathcat(path, mangle(label));
  elems[nelems].type = type;
  elems[nelems].label = strdup(s.c_str());
  elems[nelems].zone = NULL;
  elems[nelems].init = 0.0;
  elems[nelems].min = 0.0;
  elems[nelems].max = 0.0;
  elems[nelems].step = 0.0;
  nelems++;
}

inline void PdUI2::add_elem(ui_elem_type_t type, const char *label, float *zone)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  std::string s = pathcat(path, mangle(label));
  elems[nelems].type = type;
  elems[nelems].label = strdup(s.c_str());
  elems[nelems].zone = zone;
  elems[nelems].init = 0.0;
  elems[nelems].min = 0.0;
  elems[nelems].max = 1.0;
  elems[nelems].step = 1.0;
  nelems++;
}

inline void PdUI2::add_elem(ui_elem_type_t type, const char *label, float *zone,
			  float init, float min, float max, float step)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  std::string s = pathcat(path, mangle(label));
  elems[nelems].type = type;
  elems[nelems].label = strdup(s.c_str());
  elems[nelems].zone = zone;
  elems[nelems].init = init;
  elems[nelems].min = min;
  elems[nelems].max = max;
  elems[nelems].step = step;
  nelems++;
}

inline void PdUI2::add_elem(ui_elem_type_t type, const char *label, float *zone,
			  float min, float max)
{
  ui_elem_t *elems1 = (ui_elem_t*)realloc(elems, (nelems+1)*sizeof(ui_elem_t));
  if (elems1)
    elems = elems1;
  else
    return;
  std::string s = pathcat(path, mangle(label));
  elems[nelems].type = type;
  elems[nelems].label = strdup(s.c_str());
  elems[nelems].zone = zone;
  elems[nelems].init = 0.0;
  elems[nelems].min = min;
  elems[nelems].max = max;
  elems[nelems].step = 0.0;
  nelems++;
}

void PdUI2::addButton(const char* label, float* zone)
{ add_elem(UI_BUTTON, label, zone); }
void PdUI2::addCheckButton(const char* label, float* zone)
{ add_elem(UI_CHECK_BUTTON, label, zone); }
void PdUI2::addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_V_SLIDER, label, zone, init, min, max, step); }
void PdUI2::addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_H_SLIDER, label, zone, init, min, max, step); }
void PdUI2::addNumEntry(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_NUM_ENTRY, label, zone, init, min, max, step); }

void PdUI2::addHorizontalBargraph(const char* label, float* zone, float min, float max)
{ add_elem(UI_H_BARGRAPH, label, zone, min, max); }
void PdUI2::addVerticalBargraph(const char* label, float* zone, float min, float max)
{ add_elem(UI_V_BARGRAPH, label, zone, min, max); }

void PdUI2::openTabBox(const char* label)
{
  if (!path.empty()) path += "/";
  path += mangle(label);
}
void PdUI2::openHorizontalBox(const char* label)
{
  if (!path.empty()) path += "/";
  path += mangle(label);
}
void PdUI2::openVerticalBox(const char* label)
{
  if (!path.empty()) path += "/";
  path += mangle(label);
}
void PdUI2::closeBox()
{
  int pos = path.rfind("/");
  if (pos < 0) pos = 0;
  path.erase(pos);
}

void PdUI2::run() {}

/******************************************************************************
*******************************************************************************

			    FAUST DSP

*******************************************************************************
*******************************************************************************/

//----------------------------------------------------------------------------
//  FAUST generated signal processor
//----------------------------------------------------------------------------
		
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  

typedef long double quad;

#ifndef FAUSTCLASS 
#define FAUSTCLASS pluck_one_string_pitch_control_auto_timbre
#endif

class pluck_one_string_pitch_control_auto_timbre : public dsp {
  private:
	int 	iConst0;
	double 	fConst1;
	double 	fConst2;
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	FAUSTFLOAT 	fslider4;
	FAUSTFLOAT 	fslider5;
	double 	fVec0[2];
	FAUSTFLOAT 	fslider6;
	int 	iRec16[2];
	double 	fRec15[2];
	double 	fRec12[2];
	double 	fRec9[2];
	int 	IOTA;
	double 	fVec1[1024];
	double 	fVec2[2];
	double 	fConst3;
	double 	fRec8[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fslider7;
	double 	fVec3[256];
	double 	fRec18[1024];
	double 	fVec4[2];
	double 	fRec17[2];
	double 	fRec1[2];
	double 	fVec5[1024];
	double 	fVec6[2];
	double 	fRec19[2];
	double 	fRec2[2];
	FAUSTFLOAT 	fslider8;
	double 	fVec7[256];
	double 	fRec21[1024];
	double 	fVec8[2];
	double 	fRec20[2];
	double 	fRec3[2];
	double 	fConst4;
	double 	fConst5;
	double 	fConst6;
	double 	fConst7;
	double 	fConst8;
	FAUSTFLOAT 	fslider9;
	double 	fVec9[2];
	double 	fRec24[2];
	double 	fConst9;
	double 	fConst10;
	double 	fRec23[3];
	double 	fConst11;
	double 	fConst12;
	double 	fRec22[3];
	double 	fConst13;
	double 	fConst14;
	double 	fConst15;
	double 	fConst16;
	double 	fConst17;
	double 	fConst18;
	double 	fRec27[2];
	double 	fRec26[2];
	double 	fRec25[2];
	double 	fRec32[3];
	double 	fRec31[3];
	double 	fRec30[2];
	double 	fRec29[2];
	double 	fRec28[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("SAM-fx.lib/name", "freeverb");
		m->declare("SAM-fx.lib/version", "1.0");
		m->declare("SAM-fx.lib/author", "Grame");
		m->declare("SAM-fx.lib/license", "BSD");
		m->declare("SAM-fx.lib/copyright", "(c) GRAME 2006");
		m->declare("SAM-fx.lib/reference", "https://ccrma.stanford.edu/~jos/pasp/Freeverb.html");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("effect.lib/name", "Faust Audio Effect Library");
		m->declare("effect.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("effect.lib/copyright", "Julius O. Smith III");
		m->declare("effect.lib/version", "1.33");
		m->declare("effect.lib/license", "STK-4.3");
		m->declare("effect.lib/exciter_name", "Harmonic Exciter");
		m->declare("effect.lib/exciter_author", "Priyanka Shekar (pshekar@ccrma.stanford.edu)");
		m->declare("effect.lib/exciter_copyright", "Copyright (c) 2013 Priyanka Shekar");
		m->declare("effect.lib/exciter_version", "1.0");
		m->declare("effect.lib/exciter_license", "MIT License (MIT)");
		m->declare("hoa.lib/name", "HOA");
		m->declare("hoa.lib/title", "High Order Ambisonics library");
		m->declare("hoa.lib/author", "Pierre Guillot");
		m->declare("hoa.lib/copyright", "2012-2013 Guillot, Paris, Colafrancesco, CICM labex art H2H, U. Paris 8");
	}

	virtual int getNumInputs() 	{ return 2; }
	virtual int getNumOutputs() 	{ return 4; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (18.84955592153876 / double(iConst0));
		fConst2 = (1.0 / (1 + fConst1));
		fslider0 = 0.4;
		fslider1 = 28.0;
		fslider2 = 45.0;
		fslider3 = 1.6;
		fslider4 = 7.5e+02;
		fslider5 = 0.4;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		fslider6 = 0.006;
		for (int i=0; i<2; i++) iRec16[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		IOTA = 0;
		for (int i=0; i<1024; i++) fVec1[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		fConst3 = (1 - fConst1);
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		fslider7 = 5.5;
		for (int i=0; i<256; i++) fVec3[i] = 0;
		for (int i=0; i<1024; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<1024; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		fslider8 = 3.46;
		for (int i=0; i<256; i++) fVec7[i] = 0;
		for (int i=0; i<1024; i++) fRec21[i] = 0;
		for (int i=0; i<2; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		fConst4 = tan((62.83185307179586 / double(iConst0)));
		fConst5 = (1.0 / fConst4);
		fConst6 = (1.0 / (1 + ((fConst5 + 0.7653668647301795) / fConst4)));
		fConst7 = (1.0 / faustpower<2>(fConst4));
		fConst8 = (1.0 / (1 + ((1.8477590650225735 + fConst5) / fConst4)));
		fslider9 = 0.22;
		for (int i=0; i<2; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		fConst9 = (2 * (1 - fConst7));
		fConst10 = (1 + ((fConst5 - 1.8477590650225735) / fConst4));
		for (int i=0; i<3; i++) fRec23[i] = 0;
		fConst11 = (2 * (0 - fConst7));
		fConst12 = (((fConst5 - 0.7653668647301795) / fConst4) + 1);
		for (int i=0; i<3; i++) fRec22[i] = 0;
		fConst13 = exp((0 - (2.5e+03 / double(iConst0))));
		fConst14 = (1.0 - fConst13);
		fConst15 = exp((0 - (1.25e+03 / double(iConst0))));
		fConst16 = (1.0 - fConst15);
		fConst17 = exp((0 - (1e+01 / double(iConst0))));
		fConst18 = (1.0 - fConst17);
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<3; i++) fRec32[i] = 0;
		for (int i=0; i<3; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("pluck_one_string_pitch_control_auto_timbre");
		interface->addHorizontalSlider("Bridge attenuation", &fslider7, 5.5, 0.5, 7.0, 0.01);
		interface->addHorizontalSlider("Brightness", &fslider2, 45.0, 1e+01, 1e+02, 1.0);
		interface->declare(&fslider1, "MIDI", "");
		interface->addHorizontalSlider("Frequency", &fslider1, 28.0, 2e+01, 84.0, 0.05);
		interface->addHorizontalSlider("Fret attenuation", &fslider8, 3.46, 0.5, 7.0, 0.01);
		interface->declare(&fslider5, "N/(m/s)", "");
		interface->addHorizontalSlider("Pluck damping", &fslider5, 0.4, 0.0, 2.5, 0.01);
		interface->declare(&fslider6, "m", "");
		interface->addHorizontalSlider("Pluck half-width", &fslider6, 0.006, 0.001, 0.01, 0.001);
		interface->addHorizontalSlider("Pluck position along length", &fslider0, 0.4, 0.01, 0.99, 0.01);
		interface->declare(&fslider4, "N/m", "");
		interface->addHorizontalSlider("Pluck stiffness", &fslider4, 7.5e+02, 5e+01, 1e+03, 5e+01);
		interface->addHorizontalSlider("String wave impedance", &fslider3, 1.6, 0.25, 7.5, 0.1);
		interface->addHorizontalSlider("Z Volume", &fslider9, 0.22, 0.01, 1.0, 0.01);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fslider0);
		double 	fSlow1 = pow(2.0,(0.08333333333333333 * (double(fslider1) - 69.0)));
		double 	fSlow2 = (double(fslider2) * fSlow1);
		int 	iSlow3 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow2)))));
		double 	fSlow4 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow1)) - (1.1337868480725624e-05 * double(min(256, (iSlow3 - 2))))));
		int 	iSlow5 = int((fSlow4 - 1.0));
		double 	fSlow6 = (fSlow4 - iSlow5);
		double 	fSlow7 = (fSlow4 - (2 + iSlow5));
		double 	fSlow8 = (fSlow6 * fSlow7);
		double 	fSlow9 = (0 - ((1 + iSlow5) - fSlow4));
		double 	fSlow10 = (0.16666666666666666 * (fSlow8 * fSlow9));
		double 	fSlow11 = double(fslider3);
		double 	fSlow12 = double(fslider4);
		double 	fSlow13 = double(fslider5);
		double 	fSlow14 = (((2 * fSlow11) + (2.2675736961451248e-05 * fSlow12)) + fSlow13);
		double 	fSlow15 = (2.2675736961451248e-05 / fSlow14);
		double 	fSlow16 = (4.41e+04 * fSlow13);
		double 	fSlow17 = (2.0 * fSlow11);
		double 	fSlow18 = double(fslider6);
		double 	fSlow19 = (0 - fSlow18);
		double 	fSlow20 = (1.0 / fSlow14);
		int 	iSlow21 = int((fSlow4 - 2.0));
		int 	iSlow22 = int((int((3 + iSlow21)) & 1023));
		double 	fSlow23 = (fSlow4 - (3 + iSlow5));
		double 	fSlow24 = (0 - ((fSlow6 * fSlow9) * fSlow23));
		int 	iSlow25 = int((int((2 + iSlow21)) & 1023));
		double 	fSlow26 = (fSlow8 * fSlow23);
		int 	iSlow27 = int((int((1 + iSlow21)) & 1023));
		double 	fSlow28 = (0.3333333333333333 * (0 - ((fSlow7 * fSlow9) * fSlow23)));
		int 	iSlow29 = int((iSlow21 & 1023));
		int 	iSlow30 = (iSlow3 - 1);
		double 	fSlow31 = (1.0 / double(max(1, iSlow30)));
		double 	fSlow32 = (pow(1e+01,double((0 - double(fslider7)))) - 1.0);
		int 	iSlow33 = int((int(iSlow30) & 255));
		int 	iSlow34 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow2)))));
		double 	fSlow35 = (2.205e+04 * ((0.0022727272727272726 * ((1.0 - fSlow0) / fSlow1)) - (1.1337868480725624e-05 * double(min(256, (iSlow34 - 2))))));
		int 	iSlow36 = int((fSlow35 - 1.0));
		double 	fSlow37 = (fSlow35 - iSlow36);
		double 	fSlow38 = (0 - ((1 + iSlow36) - fSlow35));
		double 	fSlow39 = (fSlow37 * fSlow38);
		double 	fSlow40 = (fSlow35 - (3 + iSlow36));
		double 	fSlow41 = (0 - (fSlow39 * fSlow40));
		int 	iSlow42 = int((fSlow35 - 2.0));
		int 	iSlow43 = int((int((2 + iSlow42)) & 1023));
		double 	fSlow44 = (fSlow35 - (2 + iSlow36));
		double 	fSlow45 = ((fSlow37 * fSlow44) * fSlow40);
		int 	iSlow46 = int((int((1 + iSlow42)) & 1023));
		double 	fSlow47 = (0.3333333333333333 * (0 - ((fSlow38 * fSlow44) * fSlow40)));
		int 	iSlow48 = int((iSlow42 & 1023));
		double 	fSlow49 = (0.16666666666666666 * (fSlow39 * fSlow44));
		int 	iSlow50 = int((int((3 + iSlow42)) & 1023));
		int 	iSlow51 = (iSlow34 - 1);
		double 	fSlow52 = (1.0 / double(max(1, iSlow51)));
		double 	fSlow53 = (pow(1e+01,double((0 - double(fslider8)))) - 1.0);
		int 	iSlow54 = int((int(iSlow51) & 255));
		double 	fSlow55 = double(fslider9);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		FAUSTFLOAT* output2 = output[2];
		FAUSTFLOAT* output3 = output[3];
		for (int i=0; i<count; i++) {
			double fTemp0 = (double)input0[i];
			fVec0[0] = fTemp0;
			double fTemp1 = (fVec0[0] - fVec0[1]);
			double fTemp2 = (fRec1[1] + fRec3[1]);
			double fTemp3 = (((fSlow12 * (fVec0[0] - (0.999 * fRec9[1]))) + (fSlow16 * fTemp1)) + (fSlow17 * fTemp2));
			double fTemp4 = (((0.999 * fRec12[1]) + (fSlow15 * fTemp3)) - fVec0[0]);
			int iTemp5 = ((iRec16[1] == 0) & (fTemp4 < fSlow19));
			iRec16[0] = (iTemp5 + (iRec16[1] * ((1 - ((iRec16[1] == 1) & (fTemp4 > fSlow18))) & (1 - iTemp5))));
			int iTemp6 = ((((iRec16[0] == 0) & (fSlow19 <= fTemp4)) & (fTemp4 <= 0)) + (((iRec16[0] == 1) & (0 <= fTemp4)) & (fTemp4 <= fSlow18)));
			double fTemp7 = ((int(iTemp6))?(fSlow20 * fTemp3):fTemp2);
			fRec15[0] = ((0.999 * fRec15[1]) + (2.2675736961451248e-05 * fTemp7));
			fRec12[0] = fRec15[0];
			double 	fRec13 = fTemp7;
			int 	iRec14 = iTemp6;
			fRec9[0] = fRec12[0];
			double 	fRec10 = fRec13;
			int 	iRec11 = iRec14;
			double fTemp8 = (fRec10 - fRec1[1]);
			fVec1[IOTA&1023] = fTemp8;
			double fTemp9 = ((fSlow10 * fVec1[(IOTA-iSlow22)&1023]) + (0.5 * ((fSlow24 * fVec1[(IOTA-iSlow25)&1023]) + ((fSlow26 * fVec1[(IOTA-iSlow27)&1023]) + (fSlow28 * fVec1[(IOTA-iSlow29)&1023])))));
			fVec2[0] = fTemp9;
			fRec8[0] = (fConst2 * ((fVec2[0] - fVec2[1]) + (fConst3 * fRec8[1])));
			fRec0[0] = fRec8[0];
			double fTemp10 = (fSlow32 * fRec0[1]);
			fVec3[IOTA&255] = fTemp10;
			fRec18[IOTA&1023] = (fRec18[(IOTA-1)&1023] + (fSlow31 * (fVec3[IOTA&255] - fVec3[(IOTA-iSlow33)&255])));
			double fTemp11 = ((0.5 * ((fSlow24 * fRec18[(IOTA-iSlow25)&1023]) + ((fSlow26 * fRec18[(IOTA-iSlow27)&1023]) + (fSlow28 * fRec18[(IOTA-iSlow29)&1023])))) + (fSlow10 * fRec18[(IOTA-iSlow22)&1023]));
			fVec4[0] = fTemp11;
			fRec17[0] = (fConst2 * ((fVec4[0] - fVec4[1]) + (fConst3 * fRec17[1])));
			fRec1[0] = fRec17[0];
			double fTemp12 = (fRec10 - fRec3[1]);
			fVec5[IOTA&1023] = fTemp12;
			double fTemp13 = ((0.5 * ((fSlow41 * fVec5[(IOTA-iSlow43)&1023]) + ((fSlow45 * fVec5[(IOTA-iSlow46)&1023]) + (fSlow47 * fVec5[(IOTA-iSlow48)&1023])))) + (fSlow49 * fVec5[(IOTA-iSlow50)&1023]));
			fVec6[0] = fTemp13;
			fRec19[0] = (fConst2 * ((fVec6[0] - fVec6[1]) + (fConst3 * fRec19[1])));
			fRec2[0] = fRec19[0];
			double fTemp14 = (fSlow53 * fRec2[1]);
			fVec7[IOTA&255] = fTemp14;
			fRec21[IOTA&1023] = (fRec21[(IOTA-1)&1023] + (fSlow52 * (fVec7[IOTA&255] - fVec7[(IOTA-iSlow54)&255])));
			double fTemp15 = ((fSlow49 * fRec21[(IOTA-iSlow50)&1023]) + (0.5 * ((fSlow41 * fRec21[(IOTA-iSlow43)&1023]) + ((fSlow45 * fRec21[(IOTA-iSlow46)&1023]) + (fSlow47 * fRec21[(IOTA-iSlow48)&1023])))));
			fVec8[0] = fTemp15;
			fRec20[0] = (fConst2 * ((fVec8[0] - fVec8[1]) + (fConst3 * fRec20[1])));
			fRec3[0] = fRec20[0];
			double 	fRec4 = (iRec11 * ((fSlow12 * (fRec9[0] - fVec0[0])) + (fSlow13 * (fRec10 - (4.41e+04 * fTemp1)))));
			double 	fRec5 = 0.0;
			fVec9[0] = fSlow55;
			fRec24[0] = ((0.9985762556729113 * fRec24[1]) + (0.0007118721635443319 * (fSlow55 + fVec9[1])));
			fRec23[0] = ((fRec18[(IOTA-0)&1023] * fRec24[0]) - (fConst8 * ((fConst9 * fRec23[1]) + (fConst10 * fRec23[2]))));
			fRec22[0] = ((fConst8 * (((fConst7 * fRec23[0]) + (fConst11 * fRec23[1])) + (fConst7 * fRec23[2]))) - (fConst6 * ((fConst9 * fRec22[1]) + (fConst12 * fRec22[2]))));
			double fTemp16 = (((fConst7 * fRec22[0]) + (fConst11 * fRec22[1])) + (fConst7 * fRec22[2]));
			double fTemp17 = fabs((fConst6 * fTemp16));
			fRec27[0] = ((fConst17 * max(fTemp17, fRec27[1])) + (fConst18 * fTemp17));
			fRec26[0] = ((fConst15 * fRec26[1]) + (fConst16 * fRec27[0]));
			fRec25[0] = ((fConst13 * fRec25[1]) + (fConst14 * (0 - (0.99999999999 * max((6 + (20 * log10(fRec26[0]))), 0.0)))));
			double 	fRec6 = (fConst6 * (fTemp16 * pow(10,(0.05 * fRec25[0]))));
			fRec32[0] = ((fRec21[(IOTA-0)&1023] * fRec24[0]) - (fConst8 * ((fConst10 * fRec32[2]) + (fConst9 * fRec32[1]))));
			fRec31[0] = ((fConst8 * (((fConst7 * fRec32[0]) + (fConst11 * fRec32[1])) + (fConst7 * fRec32[2]))) - (fConst6 * ((fConst12 * fRec31[2]) + (fConst9 * fRec31[1]))));
			double fTemp18 = (((fConst7 * fRec31[0]) + (fConst11 * fRec31[1])) + (fConst7 * fRec31[2]));
			double fTemp19 = fabs((fConst6 * fTemp18));
			fRec30[0] = ((fConst17 * max(fTemp19, fRec30[1])) + (fConst18 * fTemp19));
			fRec29[0] = ((fConst15 * fRec29[1]) + (fConst16 * fRec30[0]));
			fRec28[0] = ((fConst14 * (0 - (0.99999999999 * max((6 + (20 * log10(fRec29[0]))), 0.0)))) + (fConst13 * fRec28[1]));
			double 	fRec7 = (fConst6 * (pow(10,(0.05 * fRec28[0])) * fTemp18));
			output0[i] = (FAUSTFLOAT)fRec4;
			output1[i] = (FAUSTFLOAT)fRec5;
			output2[i] = (FAUSTFLOAT)fRec6;
			output3[i] = (FAUSTFLOAT)fRec7;
			// post processing
			fRec28[1] = fRec28[0];
			fRec29[1] = fRec29[0];
			fRec30[1] = fRec30[0];
			fRec31[2] = fRec31[1]; fRec31[1] = fRec31[0];
			fRec32[2] = fRec32[1]; fRec32[1] = fRec32[0];
			fRec25[1] = fRec25[0];
			fRec26[1] = fRec26[0];
			fRec27[1] = fRec27[0];
			fRec22[2] = fRec22[1]; fRec22[1] = fRec22[0];
			fRec23[2] = fRec23[1]; fRec23[1] = fRec23[0];
			fRec24[1] = fRec24[0];
			fVec9[1] = fVec9[0];
			fRec3[1] = fRec3[0];
			fRec20[1] = fRec20[0];
			fVec8[1] = fVec8[0];
			fRec2[1] = fRec2[0];
			fRec19[1] = fRec19[0];
			fVec6[1] = fVec6[0];
			fRec1[1] = fRec1[0];
			fRec17[1] = fRec17[0];
			fVec4[1] = fVec4[0];
			fRec0[1] = fRec0[0];
			fRec8[1] = fRec8[0];
			fVec2[1] = fVec2[0];
			IOTA = IOTA+1;
			fRec9[1] = fRec9[0];
			fRec12[1] = fRec12[0];
			fRec15[1] = fRec15[0];
			iRec16[1] = iRec16[0];
			fVec0[1] = fVec0[0];
		}
	}
};



#include <stdio.h>
#include <string>
#include "m_pd.h"

#define faust_setup(name) xfaust_setup(name)
#define xfaust_setup(name) name ## _tilde_setup(void)
#define sym(name) xsym(name)
#define xsym(name) #name

// time for "active" toggle xfades in secs
#define XFADE_TIME 0.1f

static t_class *faust_class;

struct t_faust {
  t_object x_obj;
#ifdef __MINGW32__
  /* This seems to be necessary as some as yet undetermined Pd routine seems
     to write past the end of x_obj on Windows. */
  int fence; /* dummy field (not used) */
#endif
  pluck_one_string_pitch_control_auto_timbre *dsp;
  PdUI2 *ui;
  std::string *label;
  int active, xfade, n_xfade, rate, n_in, n_out;
  t_sample **inputs, **outputs, **buf;
  t_outlet *out;
  t_sample f;
};

static t_symbol *s_button, *s_checkbox, *s_vslider, *s_hslider, *s_nentry,
  *s_vbargraph, *s_hbargraph;

static inline void zero_samples(int k, int n, t_sample **out)
{
  for (int i = 0; i < k; i++)
#ifdef __STDC_IEC_559__
    /* IEC 559 a.k.a. IEEE 754 floats can be initialized faster like this */
    memset(out[i], 0, n*sizeof(t_sample));
#else
    for (int j = 0; j < n; j++)
      out[i][j] = 0.0f;
#endif
}

static inline void copy_samples(int k, int n, t_sample **out, t_sample **in)
{
  for (int i = 0; i < k; i++)
    memcpy(out[i], in[i], n*sizeof(t_sample));
}

static t_int *faust_perform(t_int *w)
{
  t_faust *x = (t_faust *)(w[1]);
  int n = (int)(w[2]);
  if (!x->dsp || !x->buf) return (w+3);
  AVOIDDENORMALS;
  if (x->xfade > 0) {
    float d = 1.0f/x->n_xfade, f = (x->xfade--)*d;
    d = d/n;
    x->dsp->compute(n, x->inputs, x->buf);
    if (x->active)
      if (x->n_in == x->n_out)
	/* xfade inputs -> buf */
	for (int j = 0; j < n; j++, f -= d)
	  for (int i = 0; i < x->n_out; i++)
	    x->outputs[i][j] = f*x->inputs[i][j]+(1.0f-f)*x->buf[i][j];
      else
	/* xfade 0 -> buf */
	for (int j = 0; j < n; j++, f -= d)
	  for (int i = 0; i < x->n_out; i++)
	    x->outputs[i][j] = (1.0f-f)*x->buf[i][j];
    else
      if (x->n_in == x->n_out)
	/* xfade buf -> inputs */
	for (int j = 0; j < n; j++, f -= d)
	  for (int i = 0; i < x->n_out; i++)
	    x->outputs[i][j] = f*x->buf[i][j]+(1.0f-f)*x->inputs[i][j];
      else
	/* xfade buf -> 0 */
	for (int j = 0; j < n; j++, f -= d)
	  for (int i = 0; i < x->n_out; i++)
	    x->outputs[i][j] = f*x->buf[i][j];
  } else if (x->active) {
    x->dsp->compute(n, x->inputs, x->buf);
    copy_samples(x->n_out, n, x->outputs, x->buf);
  } else if (x->n_in == x->n_out) {
    copy_samples(x->n_out, n, x->buf, x->inputs);
    copy_samples(x->n_out, n, x->outputs, x->buf);
  } else
    zero_samples(x->n_out, n, x->outputs);
  return (w+3);
}

static void faust_dsp(t_faust *x, t_signal **sp)
{
  int n = sp[0]->s_n, sr = (int)sp[0]->s_sr;
  if (x->rate <= 0) {
    /* default sample rate is whatever Pd tells us */
    PdUI2 *ui = x->ui;
    float *z = NULL;
    if (ui->nelems > 0 &&
	(z = (float*)malloc(ui->nelems*sizeof(float)))) {
      /* save the current control values */
      for (int i = 0; i < ui->nelems; i++)
	if (ui->elems[i].zone)
	  z[i] = *ui->elems[i].zone;
    }
    /* set the proper sample rate; this requires reinitializing the dsp */
    x->rate = sr;
    x->dsp->init(sr);
    if (z) {
      /* restore previous control values */
      for (int i = 0; i < ui->nelems; i++)
	if (ui->elems[i].zone)
	  *ui->elems[i].zone = z[i];
      free(z);
    }
  }
  if (n > 0)
    x->n_xfade = (int)(x->rate*XFADE_TIME/n);
  dsp_add(faust_perform, 2, x, n);
  for (int i = 0; i < x->n_in; i++)
    x->inputs[i] = sp[i+1]->s_vec;
  for (int i = 0; i < x->n_out; i++)
    x->outputs[i] = sp[x->n_in+i+1]->s_vec;
  if (x->buf != NULL)
    for (int i = 0; i < x->n_out; i++) {
      x->buf[i] = (t_sample*)malloc(n*sizeof(t_sample));
      if (x->buf[i] == NULL) {
	for (int j = 0; j < i; j++)
	  free(x->buf[j]);
	free(x->buf);
	x->buf = NULL;
	break;
      }
    }
}

static int pathcmp(const char *s, const char *t)
{
  int n = strlen(s), m = strlen(t);
  if (n == 0 || m == 0)
    return 0;
  else if (t[0] == '/')
    return strcmp(s, t);
  else if (n <= m || s[n-m-1] != '/')
    return strcmp(s+1, t);
  else
    return strcmp(s+n-m, t);
}

static void faust_any(t_faust *x, t_symbol *s, int argc, t_atom *argv)
{
  if (!x->dsp) return;
  PdUI2 *ui = x->ui;
  if (s == &s_bang) {
    for (int i = 0; i < ui->nelems; i++)
      if (ui->elems[i].label && ui->elems[i].zone) {
	t_atom args[6];
	t_symbol *_s;
	switch (ui->elems[i].type) {
	case UI_BUTTON:
	  _s = s_button;
	  break;
	case UI_CHECK_BUTTON:
	  _s = s_checkbox;
	  break;
	case UI_V_SLIDER:
	  _s = s_vslider;
	  break;
	case UI_H_SLIDER:
	  _s = s_hslider;
	  break;
	case UI_NUM_ENTRY:
	  _s = s_nentry;
	  break;
	case UI_V_BARGRAPH:
	  _s = s_vbargraph;
	  break;
	case UI_H_BARGRAPH:
	  _s = s_hbargraph;
	  break;
	default:
	  continue;
	}
	SETSYMBOL(&args[0], gensym(ui->elems[i].label));
	SETFLOAT(&args[1], *ui->elems[i].zone);
	SETFLOAT(&args[2], ui->elems[i].init);
	SETFLOAT(&args[3], ui->elems[i].min);
	SETFLOAT(&args[4], ui->elems[i].max);
	SETFLOAT(&args[5], ui->elems[i].step);
	outlet_anything(x->out, _s, 6, args);
      }
  } else {
    const char *label = s->s_name;
    int count = 0;
    for (int i = 0; i < ui->nelems; i++)
      if (ui->elems[i].label &&
	  pathcmp(ui->elems[i].label, label) == 0) {
	if (argc == 0) {
	  if (ui->elems[i].zone) {
	    t_atom arg;
	    SETFLOAT(&arg, *ui->elems[i].zone);
	    outlet_anything(x->out, gensym(ui->elems[i].label), 1, &arg);
	  }
	  ++count;
	} else if (argc == 1 &&
		   (argv[0].a_type == A_FLOAT ||
		    argv[0].a_type == A_DEFFLOAT) &&
		   ui->elems[i].zone) {
	  float f = atom_getfloat(argv);
	  *ui->elems[i].zone = f;
	  ++count;
	} else
	  pd_error(x, "[faust] %s: bad control argument: %s",
		   x->label->c_str(), label);
      }
    if (count == 0 && strcmp(label, "active") == 0) {
      if (argc == 0) {
	t_atom arg;
	SETFLOAT(&arg, (float)x->active);
	outlet_anything(x->out, gensym((char*)"active"), 1, &arg);
      } else if (argc == 1 &&
		 (argv[0].a_type == A_FLOAT ||
		  argv[0].a_type == A_DEFFLOAT)) {
	float f = atom_getfloat(argv);
	x->active = (int)f;
	x->xfade = x->n_xfade;
      }
    }
  }
}

static void faust_free(t_faust *x)
{
  if (x->label) delete x->label;
  if (x->dsp) delete x->dsp;
  if (x->ui) delete x->ui;
  if (x->inputs) free(x->inputs);
  if (x->outputs) free(x->outputs);
  if (x->buf) {
    for (int i = 0; i < x->n_out; i++)
      if (x->buf[i]) free(x->buf[i]);
    free(x->buf);
  }
}

static void *faust_new(t_symbol *s, int argc, t_atom *argv)
{
  t_faust *x = (t_faust*)pd_new(faust_class);
  int sr = -1;
  t_symbol *id = NULL;
  x->active = 1;
  for (int i = 0; i < argc; i++)
    if (argv[i].a_type == A_FLOAT || argv[i].a_type == A_DEFFLOAT)
      sr = (int)argv[i].a_w.w_float;
    else if (argv[i].a_type == A_SYMBOL || argv[i].a_type == A_DEFSYMBOL)
      id = argv[i].a_w.w_symbol;
  x->rate = sr;
  if (sr <= 0) sr = 44100;
  x->xfade = 0; x->n_xfade = (int)(sr*XFADE_TIME/64);
  x->inputs = x->outputs = x->buf = NULL;
    x->label = new std::string(sym(pluck_one_string_pitch_control_auto_timbre) "~");
  x->dsp = new pluck_one_string_pitch_control_auto_timbre();
  x->ui = new PdUI2(id?id->s_name:NULL);
  if (!x->dsp || !x->ui || !x->label) goto error;
  if (id) {
    *x->label += " ";
    *x->label += id->s_name;
  }
  x->n_in = x->dsp->getNumInputs();
  x->n_out = x->dsp->getNumOutputs();
  if (x->n_in > 0)
    x->inputs = (t_sample**)malloc(x->n_in*sizeof(t_sample*));
  if (x->n_out > 0) {
    x->outputs = (t_sample**)malloc(x->n_out*sizeof(t_sample*));
    x->buf = (t_sample**)malloc(x->n_out*sizeof(t_sample*));
  }
  if ((x->n_in > 0 && x->inputs == NULL) ||
      (x->n_out > 0 && (x->outputs == NULL || x->buf == NULL)))
    goto error;
  for (int i = 0; i < x->n_out; i++)
    x->buf[i] = NULL;
  x->dsp->init(sr);
  x->dsp->buildUserInterface(x->ui);
  for (int i = 0; i < x->n_in; i++)
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  x->out = outlet_new(&x->x_obj, 0);
  for (int i = 0; i < x->n_out; i++)
    outlet_new(&x->x_obj, &s_signal);
  return (void *)x;
 error:
  faust_free(x);
  x->dsp = NULL; x->ui = NULL;
  x->inputs = x->outputs = x->buf = NULL;
  return (void *)x;
}

extern "C" void faust_setup(pluck_one_string_pitch_control_auto_timbre)
{
  t_symbol *s = gensym(sym(pluck_one_string_pitch_control_auto_timbre) "~");
  faust_class =
    class_new(s, (t_newmethod)faust_new, (t_method)faust_free,
	      sizeof(t_faust), CLASS_DEFAULT,
	      A_GIMME, A_NULL);
  class_addmethod(faust_class, (t_method)faust_dsp, gensym((char*)"dsp"), A_NULL);
  class_addanything(faust_class, faust_any);
  class_addmethod(faust_class, nullfn, &s_signal, A_NULL);
  s_button = gensym((char*)"button");
  s_checkbox = gensym((char*)"checkbox");
  s_vslider = gensym((char*)"vslider");
  s_hslider = gensym((char*)"hslider");
  s_nentry = gensym((char*)"nentry");
  s_vbargraph = gensym((char*)"vbargraph");
  s_hbargraph = gensym((char*)"hbargrap");
  /* give some indication that we're loaded and ready to go */
  pluck_one_string_pitch_control_auto_timbre dsp = pluck_one_string_pitch_control_auto_timbre();
  post("[faust] %s: %d inputs, %d outputs", sym(pluck_one_string_pitch_control_auto_timbre) "~",
       dsp.getNumInputs(), dsp.getNumOutputs());
}
