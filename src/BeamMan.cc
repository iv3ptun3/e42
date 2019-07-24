// -*- C++ -*-

#include "BeamMan.hh"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ThreeVector.hh>
#include <Randomize.hh>

#include <TFile.h>
#include <TTree.h>

#include "ConfMan.hh"
#include "DCGeomMan.hh"
#include "FuncName.hh"
#include "PrintHelper.hh"

//_____________________________________________________________________________
void
BeamInfo::Print( void ) const
{
  PrintHelper helper( 4, std::ios::fixed, G4cout );
  const G4int w = 8;
  G4cout << "   "
	 << "x=" << std::setw(w) << x << " "
	 << "y=" << std::setw(w) << y << " "
	 << "u=" << std::setw(w) << u << " "
	 << "v=" << std::setw(w) << v << " "
	 << "p=(" << std::setw(w) << p.x() << ", "
	 << std::setw(w) << p.y() << ", "
	 << std::setw(w) << p.z() << ")" << G4endl;
}

//_____________________________________________________________________________
BeamMan::BeamMan( void )
  : m_is_ready( false ),
    m_file_name(),
    m_file(),
    m_param_array(),
    m_n_param(),
    m_is_vi( false ),
    m_primary_z( 0. )
{
}

//_____________________________________________________________________________
BeamMan::~BeamMan( void )
{
}

//_____________________________________________________________________________
G4bool
BeamMan::Initialize( void )
{
  const auto& gConf = ConfMan::GetInstance();
  const auto& gGeom = DCGeomMan::GetInstance();
  const G4double p0 = gConf.Get<G4double>( "BeamMom" );

  m_file = new TFile( m_file_name );
  TTree* tree = dynamic_cast<TTree*>( m_file->Get( "tree" ) );

  if( !m_file->IsOpen() || !tree )
    return false;

  m_param_array.clear();
  m_is_vi = ( gConf.Get<G4int>( "Generator" ) == 10 );
  m_primary_z = gGeom.GetLocalZ( "Vertex" );
  if( !m_is_vi )
    m_primary_z -= 1200.*CLHEP::mm; // from VO
  BeamInfo beam;
  tree->SetBranchAddress( "x", &beam.x );
  tree->SetBranchAddress( "y", &beam.y );
  tree->SetBranchAddress( "u", &beam.u );
  tree->SetBranchAddress( "v", &beam.v );
  tree->SetBranchAddress( "p", &beam.dp );

  for( Long64_t i=0, n=tree->GetEntries(); i<n; ++i ){
    tree->GetEntry( i );
    beam.x *= -1.*CLHEP::cm; // -cm -> mm
    beam.y *= -1.*CLHEP::cm; // -cm -> mm
    G4double dxdz = std::tan( -1.*beam.u*CLHEP::mrad ); // -mrad -> tan
    G4double dydz = std::tan( -1.*beam.v*CLHEP::mrad ); // -mrad -> tan
    G4double pp = p0 * ( 1. + beam.dp*CLHEP::perCent ); // dp/p[%] -> GeV/c
    G4double pz = pp / std::sqrt( dxdz*dxdz + dydz*dydz + 1. );
    beam.x += dxdz * m_primary_z;
    beam.y += dydz * m_primary_z;
    beam.z = m_primary_z;
    beam.p.set( pz*dxdz, pz*dydz, pz );
    m_param_array.push_back( beam );
  }

  m_file->Close();
  m_n_param = m_param_array.size();
  m_is_ready = true;
  return true;
}

//_____________________________________________________________________________
G4bool
BeamMan::Initialize( const G4String& filename )
{
  m_file_name = filename;
  return Initialize();
}

//_____________________________________________________________________________
const BeamInfo&
BeamMan::Get( void ) const
{
  return m_param_array.at( G4RandFlat::shootInt( m_n_param ) );
}

//_____________________________________________________________________________
void
BeamMan::Print( void ) const
{
  PrintHelper helper( 4, std::ios::fixed, G4cout );
  const G4int w = 8;

  G4cout << FUNC_NAME << G4endl;
  for( const auto& b : m_param_array ){
    G4cout << "   "
	   << "x=" << std::setw(w) << b.x << " "
	   << "y=" << std::setw(w) << b.y << " "
	   << "u=" << std::setw(w) << b.u << " "
	   << "v=" << std::setw(w) << b.v << " "
	   << "p=(" << std::setw(w) << b.p.x() << ", "
	   << std::setw(w) << b.p.y() << ", "
	   << std::setw(w) << b.p.z() << ")" << G4endl;
  }
  G4cout << "   nparam = " << m_param_array.size() << G4endl;
}
