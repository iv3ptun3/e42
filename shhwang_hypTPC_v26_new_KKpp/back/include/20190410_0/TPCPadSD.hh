// ====================================================================
//   TPCPadSD.hh
//
// ====================================================================
#ifndef TPC_PAD_SD_H
#define TPC_PAD_SD_H
 
#include "G4VSensitiveDetector.hh"
#include "TPCPadHit.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class TPCPadSD : public G4VSensitiveDetector {
private:
  G4THitsCollection<TPCPadHit>* hitsCollection;
  //  G4int pidtr[50];
  //  G4int nparticle;
  G4double select_plane;
  G4int num_plane;
  G4double deadarea;
  G4int num_deadarea;
  G4int select_dead;

  G4int env_gemdischarge;
  G4int env_gemfixdead;
  G4int env_gemdeadplane;
  G4int env_gemdeadplanedivision;
  G4double env_deadarea;

public:
  G4int ntrk;
  TPCPadSD(const G4String& name);
  virtual ~TPCPadSD();

  // virtual methods
  virtual G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);
  virtual void Initialize(G4HCofThisEvent* HCTE);
  virtual void EndOfEvent(G4HCofThisEvent* HCTE);

  virtual void DrawAll();
  virtual void PrintAll(); 
  void TPCPadSD_Set(); 
 
};

#endif
