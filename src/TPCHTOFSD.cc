// -*- C++ -*-

#include "TPCHTOFSD.hh"

#include <G4Step.hh>
#include <G4TouchableHistory.hh>
#include <G4Track.hh>
#include <G4VPhysicalVolume.hh>
#include <G4VTouchable.hh>

#include "FuncName.hh"
#include "TPCHTOFHit.hh"

//_____________________________________________________________________________
TPCHTOFSD::TPCHTOFSD( const G4String& name )
  : G4VSensitiveDetector( name ),
    m_hits_collection()
{
  collectionName.insert("hit");
}

//_____________________________________________________________________________
TPCHTOFSD::~TPCHTOFSD( void )
{
}

//_____________________________________________________________________________
void
TPCHTOFSD::Initialize( G4HCofThisEvent* HCTE )
{
  m_hits_collection = new G4THitsCollection<TPCHTOFHit>( SensitiveDetectorName,
							 collectionName[0] );
  HCTE->AddHitsCollection( GetCollectionID(0), m_hits_collection );
}

//_____________________________________________________________________________
G4bool
TPCHTOFSD::ProcessHits( G4Step* aStep, G4TouchableHistory* /* ROhist */ )
{
  const auto preStepPoint = aStep->GetPreStepPoint();
  const auto aTrack = aStep->GetTrack();
  const auto Definition = aTrack->GetDefinition();
  const G4String particleName = Definition->GetParticleName();
  const G4String particleType = Definition->GetParticleType();

  if( preStepPoint->GetStepStatus() != fGeomBoundary )
    return false;
  if( Definition->GetPDGCharge() == 0. )
    return false;

  

  m_hits_collection->insert( new TPCHTOFHit( SensitiveDetectorName, aStep ) );

  return true;
}

//_____________________________________________________________________________
void
TPCHTOFSD::EndOfEvent( G4HCofThisEvent* /* HCTE */ )
{
}

//_____________________________________________________________________________
void
TPCHTOFSD::DrawAll( void )
{
}

//_____________________________________________________________________________
void
TPCHTOFSD::PrintAll( void )
{
  m_hits_collection->PrintAllHits();
}
