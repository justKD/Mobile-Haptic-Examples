//-----------------------------------------------------
//
// Code generated with Faust 0.9.67 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
//#include <fenv.h>
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

class PdUI : public UI
{
public:
  int nelems;
  ui_elem_t *elems;
		
  PdUI();
  PdUI(const char *s);
  virtual ~PdUI();

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

PdUI::PdUI()
{
  nelems = 0;
  elems = NULL;
  path = "";
}

PdUI::PdUI(const char *s)
{
  nelems = 0;
  elems = NULL;
  path = s?s:"";
}

PdUI::~PdUI()
{
  if (elems) {
    for (int i = 0; i < nelems; i++)
      if (elems[i].label)
	free(elems[i].label);
    free(elems);
  }
}

inline void PdUI::add_elem(ui_elem_type_t type, const char *label)
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

inline void PdUI::add_elem(ui_elem_type_t type, const char *label, float *zone)
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

inline void PdUI::add_elem(ui_elem_type_t type, const char *label, float *zone,
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

inline void PdUI::add_elem(ui_elem_type_t type, const char *label, float *zone,
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

void PdUI::addButton(const char* label, float* zone)
{ add_elem(UI_BUTTON, label, zone); }
void PdUI::addCheckButton(const char* label, float* zone)
{ add_elem(UI_CHECK_BUTTON, label, zone); }
void PdUI::addVerticalSlider(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_V_SLIDER, label, zone, init, min, max, step); }
void PdUI::addHorizontalSlider(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_H_SLIDER, label, zone, init, min, max, step); }
void PdUI::addNumEntry(const char* label, float* zone, float init, float min, float max, float step)
{ add_elem(UI_NUM_ENTRY, label, zone, init, min, max, step); }

void PdUI::addHorizontalBargraph(const char* label, float* zone, float min, float max)
{ add_elem(UI_H_BARGRAPH, label, zone, min, max); }
void PdUI::addVerticalBargraph(const char* label, float* zone, float min, float max)
{ add_elem(UI_V_BARGRAPH, label, zone, min, max); }

void PdUI::openTabBox(const char* label)
{
  if (!path.empty()) path += "/";
  path += mangle(label);
}
void PdUI::openHorizontalBox(const char* label)
{
  if (!path.empty()) path += "/";
  path += mangle(label);
}
void PdUI::openVerticalBox(const char* label)
{
  if (!path.empty()) path += "/";
  path += mangle(label);
}
void PdUI::closeBox()
{
  int pos = path.rfind("/");
  if (pos < 0) pos = 0;
  path.erase(pos);
}

void PdUI::run() {}

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
#define FAUSTCLASS pluck_harp10
#endif

class pluck_harp10 : public dsp {
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
	FAUSTFLOAT 	fslider6;
	double 	fVec0[2];
	double 	fVec1[2];
	FAUSTFLOAT 	fslider7;
	int 	iRec52[2];
	double 	fRec51[2];
	double 	fRec48[2];
	double 	fRec45[2];
	int 	IOTA;
	double 	fVec2[1024];
	double 	fVec3[2];
	double 	fConst3;
	double 	fRec44[2];
	double 	fRec0[2];
	FAUSTFLOAT 	fslider8;
	double 	fVec4[256];
	double 	fRec54[1024];
	double 	fVec5[2];
	double 	fRec53[2];
	double 	fRec1[2];
	double 	fVec6[1024];
	double 	fVec7[2];
	double 	fRec55[2];
	double 	fRec2[2];
	FAUSTFLOAT 	fslider9;
	double 	fVec8[256];
	double 	fRec57[1024];
	double 	fVec9[2];
	double 	fRec56[2];
	double 	fRec3[2];
	FAUSTFLOAT 	fslider10;
	double 	fVec10[2];
	int 	iRec66[2];
	double 	fRec65[2];
	double 	fRec62[2];
	double 	fRec59[2];
	double 	fVec11[1024];
	double 	fVec12[2];
	double 	fRec58[2];
	double 	fRec4[2];
	double 	fVec13[256];
	double 	fRec68[1024];
	double 	fVec14[2];
	double 	fRec67[2];
	double 	fRec5[2];
	double 	fVec15[1024];
	double 	fVec16[2];
	double 	fRec69[2];
	double 	fRec6[2];
	double 	fVec17[256];
	double 	fRec71[1024];
	double 	fVec18[2];
	double 	fRec70[2];
	double 	fRec7[2];
	FAUSTFLOAT 	fslider11;
	double 	fVec19[2];
	int 	iRec80[2];
	double 	fRec79[2];
	double 	fRec76[2];
	double 	fRec73[2];
	double 	fVec20[1024];
	double 	fVec21[2];
	double 	fRec72[2];
	double 	fRec8[2];
	double 	fVec22[256];
	double 	fRec82[1024];
	double 	fVec23[2];
	double 	fRec81[2];
	double 	fRec9[2];
	double 	fVec24[1024];
	double 	fVec25[2];
	double 	fRec83[2];
	double 	fRec10[2];
	double 	fVec26[256];
	double 	fRec85[1024];
	double 	fVec27[2];
	double 	fRec84[2];
	double 	fRec11[2];
	FAUSTFLOAT 	fslider12;
	double 	fVec28[2];
	int 	iRec94[2];
	double 	fRec93[2];
	double 	fRec90[2];
	double 	fRec87[2];
	double 	fVec29[1024];
	double 	fVec30[2];
	double 	fRec86[2];
	double 	fRec12[2];
	double 	fVec31[256];
	double 	fRec96[1024];
	double 	fVec32[2];
	double 	fRec95[2];
	double 	fRec13[2];
	double 	fVec33[1024];
	double 	fVec34[2];
	double 	fRec97[2];
	double 	fRec14[2];
	double 	fVec35[256];
	double 	fRec99[1024];
	double 	fVec36[2];
	double 	fRec98[2];
	double 	fRec15[2];
	FAUSTFLOAT 	fslider13;
	double 	fVec37[2];
	int 	iRec108[2];
	double 	fRec107[2];
	double 	fRec104[2];
	double 	fRec101[2];
	double 	fVec38[1024];
	double 	fVec39[2];
	double 	fRec100[2];
	double 	fRec16[2];
	double 	fVec40[256];
	double 	fRec110[1024];
	double 	fVec41[2];
	double 	fRec109[2];
	double 	fRec17[2];
	double 	fVec42[1024];
	double 	fVec43[2];
	double 	fRec111[2];
	double 	fRec18[2];
	double 	fVec44[256];
	double 	fRec113[1024];
	double 	fVec45[2];
	double 	fRec112[2];
	double 	fRec19[2];
	FAUSTFLOAT 	fslider14;
	double 	fVec46[2];
	int 	iRec122[2];
	double 	fRec121[2];
	double 	fRec118[2];
	double 	fRec115[2];
	double 	fVec47[1024];
	double 	fVec48[2];
	double 	fRec114[2];
	double 	fRec20[2];
	double 	fVec49[256];
	double 	fRec124[1024];
	double 	fVec50[2];
	double 	fRec123[2];
	double 	fRec21[2];
	double 	fVec51[1024];
	double 	fVec52[2];
	double 	fRec125[2];
	double 	fRec22[2];
	double 	fVec53[256];
	double 	fRec127[1024];
	double 	fVec54[2];
	double 	fRec126[2];
	double 	fRec23[2];
	FAUSTFLOAT 	fslider15;
	double 	fVec55[2];
	int 	iRec136[2];
	double 	fRec135[2];
	double 	fRec132[2];
	double 	fRec129[2];
	double 	fVec56[1024];
	double 	fVec57[2];
	double 	fRec128[2];
	double 	fRec24[2];
	double 	fVec58[256];
	double 	fRec138[1024];
	double 	fVec59[2];
	double 	fRec137[2];
	double 	fRec25[2];
	double 	fVec60[1024];
	double 	fVec61[2];
	double 	fRec139[2];
	double 	fRec26[2];
	double 	fVec62[256];
	double 	fRec141[1024];
	double 	fVec63[2];
	double 	fRec140[2];
	double 	fRec27[2];
	FAUSTFLOAT 	fslider16;
	double 	fVec64[2];
	int 	iRec150[2];
	double 	fRec149[2];
	double 	fRec146[2];
	double 	fRec143[2];
	double 	fVec65[1024];
	double 	fVec66[2];
	double 	fRec142[2];
	double 	fRec28[2];
	double 	fVec67[256];
	double 	fRec152[1024];
	double 	fVec68[2];
	double 	fRec151[2];
	double 	fRec29[2];
	double 	fVec69[1024];
	double 	fVec70[2];
	double 	fRec153[2];
	double 	fRec30[2];
	double 	fVec71[256];
	double 	fRec155[1024];
	double 	fVec72[2];
	double 	fRec154[2];
	double 	fRec31[2];
	FAUSTFLOAT 	fslider17;
	double 	fVec73[2];
	int 	iRec164[2];
	double 	fRec163[2];
	double 	fRec160[2];
	double 	fRec157[2];
	double 	fVec74[1024];
	double 	fVec75[2];
	double 	fRec156[2];
	double 	fRec32[2];
	double 	fVec76[256];
	double 	fRec166[1024];
	double 	fVec77[2];
	double 	fRec165[2];
	double 	fRec33[2];
	double 	fVec78[1024];
	double 	fVec79[2];
	double 	fRec167[2];
	double 	fRec34[2];
	double 	fVec80[256];
	double 	fRec169[1024];
	double 	fVec81[2];
	double 	fRec168[2];
	double 	fRec35[2];
	FAUSTFLOAT 	fslider18;
	double 	fVec82[2];
	int 	iRec178[2];
	double 	fRec177[2];
	double 	fRec174[2];
	double 	fRec171[2];
	double 	fVec83[1024];
	double 	fVec84[2];
	double 	fRec170[2];
	double 	fRec36[2];
	double 	fVec85[256];
	double 	fRec180[1024];
	double 	fVec86[2];
	double 	fRec179[2];
	double 	fRec37[2];
	double 	fVec87[1024];
	double 	fVec88[2];
	double 	fRec181[2];
	double 	fRec38[2];
	double 	fVec89[256];
	double 	fRec183[1024];
	double 	fVec90[2];
	double 	fRec182[2];
	double 	fRec39[2];
	double 	fConst4;
	double 	fConst5;
	double 	fConst6;
	double 	fConst7;
	double 	fConst8;
	double 	fConst9;
	double 	fConst10;
	double 	fConst11;
	double 	fConst12;
	double 	fConst13;
	FAUSTFLOAT 	fslider19;
	double 	fVec91[2];
	double 	fRec189[2];
	double 	fVec92[2];
	double 	fRec188[2];
	double 	fConst14;
	double 	fConst15;
	double 	fRec187[3];
	double 	fConst16;
	double 	fRec186[3];
	double 	fRec185[3];
	double 	fRec184[3];
	double 	fConst17;
	double 	fConst18;
	double 	fConst19;
	double 	fConst20;
	double 	fConst21;
	double 	fConst22;
	double 	fRec192[2];
	double 	fRec191[2];
	double 	fRec190[2];
	double 	fVec93[2];
	double 	fRec197[2];
	double 	fRec196[3];
	double 	fRec195[3];
	double 	fRec194[3];
	double 	fRec193[3];
	double 	fRec200[2];
	double 	fRec199[2];
	double 	fRec198[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("SAM-fx.lib/name", "freeverb");
		m->declare("SAM-fx.lib/version", "1.0");
		m->declare("SAM-fx.lib/author", "Grame");
		m->declare("SAM-fx.lib/license", "BSD");
		m->declare("SAM-fx.lib/copyright", "(c) GRAME 2006");
		m->declare("SAM-fx.lib/reference", "https://ccrma.stanford.edu/~jos/pasp/Freeverb.html");
		m->declare("hoa.lib/name", "HOA");
		m->declare("hoa.lib/title", "High Order Ambisonics library");
		m->declare("hoa.lib/author", "Pierre Guillot");
		m->declare("hoa.lib/copyright", "2012-2013 Guillot, Paris, Colafrancesco, CICM labex art H2H, U. Paris 8");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
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
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
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
		fslider1 = 48.0;
		fslider2 = 45.0;
		fslider3 = 1.6;
		fslider4 = 0.4;
		fslider5 = 1.4e+02;
		fslider6 = 1.0;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		for (int i=0; i<2; i++) fVec1[i] = 0;
		fslider7 = 0.0045;
		for (int i=0; i<2; i++) iRec52[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		IOTA = 0;
		for (int i=0; i<1024; i++) fVec2[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		fConst3 = (1 - fConst1);
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		fslider8 = 5.5;
		for (int i=0; i<256; i++) fVec4[i] = 0;
		for (int i=0; i<1024; i++) fRec54[i] = 0;
		for (int i=0; i<2; i++) fVec5[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<1024; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		fslider9 = 3.46;
		for (int i=0; i<256; i++) fVec8[i] = 0;
		for (int i=0; i<1024; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		fslider10 = 5e+01;
		for (int i=0; i<2; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) iRec66[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<1024; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<256; i++) fVec13[i] = 0;
		for (int i=0; i<1024; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<1024; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<256; i++) fVec17[i] = 0;
		for (int i=0; i<1024; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		fslider11 = 52.0;
		for (int i=0; i<2; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) iRec80[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<1024; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<256; i++) fVec22[i] = 0;
		for (int i=0; i<1024; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<1024; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fRec10[i] = 0;
		for (int i=0; i<256; i++) fVec26[i] = 0;
		for (int i=0; i<1024; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		fslider12 = 54.0;
		for (int i=0; i<2; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) iRec94[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<1024; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fRec12[i] = 0;
		for (int i=0; i<256; i++) fVec31[i] = 0;
		for (int i=0; i<1024; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<1024; i++) fVec33[i] = 0;
		for (int i=0; i<2; i++) fVec34[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec14[i] = 0;
		for (int i=0; i<256; i++) fVec35[i] = 0;
		for (int i=0; i<1024; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fVec36[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		fslider13 = 56.0;
		for (int i=0; i<2; i++) fVec37[i] = 0;
		for (int i=0; i<2; i++) iRec108[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<2; i++) fRec104[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<1024; i++) fVec38[i] = 0;
		for (int i=0; i<2; i++) fVec39[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fRec16[i] = 0;
		for (int i=0; i<256; i++) fVec40[i] = 0;
		for (int i=0; i<1024; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fVec41[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<1024; i++) fVec42[i] = 0;
		for (int i=0; i<2; i++) fVec43[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec18[i] = 0;
		for (int i=0; i<256; i++) fVec44[i] = 0;
		for (int i=0; i<1024; i++) fRec113[i] = 0;
		for (int i=0; i<2; i++) fVec45[i] = 0;
		for (int i=0; i<2; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		fslider14 = 58.0;
		for (int i=0; i<2; i++) fVec46[i] = 0;
		for (int i=0; i<2; i++) iRec122[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<1024; i++) fVec47[i] = 0;
		for (int i=0; i<2; i++) fVec48[i] = 0;
		for (int i=0; i<2; i++) fRec114[i] = 0;
		for (int i=0; i<2; i++) fRec20[i] = 0;
		for (int i=0; i<256; i++) fVec49[i] = 0;
		for (int i=0; i<1024; i++) fRec124[i] = 0;
		for (int i=0; i<2; i++) fVec50[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<1024; i++) fVec51[i] = 0;
		for (int i=0; i<2; i++) fVec52[i] = 0;
		for (int i=0; i<2; i++) fRec125[i] = 0;
		for (int i=0; i<2; i++) fRec22[i] = 0;
		for (int i=0; i<256; i++) fVec53[i] = 0;
		for (int i=0; i<1024; i++) fRec127[i] = 0;
		for (int i=0; i<2; i++) fVec54[i] = 0;
		for (int i=0; i<2; i++) fRec126[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		fslider15 = 6e+01;
		for (int i=0; i<2; i++) fVec55[i] = 0;
		for (int i=0; i<2; i++) iRec136[i] = 0;
		for (int i=0; i<2; i++) fRec135[i] = 0;
		for (int i=0; i<2; i++) fRec132[i] = 0;
		for (int i=0; i<2; i++) fRec129[i] = 0;
		for (int i=0; i<1024; i++) fVec56[i] = 0;
		for (int i=0; i<2; i++) fVec57[i] = 0;
		for (int i=0; i<2; i++) fRec128[i] = 0;
		for (int i=0; i<2; i++) fRec24[i] = 0;
		for (int i=0; i<256; i++) fVec58[i] = 0;
		for (int i=0; i<1024; i++) fRec138[i] = 0;
		for (int i=0; i<2; i++) fVec59[i] = 0;
		for (int i=0; i<2; i++) fRec137[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<1024; i++) fVec60[i] = 0;
		for (int i=0; i<2; i++) fVec61[i] = 0;
		for (int i=0; i<2; i++) fRec139[i] = 0;
		for (int i=0; i<2; i++) fRec26[i] = 0;
		for (int i=0; i<256; i++) fVec62[i] = 0;
		for (int i=0; i<1024; i++) fRec141[i] = 0;
		for (int i=0; i<2; i++) fVec63[i] = 0;
		for (int i=0; i<2; i++) fRec140[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		fslider16 = 62.0;
		for (int i=0; i<2; i++) fVec64[i] = 0;
		for (int i=0; i<2; i++) iRec150[i] = 0;
		for (int i=0; i<2; i++) fRec149[i] = 0;
		for (int i=0; i<2; i++) fRec146[i] = 0;
		for (int i=0; i<2; i++) fRec143[i] = 0;
		for (int i=0; i<1024; i++) fVec65[i] = 0;
		for (int i=0; i<2; i++) fVec66[i] = 0;
		for (int i=0; i<2; i++) fRec142[i] = 0;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		for (int i=0; i<256; i++) fVec67[i] = 0;
		for (int i=0; i<1024; i++) fRec152[i] = 0;
		for (int i=0; i<2; i++) fVec68[i] = 0;
		for (int i=0; i<2; i++) fRec151[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<1024; i++) fVec69[i] = 0;
		for (int i=0; i<2; i++) fVec70[i] = 0;
		for (int i=0; i<2; i++) fRec153[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<256; i++) fVec71[i] = 0;
		for (int i=0; i<1024; i++) fRec155[i] = 0;
		for (int i=0; i<2; i++) fVec72[i] = 0;
		for (int i=0; i<2; i++) fRec154[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		fslider17 = 64.0;
		for (int i=0; i<2; i++) fVec73[i] = 0;
		for (int i=0; i<2; i++) iRec164[i] = 0;
		for (int i=0; i<2; i++) fRec163[i] = 0;
		for (int i=0; i<2; i++) fRec160[i] = 0;
		for (int i=0; i<2; i++) fRec157[i] = 0;
		for (int i=0; i<1024; i++) fVec74[i] = 0;
		for (int i=0; i<2; i++) fVec75[i] = 0;
		for (int i=0; i<2; i++) fRec156[i] = 0;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<256; i++) fVec76[i] = 0;
		for (int i=0; i<1024; i++) fRec166[i] = 0;
		for (int i=0; i<2; i++) fVec77[i] = 0;
		for (int i=0; i<2; i++) fRec165[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<1024; i++) fVec78[i] = 0;
		for (int i=0; i<2; i++) fVec79[i] = 0;
		for (int i=0; i<2; i++) fRec167[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<256; i++) fVec80[i] = 0;
		for (int i=0; i<1024; i++) fRec169[i] = 0;
		for (int i=0; i<2; i++) fVec81[i] = 0;
		for (int i=0; i<2; i++) fRec168[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		fslider18 = 66.0;
		for (int i=0; i<2; i++) fVec82[i] = 0;
		for (int i=0; i<2; i++) iRec178[i] = 0;
		for (int i=0; i<2; i++) fRec177[i] = 0;
		for (int i=0; i<2; i++) fRec174[i] = 0;
		for (int i=0; i<2; i++) fRec171[i] = 0;
		for (int i=0; i<1024; i++) fVec83[i] = 0;
		for (int i=0; i<2; i++) fVec84[i] = 0;
		for (int i=0; i<2; i++) fRec170[i] = 0;
		for (int i=0; i<2; i++) fRec36[i] = 0;
		for (int i=0; i<256; i++) fVec85[i] = 0;
		for (int i=0; i<1024; i++) fRec180[i] = 0;
		for (int i=0; i<2; i++) fVec86[i] = 0;
		for (int i=0; i<2; i++) fRec179[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<1024; i++) fVec87[i] = 0;
		for (int i=0; i<2; i++) fVec88[i] = 0;
		for (int i=0; i<2; i++) fRec181[i] = 0;
		for (int i=0; i<2; i++) fRec38[i] = 0;
		for (int i=0; i<256; i++) fVec89[i] = 0;
		for (int i=0; i<1024; i++) fRec183[i] = 0;
		for (int i=0; i<2; i++) fVec90[i] = 0;
		for (int i=0; i<2; i++) fRec182[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		fConst4 = tan((62.83185307179586 / double(iConst0)));
		fConst5 = (1.0 / fConst4);
		fConst6 = (1.0 / (1 + ((0.7653668647301795 + fConst5) / fConst4)));
		fConst7 = (1.0 / faustpower<2>(fConst4));
		fConst8 = (1.0 / (1 + ((1.8477590650225735 + fConst5) / fConst4)));
		fConst9 = (2 * (0 - fConst7));
		fConst10 = (1.0 / tan((2513.2741228718346 / double(iConst0))));
		fConst11 = (1 + fConst10);
		fConst12 = (0 - ((1 - fConst10) / fConst11));
		fConst13 = (1.0 / fConst11);
		fslider19 = 0.22;
		for (int i=0; i<2; i++) fVec91[i] = 0;
		for (int i=0; i<2; i++) fRec189[i] = 0;
		for (int i=0; i<2; i++) fVec92[i] = 0;
		for (int i=0; i<2; i++) fRec188[i] = 0;
		fConst14 = (2 * (1 - fConst7));
		fConst15 = (1 + ((fConst5 - 1.8477590650225735) / fConst4));
		for (int i=0; i<3; i++) fRec187[i] = 0;
		fConst16 = (1 + ((fConst5 - 0.7653668647301795) / fConst4));
		for (int i=0; i<3; i++) fRec186[i] = 0;
		for (int i=0; i<3; i++) fRec185[i] = 0;
		for (int i=0; i<3; i++) fRec184[i] = 0;
		fConst17 = exp((0 - (2.5e+03 / double(iConst0))));
		fConst18 = (1.0 - fConst17);
		fConst19 = exp((0 - (1.25e+03 / double(iConst0))));
		fConst20 = (1.0 - fConst19);
		fConst21 = exp((0 - (1e+01 / double(iConst0))));
		fConst22 = (1.0 - fConst21);
		for (int i=0; i<2; i++) fRec192[i] = 0;
		for (int i=0; i<2; i++) fRec191[i] = 0;
		for (int i=0; i<2; i++) fRec190[i] = 0;
		for (int i=0; i<2; i++) fVec93[i] = 0;
		for (int i=0; i<2; i++) fRec197[i] = 0;
		for (int i=0; i<3; i++) fRec196[i] = 0;
		for (int i=0; i<3; i++) fRec195[i] = 0;
		for (int i=0; i<3; i++) fRec194[i] = 0;
		for (int i=0; i<3; i++) fRec193[i] = 0;
		for (int i=0; i<2; i++) fRec200[i] = 0;
		for (int i=0; i<2; i++) fRec199[i] = 0;
		for (int i=0; i<2; i++) fRec198[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("pluck_harp10");
		interface->addHorizontalSlider("Bridge attenuation", &fslider8, 5.5, 0.1, 7.0, 0.01);
		interface->addHorizontalSlider("Brightness", &fslider2, 45.0, 1e+01, 1e+02, 1.0);
		interface->openHorizontalBox("FreqsA");
		interface->declare(&fslider1, "MIDI", "");
		interface->addHorizontalSlider("Frequency1", &fslider1, 48.0, 12.0, 1e+02, 0.05);
		interface->declare(&fslider10, "MIDI", "");
		interface->addHorizontalSlider("Frequency3", &fslider10, 5e+01, 12.0, 1e+02, 0.05);
		interface->closeBox();
		interface->openHorizontalBox("FreqsB");
		interface->declare(&fslider11, "MIDI", "");
		interface->addHorizontalSlider("Frequency5", &fslider11, 52.0, 12.0, 1e+02, 0.05);
		interface->declare(&fslider12, "MIDI", "");
		interface->addHorizontalSlider("Frequency7", &fslider12, 54.0, 12.0, 1e+02, 0.05);
		interface->closeBox();
		interface->openHorizontalBox("FreqsC");
		interface->declare(&fslider14, "MIDI", "");
		interface->addHorizontalSlider("Frequency11", &fslider14, 58.0, 12.0, 1e+02, 0.05);
		interface->declare(&fslider13, "MIDI", "");
		interface->addHorizontalSlider("Frequency9", &fslider13, 56.0, 12.0, 1e+02, 0.05);
		interface->closeBox();
		interface->openHorizontalBox("FreqsD");
		interface->declare(&fslider15, "MIDI", "");
		interface->addHorizontalSlider("Frequency13", &fslider15, 6e+01, 12.0, 1e+02, 0.05);
		interface->declare(&fslider16, "MIDI", "");
		interface->addHorizontalSlider("Frequency15", &fslider16, 62.0, 12.0, 1e+02, 0.05);
		interface->closeBox();
		interface->openHorizontalBox("FreqsE");
		interface->declare(&fslider17, "MIDI", "");
		interface->addHorizontalSlider("Frequency17", &fslider17, 64.0, 12.0, 1e+02, 0.05);
		interface->declare(&fslider18, "MIDI", "");
		interface->addHorizontalSlider("Frequency19", &fslider18, 66.0, 12.0, 1e+02, 0.05);
		interface->closeBox();
		interface->addHorizontalSlider("Fret attenuation", &fslider9, 3.46, 0.1, 7.0, 0.01);
		interface->declare(&fslider4, "N/(m/s)", "");
		interface->addHorizontalSlider("Pluck damping", &fslider4, 0.4, 0.0, 2.5, 0.01);
		interface->declare(&fslider7, "m", "");
		interface->addHorizontalSlider("Pluck half-width", &fslider7, 0.0045, 0.001, 0.01, 0.001);
		interface->addHorizontalSlider("Pluck position along length", &fslider0, 0.4, 0.1, 0.9, 0.01);
		interface->declare(&fslider5, "N/m", "");
		interface->addHorizontalSlider("Pluck stiffness", &fslider5, 1.4e+02, 0.0, 1e+03, 1e+01);
		interface->addHorizontalSlider("String wave impedance", &fslider3, 1.6, 0.25, 75.0, 0.1);
		interface->addHorizontalSlider("Waveguides position adjustment", &fslider6, 1.0, 0.1, 1.4, 0.01);
		interface->addHorizontalSlider("Z Volume", &fslider19, 0.22, 0.01, 1.0, 0.01);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		double 	fSlow0 = double(fslider0);
		double 	fSlow1 = pow(2.0,(0.08333333333333333 * (double(fslider1) - 69.0)));
		double 	fSlow2 = double(fslider2);
		double 	fSlow3 = (fSlow2 * fSlow1);
		int 	iSlow4 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow3)))));
		double 	fSlow5 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow1)) - (1.1337868480725624e-05 * double(min(256, (iSlow4 - 2))))));
		int 	iSlow6 = int((fSlow5 - 1.0));
		double 	fSlow7 = (fSlow5 - iSlow6);
		double 	fSlow8 = (0 - ((1 + iSlow6) - fSlow5));
		double 	fSlow9 = (fSlow5 - (3 + iSlow6));
		double 	fSlow10 = (0 - ((fSlow7 * fSlow8) * fSlow9));
		double 	fSlow11 = double(fslider3);
		double 	fSlow12 = double(fslider4);
		double 	fSlow13 = double(fslider5);
		double 	fSlow14 = ((2 * fSlow11) + (fSlow12 + (2.2675736961451248e-05 * fSlow13)));
		double 	fSlow15 = (2.2675736961451248e-05 / fSlow14);
		double 	fSlow16 = double(fslider6);
		double 	fSlow17 = (0.04 * fSlow16);
		double 	fSlow18 = (4.41e+04 * fSlow12);
		double 	fSlow19 = (2.0 * fSlow11);
		double 	fSlow20 = double(fslider7);
		double 	fSlow21 = (0 - fSlow20);
		double 	fSlow22 = (1.0 / fSlow14);
		int 	iSlow23 = int((fSlow5 - 2.0));
		int 	iSlow24 = int((int((2 + iSlow23)) & 1023));
		double 	fSlow25 = (fSlow5 - (2 + iSlow6));
		double 	fSlow26 = (fSlow7 * fSlow25);
		double 	fSlow27 = (fSlow26 * fSlow9);
		int 	iSlow28 = int((int((1 + iSlow23)) & 1023));
		double 	fSlow29 = (0.3333333333333333 * (0 - ((fSlow25 * fSlow8) * fSlow9)));
		int 	iSlow30 = int((iSlow23 & 1023));
		double 	fSlow31 = (0.16666666666666666 * (fSlow26 * fSlow8));
		int 	iSlow32 = int((int((3 + iSlow23)) & 1023));
		int 	iSlow33 = (iSlow4 - 1);
		double 	fSlow34 = (1.0 / double(max(1, iSlow33)));
		double 	fSlow35 = (pow(1e+01,double((0 - double(fslider8)))) - 1.0);
		int 	iSlow36 = int((int(iSlow33) & 255));
		double 	fSlow37 = (1.0 - fSlow0);
		int 	iSlow38 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow3)))));
		double 	fSlow39 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow1)) - (1.1337868480725624e-05 * double(min(256, (iSlow38 - 2))))));
		int 	iSlow40 = int((fSlow39 - 1.0));
		double 	fSlow41 = (fSlow39 - iSlow40);
		double 	fSlow42 = (0 - ((1 + iSlow40) - fSlow39));
		double 	fSlow43 = (fSlow41 * fSlow42);
		double 	fSlow44 = (fSlow39 - (3 + iSlow40));
		double 	fSlow45 = (0 - (fSlow43 * fSlow44));
		int 	iSlow46 = int((fSlow39 - 2.0));
		int 	iSlow47 = int((int((2 + iSlow46)) & 1023));
		double 	fSlow48 = (fSlow39 - (2 + iSlow40));
		double 	fSlow49 = ((fSlow41 * fSlow48) * fSlow44);
		int 	iSlow50 = int((int((1 + iSlow46)) & 1023));
		double 	fSlow51 = (0.3333333333333333 * (0 - ((fSlow42 * fSlow48) * fSlow44)));
		int 	iSlow52 = int((iSlow46 & 1023));
		double 	fSlow53 = (0.16666666666666666 * (fSlow43 * fSlow48));
		int 	iSlow54 = int((int((3 + iSlow46)) & 1023));
		int 	iSlow55 = (iSlow38 - 1);
		double 	fSlow56 = (1.0 / double(max(1, iSlow55)));
		double 	fSlow57 = (pow(1e+01,double((0 - double(fslider9)))) - 1.0);
		int 	iSlow58 = int((int(iSlow55) & 255));
		double 	fSlow59 = pow(2.0,(0.08333333333333333 * (double(fslider10) - 69.0)));
		double 	fSlow60 = (fSlow59 * fSlow2);
		int 	iSlow61 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow60)))));
		double 	fSlow62 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow59)) - (1.1337868480725624e-05 * double(min(256, (iSlow61 - 2))))));
		int 	iSlow63 = int((fSlow62 - 1.0));
		double 	fSlow64 = (fSlow62 - iSlow63);
		double 	fSlow65 = (0 - ((1 + iSlow63) - fSlow62));
		double 	fSlow66 = (fSlow64 * fSlow65);
		double 	fSlow67 = (fSlow62 - (3 + iSlow63));
		double 	fSlow68 = (0 - (fSlow66 * fSlow67));
		double 	fSlow69 = (0.032 * fSlow16);
		int 	iSlow70 = int((fSlow62 - 2.0));
		int 	iSlow71 = int((int((2 + iSlow70)) & 1023));
		double 	fSlow72 = (fSlow62 - (2 + iSlow63));
		double 	fSlow73 = ((fSlow64 * fSlow72) * fSlow67);
		int 	iSlow74 = int((int((1 + iSlow70)) & 1023));
		double 	fSlow75 = (0.3333333333333333 * (0 - ((fSlow65 * fSlow72) * fSlow67)));
		int 	iSlow76 = int((iSlow70 & 1023));
		double 	fSlow77 = (0.16666666666666666 * (fSlow66 * fSlow72));
		int 	iSlow78 = int((int((3 + iSlow70)) & 1023));
		int 	iSlow79 = (iSlow61 - 1);
		double 	fSlow80 = (1.0 / double(max(1, iSlow79)));
		int 	iSlow81 = int((int(iSlow79) & 255));
		int 	iSlow82 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow60)))));
		double 	fSlow83 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow59)) - (1.1337868480725624e-05 * double(min(256, (iSlow82 - 2))))));
		int 	iSlow84 = int((fSlow83 - 1.0));
		double 	fSlow85 = (fSlow83 - iSlow84);
		double 	fSlow86 = (0 - ((1 + iSlow84) - fSlow83));
		double 	fSlow87 = (fSlow85 * fSlow86);
		double 	fSlow88 = (fSlow83 - (3 + iSlow84));
		double 	fSlow89 = (0 - (fSlow87 * fSlow88));
		int 	iSlow90 = int((fSlow83 - 2.0));
		int 	iSlow91 = int((int((2 + iSlow90)) & 1023));
		double 	fSlow92 = (fSlow83 - (2 + iSlow84));
		double 	fSlow93 = ((fSlow85 * fSlow92) * fSlow88);
		int 	iSlow94 = int((int((1 + iSlow90)) & 1023));
		double 	fSlow95 = (0.3333333333333333 * (0 - ((fSlow86 * fSlow92) * fSlow88)));
		int 	iSlow96 = int((iSlow90 & 1023));
		double 	fSlow97 = (0.16666666666666666 * (fSlow87 * fSlow92));
		int 	iSlow98 = int((int((3 + iSlow90)) & 1023));
		int 	iSlow99 = (iSlow82 - 1);
		double 	fSlow100 = (1.0 / double(max(1, iSlow99)));
		int 	iSlow101 = int((int(iSlow99) & 255));
		double 	fSlow102 = pow(2.0,(0.08333333333333333 * (double(fslider11) - 69.0)));
		double 	fSlow103 = (fSlow102 * fSlow2);
		int 	iSlow104 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow103)))));
		double 	fSlow105 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow102)) - (1.1337868480725624e-05 * double(min(256, (iSlow104 - 2))))));
		int 	iSlow106 = int((fSlow105 - 1.0));
		double 	fSlow107 = (fSlow105 - iSlow106);
		double 	fSlow108 = (0 - ((1 + iSlow106) - fSlow105));
		double 	fSlow109 = (fSlow107 * fSlow108);
		double 	fSlow110 = (fSlow105 - (3 + iSlow106));
		double 	fSlow111 = (0 - (fSlow109 * fSlow110));
		double 	fSlow112 = (0.024 * fSlow16);
		int 	iSlow113 = int((fSlow105 - 2.0));
		int 	iSlow114 = int((int((2 + iSlow113)) & 1023));
		double 	fSlow115 = (fSlow105 - (2 + iSlow106));
		double 	fSlow116 = ((fSlow107 * fSlow115) * fSlow110);
		int 	iSlow117 = int((int((1 + iSlow113)) & 1023));
		double 	fSlow118 = (0.3333333333333333 * (0 - ((fSlow108 * fSlow115) * fSlow110)));
		int 	iSlow119 = int((iSlow113 & 1023));
		double 	fSlow120 = (0.16666666666666666 * (fSlow109 * fSlow115));
		int 	iSlow121 = int((int((3 + iSlow113)) & 1023));
		int 	iSlow122 = (iSlow104 - 1);
		double 	fSlow123 = (1.0 / double(max(1, iSlow122)));
		int 	iSlow124 = int((int(iSlow122) & 255));
		int 	iSlow125 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow103)))));
		double 	fSlow126 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow102)) - (1.1337868480725624e-05 * double(min(256, (iSlow125 - 2))))));
		int 	iSlow127 = int((fSlow126 - 1.0));
		double 	fSlow128 = (fSlow126 - iSlow127);
		double 	fSlow129 = (0 - ((1 + iSlow127) - fSlow126));
		double 	fSlow130 = (fSlow128 * fSlow129);
		double 	fSlow131 = (fSlow126 - (3 + iSlow127));
		double 	fSlow132 = (0 - (fSlow130 * fSlow131));
		int 	iSlow133 = int((fSlow126 - 2.0));
		int 	iSlow134 = int((int((2 + iSlow133)) & 1023));
		double 	fSlow135 = (fSlow126 - (2 + iSlow127));
		double 	fSlow136 = ((fSlow128 * fSlow135) * fSlow131);
		int 	iSlow137 = int((int((1 + iSlow133)) & 1023));
		double 	fSlow138 = (0.3333333333333333 * (0 - ((fSlow129 * fSlow135) * fSlow131)));
		int 	iSlow139 = int((iSlow133 & 1023));
		double 	fSlow140 = (0.16666666666666666 * (fSlow130 * fSlow135));
		int 	iSlow141 = int((int((3 + iSlow133)) & 1023));
		int 	iSlow142 = (iSlow125 - 1);
		double 	fSlow143 = (1.0 / double(max(1, iSlow142)));
		int 	iSlow144 = int((int(iSlow142) & 255));
		double 	fSlow145 = pow(2.0,(0.08333333333333333 * (double(fslider12) - 69.0)));
		double 	fSlow146 = (fSlow145 * fSlow2);
		int 	iSlow147 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow146)))));
		double 	fSlow148 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow145)) - (1.1337868480725624e-05 * double(min(256, (iSlow147 - 2))))));
		int 	iSlow149 = int((fSlow148 - 1.0));
		double 	fSlow150 = (fSlow148 - iSlow149);
		double 	fSlow151 = (0 - ((1 + iSlow149) - fSlow148));
		double 	fSlow152 = (fSlow150 * fSlow151);
		double 	fSlow153 = (fSlow148 - (3 + iSlow149));
		double 	fSlow154 = (0 - (fSlow152 * fSlow153));
		double 	fSlow155 = (0.016 * fSlow16);
		int 	iSlow156 = int((fSlow148 - 2.0));
		int 	iSlow157 = int((int((2 + iSlow156)) & 1023));
		double 	fSlow158 = (fSlow148 - (2 + iSlow149));
		double 	fSlow159 = ((fSlow150 * fSlow158) * fSlow153);
		int 	iSlow160 = int((int((1 + iSlow156)) & 1023));
		double 	fSlow161 = (0.3333333333333333 * (0 - ((fSlow151 * fSlow158) * fSlow153)));
		int 	iSlow162 = int((iSlow156 & 1023));
		double 	fSlow163 = (0.16666666666666666 * (fSlow152 * fSlow158));
		int 	iSlow164 = int((int((3 + iSlow156)) & 1023));
		int 	iSlow165 = (iSlow147 - 1);
		double 	fSlow166 = (1.0 / double(max(1, iSlow165)));
		int 	iSlow167 = int((int(iSlow165) & 255));
		int 	iSlow168 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow146)))));
		double 	fSlow169 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow145)) - (1.1337868480725624e-05 * double(min(256, (iSlow168 - 2))))));
		int 	iSlow170 = int((fSlow169 - 1.0));
		double 	fSlow171 = (fSlow169 - iSlow170);
		double 	fSlow172 = (0 - ((1 + iSlow170) - fSlow169));
		double 	fSlow173 = (fSlow171 * fSlow172);
		double 	fSlow174 = (fSlow169 - (3 + iSlow170));
		double 	fSlow175 = (0 - (fSlow173 * fSlow174));
		int 	iSlow176 = int((fSlow169 - 2.0));
		int 	iSlow177 = int((int((2 + iSlow176)) & 1023));
		double 	fSlow178 = (fSlow169 - (2 + iSlow170));
		double 	fSlow179 = ((fSlow171 * fSlow178) * fSlow174);
		int 	iSlow180 = int((int((1 + iSlow176)) & 1023));
		double 	fSlow181 = (0.3333333333333333 * (0 - ((fSlow172 * fSlow178) * fSlow174)));
		int 	iSlow182 = int((iSlow176 & 1023));
		double 	fSlow183 = (0.16666666666666666 * (fSlow173 * fSlow178));
		int 	iSlow184 = int((int((3 + iSlow176)) & 1023));
		int 	iSlow185 = (iSlow168 - 1);
		double 	fSlow186 = (1.0 / double(max(1, iSlow185)));
		int 	iSlow187 = int((int(iSlow185) & 255));
		double 	fSlow188 = pow(2.0,(0.08333333333333333 * (double(fslider13) - 69.0)));
		double 	fSlow189 = (fSlow188 * fSlow2);
		int 	iSlow190 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow189)))));
		double 	fSlow191 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow188)) - (1.1337868480725624e-05 * double(min(256, (iSlow190 - 2))))));
		int 	iSlow192 = int((fSlow191 - 1.0));
		double 	fSlow193 = (fSlow191 - iSlow192);
		double 	fSlow194 = (0 - ((1 + iSlow192) - fSlow191));
		double 	fSlow195 = (fSlow193 * fSlow194);
		double 	fSlow196 = (fSlow191 - (3 + iSlow192));
		double 	fSlow197 = (0 - (fSlow195 * fSlow196));
		double 	fSlow198 = (0.004 * fSlow16);
		int 	iSlow199 = int((fSlow191 - 2.0));
		int 	iSlow200 = int((int((2 + iSlow199)) & 1023));
		double 	fSlow201 = (fSlow191 - (2 + iSlow192));
		double 	fSlow202 = ((fSlow193 * fSlow201) * fSlow196);
		int 	iSlow203 = int((int((1 + iSlow199)) & 1023));
		double 	fSlow204 = (0.3333333333333333 * (0 - ((fSlow194 * fSlow201) * fSlow196)));
		int 	iSlow205 = int((iSlow199 & 1023));
		double 	fSlow206 = (0.16666666666666666 * (fSlow195 * fSlow201));
		int 	iSlow207 = int((int((3 + iSlow199)) & 1023));
		int 	iSlow208 = (iSlow190 - 1);
		double 	fSlow209 = (1.0 / double(max(1, iSlow208)));
		int 	iSlow210 = int((int(iSlow208) & 255));
		int 	iSlow211 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow189)))));
		double 	fSlow212 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow188)) - (1.1337868480725624e-05 * double(min(256, (iSlow211 - 2))))));
		int 	iSlow213 = int((fSlow212 - 1.0));
		double 	fSlow214 = (fSlow212 - iSlow213);
		double 	fSlow215 = (0 - ((1 + iSlow213) - fSlow212));
		double 	fSlow216 = (fSlow214 * fSlow215);
		double 	fSlow217 = (fSlow212 - (3 + iSlow213));
		double 	fSlow218 = (0 - (fSlow216 * fSlow217));
		int 	iSlow219 = int((fSlow212 - 2.0));
		int 	iSlow220 = int((int((2 + iSlow219)) & 1023));
		double 	fSlow221 = (fSlow212 - (2 + iSlow213));
		double 	fSlow222 = ((fSlow214 * fSlow221) * fSlow217);
		int 	iSlow223 = int((int((1 + iSlow219)) & 1023));
		double 	fSlow224 = (0.3333333333333333 * (0 - ((fSlow215 * fSlow221) * fSlow217)));
		int 	iSlow225 = int((iSlow219 & 1023));
		double 	fSlow226 = (0.16666666666666666 * (fSlow216 * fSlow221));
		int 	iSlow227 = int((int((3 + iSlow219)) & 1023));
		int 	iSlow228 = (iSlow211 - 1);
		double 	fSlow229 = (1.0 / double(max(1, iSlow228)));
		int 	iSlow230 = int((int(iSlow228) & 255));
		double 	fSlow231 = pow(2.0,(0.08333333333333333 * (double(fslider14) - 69.0)));
		double 	fSlow232 = (fSlow231 * fSlow2);
		int 	iSlow233 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow232)))));
		double 	fSlow234 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow231)) - (1.1337868480725624e-05 * double(min(256, (iSlow233 - 2))))));
		int 	iSlow235 = int((fSlow234 - 1.0));
		double 	fSlow236 = (fSlow234 - iSlow235);
		double 	fSlow237 = (0 - ((1 + iSlow235) - fSlow234));
		double 	fSlow238 = (fSlow236 * fSlow237);
		double 	fSlow239 = (fSlow234 - (3 + iSlow235));
		double 	fSlow240 = (0 - (fSlow238 * fSlow239));
		int 	iSlow241 = int((fSlow234 - 2.0));
		int 	iSlow242 = int((int((2 + iSlow241)) & 1023));
		double 	fSlow243 = (fSlow234 - (2 + iSlow235));
		double 	fSlow244 = ((fSlow236 * fSlow243) * fSlow239);
		int 	iSlow245 = int((int((1 + iSlow241)) & 1023));
		double 	fSlow246 = (0.3333333333333333 * (0 - ((fSlow237 * fSlow243) * fSlow239)));
		int 	iSlow247 = int((iSlow241 & 1023));
		double 	fSlow248 = (0.16666666666666666 * (fSlow238 * fSlow243));
		int 	iSlow249 = int((int((3 + iSlow241)) & 1023));
		int 	iSlow250 = (iSlow233 - 1);
		double 	fSlow251 = (1.0 / double(max(1, iSlow250)));
		int 	iSlow252 = int((int(iSlow250) & 255));
		int 	iSlow253 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow232)))));
		double 	fSlow254 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow231)) - (1.1337868480725624e-05 * double(min(256, (iSlow253 - 2))))));
		int 	iSlow255 = int((fSlow254 - 1.0));
		double 	fSlow256 = (fSlow254 - iSlow255);
		double 	fSlow257 = (0 - ((1 + iSlow255) - fSlow254));
		double 	fSlow258 = (fSlow256 * fSlow257);
		double 	fSlow259 = (fSlow254 - (3 + iSlow255));
		double 	fSlow260 = (0 - (fSlow258 * fSlow259));
		int 	iSlow261 = int((fSlow254 - 2.0));
		int 	iSlow262 = int((int((2 + iSlow261)) & 1023));
		double 	fSlow263 = (fSlow254 - (2 + iSlow255));
		double 	fSlow264 = ((fSlow256 * fSlow263) * fSlow259);
		int 	iSlow265 = int((int((1 + iSlow261)) & 1023));
		double 	fSlow266 = (0.3333333333333333 * (0 - ((fSlow257 * fSlow263) * fSlow259)));
		int 	iSlow267 = int((iSlow261 & 1023));
		double 	fSlow268 = (0.16666666666666666 * (fSlow258 * fSlow263));
		int 	iSlow269 = int((int((3 + iSlow261)) & 1023));
		int 	iSlow270 = (iSlow253 - 1);
		double 	fSlow271 = (1.0 / double(max(1, iSlow270)));
		int 	iSlow272 = int((int(iSlow270) & 255));
		double 	fSlow273 = pow(2.0,(0.08333333333333333 * (double(fslider15) - 69.0)));
		double 	fSlow274 = (fSlow273 * fSlow2);
		int 	iSlow275 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow274)))));
		double 	fSlow276 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow273)) - (1.1337868480725624e-05 * double(min(256, (iSlow275 - 2))))));
		int 	iSlow277 = int((fSlow276 - 1.0));
		double 	fSlow278 = (fSlow276 - iSlow277);
		double 	fSlow279 = (0 - ((1 + iSlow277) - fSlow276));
		double 	fSlow280 = (fSlow278 * fSlow279);
		double 	fSlow281 = (fSlow276 - (3 + iSlow277));
		double 	fSlow282 = (0 - (fSlow280 * fSlow281));
		double 	fSlow283 = (0.012 * fSlow16);
		int 	iSlow284 = int((fSlow276 - 2.0));
		int 	iSlow285 = int((int((2 + iSlow284)) & 1023));
		double 	fSlow286 = (fSlow276 - (2 + iSlow277));
		double 	fSlow287 = ((fSlow278 * fSlow286) * fSlow281);
		int 	iSlow288 = int((int((1 + iSlow284)) & 1023));
		double 	fSlow289 = (0.3333333333333333 * (0 - ((fSlow279 * fSlow286) * fSlow281)));
		int 	iSlow290 = int((iSlow284 & 1023));
		double 	fSlow291 = (0.16666666666666666 * (fSlow280 * fSlow286));
		int 	iSlow292 = int((int((3 + iSlow284)) & 1023));
		int 	iSlow293 = (iSlow275 - 1);
		double 	fSlow294 = (1.0 / double(max(1, iSlow293)));
		int 	iSlow295 = int((int(iSlow293) & 255));
		int 	iSlow296 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow274)))));
		double 	fSlow297 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow273)) - (1.1337868480725624e-05 * double(min(256, (iSlow296 - 2))))));
		int 	iSlow298 = int((fSlow297 - 1.0));
		double 	fSlow299 = (fSlow297 - iSlow298);
		double 	fSlow300 = (0 - ((1 + iSlow298) - fSlow297));
		double 	fSlow301 = (fSlow299 * fSlow300);
		double 	fSlow302 = (fSlow297 - (3 + iSlow298));
		double 	fSlow303 = (0 - (fSlow301 * fSlow302));
		int 	iSlow304 = int((fSlow297 - 2.0));
		int 	iSlow305 = int((int((2 + iSlow304)) & 1023));
		double 	fSlow306 = (fSlow297 - (2 + iSlow298));
		double 	fSlow307 = ((fSlow299 * fSlow306) * fSlow302);
		int 	iSlow308 = int((int((1 + iSlow304)) & 1023));
		double 	fSlow309 = (0.3333333333333333 * (0 - ((fSlow300 * fSlow306) * fSlow302)));
		int 	iSlow310 = int((iSlow304 & 1023));
		double 	fSlow311 = (0.16666666666666666 * (fSlow301 * fSlow306));
		int 	iSlow312 = int((int((3 + iSlow304)) & 1023));
		int 	iSlow313 = (iSlow296 - 1);
		double 	fSlow314 = (1.0 / double(max(1, iSlow313)));
		int 	iSlow315 = int((int(iSlow313) & 255));
		double 	fSlow316 = pow(2.0,(0.08333333333333333 * (double(fslider16) - 69.0)));
		double 	fSlow317 = (fSlow2 * fSlow316);
		int 	iSlow318 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow317)))));
		double 	fSlow319 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow316)) - (1.1337868480725624e-05 * double(min(256, (iSlow318 - 2))))));
		int 	iSlow320 = int((fSlow319 - 1.0));
		double 	fSlow321 = (0 - ((1 + iSlow320) - fSlow319));
		double 	fSlow322 = (fSlow319 - (3 + iSlow320));
		double 	fSlow323 = (fSlow319 - iSlow320);
		double 	fSlow324 = (0 - ((fSlow321 * fSlow322) * fSlow323));
		double 	fSlow325 = (0.02 * fSlow16);
		int 	iSlow326 = int((fSlow319 - 2.0));
		int 	iSlow327 = int((int((2 + iSlow326)) & 1023));
		double 	fSlow328 = (fSlow319 - (2 + iSlow320));
		double 	fSlow329 = ((fSlow328 * fSlow322) * fSlow323);
		int 	iSlow330 = int((int((1 + iSlow326)) & 1023));
		double 	fSlow331 = (fSlow321 * fSlow328);
		double 	fSlow332 = (0.3333333333333333 * (0 - (fSlow331 * fSlow322)));
		int 	iSlow333 = int((iSlow326 & 1023));
		double 	fSlow334 = (0.16666666666666666 * (fSlow331 * fSlow323));
		int 	iSlow335 = int((int((3 + iSlow326)) & 1023));
		int 	iSlow336 = (iSlow318 - 1);
		double 	fSlow337 = (1.0 / double(max(1, iSlow336)));
		int 	iSlow338 = int((int(iSlow336) & 255));
		int 	iSlow339 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow317)))));
		double 	fSlow340 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow316)) - (1.1337868480725624e-05 * double(min(256, (iSlow339 - 2))))));
		int 	iSlow341 = int((fSlow340 - 1.0));
		double 	fSlow342 = (fSlow340 - (3 + iSlow341));
		double 	fSlow343 = (fSlow340 - iSlow341);
		double 	fSlow344 = (fSlow342 * fSlow343);
		double 	fSlow345 = (0 - ((1 + iSlow341) - fSlow340));
		double 	fSlow346 = (0 - (fSlow344 * fSlow345));
		int 	iSlow347 = int((fSlow340 - 2.0));
		int 	iSlow348 = int((int((2 + iSlow347)) & 1023));
		double 	fSlow349 = (fSlow340 - (2 + iSlow341));
		double 	fSlow350 = (fSlow344 * fSlow349);
		int 	iSlow351 = int((int((1 + iSlow347)) & 1023));
		double 	fSlow352 = (0.3333333333333333 * (0 - ((fSlow342 * fSlow345) * fSlow349)));
		int 	iSlow353 = int((iSlow347 & 1023));
		double 	fSlow354 = (0.16666666666666666 * ((fSlow343 * fSlow345) * fSlow349));
		int 	iSlow355 = int((int((3 + iSlow347)) & 1023));
		int 	iSlow356 = (iSlow339 - 1);
		double 	fSlow357 = (1.0 / double(max(1, iSlow356)));
		int 	iSlow358 = int((int(iSlow356) & 255));
		double 	fSlow359 = pow(2.0,(0.08333333333333333 * (double(fslider17) - 69.0)));
		double 	fSlow360 = (fSlow359 * fSlow2);
		int 	iSlow361 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow360)))));
		double 	fSlow362 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow359)) - (1.1337868480725624e-05 * double(min(256, (iSlow361 - 2))))));
		int 	iSlow363 = int((fSlow362 - 1.0));
		double 	fSlow364 = (fSlow362 - iSlow363);
		double 	fSlow365 = (0 - ((1 + iSlow363) - fSlow362));
		double 	fSlow366 = (fSlow364 * fSlow365);
		double 	fSlow367 = (fSlow362 - (3 + iSlow363));
		double 	fSlow368 = (0 - (fSlow366 * fSlow367));
		double 	fSlow369 = (0.028 * fSlow16);
		int 	iSlow370 = int((fSlow362 - 2.0));
		int 	iSlow371 = int((int((2 + iSlow370)) & 1023));
		double 	fSlow372 = (fSlow362 - (2 + iSlow363));
		double 	fSlow373 = ((fSlow364 * fSlow372) * fSlow367);
		int 	iSlow374 = int((int((1 + iSlow370)) & 1023));
		double 	fSlow375 = (0.3333333333333333 * (0 - ((fSlow365 * fSlow372) * fSlow367)));
		int 	iSlow376 = int((iSlow370 & 1023));
		double 	fSlow377 = (0.16666666666666666 * (fSlow366 * fSlow372));
		int 	iSlow378 = int((int((3 + iSlow370)) & 1023));
		int 	iSlow379 = (iSlow361 - 1);
		double 	fSlow380 = (1.0 / double(max(1, iSlow379)));
		int 	iSlow381 = int((int(iSlow379) & 255));
		int 	iSlow382 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow360)))));
		double 	fSlow383 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow359)) - (1.1337868480725624e-05 * double(min(256, (iSlow382 - 2))))));
		int 	iSlow384 = int((fSlow383 - 1.0));
		double 	fSlow385 = (fSlow383 - iSlow384);
		double 	fSlow386 = (0 - ((1 + iSlow384) - fSlow383));
		double 	fSlow387 = (fSlow385 * fSlow386);
		double 	fSlow388 = (fSlow383 - (3 + iSlow384));
		double 	fSlow389 = (0 - (fSlow387 * fSlow388));
		int 	iSlow390 = int((fSlow383 - 2.0));
		int 	iSlow391 = int((int((2 + iSlow390)) & 1023));
		double 	fSlow392 = (fSlow383 - (2 + iSlow384));
		double 	fSlow393 = ((fSlow385 * fSlow392) * fSlow388);
		int 	iSlow394 = int((int((1 + iSlow390)) & 1023));
		double 	fSlow395 = (0.3333333333333333 * (0 - ((fSlow386 * fSlow392) * fSlow388)));
		int 	iSlow396 = int((iSlow390 & 1023));
		double 	fSlow397 = (0.16666666666666666 * (fSlow387 * fSlow392));
		int 	iSlow398 = int((int((3 + iSlow390)) & 1023));
		int 	iSlow399 = (iSlow382 - 1);
		double 	fSlow400 = (1.0 / double(max(1, iSlow399)));
		int 	iSlow401 = int((int(iSlow399) & 255));
		double 	fSlow402 = pow(2.0,(0.08333333333333333 * (double(fslider18) - 69.0)));
		double 	fSlow403 = (fSlow2 * fSlow402);
		int 	iSlow404 = int((4.41e+04 / min(2.205e+04, min(1.7e+04, (352.0 * fSlow403)))));
		double 	fSlow405 = (2.205e+04 * ((0.0022727272727272726 * (fSlow0 / fSlow402)) - (1.1337868480725624e-05 * double(min(256, (iSlow404 - 2))))));
		int 	iSlow406 = int((fSlow405 - 1.0));
		double 	fSlow407 = (fSlow405 - iSlow406);
		double 	fSlow408 = (0 - ((1 + iSlow406) - fSlow405));
		double 	fSlow409 = (fSlow407 * fSlow408);
		double 	fSlow410 = (fSlow405 - (3 + iSlow406));
		double 	fSlow411 = (0 - (fSlow409 * fSlow410));
		double 	fSlow412 = (0.036 * fSlow16);
		int 	iSlow413 = int((fSlow405 - 2.0));
		int 	iSlow414 = int((int((2 + iSlow413)) & 1023));
		double 	fSlow415 = (fSlow405 - (2 + iSlow406));
		double 	fSlow416 = ((fSlow407 * fSlow415) * fSlow410);
		int 	iSlow417 = int((int((1 + iSlow413)) & 1023));
		double 	fSlow418 = (0.3333333333333333 * (0 - ((fSlow408 * fSlow415) * fSlow410)));
		int 	iSlow419 = int((iSlow413 & 1023));
		double 	fSlow420 = (0.16666666666666666 * (fSlow409 * fSlow415));
		int 	iSlow421 = int((int((3 + iSlow413)) & 1023));
		int 	iSlow422 = (iSlow404 - 1);
		double 	fSlow423 = (1.0 / double(max(1, iSlow422)));
		int 	iSlow424 = int((int(iSlow422) & 255));
		int 	iSlow425 = int((4.41e+04 / min(2.205e+04, min(2.1e+04, (484.00000000000006 * fSlow403)))));
		double 	fSlow426 = (2.205e+04 * ((0.0022727272727272726 * (fSlow37 / fSlow402)) - (1.1337868480725624e-05 * double(min(256, (iSlow425 - 2))))));
		int 	iSlow427 = int((fSlow426 - 1.0));
		double 	fSlow428 = (fSlow426 - iSlow427);
		double 	fSlow429 = (0 - ((1 + iSlow427) - fSlow426));
		double 	fSlow430 = (fSlow428 * fSlow429);
		double 	fSlow431 = (fSlow426 - (3 + iSlow427));
		double 	fSlow432 = (0 - (fSlow430 * fSlow431));
		int 	iSlow433 = int((fSlow426 - 2.0));
		int 	iSlow434 = int((int((2 + iSlow433)) & 1023));
		double 	fSlow435 = (fSlow426 - (2 + iSlow427));
		double 	fSlow436 = ((fSlow428 * fSlow435) * fSlow431);
		int 	iSlow437 = int((int((1 + iSlow433)) & 1023));
		double 	fSlow438 = (0.3333333333333333 * (0 - ((fSlow429 * fSlow435) * fSlow431)));
		int 	iSlow439 = int((iSlow433 & 1023));
		double 	fSlow440 = (0.16666666666666666 * (fSlow430 * fSlow435));
		int 	iSlow441 = int((int((3 + iSlow433)) & 1023));
		int 	iSlow442 = (iSlow425 - 1);
		double 	fSlow443 = (1.0 / double(max(1, iSlow442)));
		int 	iSlow444 = int((int(iSlow442) & 255));
		double 	fSlow445 = double(fslider19);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		FAUSTFLOAT* output2 = output[2];
		FAUSTFLOAT* output3 = output[3];
		for (int i=0; i<count; i++) {
			double fTemp0 = (double)input0[i];
			fVec0[0] = fTemp0;
			double fTemp1 = (fSlow17 + fVec0[0]);
			fVec1[0] = fTemp1;
			double fTemp2 = (fRec1[1] + fRec3[1]);
			double fTemp3 = (((fSlow13 * (fVec1[0] - (0.999 * fRec45[1]))) + (fSlow18 * (fVec1[0] - fVec1[1]))) + (fSlow19 * fTemp2));
			double fTemp4 = (((0.999 * fRec48[1]) + (fSlow15 * fTemp3)) - fVec1[0]);
			int iTemp5 = ((iRec52[1] == 0) & (fTemp4 < fSlow21));
			iRec52[0] = (iTemp5 + (iRec52[1] * ((1 - ((iRec52[1] == 1) & (fTemp4 > fSlow20))) & (1 - iTemp5))));
			int iTemp6 = ((((iRec52[0] == 0) & (fSlow21 <= fTemp4)) & (fTemp4 <= 0)) + (((iRec52[0] == 1) & (0 <= fTemp4)) & (fTemp4 <= fSlow20)));
			double fTemp7 = ((int(iTemp6))?(fSlow22 * fTemp3):fTemp2);
			fRec51[0] = ((0.999 * fRec51[1]) + (2.2675736961451248e-05 * fTemp7));
			fRec48[0] = fRec51[0];
			double 	fRec49 = fTemp7;
			int 	iRec50 = iTemp6;
			fRec45[0] = fRec48[0];
			double 	fRec46 = fRec49;
			int 	iRec47 = iRec50;
			double fTemp8 = (fRec46 - fRec1[1]);
			fVec2[IOTA&1023] = fTemp8;
			double fTemp9 = ((0.5 * ((fSlow10 * fVec2[(IOTA-iSlow24)&1023]) + ((fSlow27 * fVec2[(IOTA-iSlow28)&1023]) + (fSlow29 * fVec2[(IOTA-iSlow30)&1023])))) + (fSlow31 * fVec2[(IOTA-iSlow32)&1023]));
			fVec3[0] = fTemp9;
			fRec44[0] = (fConst2 * ((fVec3[0] - fVec3[1]) + (fConst3 * fRec44[1])));
			fRec0[0] = fRec44[0];
			double fTemp10 = (fSlow35 * fRec0[1]);
			fVec4[IOTA&255] = fTemp10;
			fRec54[IOTA&1023] = (fRec54[(IOTA-1)&1023] + (fSlow34 * (fVec4[IOTA&255] - fVec4[(IOTA-iSlow36)&255])));
			double fTemp11 = ((0.5 * ((fSlow10 * fRec54[(IOTA-iSlow24)&1023]) + ((fSlow27 * fRec54[(IOTA-iSlow28)&1023]) + (fSlow29 * fRec54[(IOTA-iSlow30)&1023])))) + (fSlow31 * fRec54[(IOTA-iSlow32)&1023]));
			fVec5[0] = fTemp11;
			fRec53[0] = (fConst2 * ((fVec5[0] - fVec5[1]) + (fConst3 * fRec53[1])));
			fRec1[0] = fRec53[0];
			double fTemp12 = (fRec46 - fRec3[1]);
			fVec6[IOTA&1023] = fTemp12;
			double fTemp13 = ((0.5 * ((fSlow45 * fVec6[(IOTA-iSlow47)&1023]) + ((fSlow49 * fVec6[(IOTA-iSlow50)&1023]) + (fSlow51 * fVec6[(IOTA-iSlow52)&1023])))) + (fSlow53 * fVec6[(IOTA-iSlow54)&1023]));
			fVec7[0] = fTemp13;
			fRec55[0] = (fConst2 * ((fVec7[0] - fVec7[1]) + (fConst3 * fRec55[1])));
			fRec2[0] = fRec55[0];
			double fTemp14 = (fSlow57 * fRec2[1]);
			fVec8[IOTA&255] = fTemp14;
			fRec57[IOTA&1023] = (fRec57[(IOTA-1)&1023] + (fSlow56 * (fVec8[IOTA&255] - fVec8[(IOTA-iSlow58)&255])));
			double fTemp15 = ((0.5 * ((fSlow45 * fRec57[(IOTA-iSlow47)&1023]) + ((fSlow49 * fRec57[(IOTA-iSlow50)&1023]) + (fSlow51 * fRec57[(IOTA-iSlow52)&1023])))) + (fSlow53 * fRec57[(IOTA-iSlow54)&1023]));
			fVec9[0] = fTemp15;
			fRec56[0] = (fConst2 * ((fVec9[0] - fVec9[1]) + (fConst3 * fRec56[1])));
			fRec3[0] = fRec56[0];
			double fTemp16 = (fSlow69 + fVec0[0]);
			fVec10[0] = fTemp16;
			double fTemp17 = (fRec5[1] + fRec7[1]);
			double fTemp18 = (((fSlow13 * (fVec10[0] - (0.999 * fRec59[1]))) + (fSlow18 * (fVec10[0] - fVec10[1]))) + (fSlow19 * fTemp17));
			double fTemp19 = (((0.999 * fRec62[1]) + (fSlow15 * fTemp18)) - fVec10[0]);
			int iTemp20 = ((iRec66[1] == 0) & (fTemp19 < fSlow21));
			iRec66[0] = (iTemp20 + (iRec66[1] * ((1 - ((iRec66[1] == 1) & (fTemp19 > fSlow20))) & (1 - iTemp20))));
			int iTemp21 = ((((iRec66[0] == 0) & (fSlow21 <= fTemp19)) & (fTemp19 <= 0)) + (((iRec66[0] == 1) & (0 <= fTemp19)) & (fTemp19 <= fSlow20)));
			double fTemp22 = ((int(iTemp21))?(fSlow22 * fTemp18):fTemp17);
			fRec65[0] = ((0.999 * fRec65[1]) + (2.2675736961451248e-05 * fTemp22));
			fRec62[0] = fRec65[0];
			double 	fRec63 = fTemp22;
			int 	iRec64 = iTemp21;
			fRec59[0] = fRec62[0];
			double 	fRec60 = fRec63;
			int 	iRec61 = iRec64;
			double fTemp23 = (fRec60 - fRec5[1]);
			fVec11[IOTA&1023] = fTemp23;
			double fTemp24 = ((0.5 * ((fSlow68 * fVec11[(IOTA-iSlow71)&1023]) + ((fSlow73 * fVec11[(IOTA-iSlow74)&1023]) + (fSlow75 * fVec11[(IOTA-iSlow76)&1023])))) + (fSlow77 * fVec11[(IOTA-iSlow78)&1023]));
			fVec12[0] = fTemp24;
			fRec58[0] = (fConst2 * ((fVec12[0] - fVec12[1]) + (fConst3 * fRec58[1])));
			fRec4[0] = fRec58[0];
			double fTemp25 = (fSlow35 * fRec4[1]);
			fVec13[IOTA&255] = fTemp25;
			fRec68[IOTA&1023] = (fRec68[(IOTA-1)&1023] + (fSlow80 * (fVec13[IOTA&255] - fVec13[(IOTA-iSlow81)&255])));
			double fTemp26 = ((0.5 * ((fSlow68 * fRec68[(IOTA-iSlow71)&1023]) + ((fSlow73 * fRec68[(IOTA-iSlow74)&1023]) + (fSlow75 * fRec68[(IOTA-iSlow76)&1023])))) + (fSlow77 * fRec68[(IOTA-iSlow78)&1023]));
			fVec14[0] = fTemp26;
			fRec67[0] = (fConst2 * ((fVec14[0] - fVec14[1]) + (fConst3 * fRec67[1])));
			fRec5[0] = fRec67[0];
			double fTemp27 = (fRec60 - fRec7[1]);
			fVec15[IOTA&1023] = fTemp27;
			double fTemp28 = ((0.5 * ((fSlow89 * fVec15[(IOTA-iSlow91)&1023]) + ((fSlow93 * fVec15[(IOTA-iSlow94)&1023]) + (fSlow95 * fVec15[(IOTA-iSlow96)&1023])))) + (fSlow97 * fVec15[(IOTA-iSlow98)&1023]));
			fVec16[0] = fTemp28;
			fRec69[0] = (fConst2 * ((fVec16[0] - fVec16[1]) + (fConst3 * fRec69[1])));
			fRec6[0] = fRec69[0];
			double fTemp29 = (fSlow57 * fRec6[1]);
			fVec17[IOTA&255] = fTemp29;
			fRec71[IOTA&1023] = (fRec71[(IOTA-1)&1023] + (fSlow100 * (fVec17[IOTA&255] - fVec17[(IOTA-iSlow101)&255])));
			double fTemp30 = ((0.5 * ((fSlow89 * fRec71[(IOTA-iSlow91)&1023]) + ((fSlow93 * fRec71[(IOTA-iSlow94)&1023]) + (fSlow95 * fRec71[(IOTA-iSlow96)&1023])))) + (fSlow97 * fRec71[(IOTA-iSlow98)&1023]));
			fVec18[0] = fTemp30;
			fRec70[0] = (fConst2 * ((fVec18[0] - fVec18[1]) + (fConst3 * fRec70[1])));
			fRec7[0] = fRec70[0];
			double fTemp31 = (fSlow112 + fVec0[0]);
			fVec19[0] = fTemp31;
			double fTemp32 = (fRec9[1] + fRec11[1]);
			double fTemp33 = (((fSlow13 * (fVec19[0] - (0.999 * fRec73[1]))) + (fSlow18 * (fVec19[0] - fVec19[1]))) + (fSlow19 * fTemp32));
			double fTemp34 = (((0.999 * fRec76[1]) + (fSlow15 * fTemp33)) - fVec19[0]);
			int iTemp35 = ((iRec80[1] == 0) & (fTemp34 < fSlow21));
			iRec80[0] = (iTemp35 + (iRec80[1] * ((1 - ((iRec80[1] == 1) & (fTemp34 > fSlow20))) & (1 - iTemp35))));
			int iTemp36 = ((((iRec80[0] == 0) & (fSlow21 <= fTemp34)) & (fTemp34 <= 0)) + (((iRec80[0] == 1) & (0 <= fTemp34)) & (fTemp34 <= fSlow20)));
			double fTemp37 = ((int(iTemp36))?(fSlow22 * fTemp33):fTemp32);
			fRec79[0] = ((0.999 * fRec79[1]) + (2.2675736961451248e-05 * fTemp37));
			fRec76[0] = fRec79[0];
			double 	fRec77 = fTemp37;
			int 	iRec78 = iTemp36;
			fRec73[0] = fRec76[0];
			double 	fRec74 = fRec77;
			int 	iRec75 = iRec78;
			double fTemp38 = (fRec74 - fRec9[1]);
			fVec20[IOTA&1023] = fTemp38;
			double fTemp39 = ((0.5 * ((fSlow111 * fVec20[(IOTA-iSlow114)&1023]) + ((fSlow116 * fVec20[(IOTA-iSlow117)&1023]) + (fSlow118 * fVec20[(IOTA-iSlow119)&1023])))) + (fSlow120 * fVec20[(IOTA-iSlow121)&1023]));
			fVec21[0] = fTemp39;
			fRec72[0] = (fConst2 * ((fVec21[0] - fVec21[1]) + (fConst3 * fRec72[1])));
			fRec8[0] = fRec72[0];
			double fTemp40 = (fSlow35 * fRec8[1]);
			fVec22[IOTA&255] = fTemp40;
			fRec82[IOTA&1023] = (fRec82[(IOTA-1)&1023] + (fSlow123 * (fVec22[IOTA&255] - fVec22[(IOTA-iSlow124)&255])));
			double fTemp41 = ((0.5 * ((fSlow111 * fRec82[(IOTA-iSlow114)&1023]) + ((fSlow116 * fRec82[(IOTA-iSlow117)&1023]) + (fSlow118 * fRec82[(IOTA-iSlow119)&1023])))) + (fSlow120 * fRec82[(IOTA-iSlow121)&1023]));
			fVec23[0] = fTemp41;
			fRec81[0] = (fConst2 * ((fVec23[0] - fVec23[1]) + (fConst3 * fRec81[1])));
			fRec9[0] = fRec81[0];
			double fTemp42 = (fRec74 - fRec11[1]);
			fVec24[IOTA&1023] = fTemp42;
			double fTemp43 = ((0.5 * ((fSlow132 * fVec24[(IOTA-iSlow134)&1023]) + ((fSlow136 * fVec24[(IOTA-iSlow137)&1023]) + (fSlow138 * fVec24[(IOTA-iSlow139)&1023])))) + (fSlow140 * fVec24[(IOTA-iSlow141)&1023]));
			fVec25[0] = fTemp43;
			fRec83[0] = (fConst2 * ((fVec25[0] - fVec25[1]) + (fConst3 * fRec83[1])));
			fRec10[0] = fRec83[0];
			double fTemp44 = (fSlow57 * fRec10[1]);
			fVec26[IOTA&255] = fTemp44;
			fRec85[IOTA&1023] = (fRec85[(IOTA-1)&1023] + (fSlow143 * (fVec26[IOTA&255] - fVec26[(IOTA-iSlow144)&255])));
			double fTemp45 = ((0.5 * ((fSlow132 * fRec85[(IOTA-iSlow134)&1023]) + ((fSlow136 * fRec85[(IOTA-iSlow137)&1023]) + (fSlow138 * fRec85[(IOTA-iSlow139)&1023])))) + (fSlow140 * fRec85[(IOTA-iSlow141)&1023]));
			fVec27[0] = fTemp45;
			fRec84[0] = (fConst2 * ((fVec27[0] - fVec27[1]) + (fConst3 * fRec84[1])));
			fRec11[0] = fRec84[0];
			double fTemp46 = (fSlow155 + fVec0[0]);
			fVec28[0] = fTemp46;
			double fTemp47 = (fRec13[1] + fRec15[1]);
			double fTemp48 = (((fSlow13 * (fVec28[0] - (0.999 * fRec87[1]))) + (fSlow18 * (fVec28[0] - fVec28[1]))) + (fSlow19 * fTemp47));
			double fTemp49 = (((0.999 * fRec90[1]) + (fSlow15 * fTemp48)) - fVec28[0]);
			int iTemp50 = ((iRec94[1] == 0) & (fTemp49 < fSlow21));
			iRec94[0] = (iTemp50 + (iRec94[1] * ((1 - ((iRec94[1] == 1) & (fTemp49 > fSlow20))) & (1 - iTemp50))));
			int iTemp51 = ((((iRec94[0] == 0) & (fSlow21 <= fTemp49)) & (fTemp49 <= 0)) + (((iRec94[0] == 1) & (0 <= fTemp49)) & (fTemp49 <= fSlow20)));
			double fTemp52 = ((int(iTemp51))?(fSlow22 * fTemp48):fTemp47);
			fRec93[0] = ((0.999 * fRec93[1]) + (2.2675736961451248e-05 * fTemp52));
			fRec90[0] = fRec93[0];
			double 	fRec91 = fTemp52;
			int 	iRec92 = iTemp51;
			fRec87[0] = fRec90[0];
			double 	fRec88 = fRec91;
			int 	iRec89 = iRec92;
			double fTemp53 = (fRec88 - fRec13[1]);
			fVec29[IOTA&1023] = fTemp53;
			double fTemp54 = ((0.5 * ((fSlow154 * fVec29[(IOTA-iSlow157)&1023]) + ((fSlow159 * fVec29[(IOTA-iSlow160)&1023]) + (fSlow161 * fVec29[(IOTA-iSlow162)&1023])))) + (fSlow163 * fVec29[(IOTA-iSlow164)&1023]));
			fVec30[0] = fTemp54;
			fRec86[0] = (fConst2 * ((fVec30[0] - fVec30[1]) + (fConst3 * fRec86[1])));
			fRec12[0] = fRec86[0];
			double fTemp55 = (fSlow35 * fRec12[1]);
			fVec31[IOTA&255] = fTemp55;
			fRec96[IOTA&1023] = (fRec96[(IOTA-1)&1023] + (fSlow166 * (fVec31[IOTA&255] - fVec31[(IOTA-iSlow167)&255])));
			double fTemp56 = ((0.5 * ((fSlow154 * fRec96[(IOTA-iSlow157)&1023]) + ((fSlow159 * fRec96[(IOTA-iSlow160)&1023]) + (fSlow161 * fRec96[(IOTA-iSlow162)&1023])))) + (fSlow163 * fRec96[(IOTA-iSlow164)&1023]));
			fVec32[0] = fTemp56;
			fRec95[0] = (fConst2 * ((fVec32[0] - fVec32[1]) + (fConst3 * fRec95[1])));
			fRec13[0] = fRec95[0];
			double fTemp57 = (fRec88 - fRec15[1]);
			fVec33[IOTA&1023] = fTemp57;
			double fTemp58 = ((0.5 * ((fSlow175 * fVec33[(IOTA-iSlow177)&1023]) + ((fSlow179 * fVec33[(IOTA-iSlow180)&1023]) + (fSlow181 * fVec33[(IOTA-iSlow182)&1023])))) + (fSlow183 * fVec33[(IOTA-iSlow184)&1023]));
			fVec34[0] = fTemp58;
			fRec97[0] = (fConst2 * ((fVec34[0] - fVec34[1]) + (fConst3 * fRec97[1])));
			fRec14[0] = fRec97[0];
			double fTemp59 = (fSlow57 * fRec14[1]);
			fVec35[IOTA&255] = fTemp59;
			fRec99[IOTA&1023] = (fRec99[(IOTA-1)&1023] + (fSlow186 * (fVec35[IOTA&255] - fVec35[(IOTA-iSlow187)&255])));
			double fTemp60 = ((0.5 * ((fSlow175 * fRec99[(IOTA-iSlow177)&1023]) + ((fSlow179 * fRec99[(IOTA-iSlow180)&1023]) + (fSlow181 * fRec99[(IOTA-iSlow182)&1023])))) + (fSlow183 * fRec99[(IOTA-iSlow184)&1023]));
			fVec36[0] = fTemp60;
			fRec98[0] = (fConst2 * ((fVec36[0] - fVec36[1]) + (fConst3 * fRec98[1])));
			fRec15[0] = fRec98[0];
			double fTemp61 = (fSlow198 + fVec0[0]);
			fVec37[0] = fTemp61;
			double fTemp62 = (fRec17[1] + fRec19[1]);
			double fTemp63 = (((fSlow13 * (fVec37[0] - (0.999 * fRec101[1]))) + (fSlow18 * (fVec37[0] - fVec37[1]))) + (fSlow19 * fTemp62));
			double fTemp64 = (((0.999 * fRec104[1]) + (fSlow15 * fTemp63)) - fVec37[0]);
			int iTemp65 = ((iRec108[1] == 0) & (fTemp64 < fSlow21));
			iRec108[0] = (iTemp65 + (iRec108[1] * ((1 - ((iRec108[1] == 1) & (fTemp64 > fSlow20))) & (1 - iTemp65))));
			int iTemp66 = ((((iRec108[0] == 0) & (fSlow21 <= fTemp64)) & (fTemp64 <= 0)) + (((iRec108[0] == 1) & (0 <= fTemp64)) & (fTemp64 <= fSlow20)));
			double fTemp67 = ((int(iTemp66))?(fSlow22 * fTemp63):fTemp62);
			fRec107[0] = ((0.999 * fRec107[1]) + (2.2675736961451248e-05 * fTemp67));
			fRec104[0] = fRec107[0];
			double 	fRec105 = fTemp67;
			int 	iRec106 = iTemp66;
			fRec101[0] = fRec104[0];
			double 	fRec102 = fRec105;
			int 	iRec103 = iRec106;
			double fTemp68 = (fRec102 - fRec17[1]);
			fVec38[IOTA&1023] = fTemp68;
			double fTemp69 = ((0.5 * ((fSlow197 * fVec38[(IOTA-iSlow200)&1023]) + ((fSlow202 * fVec38[(IOTA-iSlow203)&1023]) + (fSlow204 * fVec38[(IOTA-iSlow205)&1023])))) + (fSlow206 * fVec38[(IOTA-iSlow207)&1023]));
			fVec39[0] = fTemp69;
			fRec100[0] = (fConst2 * ((fVec39[0] - fVec39[1]) + (fConst3 * fRec100[1])));
			fRec16[0] = fRec100[0];
			double fTemp70 = (fSlow35 * fRec16[1]);
			fVec40[IOTA&255] = fTemp70;
			fRec110[IOTA&1023] = (fRec110[(IOTA-1)&1023] + (fSlow209 * (fVec40[IOTA&255] - fVec40[(IOTA-iSlow210)&255])));
			double fTemp71 = ((fSlow206 * fRec110[(IOTA-iSlow207)&1023]) + (0.5 * ((fSlow197 * fRec110[(IOTA-iSlow200)&1023]) + ((fSlow202 * fRec110[(IOTA-iSlow203)&1023]) + (fSlow204 * fRec110[(IOTA-iSlow205)&1023])))));
			fVec41[0] = fTemp71;
			fRec109[0] = (fConst2 * ((fVec41[0] - fVec41[1]) + (fConst3 * fRec109[1])));
			fRec17[0] = fRec109[0];
			double fTemp72 = (fRec102 - fRec19[1]);
			fVec42[IOTA&1023] = fTemp72;
			double fTemp73 = ((0.5 * ((fSlow218 * fVec42[(IOTA-iSlow220)&1023]) + ((fSlow222 * fVec42[(IOTA-iSlow223)&1023]) + (fSlow224 * fVec42[(IOTA-iSlow225)&1023])))) + (fSlow226 * fVec42[(IOTA-iSlow227)&1023]));
			fVec43[0] = fTemp73;
			fRec111[0] = (fConst2 * ((fVec43[0] - fVec43[1]) + (fConst3 * fRec111[1])));
			fRec18[0] = fRec111[0];
			double fTemp74 = (fSlow57 * fRec18[1]);
			fVec44[IOTA&255] = fTemp74;
			fRec113[IOTA&1023] = (fRec113[(IOTA-1)&1023] + (fSlow229 * (fVec44[IOTA&255] - fVec44[(IOTA-iSlow230)&255])));
			double fTemp75 = ((0.5 * ((fSlow218 * fRec113[(IOTA-iSlow220)&1023]) + ((fSlow222 * fRec113[(IOTA-iSlow223)&1023]) + (fSlow224 * fRec113[(IOTA-iSlow225)&1023])))) + (fSlow226 * fRec113[(IOTA-iSlow227)&1023]));
			fVec45[0] = fTemp75;
			fRec112[0] = (fConst2 * ((fVec45[0] - fVec45[1]) + (fConst3 * fRec112[1])));
			fRec19[0] = fRec112[0];
			fVec46[0] = (fVec0[0] - fSlow198);
			double fTemp76 = (fRec21[1] + fRec23[1]);
			double fTemp77 = (((fSlow13 * (fVec0[0] - (fSlow198 + (0.999 * fRec115[1])))) + (fSlow18 * (fVec0[0] - (fSlow198 + fVec46[1])))) + (fSlow19 * fTemp76));
			double fTemp78 = ((fSlow198 + ((0.999 * fRec118[1]) + (fSlow15 * fTemp77))) - fVec0[0]);
			int iTemp79 = ((iRec122[1] == 0) & (fTemp78 < fSlow21));
			iRec122[0] = (iTemp79 + (iRec122[1] * ((1 - ((iRec122[1] == 1) & (fTemp78 > fSlow20))) & (1 - iTemp79))));
			int iTemp80 = ((((iRec122[0] == 0) & (fSlow21 <= fTemp78)) & (fTemp78 <= 0)) + (((iRec122[0] == 1) & (0 <= fTemp78)) & (fTemp78 <= fSlow20)));
			double fTemp81 = ((int(iTemp80))?(fSlow22 * fTemp77):fTemp76);
			fRec121[0] = ((0.999 * fRec121[1]) + (2.2675736961451248e-05 * fTemp81));
			fRec118[0] = fRec121[0];
			double 	fRec119 = fTemp81;
			int 	iRec120 = iTemp80;
			fRec115[0] = fRec118[0];
			double 	fRec116 = fRec119;
			int 	iRec117 = iRec120;
			double fTemp82 = (fRec116 - fRec21[1]);
			fVec47[IOTA&1023] = fTemp82;
			double fTemp83 = ((0.5 * ((fSlow240 * fVec47[(IOTA-iSlow242)&1023]) + ((fSlow244 * fVec47[(IOTA-iSlow245)&1023]) + (fSlow246 * fVec47[(IOTA-iSlow247)&1023])))) + (fSlow248 * fVec47[(IOTA-iSlow249)&1023]));
			fVec48[0] = fTemp83;
			fRec114[0] = (fConst2 * ((fVec48[0] - fVec48[1]) + (fConst3 * fRec114[1])));
			fRec20[0] = fRec114[0];
			double fTemp84 = (fSlow35 * fRec20[1]);
			fVec49[IOTA&255] = fTemp84;
			fRec124[IOTA&1023] = (fRec124[(IOTA-1)&1023] + (fSlow251 * (fVec49[IOTA&255] - fVec49[(IOTA-iSlow252)&255])));
			double fTemp85 = ((0.5 * ((fSlow240 * fRec124[(IOTA-iSlow242)&1023]) + ((fSlow244 * fRec124[(IOTA-iSlow245)&1023]) + (fSlow246 * fRec124[(IOTA-iSlow247)&1023])))) + (fSlow248 * fRec124[(IOTA-iSlow249)&1023]));
			fVec50[0] = fTemp85;
			fRec123[0] = (fConst2 * ((fVec50[0] - fVec50[1]) + (fConst3 * fRec123[1])));
			fRec21[0] = fRec123[0];
			double fTemp86 = (fRec116 - fRec23[1]);
			fVec51[IOTA&1023] = fTemp86;
			double fTemp87 = ((0.5 * ((fSlow260 * fVec51[(IOTA-iSlow262)&1023]) + ((fSlow264 * fVec51[(IOTA-iSlow265)&1023]) + (fSlow266 * fVec51[(IOTA-iSlow267)&1023])))) + (fSlow268 * fVec51[(IOTA-iSlow269)&1023]));
			fVec52[0] = fTemp87;
			fRec125[0] = (fConst2 * ((fVec52[0] - fVec52[1]) + (fConst3 * fRec125[1])));
			fRec22[0] = fRec125[0];
			double fTemp88 = (fSlow57 * fRec22[1]);
			fVec53[IOTA&255] = fTemp88;
			fRec127[IOTA&1023] = (fRec127[(IOTA-1)&1023] + (fSlow271 * (fVec53[IOTA&255] - fVec53[(IOTA-iSlow272)&255])));
			double fTemp89 = ((0.5 * ((fSlow260 * fRec127[(IOTA-iSlow262)&1023]) + ((fSlow264 * fRec127[(IOTA-iSlow265)&1023]) + (fSlow266 * fRec127[(IOTA-iSlow267)&1023])))) + (fSlow268 * fRec127[(IOTA-iSlow269)&1023]));
			fVec54[0] = fTemp89;
			fRec126[0] = (fConst2 * ((fVec54[0] - fVec54[1]) + (fConst3 * fRec126[1])));
			fRec23[0] = fRec126[0];
			fVec55[0] = (fVec0[0] - fSlow283);
			double fTemp90 = (fRec25[1] + fRec27[1]);
			double fTemp91 = (((fSlow13 * (fVec0[0] - (fSlow283 + (0.999 * fRec129[1])))) + (fSlow18 * (fVec0[0] - (fSlow283 + fVec55[1])))) + (fSlow19 * fTemp90));
			double fTemp92 = ((fSlow283 + ((0.999 * fRec132[1]) + (fSlow15 * fTemp91))) - fVec0[0]);
			int iTemp93 = ((iRec136[1] == 0) & (fTemp92 < fSlow21));
			iRec136[0] = (iTemp93 + (iRec136[1] * ((1 - ((iRec136[1] == 1) & (fTemp92 > fSlow20))) & (1 - iTemp93))));
			int iTemp94 = ((((iRec136[0] == 0) & (fSlow21 <= fTemp92)) & (fTemp92 <= 0)) + (((iRec136[0] == 1) & (0 <= fTemp92)) & (fTemp92 <= fSlow20)));
			double fTemp95 = ((int(iTemp94))?(fSlow22 * fTemp91):fTemp90);
			fRec135[0] = ((0.999 * fRec135[1]) + (2.2675736961451248e-05 * fTemp95));
			fRec132[0] = fRec135[0];
			double 	fRec133 = fTemp95;
			int 	iRec134 = iTemp94;
			fRec129[0] = fRec132[0];
			double 	fRec130 = fRec133;
			int 	iRec131 = iRec134;
			double fTemp96 = (fRec130 - fRec25[1]);
			fVec56[IOTA&1023] = fTemp96;
			double fTemp97 = ((0.5 * ((fSlow282 * fVec56[(IOTA-iSlow285)&1023]) + ((fSlow287 * fVec56[(IOTA-iSlow288)&1023]) + (fSlow289 * fVec56[(IOTA-iSlow290)&1023])))) + (fSlow291 * fVec56[(IOTA-iSlow292)&1023]));
			fVec57[0] = fTemp97;
			fRec128[0] = (fConst2 * ((fVec57[0] - fVec57[1]) + (fConst3 * fRec128[1])));
			fRec24[0] = fRec128[0];
			double fTemp98 = (fSlow35 * fRec24[1]);
			fVec58[IOTA&255] = fTemp98;
			fRec138[IOTA&1023] = (fRec138[(IOTA-1)&1023] + (fSlow294 * (fVec58[IOTA&255] - fVec58[(IOTA-iSlow295)&255])));
			double fTemp99 = ((0.5 * (((fSlow287 * fRec138[(IOTA-iSlow288)&1023]) + (fSlow282 * fRec138[(IOTA-iSlow285)&1023])) + (fSlow289 * fRec138[(IOTA-iSlow290)&1023]))) + (fSlow291 * fRec138[(IOTA-iSlow292)&1023]));
			fVec59[0] = fTemp99;
			fRec137[0] = (fConst2 * ((fVec59[0] - fVec59[1]) + (fConst3 * fRec137[1])));
			fRec25[0] = fRec137[0];
			double fTemp100 = (fRec130 - fRec27[1]);
			fVec60[IOTA&1023] = fTemp100;
			double fTemp101 = ((0.5 * ((fSlow303 * fVec60[(IOTA-iSlow305)&1023]) + ((fSlow307 * fVec60[(IOTA-iSlow308)&1023]) + (fSlow309 * fVec60[(IOTA-iSlow310)&1023])))) + (fSlow311 * fVec60[(IOTA-iSlow312)&1023]));
			fVec61[0] = fTemp101;
			fRec139[0] = (fConst2 * ((fVec61[0] - fVec61[1]) + (fConst3 * fRec139[1])));
			fRec26[0] = fRec139[0];
			double fTemp102 = (fSlow57 * fRec26[1]);
			fVec62[IOTA&255] = fTemp102;
			fRec141[IOTA&1023] = (fRec141[(IOTA-1)&1023] + (fSlow314 * (fVec62[IOTA&255] - fVec62[(IOTA-iSlow315)&255])));
			double fTemp103 = ((0.5 * ((fSlow303 * fRec141[(IOTA-iSlow305)&1023]) + ((fSlow307 * fRec141[(IOTA-iSlow308)&1023]) + (fSlow309 * fRec141[(IOTA-iSlow310)&1023])))) + (fSlow311 * fRec141[(IOTA-iSlow312)&1023]));
			fVec63[0] = fTemp103;
			fRec140[0] = (fConst2 * ((fVec63[0] - fVec63[1]) + (fConst3 * fRec140[1])));
			fRec27[0] = fRec140[0];
			fVec64[0] = (fVec0[0] - fSlow325);
			double fTemp104 = (fRec29[1] + fRec31[1]);
			double fTemp105 = (((fSlow13 * (fVec0[0] - (fSlow325 + (0.999 * fRec143[1])))) + (fSlow18 * (fVec0[0] - (fSlow325 + fVec64[1])))) + (fSlow19 * fTemp104));
			double fTemp106 = ((fSlow325 + ((0.999 * fRec146[1]) + (fSlow15 * fTemp105))) - fVec0[0]);
			int iTemp107 = ((iRec150[1] == 0) & (fTemp106 < fSlow21));
			iRec150[0] = (iTemp107 + (iRec150[1] * ((1 - ((iRec150[1] == 1) & (fTemp106 > fSlow20))) & (1 - iTemp107))));
			int iTemp108 = ((((iRec150[0] == 0) & (fSlow21 <= fTemp106)) & (fTemp106 <= 0)) + (((iRec150[0] == 1) & (0 <= fTemp106)) & (fTemp106 <= fSlow20)));
			double fTemp109 = ((int(iTemp108))?(fSlow22 * fTemp105):fTemp104);
			fRec149[0] = ((0.999 * fRec149[1]) + (2.2675736961451248e-05 * fTemp109));
			fRec146[0] = fRec149[0];
			double 	fRec147 = fTemp109;
			int 	iRec148 = iTemp108;
			fRec143[0] = fRec146[0];
			double 	fRec144 = fRec147;
			int 	iRec145 = iRec148;
			double fTemp110 = (fRec144 - fRec29[1]);
			fVec65[IOTA&1023] = fTemp110;
			double fTemp111 = ((0.5 * ((fSlow324 * fVec65[(IOTA-iSlow327)&1023]) + ((fSlow329 * fVec65[(IOTA-iSlow330)&1023]) + (fSlow332 * fVec65[(IOTA-iSlow333)&1023])))) + (fSlow334 * fVec65[(IOTA-iSlow335)&1023]));
			fVec66[0] = fTemp111;
			fRec142[0] = (fConst2 * ((fVec66[0] - fVec66[1]) + (fConst3 * fRec142[1])));
			fRec28[0] = fRec142[0];
			double fTemp112 = (fSlow35 * fRec28[1]);
			fVec67[IOTA&255] = fTemp112;
			fRec152[IOTA&1023] = (fRec152[(IOTA-1)&1023] + (fSlow337 * (fVec67[IOTA&255] - fVec67[(IOTA-iSlow338)&255])));
			double fTemp113 = ((0.5 * ((fSlow324 * fRec152[(IOTA-iSlow327)&1023]) + ((fSlow329 * fRec152[(IOTA-iSlow330)&1023]) + (fSlow332 * fRec152[(IOTA-iSlow333)&1023])))) + (fSlow334 * fRec152[(IOTA-iSlow335)&1023]));
			fVec68[0] = fTemp113;
			fRec151[0] = (fConst2 * ((fVec68[0] - fVec68[1]) + (fConst3 * fRec151[1])));
			fRec29[0] = fRec151[0];
			double fTemp114 = (fRec144 - fRec31[1]);
			fVec69[IOTA&1023] = fTemp114;
			double fTemp115 = ((0.5 * ((fSlow346 * fVec69[(IOTA-iSlow348)&1023]) + ((fSlow350 * fVec69[(IOTA-iSlow351)&1023]) + (fSlow352 * fVec69[(IOTA-iSlow353)&1023])))) + (fSlow354 * fVec69[(IOTA-iSlow355)&1023]));
			fVec70[0] = fTemp115;
			fRec153[0] = (fConst2 * ((fVec70[0] - fVec70[1]) + (fConst3 * fRec153[1])));
			fRec30[0] = fRec153[0];
			double fTemp116 = (fSlow57 * fRec30[1]);
			fVec71[IOTA&255] = fTemp116;
			fRec155[IOTA&1023] = (fRec155[(IOTA-1)&1023] + (fSlow357 * (fVec71[IOTA&255] - fVec71[(IOTA-iSlow358)&255])));
			double fTemp117 = ((0.5 * ((fSlow346 * fRec155[(IOTA-iSlow348)&1023]) + ((fSlow350 * fRec155[(IOTA-iSlow351)&1023]) + (fSlow352 * fRec155[(IOTA-iSlow353)&1023])))) + (fSlow354 * fRec155[(IOTA-iSlow355)&1023]));
			fVec72[0] = fTemp117;
			fRec154[0] = (fConst2 * ((fVec72[0] - fVec72[1]) + (fConst3 * fRec154[1])));
			fRec31[0] = fRec154[0];
			fVec73[0] = (fVec0[0] - fSlow369);
			double fTemp118 = (fRec33[1] + fRec35[1]);
			double fTemp119 = (((fSlow13 * (fVec0[0] - (fSlow369 + (0.999 * fRec157[1])))) + (fSlow18 * (fVec0[0] - (fSlow369 + fVec73[1])))) + (fSlow19 * fTemp118));
			double fTemp120 = ((fSlow369 + ((0.999 * fRec160[1]) + (fSlow15 * fTemp119))) - fVec0[0]);
			int iTemp121 = ((iRec164[1] == 0) & (fTemp120 < fSlow21));
			iRec164[0] = (iTemp121 + (iRec164[1] * ((1 - ((iRec164[1] == 1) & (fTemp120 > fSlow20))) & (1 - iTemp121))));
			int iTemp122 = ((((iRec164[0] == 0) & (fSlow21 <= fTemp120)) & (fTemp120 <= 0)) + (((iRec164[0] == 1) & (0 <= fTemp120)) & (fTemp120 <= fSlow20)));
			double fTemp123 = ((int(iTemp122))?(fSlow22 * fTemp119):fTemp118);
			fRec163[0] = ((0.999 * fRec163[1]) + (2.2675736961451248e-05 * fTemp123));
			fRec160[0] = fRec163[0];
			double 	fRec161 = fTemp123;
			int 	iRec162 = iTemp122;
			fRec157[0] = fRec160[0];
			double 	fRec158 = fRec161;
			int 	iRec159 = iRec162;
			double fTemp124 = (fRec158 - fRec33[1]);
			fVec74[IOTA&1023] = fTemp124;
			double fTemp125 = ((0.5 * ((fSlow368 * fVec74[(IOTA-iSlow371)&1023]) + ((fSlow373 * fVec74[(IOTA-iSlow374)&1023]) + (fSlow375 * fVec74[(IOTA-iSlow376)&1023])))) + (fSlow377 * fVec74[(IOTA-iSlow378)&1023]));
			fVec75[0] = fTemp125;
			fRec156[0] = (fConst2 * ((fConst3 * fRec156[1]) + (fVec75[0] - fVec75[1])));
			fRec32[0] = fRec156[0];
			double fTemp126 = (fSlow35 * fRec32[1]);
			fVec76[IOTA&255] = fTemp126;
			fRec166[IOTA&1023] = (fRec166[(IOTA-1)&1023] + (fSlow380 * (fVec76[IOTA&255] - fVec76[(IOTA-iSlow381)&255])));
			double fTemp127 = ((0.5 * ((fSlow368 * fRec166[(IOTA-iSlow371)&1023]) + ((fSlow373 * fRec166[(IOTA-iSlow374)&1023]) + (fSlow375 * fRec166[(IOTA-iSlow376)&1023])))) + (fSlow377 * fRec166[(IOTA-iSlow378)&1023]));
			fVec77[0] = fTemp127;
			fRec165[0] = (fConst2 * ((fVec77[0] - fVec77[1]) + (fConst3 * fRec165[1])));
			fRec33[0] = fRec165[0];
			double fTemp128 = (fRec158 - fRec35[1]);
			fVec78[IOTA&1023] = fTemp128;
			double fTemp129 = ((0.5 * ((fSlow389 * fVec78[(IOTA-iSlow391)&1023]) + ((fSlow393 * fVec78[(IOTA-iSlow394)&1023]) + (fSlow395 * fVec78[(IOTA-iSlow396)&1023])))) + (fSlow397 * fVec78[(IOTA-iSlow398)&1023]));
			fVec79[0] = fTemp129;
			fRec167[0] = (fConst2 * ((fVec79[0] - fVec79[1]) + (fConst3 * fRec167[1])));
			fRec34[0] = fRec167[0];
			double fTemp130 = (fSlow57 * fRec34[1]);
			fVec80[IOTA&255] = fTemp130;
			fRec169[IOTA&1023] = (fRec169[(IOTA-1)&1023] + (fSlow400 * (fVec80[IOTA&255] - fVec80[(IOTA-iSlow401)&255])));
			double fTemp131 = ((0.5 * ((fSlow389 * fRec169[(IOTA-iSlow391)&1023]) + ((fSlow393 * fRec169[(IOTA-iSlow394)&1023]) + (fSlow395 * fRec169[(IOTA-iSlow396)&1023])))) + (fSlow397 * fRec169[(IOTA-iSlow398)&1023]));
			fVec81[0] = fTemp131;
			fRec168[0] = (fConst2 * ((fVec81[0] - fVec81[1]) + (fConst3 * fRec168[1])));
			fRec35[0] = fRec168[0];
			fVec82[0] = (fVec0[0] - fSlow412);
			double fTemp132 = (fRec37[1] + fRec39[1]);
			double fTemp133 = (((fSlow13 * (fVec0[0] - (fSlow412 + (0.999 * fRec171[1])))) + (fSlow18 * (fVec0[0] - (fSlow412 + fVec82[1])))) + (fSlow19 * fTemp132));
			double fTemp134 = ((fSlow412 + ((0.999 * fRec174[1]) + (fSlow15 * fTemp133))) - fVec0[0]);
			int iTemp135 = ((iRec178[1] == 0) & (fTemp134 < fSlow21));
			iRec178[0] = (iTemp135 + (iRec178[1] * ((1 - ((iRec178[1] == 1) & (fTemp134 > fSlow20))) & (1 - iTemp135))));
			int iTemp136 = ((((iRec178[0] == 0) & (fSlow21 <= fTemp134)) & (fTemp134 <= 0)) + (((iRec178[0] == 1) & (0 <= fTemp134)) & (fTemp134 <= fSlow20)));
			double fTemp137 = ((int(iTemp136))?(fSlow22 * fTemp133):fTemp132);
			fRec177[0] = ((0.999 * fRec177[1]) + (2.2675736961451248e-05 * fTemp137));
			fRec174[0] = fRec177[0];
			double 	fRec175 = fTemp137;
			int 	iRec176 = iTemp136;
			fRec171[0] = fRec174[0];
			double 	fRec172 = fRec175;
			int 	iRec173 = iRec176;
			double fTemp138 = (fRec172 - fRec37[1]);
			fVec83[IOTA&1023] = fTemp138;
			double fTemp139 = ((0.5 * ((fSlow411 * fVec83[(IOTA-iSlow414)&1023]) + ((fSlow416 * fVec83[(IOTA-iSlow417)&1023]) + (fSlow418 * fVec83[(IOTA-iSlow419)&1023])))) + (fSlow420 * fVec83[(IOTA-iSlow421)&1023]));
			fVec84[0] = fTemp139;
			fRec170[0] = (fConst2 * ((fVec84[0] - fVec84[1]) + (fConst3 * fRec170[1])));
			fRec36[0] = fRec170[0];
			double fTemp140 = (fSlow35 * fRec36[1]);
			fVec85[IOTA&255] = fTemp140;
			fRec180[IOTA&1023] = (fRec180[(IOTA-1)&1023] + (fSlow423 * (fVec85[IOTA&255] - fVec85[(IOTA-iSlow424)&255])));
			double fTemp141 = ((0.5 * ((fSlow411 * fRec180[(IOTA-iSlow414)&1023]) + ((fSlow416 * fRec180[(IOTA-iSlow417)&1023]) + (fSlow418 * fRec180[(IOTA-iSlow419)&1023])))) + (fSlow420 * fRec180[(IOTA-iSlow421)&1023]));
			fVec86[0] = fTemp141;
			fRec179[0] = (fConst2 * ((fVec86[0] - fVec86[1]) + (fConst3 * fRec179[1])));
			fRec37[0] = fRec179[0];
			double fTemp142 = (fRec172 - fRec39[1]);
			fVec87[IOTA&1023] = fTemp142;
			double fTemp143 = ((0.5 * ((fSlow432 * fVec87[(IOTA-iSlow434)&1023]) + ((fSlow436 * fVec87[(IOTA-iSlow437)&1023]) + (fSlow438 * fVec87[(IOTA-iSlow439)&1023])))) + (fSlow440 * fVec87[(IOTA-iSlow441)&1023]));
			fVec88[0] = fTemp143;
			fRec181[0] = (fConst2 * ((fVec88[0] - fVec88[1]) + (fConst3 * fRec181[1])));
			fRec38[0] = fRec181[0];
			double fTemp144 = (fSlow57 * fRec38[1]);
			fVec89[IOTA&255] = fTemp144;
			fRec183[IOTA&1023] = (fRec183[(IOTA-1)&1023] + (fSlow443 * (fVec89[IOTA&255] - fVec89[(IOTA-iSlow444)&255])));
			double fTemp145 = ((0.5 * ((fSlow432 * fRec183[(IOTA-iSlow434)&1023]) + ((fSlow436 * fRec183[(IOTA-iSlow437)&1023]) + (fSlow438 * fRec183[(IOTA-iSlow439)&1023])))) + (fSlow440 * fRec183[(IOTA-iSlow441)&1023]));
			fVec90[0] = fTemp145;
			fRec182[0] = (fConst2 * ((fVec90[0] - fVec90[1]) + (fConst3 * fRec182[1])));
			fRec39[0] = fRec182[0];
			double fTemp146 = (4.41e+04 * (fVec0[0] - fVec0[1]));
			double 	fRec40 = ((((((((((((fSlow13 * (fRec45[0] - fVec1[0])) + (fSlow12 * (fRec46 - fTemp146))) * iRec47) + (((fSlow13 * (fRec59[0] - fVec10[0])) + (fSlow12 * (fRec60 - fTemp146))) * iRec61)) + (((fSlow13 * (fRec73[0] - fVec19[0])) + (fSlow12 * (fRec74 - fTemp146))) * iRec75)) + (((fSlow13 * (fRec87[0] - fVec28[0])) + (fSlow12 * (fRec88 - fTemp146))) * iRec89)) + (((fSlow13 * (fRec101[0] - fVec37[0])) + (fSlow12 * (fRec102 - fTemp146))) * iRec103)) + (((fSlow13 * ((fSlow198 + fRec115[0]) - fVec0[0])) + (fSlow12 * (fRec116 - fTemp146))) * iRec117)) + (((fSlow13 * ((fSlow283 + fRec129[0]) - fVec0[0])) + (fSlow12 * (fRec130 - fTemp146))) * iRec131)) + (((fSlow13 * ((fSlow325 + fRec143[0]) - fVec0[0])) + (fSlow12 * (fRec144 - fTemp146))) * iRec145)) + (((fSlow13 * ((fSlow369 + fRec157[0]) - fVec0[0])) + (fSlow12 * (fRec158 - fTemp146))) * iRec159)) + (((fSlow13 * ((fSlow412 + fRec171[0]) - fVec0[0])) + (fSlow12 * (fRec172 - fTemp146))) * iRec173));
			double 	fRec41 = 0.0;
			double fTemp147 = fRec183[(IOTA-0)&1023];
			fVec91[0] = fSlow445;
			fRec189[0] = ((0.9985762556729113 * fRec189[1]) + (0.0007118721635443319 * (fSlow445 + fVec91[1])));
			double fTemp148 = ((((((((((fRec54[(IOTA-0)&1023] + (0.85 * fRec68[(IOTA-0)&1023])) + (0.75 * fRec82[(IOTA-0)&1023])) + (0.65 * fRec96[(IOTA-0)&1023])) + (0.55 * fRec110[(IOTA-0)&1023])) + (0.45 * fRec124[(IOTA-0)&1023])) + (0.35 * fRec138[(IOTA-0)&1023])) + (0.25 * fRec152[(IOTA-0)&1023])) + (0.15 * fRec166[(IOTA-0)&1023])) + (0.05 * fTemp147)) * fRec189[0]);
			fVec92[0] = fTemp148;
			fRec188[0] = ((fConst12 * fRec188[1]) + (fConst13 * (fVec92[0] + fVec92[1])));
			fRec187[0] = (fRec188[0] - (fConst8 * ((fConst14 * fRec187[1]) + (fConst15 * fRec187[2]))));
			fRec186[0] = ((fConst8 * (((fConst9 * fRec187[1]) + (fConst7 * fRec187[0])) + (fConst7 * fRec187[2]))) - (fConst6 * ((fConst14 * fRec186[1]) + (fConst16 * fRec186[2]))));
			fRec185[0] = ((fConst6 * (((fConst7 * fRec186[0]) + (fConst9 * fRec186[1])) + (fConst7 * fRec186[2]))) - (fConst8 * ((fConst15 * fRec185[2]) + (fConst14 * fRec185[1]))));
			fRec184[0] = ((fConst8 * ((fConst7 * fRec185[2]) + ((fConst7 * fRec185[0]) + (fConst9 * fRec185[1])))) - (fConst6 * ((fConst16 * fRec184[2]) + (fConst14 * fRec184[1]))));
			double fTemp149 = (((fConst7 * fRec184[0]) + (fConst9 * fRec184[1])) + (fConst7 * fRec184[2]));
			double fTemp150 = fabs((fConst6 * fTemp149));
			fRec192[0] = ((fConst21 * max(fTemp150, fRec192[1])) + (fConst22 * fTemp150));
			fRec191[0] = ((fConst19 * fRec191[1]) + (fConst20 * fRec192[0]));
			fRec190[0] = ((fConst17 * fRec190[1]) + (fConst18 * (0 - (0.99999999999 * max((6 + (20 * log10(fRec191[0]))), 0.0)))));
			double 	fRec42 = (fConst6 * (fTemp149 * pow(10,(0.05 * fRec190[0]))));
			double fTemp151 = (fRec189[0] * ((((((((((0.05 * fRec57[(IOTA-0)&1023]) + (0.15 * fRec71[(IOTA-0)&1023])) + (0.25 * fRec85[(IOTA-0)&1023])) + (0.35 * fRec99[(IOTA-0)&1023])) + (0.45 * fRec113[(IOTA-0)&1023])) + (0.55 * fRec127[(IOTA-0)&1023])) + (0.65 * fRec141[(IOTA-0)&1023])) + (0.75 * fRec155[(IOTA-0)&1023])) + (0.85 * fRec169[(IOTA-0)&1023])) + (0.95 * fTemp147)));
			fVec93[0] = fTemp151;
			fRec197[0] = ((fConst12 * fRec197[1]) + (fConst13 * (fVec93[0] + fVec93[1])));
			fRec196[0] = (fRec197[0] - (fConst8 * ((fConst15 * fRec196[2]) + (fConst14 * fRec196[1]))));
			fRec195[0] = ((fConst8 * ((fConst7 * fRec196[2]) + ((fConst7 * fRec196[0]) + (fConst9 * fRec196[1])))) - (fConst6 * ((fConst16 * fRec195[2]) + (fConst14 * fRec195[1]))));
			fRec194[0] = ((fConst6 * (((fConst7 * fRec195[0]) + (fConst9 * fRec195[1])) + (fConst7 * fRec195[2]))) - (fConst8 * ((fConst15 * fRec194[2]) + (fConst14 * fRec194[1]))));
			fRec193[0] = ((fConst8 * ((fConst7 * fRec194[2]) + ((fConst7 * fRec194[0]) + (fConst9 * fRec194[1])))) - (fConst6 * ((fConst16 * fRec193[2]) + (fConst14 * fRec193[1]))));
			double fTemp152 = (((fConst7 * fRec193[0]) + (fConst9 * fRec193[1])) + (fConst7 * fRec193[2]));
			double fTemp153 = fabs((fConst6 * fTemp152));
			fRec200[0] = ((fConst21 * max(fTemp153, fRec200[1])) + (fConst22 * fTemp153));
			fRec199[0] = ((fConst19 * fRec199[1]) + (fConst20 * fRec200[0]));
			fRec198[0] = ((fConst17 * fRec198[1]) + (fConst18 * (0 - (0.99999999999 * max((6 + (20 * log10(fRec199[0]))), 0.0)))));
			double 	fRec43 = (fConst6 * (fTemp152 * pow(10,(0.05 * fRec198[0]))));
			output0[i] = (FAUSTFLOAT)fRec40;
			output1[i] = (FAUSTFLOAT)fRec41;
			output2[i] = (FAUSTFLOAT)fRec42;
			output3[i] = (FAUSTFLOAT)fRec43;
			// post processing
			fRec198[1] = fRec198[0];
			fRec199[1] = fRec199[0];
			fRec200[1] = fRec200[0];
			fRec193[2] = fRec193[1]; fRec193[1] = fRec193[0];
			fRec194[2] = fRec194[1]; fRec194[1] = fRec194[0];
			fRec195[2] = fRec195[1]; fRec195[1] = fRec195[0];
			fRec196[2] = fRec196[1]; fRec196[1] = fRec196[0];
			fRec197[1] = fRec197[0];
			fVec93[1] = fVec93[0];
			fRec190[1] = fRec190[0];
			fRec191[1] = fRec191[0];
			fRec192[1] = fRec192[0];
			fRec184[2] = fRec184[1]; fRec184[1] = fRec184[0];
			fRec185[2] = fRec185[1]; fRec185[1] = fRec185[0];
			fRec186[2] = fRec186[1]; fRec186[1] = fRec186[0];
			fRec187[2] = fRec187[1]; fRec187[1] = fRec187[0];
			fRec188[1] = fRec188[0];
			fVec92[1] = fVec92[0];
			fRec189[1] = fRec189[0];
			fVec91[1] = fVec91[0];
			fRec39[1] = fRec39[0];
			fRec182[1] = fRec182[0];
			fVec90[1] = fVec90[0];
			fRec38[1] = fRec38[0];
			fRec181[1] = fRec181[0];
			fVec88[1] = fVec88[0];
			fRec37[1] = fRec37[0];
			fRec179[1] = fRec179[0];
			fVec86[1] = fVec86[0];
			fRec36[1] = fRec36[0];
			fRec170[1] = fRec170[0];
			fVec84[1] = fVec84[0];
			fRec171[1] = fRec171[0];
			fRec174[1] = fRec174[0];
			fRec177[1] = fRec177[0];
			iRec178[1] = iRec178[0];
			fVec82[1] = fVec82[0];
			fRec35[1] = fRec35[0];
			fRec168[1] = fRec168[0];
			fVec81[1] = fVec81[0];
			fRec34[1] = fRec34[0];
			fRec167[1] = fRec167[0];
			fVec79[1] = fVec79[0];
			fRec33[1] = fRec33[0];
			fRec165[1] = fRec165[0];
			fVec77[1] = fVec77[0];
			fRec32[1] = fRec32[0];
			fRec156[1] = fRec156[0];
			fVec75[1] = fVec75[0];
			fRec157[1] = fRec157[0];
			fRec160[1] = fRec160[0];
			fRec163[1] = fRec163[0];
			iRec164[1] = iRec164[0];
			fVec73[1] = fVec73[0];
			fRec31[1] = fRec31[0];
			fRec154[1] = fRec154[0];
			fVec72[1] = fVec72[0];
			fRec30[1] = fRec30[0];
			fRec153[1] = fRec153[0];
			fVec70[1] = fVec70[0];
			fRec29[1] = fRec29[0];
			fRec151[1] = fRec151[0];
			fVec68[1] = fVec68[0];
			fRec28[1] = fRec28[0];
			fRec142[1] = fRec142[0];
			fVec66[1] = fVec66[0];
			fRec143[1] = fRec143[0];
			fRec146[1] = fRec146[0];
			fRec149[1] = fRec149[0];
			iRec150[1] = iRec150[0];
			fVec64[1] = fVec64[0];
			fRec27[1] = fRec27[0];
			fRec140[1] = fRec140[0];
			fVec63[1] = fVec63[0];
			fRec26[1] = fRec26[0];
			fRec139[1] = fRec139[0];
			fVec61[1] = fVec61[0];
			fRec25[1] = fRec25[0];
			fRec137[1] = fRec137[0];
			fVec59[1] = fVec59[0];
			fRec24[1] = fRec24[0];
			fRec128[1] = fRec128[0];
			fVec57[1] = fVec57[0];
			fRec129[1] = fRec129[0];
			fRec132[1] = fRec132[0];
			fRec135[1] = fRec135[0];
			iRec136[1] = iRec136[0];
			fVec55[1] = fVec55[0];
			fRec23[1] = fRec23[0];
			fRec126[1] = fRec126[0];
			fVec54[1] = fVec54[0];
			fRec22[1] = fRec22[0];
			fRec125[1] = fRec125[0];
			fVec52[1] = fVec52[0];
			fRec21[1] = fRec21[0];
			fRec123[1] = fRec123[0];
			fVec50[1] = fVec50[0];
			fRec20[1] = fRec20[0];
			fRec114[1] = fRec114[0];
			fVec48[1] = fVec48[0];
			fRec115[1] = fRec115[0];
			fRec118[1] = fRec118[0];
			fRec121[1] = fRec121[0];
			iRec122[1] = iRec122[0];
			fVec46[1] = fVec46[0];
			fRec19[1] = fRec19[0];
			fRec112[1] = fRec112[0];
			fVec45[1] = fVec45[0];
			fRec18[1] = fRec18[0];
			fRec111[1] = fRec111[0];
			fVec43[1] = fVec43[0];
			fRec17[1] = fRec17[0];
			fRec109[1] = fRec109[0];
			fVec41[1] = fVec41[0];
			fRec16[1] = fRec16[0];
			fRec100[1] = fRec100[0];
			fVec39[1] = fVec39[0];
			fRec101[1] = fRec101[0];
			fRec104[1] = fRec104[0];
			fRec107[1] = fRec107[0];
			iRec108[1] = iRec108[0];
			fVec37[1] = fVec37[0];
			fRec15[1] = fRec15[0];
			fRec98[1] = fRec98[0];
			fVec36[1] = fVec36[0];
			fRec14[1] = fRec14[0];
			fRec97[1] = fRec97[0];
			fVec34[1] = fVec34[0];
			fRec13[1] = fRec13[0];
			fRec95[1] = fRec95[0];
			fVec32[1] = fVec32[0];
			fRec12[1] = fRec12[0];
			fRec86[1] = fRec86[0];
			fVec30[1] = fVec30[0];
			fRec87[1] = fRec87[0];
			fRec90[1] = fRec90[0];
			fRec93[1] = fRec93[0];
			iRec94[1] = iRec94[0];
			fVec28[1] = fVec28[0];
			fRec11[1] = fRec11[0];
			fRec84[1] = fRec84[0];
			fVec27[1] = fVec27[0];
			fRec10[1] = fRec10[0];
			fRec83[1] = fRec83[0];
			fVec25[1] = fVec25[0];
			fRec9[1] = fRec9[0];
			fRec81[1] = fRec81[0];
			fVec23[1] = fVec23[0];
			fRec8[1] = fRec8[0];
			fRec72[1] = fRec72[0];
			fVec21[1] = fVec21[0];
			fRec73[1] = fRec73[0];
			fRec76[1] = fRec76[0];
			fRec79[1] = fRec79[0];
			iRec80[1] = iRec80[0];
			fVec19[1] = fVec19[0];
			fRec7[1] = fRec7[0];
			fRec70[1] = fRec70[0];
			fVec18[1] = fVec18[0];
			fRec6[1] = fRec6[0];
			fRec69[1] = fRec69[0];
			fVec16[1] = fVec16[0];
			fRec5[1] = fRec5[0];
			fRec67[1] = fRec67[0];
			fVec14[1] = fVec14[0];
			fRec4[1] = fRec4[0];
			fRec58[1] = fRec58[0];
			fVec12[1] = fVec12[0];
			fRec59[1] = fRec59[0];
			fRec62[1] = fRec62[0];
			fRec65[1] = fRec65[0];
			iRec66[1] = iRec66[0];
			fVec10[1] = fVec10[0];
			fRec3[1] = fRec3[0];
			fRec56[1] = fRec56[0];
			fVec9[1] = fVec9[0];
			fRec2[1] = fRec2[0];
			fRec55[1] = fRec55[0];
			fVec7[1] = fVec7[0];
			fRec1[1] = fRec1[0];
			fRec53[1] = fRec53[0];
			fVec5[1] = fVec5[0];
			fRec0[1] = fRec0[0];
			fRec44[1] = fRec44[0];
			fVec3[1] = fVec3[0];
			IOTA = IOTA+1;
			fRec45[1] = fRec45[0];
			fRec48[1] = fRec48[0];
			fRec51[1] = fRec51[0];
			iRec52[1] = iRec52[0];
			fVec1[1] = fVec1[0];
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
  pluck_harp10 *dsp;
  PdUI *ui;
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
    PdUI *ui = x->ui;
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
  PdUI *ui = x->ui;
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
    x->label = new std::string(sym(pluck_harp10) "~");
  x->dsp = new pluck_harp10();
  x->ui = new PdUI(id?id->s_name:NULL);
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

extern "C" void faust_setup(pluck_harp10)
{
  //fesetenv(FE_DFL_DISABLE_DENORMS_ENV);
  t_symbol *s = gensym(sym(pluck_harp10) "~");
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
  pluck_harp10 dsp = pluck_harp10();
  post("[faust] %s: %d inputs, %d outputs", sym(pluck_harp10) "~",
       dsp.getNumInputs(), dsp.getNumOutputs());
}
