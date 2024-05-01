// -*- C++ -*-

#ifndef BEAM_MAN_HH
#define BEAM_MAN_HH

#include <string>
#include <map>
#include <vector>

#include <globals.hh>
#include <G4ThreeVector.hh>
#include "TLorentzVector.h"
#include <Rtypes.h>
using namespace std;
class TFile;

//_____________________________________________________________________________
struct BeamInfo
{
  G4double      x; // [mm]
  G4double      y; // [mm]
  G4double      u; // [mrad] -> dxdz in k18-analyzer
  G4double      v; // [mrad]
  G4double      dp; // [%]
  G4ThreeVector p; // [GeV/c]
  G4double      z; // [mm]
  G4double      m2; // [GeV / c2]
  G4double      q; 
  G4int					trigpat[32];
	G4double GetX( G4double offset=0. ) const;
  G4double GetY( G4double offset=0. ) const;
	G4int GetTrigPat(G4int flag) const;
  void     Print( void ) const;
	G4int evnum;
	G4int runnum;
	G4int ntBeam;
};
struct MMVertex
{
  G4double      x; // [mm]
  G4double      y; // [mm]
  G4double      z; // [mm]
	vector<G4ThreeVector> Moms;
	vector<G4double> q;
  G4int					trigpat[32];
	G4int GetTrigPat(G4int flag) const;
	G4int evnum;
	G4int Bh2Seg[8];
	G4int SchSeg[64];
	G4int TofSeg[24];
	
	G4int ntK18;
	vector<vector<double>>xvpHS;
	vector<vector<double>>yvpHS;
	vector<vector<double>>zvpHS;
	vector<double>xtgtHS;
	vector<double>ytgtHS;
	vector<double>ztgtHS;
	vector<double>xoutK18;
	vector<double>youtK18;
	vector<double>uoutK18;
	vector<double>voutK18;
	vector<double>p_3rd;
	vector<vector<double>> layerK18;
	vector<vector<double>> wireK18;
	vector<vector<double>> localhitposK18;
	G4int ntKurama;
	vector<vector<double>>xvpKurama;
	vector<vector<double>>yvpKurama;
	vector<vector<double>>zvpKurama;
	vector<double>xtgtKurama;
	vector<double>ytgtKurama;
	vector<double>xout;
	vector<double>yout;
	vector<double>zout;
	vector<double>pxout;
	vector<double>pyout;
	vector<double>pzout;
	vector<vector<double>> layer;
	vector<vector<double>> wire;
	vector<vector<double>> localhitpos;
};
struct KpKurama
{
	G4int ntKurama;
	vector<vector<double>>xvpKurama;
	vector<vector<double>>yvpKurama;
	vector<vector<double>>zvpKurama;
	vector<double>xtgtKurama;
	vector<double>ytgtKurama;
	vector<double>xout;
	vector<double>yout;
	vector<double>zout;
	vector<double>pxout;
	vector<double>pyout;
	vector<double>pzout;
	vector<vector<double>> layer;
	vector<vector<double>> wire;
	vector<vector<double>> localhitpos;
};

//_____________________________________________________________________________
class BeamMan
{
public:
  static G4String ClassName( void );
  static BeamMan& GetInstance( void );
  ~BeamMan( void );

private:
  BeamMan( void );
  BeamMan( const BeamMan&  );
  BeamMan& operator =( const BeamMan& );

private:
  typedef std::vector<BeamInfo> ParamArray;
  typedef std::vector<MMVertex> MMArray;
  G4bool        m_is_ready;
  G4String      m_file_name;
  TFile*        m_file;
  ParamArray    m_param_array;
  MMArray    		m_mm_array;
  G4int         m_n_param;
  G4bool        m_is_vi; // true:VI or false:VO
  G4bool        m_is_k18=0;
  G4bool        m_is_kurama=0;
  G4bool        m_is_missmassXi=0;
  G4double      m_primary_z; // from VI or VO
  G4double      m_target_z;
  G4ThreeVector m_vi_pos;

public:
  const BeamInfo&      Get( void ) const;
  const BeamInfo&      Get( G4int iev ) const;
  const MMVertex&      GetVertex( void ) const;
  const MMVertex&      GetVertex( G4int iev ) const;
  G4double             GetPrimaryZ( void ) const { return m_primary_z; }
  const G4ThreeVector& GetVIPosition( void ) const { return m_vi_pos; }
  G4bool               Initialize( void );
  G4bool               Initialize( const G4String& filename );
  G4bool               IsReady( void ) const { return m_is_ready; }
  G4bool               IsK18( void ) const { return m_is_k18; }
  G4bool               IsKurama( void ) const { return m_is_kurama; }
  G4bool               IsMissMassXi( void ) const { return m_is_missmassXi; }
  void                 Print( void ) const;
  void                 SetPrimaryZ( G4double z ){ m_primary_z = z; }
  void                 SetVIPosition( G4ThreeVector pos ){ m_vi_pos = pos; }
};

//_____________________________________________________________________________
inline G4String
BeamMan::ClassName( void )
{
  static G4String s_name("BeamMan");
  return s_name;
}

//_____________________________________________________________________________
inline BeamMan&
BeamMan::GetInstance( void )
{
  static BeamMan s_instance;
  return s_instance;
}

#endif
