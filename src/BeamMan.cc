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
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include "ConfMan.hh"
#include "DCGeomMan.hh"
#include "FuncName.hh"
#include "PrintHelper.hh"

//_____________________________________________________________________________
namespace{
	int ntBeam,evnum,runnum;
	double xout[5];
	double yout[5];
	double uout[5];
	double vout[5];
	double pBeam[5];
	double qBeam[5];
	double m2Beam[5];
	int trigpat[32];
	int ntK18,ntKurama;
  TTreeReaderValue<vector<double>>* pHS=nullptr;
  TTreeReaderValue<vector<double>>* utgtHS=nullptr;
  TTreeReaderValue<vector<double>>* vtgtHS=nullptr;
  TTreeReaderValue<vector<double>>* pTPCKurama=nullptr;
  TTreeReaderValue<vector<double>>* qTPCKurama=nullptr;
  TTreeReaderValue<vector<double>>* m2TPCKurama=nullptr;
  TTreeReaderValue<vector<double>>* utgtTPCKurama=nullptr;
  TTreeReaderValue<vector<double>>* vtgtTPCKurama=nullptr;
}
G4double
BeamInfo::GetX( G4double offset ) const
{
  return x + std::tan( -1.*u*CLHEP::mrad )*offset;
}

//_____________________________________________________________________________
G4double
BeamInfo::GetY( G4double offset ) const
{
  return y + std::tan( -1.*v*CLHEP::mrad )*offset;
}

//_____________________________________________________________________________
G4int 
BeamInfo::GetTrigPat(G4int flag) const
{
	return trigpat[flag];
}

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

  if( m_file_name.isNull() )
    return true;
  
	m_param_array.clear();
	m_mm_array.clear();
  G4int generator = gConf.Get<G4int>( "Generator" );
	m_is_vi = ( gConf.Get<G4int>( "Generator" ) == 10 );
	if( abs(generator) == 135 or abs(generator) == 493 or abs(generator) ==938 ){
		m_is_k18 = 1;
		G4cout<<"Generating K18 Beam"<<G4endl;
	}
	if( abs(generator) == 100 ){
		m_is_kurama = 1;
	}
	if(	generator == 181321 ){
		m_is_missmassXi = 1;
	}
	G4cout<<"Hello"<<G4endl;
  m_primary_z = gGeom.GetLocalZ( "Vertex" );
  m_target_z = gGeom.GetLocalZ( "SHSTarget" );
  if( !m_is_vi )
    m_primary_z -= 1318.9*CLHEP::mm; // from VO
	TTree* tree = nullptr;
	TTreeReader* reader = nullptr;
	m_file = new TFile( m_file_name );
	if(m_is_k18){
		tree = (TTree*) m_file->Get( "k18track" );
	}
	else if (m_is_kurama){
		tree = (TTree*) m_file->Get( "kurama" );
	}
	else if(m_is_missmassXi){
		tree = (TTree*)  m_file->Get( "tpc"  );
	}
	else{
		tree = (TTree*) m_file->Get("tree");
	}
	cout<<tree->GetEntries()<<endl;

  if( !m_file->IsOpen() || !tree )
    return false;
	BeamInfo beam;
	if(m_is_k18){
		tree->SetBranchAddress( "ntK18",&ntBeam);
		tree->SetBranchAddress( "evnum",&evnum);
		tree->SetBranchAddress( "runnum",&runnum);
		tree->SetBranchAddress( "xout",xout);
		tree->SetBranchAddress( "yout",yout);
		tree->SetBranchAddress( "uout",uout);
		tree->SetBranchAddress( "vout",vout);
		tree->SetBranchAddress( "pHS",pBeam);
		tree->SetBranchAddress( "trigpat",trigpat);
	}
	else if (m_is_kurama){
		tree->SetBranchAddress( "ntKurama",&ntBeam);
		tree->SetBranchAddress( "evnum",&evnum);
		tree->SetBranchAddress( "runnum",&runnum);
		tree->SetBranchAddress( "xtgtKurama",xout);
		tree->SetBranchAddress( "ytgtKurama",yout);
		tree->SetBranchAddress( "utgtKurama",uout);
		tree->SetBranchAddress( "vtgtKurama",vout);
		tree->SetBranchAddress( "pKurama",pBeam);
		tree->SetBranchAddress( "qKurama",qBeam);
		tree->SetBranchAddress( "m2",m2Beam);
		tree->SetBranchAddress( "trigpat",trigpat);
	}
	else if (m_is_missmassXi){
		reader = new TTreeReader("tpc",m_file);
		tree->SetBranchAddress( "evnum",&evnum);
		tree->SetBranchAddress( "runnum",&runnum);
		tree->SetBranchAddress("ntK18",&ntK18);
		
		pHS = new TTreeReaderValue<vector<double>>(*reader,"pHS"); 
		utgtHS = new TTreeReaderValue<vector<double>>(*reader,"utgtHS"); 
		vtgtHS = new TTreeReaderValue<vector<double>>(*reader,"vtgtHS"); 
		
		tree->SetBranchAddress("ntKurama",&ntKurama);
		pTPCKurama = new TTreeReaderValue<vector<double>>(*reader,"pTPCKurama"); 
		qTPCKurama = new TTreeReaderValue<vector<double>>(*reader,"qTPCKurama"); 
		utgtTPCKurama = new TTreeReaderValue<vector<double>>(*reader,"utgtTPCKurama"); 
		vtgtTPCKurama = new TTreeReaderValue<vector<double>>(*reader,"vtgtTPCKurama"); 
		m2TPCKurama = new TTreeReaderValue<vector<double>>(*reader,"m2TPCKurama"); 
	}
	else{
		tree->SetBranchAddress( "x", &beam.x );
		tree->SetBranchAddress( "y", &beam.y );
		tree->SetBranchAddress( "u", &beam.u );
		tree->SetBranchAddress( "v", &beam.v );
		tree->SetBranchAddress( "p", &beam.dp );
	}
	std::cout<<"BeamEvents = "<<tree->GetEntries()<<std::endl;
  for( Long64_t i=0, n=tree->GetEntries(); i<n; ++i ){
    tree->GetEntry( i );
		if(i%100000==0)G4cout<<Form("Event %lld/%lld",i,tree->GetEntries())<<G4endl;
    if(reader)reader->Next();
		if(m_is_k18 or m_is_kurama){
			beam.x=0;
			beam.y=0;
			beam.z=0;
			beam.u=0;
			beam.v=0;
			beam.p.set(0,0,0);
			beam.evnum = evnum;
			beam.runnum = runnum;
			beam.ntBeam = ntBeam;
			for(int it=0;it<ntBeam;++it){
				beam.x = xout[0];
				beam.y = yout[0];
				beam.u = uout[0];// u,v definition = dxdz,dydz, not mrad. 
				beam.v = yout[0];
				double pz = pBeam[0] / sqrt(uout[0]*uout[0]+vout[0]*vout[0]+1);
				beam.p.set(pz * uout[0],pz* vout[0], pz);
				if(m_is_kurama){
					beam.z = m_target_z;
					beam.m2 = m2Beam[0];
					beam.q = qBeam[0];
				}
				else{
					beam.z = m_primary_z;//VO
				}
			}
			for(int itrg=0;itrg<32;++itrg){
				beam.trigpat[itrg] = trigpat[itrg];
			}
			m_param_array.push_back( beam );
		}
		else if(m_is_missmassXi){
			for(int itk18=0;itk18<ntK18;++itk18){
				double ub = (*utgtHS)->at(itk18);
				double vb = (*vtgtHS)->at(itk18);
				double nb = hypot(hypot(1,ub),vb);
				double pb = (*pHS)->at(itk18);
				double pzb = pb/nb;
				G4ThreeVector TVKm(pzb*ub,pzb*vb,pzb);
			for(int itkurama=0;itkurama<ntKurama;++itkurama){
				double qKp = (*qTPCKurama)->at(itkurama);
				double m2Kp = (*m2TPCKurama)->at(itkurama);
				double pKp = (*pTPCKurama)->at(itkurama);
				
				double us = (*utgtTPCKurama)->at(itkurama);
				double vs = (*vtgtTPCKurama)->at(itkurama);
				double ns = hypot(hypot(1,us),vs);
				double pzs = pKp/ns;
				G4ThreeVector TVKp(pzs*us,pzs*vs,pzs);

				if(pKp > 1.1 and pKp < 1.4 and qKp > 0 and m2Kp > 0.12 and m2Kp < 0.3){
					MMVertex MMVert;
					MMVert.Moms.push_back(TVKm);
					MMVert.Moms.push_back(TVKp);
					MMVert.Moms.push_back(TVKm-TVKp);
					m_mm_array.push_back(MMVert);		
				}
			}
			}
		}
		else{
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

const BeamInfo&
BeamMan::Get( G4int iev ) const
{
	auto b =	m_param_array.at(iev);
	return m_param_array.at( iev%m_n_param );
}

const MMVertex&
BeamMan::GetVertex( void ) const
{
 	int nev = m_mm_array.size(); 
	return m_mm_array.at( G4RandFlat::shootInt( nev ) );
}

const MMVertex&
BeamMan::GetVertex( G4int iev ) const
{
 	int nev = m_mm_array.size(); 
	return m_mm_array.at( iev%nev );
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
