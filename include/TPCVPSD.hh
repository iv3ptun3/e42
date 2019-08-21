// -*- C++ -*-

#ifndef TPC_VP_SD_HH
#define TPC_VP_SD_HH

#include <G4VSensitiveDetector.hh>

#include "TPCVPHit.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

//_____________________________________________________________________________
class TPCVPSD : public G4VSensitiveDetector
{
public:
  static G4String ClassName( void );
  TPCVPSD( const G4String& name );
  virtual ~TPCVPSD( void );

private:
  G4THitsCollection<TPCVPHit>* m_hits_collection;

public:
  virtual G4bool ProcessHits( G4Step* aStep, G4TouchableHistory* ROhist );
  virtual void   Initialize( G4HCofThisEvent* HCTE );
  virtual void   EndOfEvent( G4HCofThisEvent* HCTE );
  virtual void   DrawAll( void );
  virtual void   PrintAll( void );
};

//_____________________________________________________________________________
inline G4String
TPCVPSD::ClassName( void )
{
  static G4String s_name("TPCVPSD");
  return s_name;
}

#endif
