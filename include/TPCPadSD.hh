// -*- C++ -*-

#ifndef TPC_PAD_SD_HH
#define TPC_PAD_SD_HH

#include <G4VSensitiveDetector.hh>

#include "TPCPadHit.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

//_____________________________________________________________________________
class TPCPadSD : public G4VSensitiveDetector
{
public:
  static G4String ClassName( void );
  TPCPadSD( const G4String& name );
  virtual ~TPCPadSD( void );

private:
  G4THitsCollection<TPCPadHit>* hitsCollection;
  G4double select_plane;
  G4int num_plane;
  G4int num_deadarea;
  G4int select_dead;
  G4double m_target_z;
  G4double m_center_z;

  G4int m_gem_discharge;
  G4int m_gem_fix_dead;
  G4int m_gem_dead_plane;
  G4int m_gem_dead_plane_division;
  G4double m_dead_area;

	G4double DensityEffectCorrection(G4double betagamma, G4double* par);
	G4double TPCdEdx(G4double mass, G4double beta);
	G4double TPCdEdxSig(G4double mass, G4double mom);
  G4int    TPCClusterSize(G4double mass, G4double mom, G4int layer);

public:
  G4int ntrk;
  virtual G4bool ProcessHits( G4Step* aStep, G4TouchableHistory* ROhist );
  virtual void   Initialize( G4HCofThisEvent* HCTE );
  virtual void   EndOfEvent( G4HCofThisEvent* HCTE );
  virtual void   DrawAll( void );
  virtual void   PrintAll( void );
};

//_____________________________________________________________________________
inline G4String
TPCPadSD::ClassName( void )
{
  static G4String s_name("TPCPadSD");
  return s_name;
}
#endif
