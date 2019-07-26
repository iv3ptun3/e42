// -*- C++ -*-

#include "TPCAnaManager.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include <G4ThreeVector.hh>
#include <Randomize.hh>

#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TTree.h>

#include "ConfMan.hh"
#include "FuncName.hh"
#include "ResHypTPC.hh"
#include "RungeKuttaTracker.hh"
#include "minuit2.hh"
#include "switch.h"
#include "track.hh"
#include "VHitInfo.hh"

namespace
{
  const ConfMan& gConf = ConfMan::GetInstance();
  TTree* tree;
  Event event;
  std::map<TString, TH1*> hmap;
}

//_____________________________________________________________________________
TPCAnaManager::TPCAnaManager( void )
{
  tree = new TTree( "tree", "GEANT4 simulation for HypTPC" );
  event.pb = new TVector3;
  tree->Branch( "evnum", &event.evnum, "evnum/I" );
  tree->Branch( "pb", "TVector3", event.pb );
  tree->Branch( "nhPrm", &event.nhPrm, "nhPrm/I" );
  tree->Branch( "pidPrm", event.pidPrm, "pidPrm[nhPrm]/I" );
  tree->Branch( "xPrm", event.xPrm, "xPrm[nhPrm]/D" );
  tree->Branch( "yPrm", event.yPrm, "yPrm[nhPrm]/D" );
  tree->Branch( "zPrm", event.zPrm, "zPrm[nhPrm]/D" );
  tree->Branch( "pxPrm", event.pxPrm, "pxPrm[nhPrm]/D" );
  tree->Branch( "pyPrm", event.pyPrm, "pyPrm[nhPrm]/D" );
  tree->Branch( "pzPrm", event.pzPrm, "pzPrm[nhPrm]/D" );
  tree->Branch( "ptPrm", event.ptPrm, "ptPrm[nhPrm]/D" );
  tree->Branch( "mPrm", event.mPrm, "mPrm[nhPrm]/D" );
  tree->Branch( "thetaPrm", event.thetaPrm, "thetaPrm[nhPrm]/D" );

  tree->Branch("mm_d",&event.mm_d,"mm_d/D");
  // tree->Branch("mm_p",&event.mm_p,"mm_p/D");
  tree->Branch("theta",&event.theta,"theta/D");
  tree->Branch("theta_scat",&event.theta_scat,"theta_scat/D");
  tree->Branch("theta_CM",&event.theta_CM,"theta_CM/D");

  // tree->Branch("mm",&event.mm,"mm/D");

  //generator mode
  tree->Branch("gen",&event.gen,"gen/I");
  tree->Branch("mode",&event.mode,"mode/I");

  ///////shhwang tpc hit step

  // tree->Branch("nttpc",&event.nttpc,"nttpc/I");
  // tree->Branch("ntrk",event.ntrk,"ntrk[nttpc]/I");
  // tree->Branch("ititpc",event.ititpc,"ititpc[nttpc]/I");
  // tree->Branch("idtpc",event.idtpc,"idtpc[nttpc]/I");
  // tree->Branch("xtpc",event.xtpc,"xtpc[nttpc]/D");
  // tree->Branch("ytpc",event.ytpc,"ytpc[nttpc]/D");
  // tree->Branch("ztpc",event.ztpc,"ztpc[nttpc]/D");
  // tree->Branch("x0tpc",event.x0tpc,"x0tpc[nttpc]/D");
  // tree->Branch("y0tpc",event.y0tpc,"y0tpc[nttpc]/D");
  // tree->Branch("z0tpc",event.z0tpc,"z0tpc[nttpc]/D");
  // tree->Branch("resoX",event.resoX,"resoX[nttpc]/D");
  // tree->Branch("pxtpc",event.pxtpc,"pxtpc[nttpc]/D");
  // tree->Branch("pytpc",event.pytpc,"pytpc[nttpc]/D");
  // tree->Branch("pztpc",event.pztpc,"pztpc[nttpc]/D");
  // tree->Branch("pptpc",event.pptpc,"pptpc[nttpc]/D");   // total mometum
  // tree->Branch("masstpc",event.masstpc,"masstpc[nttpc]/D");   // mass TPC
  // //  tree->Branch("betatpc",event.betatpc,"betatpc[nttpc]/D");
  // // tree->Branch("edeptpc",event.edeptpc,"edeptpc[nttpc]/D");
  // //tree->Branch("dedxtpc",event.dedxtpc,"dedxtpc[nttpc]/D");
  // //tree->Branch("slengthtpc",event.slengthtpc,"slengthtpc[nttpc]/D");
  // tree->Branch("laytpc",event.laytpc,"laytpc[nttpc]/I");
  // tree->Branch("rowtpc",event.rowtpc,"rowtpc[nttpc]/I");
  // tree->Branch("parentID",event.parentID,"parentID[nttpc]/I");

  //// study on multiplicity
  // tree->Branch("nthlay",event.nthlay,"nthlay[nttpc]/I");
  // tree->Branch("nthpad",event.nthpad,"nthpad[nttpc]/I");
  // tree->Branch("laypad",event.laypad,"laytpadpc[nttpc][40][250]/I");


  //shhwang ntrtpc --> number of trak in tpc
  tree->Branch("ntrtpc",&event.ntrtpc,"ntrtpc/I");
  tree->Branch("trpmtpc",event.trpmtpc,"trpmtpc[ntrtpc]/D");
  tree->Branch("trqqtpc",event.trqqtpc,"trqqtpc[ntrtpc]/I");
  tree->Branch("trpidtpc",event.trpidtpc,"trpidtpc[ntrtpc]/I");
  tree->Branch("trparentidtpc",event.trparentidtpc,"trparentidtpc[ntrtpc]/I");
  //tree->Branch("trparentid_pid_tpc",event.trparentid_pid_tpc,"trparentid_pid_tpc[ntrtpc]/I");

  tree->Branch("trpxtpc",event.trpxtpc,"trpxtpc[ntrtpc]/D");
  tree->Branch("trpytpc",event.trpytpc,"trpytpc[ntrtpc]/D");
  tree->Branch("trpztpc",event.trpztpc,"trpztpc[ntrtpc]/D");
  tree->Branch("trpptpc",event.trpptpc,"trpptpc[ntrtpc]/D");
  tree->Branch("trpttpc",event.trpttpc,"trpttpc[ntrtpc]/D");

  tree->Branch("trpxtpcfit",event.trpxtpcfit,"trpxtpcfit[ntrtpc]/D");
  tree->Branch("trpytpcfit",event.trpytpcfit,"trpytpcfit[ntrtpc]/D");
  tree->Branch("trpztpcfit",event.trpztpcfit,"trpztpcfit[ntrtpc]/D");
  tree->Branch("trpptpcfit",event.trpptpcfit,"trpptpcfit[ntrtpc]/D");
  tree->Branch("trpttpcfit",event.trpttpcfit,"trpttpcfit[ntrtpc]/D");

  tree->Branch("vtpxtpc",event.vtpxtpc,"vtpxtpc[ntrtpc]/D");
  tree->Branch("vtpytpc",event.vtpytpc,"vtpytpc[ntrtpc]/D");
  tree->Branch("vtpztpc",event.vtpztpc,"vtpztpc[ntrtpc]/D");
  tree->Branch("vtpptpc",event.vtpptpc,"vtpptpc[ntrtpc]/D");

  tree->Branch("vtxtpc",event.vtxtpc,"vtxtpc[ntrtpc]/D");
  tree->Branch("vtytpc",event.vtytpc,"vtytpc[ntrtpc]/D");
  tree->Branch("vtztpc",event.vtztpc,"vtztpc[ntrtpc]/D");

  tree->Branch("vtxtpcfit",event.vtxtpcfit,"vtxtpcfit[ntrtpc]/D");
  tree->Branch("vtytpcfit",event.vtytpcfit,"vtytpcfit[ntrtpc]/D");
  tree->Branch("vtztpcfit",event.vtztpcfit,"vtztpcfit[ntrtpc]/D");

  // tree->Branch("trdetpc",event.trdetpc,"trdetpc[ntrtpc]/D");
  // tree->Branch("trlentpc",event.trlentpc,"trlentpc[ntrtpc]/D");
  // tree->Branch("trdedxtpc",event.trdedxtpc,"trdedxtpc[ntrtpc]/D");
  // tree->Branch("trdedxtrtpc",event.trdedxtrtpc,"trdedxtrtpc[ntrtpc]/D");
  tree->Branch("trlaytpc",event.trlaytpc,"trlaytpc[ntrtpc]/I");
  tree->Branch("cir_r",event.cir_r,"cir_r[ntrtpc]/D");
  tree->Branch("cir_x",event.cir_x,"cir_x[ntrtpc]/D");
  tree->Branch("cir_z",event.cir_z,"cir_z[ntrtpc]/D");
  tree->Branch("cir_fit",event.cir_fit,"cir_fit[ntrtpc]/D");
  tree->Branch("vtx_flag",event.vtx_flag,"vtx_flag[ntrtpc]/I");
  tree->Branch("a_fory",event.a_fory,"a_fory[ntrtpc]/D");
  tree->Branch("b_fory",event.b_fory,"b_fory[ntrtpc]/D");

  // TARGET
  tree->Branch( "nhTgt", &event.nhTgt, "nhTgt/I" );
  tree->Branch( "tidTgt", event.tidTgt, "tidTgt[nhTgt]/I" );
  tree->Branch( "pidTgt", event.pidTgt, "pidTgt[nhTgt]/I" );
  tree->Branch( "prtTgt", event.prtTgt, "prtTgt[nhTgt]/I" );
  tree->Branch( "xTgt", event.xTgt, "xTgt[nhTgt]/D" );
  tree->Branch( "yTgt", event.yTgt, "yTgt[nhTgt]/D" );
  tree->Branch( "zTgt", event.zTgt, "zTgt[nhTgt]/D" );
  tree->Branch( "vtxTgt", event.vtxTgt, "vtxTgt[nhTgt]/D" );
  tree->Branch( "vtyTgt", event.vtyTgt, "vtyTgt[nhTgt]/D" );
  tree->Branch( "vtzTgt", event.vtzTgt, "vtzTgt[nhTgt]/D" );
  // HTOF
  tree->Branch( "nhHtof", &event.nhHtof, "nhHtof/I" );
  tree->Branch( "tidHtof", event.tidHtof, "tidHtof[nhHtof]/I" );
  tree->Branch( "pidHtof", event.pidHtof, "pidHtof[nhHtof]/I" );
  tree->Branch( "didHtof", event.didHtof, "didHtof[nhHtof]/I" );
  tree->Branch( "prtHtof", event.prtHtof, "prtHtof[nhHtof]/I" );
  tree->Branch( "qHtof", event.qHtof, "qHtof[nhHtof]/I" );
  tree->Branch( "massHtof", event.massHtof, "massHtof[nhHtof]/D" );
  tree->Branch( "xHtof", event.xHtof, "xHtof[nhHtof]/D" );
  tree->Branch( "yHtof", event.yHtof, "yHtof[nhHtof]/D" );
  tree->Branch( "zHtof", event.zHtof, "zHtof[nhHtof]/D" );
  tree->Branch( "pxHtof", event.pxHtof, "pxHtof[nhHtof]/D" );
  tree->Branch( "pyHtof", event.pyHtof, "pyHtof[nhHtof]/D" );
  tree->Branch( "pzHtof", event.pzHtof, "pzHtof[nhHtof]/D" );
  tree->Branch( "ppHtof", event.ppHtof, "ppHtof[nhHtof]/D" );
  tree->Branch( "tofHtof", event.tofHtof, "tofHtof[nhHtof]/D" );
  tree->Branch( "vtpxHtof", event.vtpxHtof, "vtpxHtof[nhHtof]/D" );
  tree->Branch( "vtpyHtof", event.vtpyHtof, "vtpyHtof[nhHtof]/D" );
  tree->Branch( "vtpzHtof", event.vtpzHtof, "vtpzHtof[nhHtof]/D" );
  tree->Branch( "vtppHtof", event.vtppHtof, "vtppHtof[nhHtof]/D" );
  tree->Branch( "vtxHtof", event.vtxHtof, "vtxHtof[nhHtof]/D" );
  tree->Branch( "vtyHtof", event.vtyHtof, "vtyHtof[nhHtof]/D" );
  tree->Branch( "vtzHtof", event.vtzHtof, "vtzHtof[nhHtof]/D" );
  tree->Branch( "lengthHtof", event.lengthHtof, "lengthHtof[nhHtof]/D" );
  // SDC
  tree->Branch( "nhSdc", &event.nhSdc, "nhSdc/I" );
  tree->Branch( "tidSdc", event.tidSdc, "tidSdc[nhSdc]/I" );
  tree->Branch( "pidSdc", event.pidSdc, "pidSdc[nhSdc]/I" );
  tree->Branch( "didSdc", event.didSdc, "didSdc[nhSdc]/I" );
  tree->Branch( "prtSdc", event.prtSdc, "prtSdc[nhSdc]/I" );
  tree->Branch( "qSdc", event.qSdc, "qSdc[nhSdc]/I" );
  tree->Branch( "massSdc", event.massSdc, "massSdc[nhSdc]/D" );
  tree->Branch( "xSdc", event.xSdc, "xSdc[nhSdc]/D" );
  tree->Branch( "ySdc", event.ySdc, "ySdc[nhSdc]/D" );
  tree->Branch( "zSdc", event.zSdc, "zSdc[nhSdc]/D" );
  tree->Branch( "pxSdc", event.pxSdc, "pxSdc[nhSdc]/D" );
  tree->Branch( "pySdc", event.pySdc, "pySdc[nhSdc]/D" );
  tree->Branch( "pzSdc", event.pzSdc, "pzSdc[nhSdc]/D" );
  tree->Branch( "ppSdc", event.ppSdc, "ppSdc[nhSdc]/D" );
  tree->Branch( "tofSdc", event.tofSdc, "tofSdc[nhSdc]/D" );
  tree->Branch( "vtpxSdc", event.vtpxSdc, "vtpxSdc[nhSdc]/D" );
  tree->Branch( "vtpySdc", event.vtpySdc, "vtpySdc[nhSdc]/D" );
  tree->Branch( "vtpzSdc", event.vtpzSdc, "vtpzSdc[nhSdc]/D" );
  tree->Branch( "vtppSdc", event.vtppSdc, "vtppSdc[nhSdc]/D" );
  tree->Branch( "vtxSdc", event.vtxSdc, "vtxSdc[nhSdc]/D" );
  tree->Branch( "vtySdc", event.vtySdc, "vtySdc[nhSdc]/D" );
  tree->Branch( "vtzSdc", event.vtzSdc, "vtzSdc[nhSdc]/D" );
  tree->Branch( "lengthSdc", event.lengthSdc, "lengthSdc[nhSdc]/D" );
  // SCH
  tree->Branch( "nhSch", &event.nhSch, "nhSch/I" );
  tree->Branch( "tidSch", event.tidSch, "tidSch[nhSch]/I" );
  tree->Branch( "pidSch", event.pidSch, "pidSch[nhSch]/I" );
  tree->Branch( "didSch", event.didSch, "didSch[nhSch]/I" );
  tree->Branch( "prtSch", event.prtSch, "prtSch[nhSch]/I" );
  tree->Branch( "qSch", event.qSch, "qSch[nhSch]/I" );
  tree->Branch( "massSch", event.massSch, "massSch[nhSch]/D" );
  tree->Branch( "xSch", event.xSch, "xSch[nhSch]/D" );
  tree->Branch( "ySch", event.ySch, "ySch[nhSch]/D" );
  tree->Branch( "zSch", event.zSch, "zSch[nhSch]/D" );
  tree->Branch( "pxSch", event.pxSch, "pxSch[nhSch]/D" );
  tree->Branch( "pySch", event.pySch, "pySch[nhSch]/D" );
  tree->Branch( "pzSch", event.pzSch, "pzSch[nhSch]/D" );
  tree->Branch( "ppSch", event.ppSch, "ppSch[nhSch]/D" );
  tree->Branch( "tofSch", event.tofSch, "tofSch[nhSch]/D" );
  tree->Branch( "vtpxSch", event.vtpxSch, "vtpxSch[nhSch]/D" );
  tree->Branch( "vtpySch", event.vtpySch, "vtpySch[nhSch]/D" );
  tree->Branch( "vtpzSch", event.vtpzSch, "vtpzSch[nhSch]/D" );
  tree->Branch( "vtppSch", event.vtppSch, "vtppSch[nhSch]/D" );
  tree->Branch( "vtxSch", event.vtxSch, "vtxSch[nhSch]/D" );
  tree->Branch( "vtySch", event.vtySch, "vtySch[nhSch]/D" );
  tree->Branch( "vtzSch", event.vtzSch, "vtzSch[nhSch]/D" );
  tree->Branch( "lengthSch", event.lengthSch, "lengthSch[nhSch]/D" );
  // FTOF
  tree->Branch( "nhFtof", &event.nhFtof, "nhFtof/I" );
  tree->Branch( "tidFtof", event.tidFtof, "tidFtof[nhFtof]/I" );
  tree->Branch( "pidFtof", event.pidFtof, "pidFtof[nhFtof]/I" );
  tree->Branch( "didFtof", event.didFtof, "didFtof[nhFtof]/I" );
  tree->Branch( "prtFtof", event.prtFtof, "prtFtof[nhFtof]/I" );
  tree->Branch( "qFtof", event.qFtof, "qFtof[nhFtof]/I" );
  tree->Branch( "massFtof", event.massFtof, "massFtof[nhFtof]/D" );
  tree->Branch( "xFtof", event.xFtof, "xFtof[nhFtof]/D" );
  tree->Branch( "yFtof", event.yFtof, "yFtof[nhFtof]/D" );
  tree->Branch( "zFtof", event.zFtof, "zFtof[nhFtof]/D" );
  tree->Branch( "pxFtof", event.pxFtof, "pxFtof[nhFtof]/D" );
  tree->Branch( "pyFtof", event.pyFtof, "pyFtof[nhFtof]/D" );
  tree->Branch( "pzFtof", event.pzFtof, "pzFtof[nhFtof]/D" );
  tree->Branch( "ppFtof", event.ppFtof, "ppFtof[nhFtof]/D" );
  tree->Branch( "tofFtof", event.tofFtof, "tofFtof[nhFtof]/D" );
  tree->Branch( "vtpxFtof", event.vtpxFtof, "vtpxFtof[nhFtof]/D" );
  tree->Branch( "vtpyFtof", event.vtpyFtof, "vtpyFtof[nhFtof]/D" );
  tree->Branch( "vtpzFtof", event.vtpzFtof, "vtpzFtof[nhFtof]/D" );
  tree->Branch( "vtppFtof", event.vtppFtof, "vtppFtof[nhFtof]/D" );
  tree->Branch( "vtxFtof", event.vtxFtof, "vtxFtof[nhFtof]/D" );
  tree->Branch( "vtyFtof", event.vtyFtof, "vtyFtof[nhFtof]/D" );
  tree->Branch( "vtzFtof", event.vtzFtof, "vtzFtof[nhFtof]/D" );
  tree->Branch( "lengthFtof", event.lengthFtof, "lengthFtof[nhFtof]/D" );
}

//_____________________________________________________________________________
TPCAnaManager::~TPCAnaManager( void )
{
}

//_____________________________________________________________________________
void
TPCAnaManager::BeginOfRunAction( G4int /* runnum */ )
{
  event.evnum = 0;

  G4double target_pos_z=-143.;
  truncated_mean_cut = gConf.Get<G4double>("TruncatedMeanCut");
  env_Experiment_num = gConf.Get<G4int>("Experiment");
  //out side less 100 mm. 10+5*x < 100 mm is pad_in_num
  pad_length_in = gConf.Get<G4double>("PadLengthIn");
  pad_length_out = gConf.Get<G4double>("PadLengthOut");
  pad_gap = gConf.Get<G4double>("PadGap");

  ////pad configure
  env_pad_config = gConf.Get<G4int>("PadConfigure");
  pad_in_num = gConf.Get<G4int>("PadNumIn");
  pad_out_num = gConf.Get<G4int>("PadNumOut");
  pad_in_width = gConf.Get<G4double>("PadWidthOut");
  pad_out_width = gConf.Get<G4double>("PadWidthOut");

  env_on_off_helm = gConf.Get<G4int>("ShsFieldMap");

  if( env_on_off_helm == 0 ){
    env_helm_field = gConf.Get<G4int>("ShsField");
  }else{
    G4cout << "Env of the Helmholt_fieldmap is wrong" << G4endl;
    exit(-1);
  }

  for(G4int i=0.;i<40;i++){
    angle[i]=0;
    seg_angle[i]=0;
    seg_width[i]=0;
    numpads[i]=0;

    pad_in[i]=0;
    pad_out[i]=0;
  }
  tpc_rad=250;
  G4double cen_diff=fabs(target_pos_z);

  if( env_pad_config ==1 ){
    for(G4int i=0;i<pad_in_num+pad_out_num;i++){
      if(i<pad_in_num){
	pad_in[i]=10.+(pad_length_in+pad_gap)*i;
	pad_out[i]=10.+(pad_length_in+pad_gap)*i+pad_length_in;
	angle[i]=360.;
      }else {
	pad_in[i]=10.+(pad_length_in+pad_gap)*pad_in_num+(pad_length_out+pad_gap)*(i-pad_in_num);
	pad_out[i]=10.+(pad_length_in+pad_gap)*pad_in_num+(pad_length_out+pad_gap)*(i-pad_in_num) + pad_length_out;
	angle[i]=180.-acos((pow(pad_out[i],2)+pow(cen_diff,2)-pow(tpc_rad,2))/(2*pad_out[i]*cen_diff))*180./acos(-1.);
      }
      //      G4cout<<angle[i]<<G4endl;
      //      G4cout<<pad_in[i]<<G4endl;
    }


  }else if( env_pad_config ==2 ){
    for(G4int i=0;i<pad_in_num+pad_out_num;i++){
      if(i<pad_in_num){
	pad_in[i]=10.+(pad_length_in+pad_gap)*i;
	pad_out[i]=10.+(pad_length_in+pad_gap)*i+pad_length_in;
	angle[i]=360.;
	if(i==0){
	  numpads[i]=48.;
	}else if(i<pad_in_num){
	  numpads[i]=24.*2.*(i+1.)/2.;
	}
      }else {
	pad_in[i]=10.+(pad_length_in+pad_gap)*pad_in_num+(pad_length_out+pad_gap)*(i-pad_in_num);
	pad_out[i]=10.+(pad_length_in+pad_gap)*pad_in_num+(pad_length_out+pad_gap)*(i-pad_in_num) + pad_length_out;
      }
    }
    angle[10]=180.-155.35;
    angle[11]=180.-144.8;
    angle[12]=180.-138.;
    angle[13]=180.-116.73;
    angle[14]=180.-106.;
    angle[15]=180.-98.77;
    angle[16]=180.-94.29;
    angle[17]=180.-89.8;
    angle[18]=180.-87.18;
    angle[19]=180.-84.16;
    angle[20]=180.-81.48;
    angle[21]=180.-73.39;
    angle[22]=180.-65.51011;
    angle[23]=180.-60.19;
    angle[24]=180.-56.35239;
    angle[25]=180.-52.85;
    angle[26]=180.-50.14;
    angle[27]=180.-47.17;
    angle[28]=180.-41.24;
    angle[29]=180.-29.;
    angle[30]=180.-23.23;
    angle[31]=180.-18.69;

    numpads[10]=208.;
    numpads[11]=218.;
    numpads[12]=230.;
    numpads[13]=214.;
    numpads[14]=212.;
    numpads[15]=214.;
    numpads[16]=220.;
    numpads[17]=224.;
    numpads[18]=232.;
    numpads[19]=238.;
    numpads[20]=244.;
    numpads[21]=232.;
    numpads[22]=218.;
    numpads[23]=210.;
    numpads[24]=206.;
    numpads[25]=202.;
    numpads[26]=200.;
    numpads[27]=196.;
    numpads[28]=178.;
    numpads[29]=130.;
    numpads[30]=108.;
    numpads[31]=90.;
    G4int all_channels=0;
    G4int all_channels2=0;
    G4int num_pad_check=0;


    for(G4int i=0;i<pad_in_num+pad_out_num;i++){
      if(i<pad_in_num){
	seg_angle[i]=360./double(numpads[i]);
	seg_width[i]=pad_in[i]*(angle[i])*CLHEP::pi/180./numpads[i];

	num_pad_check=angle[i]/seg_angle[i];
      }else if(i>=pad_in_num){
	seg_angle[i]=(180.-angle[i])*2/double(numpads[i]);
	seg_width[i]=pad_in[i]*(180-angle[i])*2.*acos(-1.)/180./numpads[i];
	num_pad_check=(180.-angle[i])*2/seg_angle[i];
      }

      G4cout<<i<<" degree :"<<seg_angle[i]<<G4endl;
      G4cout<<i<<" width :"<<seg_angle[i]*acos(-1.)/180.*pad_in[i]<<G4endl;

      all_channels=all_channels+numpads[i];
      all_channels2=all_channels2+num_pad_check;
    }
    G4cout<<"------------------------"<<G4endl;
    G4cout<<"Total pads:"<<all_channels<<G4endl;
    G4cout<<"Total pads(check):"<<all_channels<<G4endl;
    G4cout<<"------------------------"<<G4endl;
  }

  for( auto& p : hmap ){
    delete p.second;
  }
  hmap.clear();
  TString key;
  key = "Time";
  hmap[key] = new TH1D( key, key, 400, 0.0, 10.0 );
  for( G4int i=0; i<G4ThreeVector::SIZE; ++i ){
    key = Form( "Pos%d", i );
    hmap[key] = new TH1D( key, key, 500, -25.0*CLHEP::cm, 25.0*CLHEP::cm );
    key = Form( "Mom%d", i );
    if( i==2 )
      hmap[key] = new TH1D( key, key, 400, 0.0, 2.0 );
    else
      hmap[key] = new TH1D( key, key, 400, -2.0, 2. );
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::EndOfRunAction( void )
{
  tree->Write();
  for( auto& p : hmap ){
    p.second->Write();
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::BeginOfEventAction( void )
{
  HitNum=0;
  tpctrNum=0;

  //for K+
  HitNum_K=0;
  //  tpctrNum_K=0;

  //for proton
  HitNum_p=0;
  //  tpctrNum_K=0;

  event.nhPrm = 0;
  for( Int_t i=0; i<MaxPrimaryParticle; ++i ){
    event.pidPrm[i] = -9999;
    event.xPrm[i] = -9999.;
    event.yPrm[i] = -9999.;
    event.zPrm[i] = -9999.;
    event.pxPrm[i] = -9999.;
    event.pyPrm[i] = -9999.;
    event.pzPrm[i] = -9999.;
    event.ptPrm[i] = -9999.;
    event.thetaPrm[i] = -9999.;
    event.mPrm[i] = -9999.;
  }

  event.nhTgt = 0;
  event.nhHtof = 0;
  event.nhSdc = 0;
  event.nhSch = 0;
  event.nhFtof = 0;
  for( G4int i=0; i<MaxHits; ++i ){
    // TARGET
    event.pidTgt[i] = -9999;
    event.tidTgt[i] = -9999;
    event.prtTgt[i] = -9999;
    event.xTgt[i] = -9999.;
    event.yTgt[i] = -9999.;
    event.zTgt[i] = -9999.;
    event.vtxTgt[i] = -9999.;
    event.vtyTgt[i] = -9999.;
    event.vtzTgt[i] = -9999.;
    // HTOF
    event.tidHtof[i] = -9999;
    event.pidHtof[i] = -9999;
    event.didHtof[i] = -9999;
    event.prtHtof[i] = -9999;
    event.qHtof[i] = -9999;
    event.massHtof[i] = -9999.;
    event.xHtof[i] = -9999.;
    event.yHtof[i] = -9999.;
    event.zHtof[i] = -9999.;
    event.pxHtof[i] = -9999.;
    event.pyHtof[i] = -9999.;
    event.pzHtof[i] = -9999.;
    event.tofHtof[i] = -9999.;
    event.vtppHtof[i] = -9999.;
    event.vtpxHtof[i] = -9999.;
    event.vtpyHtof[i] = -9999.;
    event.vtpzHtof[i] = -9999.;
    event.vtxHtof[i] = -9999.;
    event.vtyHtof[i] = -9999.;
    event.vtzHtof[i] = -9999.;
    // SDC
    event.tidSdc[i] = -9999;
    event.pidSdc[i] = -9999;
    event.didSdc[i] = -9999;
    event.prtSdc[i] = -9999;
    event.qSdc[i] = -9999;
    event.massSdc[i] = -9999.;
    event.xSdc[i] = -9999.;
    event.ySdc[i] = -9999.;
    event.zSdc[i] = -9999.;
    event.pxSdc[i] = -9999.;
    event.pySdc[i] = -9999.;
    event.pzSdc[i] = -9999.;
    event.tofSdc[i] = -9999.;
    event.vtppSdc[i] = -9999.;
    event.vtpxSdc[i] = -9999.;
    event.vtpySdc[i] = -9999.;
    event.vtpzSdc[i] = -9999.;
    event.vtxSdc[i] = -9999.;
    event.vtySdc[i] = -9999.;
    event.vtzSdc[i] = -9999.;
    // SCH
    event.tidSch[i] = -9999;
    event.pidSch[i] = -9999;
    event.didSch[i] = -9999;
    event.prtSch[i] = -9999;
    event.qSch[i] = -9999;
    event.massSch[i] = -9999.;
    event.xSch[i] = -9999.;
    event.ySch[i] = -9999.;
    event.zSch[i] = -9999.;
    event.pxSch[i] = -9999.;
    event.pySch[i] = -9999.;
    event.pzSch[i] = -9999.;
    event.tofSch[i] = -9999.;
    event.vtppSch[i] = -9999.;
    event.vtpxSch[i] = -9999.;
    event.vtpySch[i] = -9999.;
    event.vtpzSch[i] = -9999.;
    event.vtxSch[i] = -9999.;
    event.vtySch[i] = -9999.;
    event.vtzSch[i] = -9999.;
    // FTOF
    event.tidFtof[i] = -9999;
    event.pidFtof[i] = -9999;
    event.didFtof[i] = -9999;
    event.prtFtof[i] = -9999;
    event.qFtof[i] = -9999;
    event.massFtof[i] = -9999.;
    event.xFtof[i] = -9999.;
    event.yFtof[i] = -9999.;
    event.zFtof[i] = -9999.;
    event.pxFtof[i] = -9999.;
    event.pyFtof[i] = -9999.;
    event.pzFtof[i] = -9999.;
    event.tofFtof[i] = -9999.;
    event.vtppFtof[i] = -9999.;
    event.vtpxFtof[i] = -9999.;
    event.vtpyFtof[i] = -9999.;
    event.vtpzFtof[i] = -9999.;
    event.vtxFtof[i] = -9999.;
    event.vtyFtof[i] = -9999.;
    event.vtzFtof[i] = -9999.;
  }

  event.nttpc = 0;
  event.ntrtpc = 0;

  event.HitNum_K=-1;

  event.HitNum_p=-1;


  event.mm_d = 0.;
  event.mm_p = 0.;
  event.theta = 0.;
  event.theta_scat = 0.;
  event.theta_CM = 0.;


  /* ntrtpc initialization */

  for( G4int i=0; i<MaxHitsTPC;++i ){
    event.trpidtpc[i]  = -1;
    event.trparentidtpc[i]  = -1;
    event.trparentid_pid_tpc[i]  = -1;


    event.trpptpc[i]  = -9999.9999;
    event.trpttpc[i]  = -9999.9999;
    event.trpxtpc[i]  = -9999.9999;
    event.trpytpc[i]  = -9999.9999;
    event.trpztpc[i]  = -9999.9999;

    event.vtpxtpc[i]  = -9999.9999;
    event.vtpytpc[i]  = -9999.9999;
    event.vtpztpc[i]  = -9999.9999;

    event.vtxtpc[i]  = -9999.9999;
    event.vtytpc[i]  = -9999.9999;
    event.vtztpc[i]  = -9999.9999;

    event.trpttpcfit[i]  = -9999.9999;

    event.trpptpcfit[i]  = -9999.9999;
    event.trpxtpcfit[i]  = -9999.9999;
    event.trpytpcfit[i]  = -9999.9999;
    event.trpztpcfit[i]  = -9999.9999;

    event.trpmtpc[i]  = -9999.9999;
    event.trqqtpc[i]  = -9999;

    event.trdetpc[i]  = -9999.9999;
    event.trlentpc[i]  = -9999.9999;
    event.trdedxtpc[i]  = -9999.9999;
    event.trdedxtrtpc[i]  = -9999.9999;
    event.trlaytpc[i]  = -9999;

    event.cir_r[i]  = -9999.9999;
    event.cir_x[i]  = -9999.9999;
    event.cir_z[i]  = -9999.9999;
    event.cir_fit[i]  = -9999.9999;

    event.vtx_flag[i]  = -1;
    event.a_fory[i]  = -9999.9999;
    event.b_fory[i]  = -9999.9999;
  }


  for(int i=0;i<MaxTrack;i++){

    /// initialization pad multiplicity
    event.nthlay[i]=-9999.;
    event.nthpad[i]=-9999.;
    for(int j = 0; j< MaxNthLay;j++){
      for(int k = 0; k< MaxNthPad;k++){
	event.laypad[i][j][k]  = 0.;
      }
    }
    //////////////


    event.xtpc[i] = -9999.9;
    event.ytpc[i] = -9999.9;
    event.ztpc[i] = -9999.9;

    event.x0tpc[i] = -9999.9;
    event.y0tpc[i] = -9999.9;
    event.z0tpc[i] = -9999.9;
    event.resoX[i] = -9999.9;

    event.pxtpc[i] = -9999.9;
    event.pytpc[i] = -9999.9;
    event.pztpc[i] = -9999.9;
    event.pptpc[i] = -9999.9;

    event.masstpc[i] = -9999.9;

    event.betatpc[i] = -9999.9;

    event.edeptpc[i] = -9999.9;

    event.ititpc[i] = -1;
    event.idtpc[i] = -1;
    event.laytpc[i] = -1;
    event.rowtpc[i] = -1;
    event.parentID[i] = -1;
  }
}

//_____________________________________________________________________________
int
TPCAnaManager::EndOfEventAction( void )
{
  event.evnum++;

  //Fill Primary Infomation for E27
  if( env_Experiment_num ==27 || env_Experiment_num ==45 ){
    event.mm_d = primaryInfo.mm_d;
    event.mm_p = primaryInfo.mm_p;
    event.theta = primaryInfo.theta;
    event.theta_scat = primaryInfo.theta_scat;
    event.theta_CM = primaryInfo.theta_CM;
    event.mm = CLHEP::mm;
  }

  if( HitNum > 0){
    G4int c[MAX_TRACK] = {};

    for(G4int i=0;i<MAX_TRACK;i++){
      mean[i]=0.;
      trmean[i]=0.;
    }

    G4double vtxxfit[MAX_TRACK];//read fit parameters
    G4double vtxyfit[MAX_TRACK];//read fit parameters
    G4double vtxzfit[MAX_TRACK];//read fit parameters

    G4double vtxpxfit[MAX_TRACK];//read fit parameters
    // G4double vtxpyfit[MAX_TRACK];//read fit parameters
    G4double vtxpzfit[MAX_TRACK];//read fit parameters

    for(G4int i=0;i<MAX_TRACK;i++){
      vtxxfit[i]=-9999.9999;
      vtxyfit[i]=-9999.9999;
      vtxzfit[i]=-9999.9999;
      vtxpxfit[i]=-9999.9999;
      // vtxpyfit[i]=-9999.9999;
      vtxpzfit[i]=-9999.9999;
      Pz[i]=-9999.9999;
    }

    G4double x[MAX_TRACK][MAXtpctrhitNum]={{-9999.9999},{-9999.9999}};;
    G4double z[MAX_TRACK][MAXtpctrhitNum]={{-9999.9999},{-9999.9999}};
    G4double y[MAX_TRACK][MAXtpctrhitNum]={{-9999.9999},{-9999.9999}};
    G4double ede[MAX_TRACK][MAXtpctrhitNum]={{0.},{0.}};

    ////// shhwang position read
    ///shhwang code


    if(tpctrNum>9){
      G4cout<<"Error--> over the number of tracks in the TPC:"<<tpctrNum<<G4endl;
    }

    G4int sh_paID[MAX_TRACK] = {};
    for( G4int i=0; i<HitNum; i++){
      G4int ii=counterData[i].ntrk;
      x[ii][c[ii]]=counterData[i].pos[0];
      z[ii][c[ii]]=counterData[i].pos[2];
      y[ii][c[ii]]=counterData[i].pos[1];
      sh_paID[ii]=counterData[i].parentID;
      ede[ii][c[ii]]=counterData[i].dedx;
      c[ii]=c[ii]+1;
    }

    G4double test[MAX_TRACK]={-1};
    G4double cx[MAX_TRACK]={-9999.9999};
    G4double cz[MAX_TRACK]={-9999.9999};
    G4double cir_x[MAX_TRACK]={-9999.9999};
    G4double cir_z[MAX_TRACK]={-9999.9999};
    G4double rad[MAX_TRACK]={-9999.9999};
    G4double a_fory[MAX_TRACK]={-9999.9999};
    G4double b_fory[MAX_TRACK]={-9999.9999};
    G4double theta0_fory[MAX_TRACK]={-9999.9999};
    G4int vtx_flag[MAX_TRACK]={-1};

    for(G4int i=0;i<MAX_TRACK;i++){
      cir_r[i]=-9999.9999;
      cir_x[i]=-9999.9999;
      cir_z[i]=-9999.9999;
      mean[i]=-9999.9999;
    }



    for( G4int kk=0; kk<tpctrNum; kk++){
      if(c[kk]>3.){
	//	G4cout<<"start circle fit"<<G4endl;
	test[kk]=circleFit(x[kk],z[kk],y[kk],c[kk],&cx[kk],&cz[kk],&rad[kk],&Pz[kk],
			   &a_fory[kk], &b_fory[kk], &theta0_fory[kk]);
	if(test[kk]!=-1.){
	  cir_r[kk]=rad[kk];
	  cir_x[kk]=cx[kk];
	  cir_z[kk]=cz[kk];
	}

      }
    }
    G4double mom_theta[MAX_TRACK]={0.};

    // calcute vtx with production points
    for(G4int i=0;i<MAX_TRACK;i++){
      G4double rho1 = rad[i];
      G4double cx1 = cx[i];
      G4double cz1 = cz[i];
      G4double cx2 = event.xPrm[0];
      G4double cz2 = event.zPrm[0];
      G4double theta12=atan2(cz2-cz1, cx2-cx1);
      G4double ca1=a_fory[i];
      G4double cb1=b_fory[i];
      G4double ct01=theta0_fory[i];


      G4double cent_dist=sqrt(pow(cx1-cx2,2)+pow(cz1-cz2,2));

      vtxxfit[i]=cos(theta12)*cent_dist+cx1;
      vtxzfit[i]=sin(theta12)*cent_dist+cz1;
      vtxyfit[i]=-1.*tpcData[i].tpcqq*ca1*rho1*(theta12-ct01)+cb1;

      mom_theta[i]=atan2(vtxzfit[i]-cz[i],vtxxfit[i]-cx[i])-acos(-1.)/2;

      vtxpxfit[i]=cos(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);
      vtxpzfit[i]=sin(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);

      vtx_flag[i]=1;
    }

    ////think about parent ID
    ////--> find the track with same parent ID
    //// sh_

    for(G4int i=0;i<MAX_TRACK;i++){
      for(G4int j=i;j<MAX_TRACK;j++){
	if(i!=j && (test[i]>0 && test[j]>0) ){
	  if(sh_paID[i]==sh_paID[j] && sh_paID[i]>0. && sh_paID[j]>0.){
	    //	    G4cout<<"vtx1"<<env_helm_field<<G4endl;
	    G4double rho1=rad[i];
	    G4double rho2=rad[j];

	    G4double cx1=cx[i];
	    G4double cz1=cz[i];
	    G4double ca1=a_fory[i];
	    G4double cb1=b_fory[i];
	    G4double ct01=theta0_fory[i];

	    G4double cx2=cx[j];
	    G4double cz2=cz[j];
	    G4double ca2=a_fory[j];
	    G4double cb2=b_fory[j];
	    G4double ct02=theta0_fory[j];


	    G4double cent_dist=sqrt(pow(cx1-cx2,2)+pow(cz1-cz2,2));

	    double point1[3]={0};
	    double point2[3]={0};
	    G4int k;

	    if((cent_dist-(rho1+rho2))>0.){


	      G4double theta12=atan2(cz2-cz1,cx2-cx1);
	      G4double centr=rho1+(cent_dist-(rho1+rho2))/2;

	      point1[0]=cos(theta12)*centr+cx1;
	      point1[1]=sin(theta12)*centr+cz1;
	      point1[2]=-1.*tpcData[i].tpcqq*ca1*rho1*(theta12-ct01)+cb1;

	      G4double theta21=atan2(cz1-cz2,cx1-cx2);
	      G4double centr1=rho2+(cent_dist-(rho1+rho2))/2;
	      point2[0]=cos(theta21)*centr1+cx2;
	      point2[1]=sin(theta21)*centr1+cz2;
	      point2[2]=-1.*tpcData[j].tpcqq*ca2*rho2*(theta21-ct02)+cb2;

	      vtxxfit[i]=point1[0];
	      vtxzfit[i]=point1[1];
	      vtxyfit[i]=(point1[2]+point2[2])/2.;
	      vtxxfit[j]=point2[0];
	      vtxzfit[j]=point2[1];
	      vtxyfit[j]=(point1[2]+point2[2])/2.;
	      vtx_flag[i]=2;
	      vtx_flag[j]=2;
	      //
	    }else  if((cent_dist+fmin(rho1,rho2))<fmax(rho1,rho2)){
	      if(rho1>=rho2){ //rho1>rho2
		G4double theta12=atan2(cz2-cz1,cx2-cx1);
		G4double centr=rho1-(rho1-cent_dist-rho2)/2; //rho1>rho2
		point1[0]=cos(theta12)*centr+cx1;
		point1[1]=sin(theta12)*centr+cz1;
		point1[2]=-1.*tpcData[i].tpcqq*ca1*rho1*(theta12-ct01)+cb1;

		G4double theta21=atan2(cz2-cz1,cx2-cx1);
		G4double centr1=rho2+(rho1-cent_dist-rho2)/2.; //rho1>rho2
		point2[0]=cos(theta21)*centr1+cx2;
		point2[1]=sin(theta21)*centr1+cz2;
		point2[2]=-1.*tpcData[j].tpcqq*ca2*rho2*(theta21-ct02)+cb2;
		//		G4cout<<"test1"<<G4endl;

	      }else if(rho2>rho1){ //rho1<rho2
		G4double theta12=atan2(cz1-cz2,cx1-cx2);
		G4double centr=rho2-(rho2-cent_dist-rho1)/2; //rho1<rho2
		point1[0]=cos(theta12)*centr+cx2;
		point1[1]=sin(theta12)*centr+cz2;
		point1[2]=-1.*tpcData[j].tpcqq*ca2*rho2*(theta12-ct02)+cb2;

		G4double theta21=atan2(cz1-cz2,cx1-cx2);
		G4double centr1=rho1+(rho2-cent_dist-rho1)/2; //rho1<rho2
		point2[0]=cos(theta21)*centr1+cx1;
		point2[1]=sin(theta21)*centr1+cz1;
		point2[2]=-1.*tpcData[i].tpcqq*ca1*rho1*(theta21-ct01)+cb1;
	      }

	      vtxxfit[i]=point1[0];
	      vtxzfit[i]=point1[1];
	      vtxyfit[i]=(point1[2]+point2[2])/2.;
	      // vtxxfit[j]=point1[0];
	      // vtxzfit[j]=point1[1];
	      // vtxyfit[j]=point1[2];
	      vtxxfit[j]=point2[0];
	      vtxzfit[j]=point2[1];
	      vtxyfit[j]=(point1[2]+point2[2])/2.;

	      vtx_flag[i]=3;
	      vtx_flag[j]=3;
	    } else {

	      //k = CircleIntersect(cx1,cz1,rho1,cx2,cz2,rho2,point1,point2);
	      k = CircleIntersect(cx1,cz1,rho1,cx2,cz2,rho2,ca1,cb1,ct01,tpcData[i].tpcqq,ca2,cb2,ct02,tpcData[j].tpcqq,point1,point2);
	      if(k == 0) {
		G4cout << "no solution" << G4endl;
	      }else if(k>0){


		G4double dist1=sqrt(pow(point1[0]-tpcData[i].tpcvtxx,2)+pow(point1[1]-tpcData[i].tpcvtxz,2));
		G4double dist2=sqrt(pow(point2[0]-tpcData[i].tpcvtxx,2)+pow(point2[1]-tpcData[i].tpcvtxz,2));

		if(dist1<=dist2){//point1 is correct
		  vtxxfit[i]=point1[0];
		  vtxzfit[i]=point1[1];
		  vtxyfit[i]=point1[2];

		  vtxxfit[j]=point1[0];
		  vtxzfit[j]=point1[1];
		  vtxyfit[j]=point1[2];
		}else if(dist1>dist2){//point1 is correct
		  vtxxfit[i]=point2[0];
		  vtxzfit[i]=point2[1];
		  vtxyfit[i]=point2[2];

		  vtxxfit[j]=point2[0];
		  vtxzfit[j]=point2[1];
		  vtxyfit[j]=point2[2];
		}
		vtx_flag[i]=4;
		vtx_flag[j]=4;
	      }

	      mom_theta[i]=atan2(vtxzfit[i]-cz[i],vtxxfit[i]-cx[i])-acos(-1.)/2;
	      mom_theta[j]=atan2(vtxzfit[j]-cz[j],vtxxfit[j]-cx[j])-acos(-1.)/2;


	      // std::cout<<"x01="<<x[i][0]<<", x2="<<x[j][0]<<std::endl;
	      // std::cout<<"y01="<<y[i][0]<<", y2="<<y[j][0]<<std::endl;
	      // std::cout<<"z01="<<z[i][0]<<", z2="<<z[j][0]<<std::endl;

	      // std::cout<<"vtx fit="<<vtxxfit[i]<<", true vtx="<<tpcData[i].tpcvtxx<<std::endl;
	      // std::cout<<"vty fit="<<vtxyfit[i]<<", true vty="<<tpcData[i].tpcvtxy<<std::endl;
	      // std::cout<<"vtz fit="<<vtxzfit[i]<<", true vtz="<<tpcData[i].tpcvtxz<<std::endl;
	      //getchar();


	      vtxpxfit[i]=cos(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);
	      vtxpzfit[i]=sin(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);
	      vtxpxfit[j]=cos(mom_theta[j])*(cir_r[j])*(-0.299792458)*(env_helm_field)*(tpcData[j].tpcqq);
	      vtxpzfit[j]=sin(mom_theta[j])*(cir_r[j])*(-0.299792458)*(env_helm_field)*(tpcData[j].tpcqq);
	      //	    G4cout<<"bfield:"<<env_helm_field<<G4endl;


	    }

	    ///from vertex particle, but it need more than 2
	  }else if(sh_paID[i]==sh_paID[j] && sh_paID[i]==0. && sh_paID[j]==0.){
	    //	    G4cout<<"vtx2"<<env_helm_field<<G4endl;
	    G4double rho1=rad[i];
	    G4double rho2=rad[j];

	    G4double cx1=cx[i];
	    G4double cz1=cz[i];
	    G4double ca1=a_fory[i];
	    G4double cb1=b_fory[i];
	    G4double ct01=theta0_fory[i];

	    G4double cx2=cx[j];
	    G4double cz2=cz[j];
	    G4double ca2=a_fory[j];
	    G4double cb2=b_fory[j];
	    G4double ct02=theta0_fory[j];

	    G4double cent_dist=sqrt(pow(cx1-cx2,2)+pow(cz1-cz2,2));

	    double point1[3]={0};
	    double point2[3]={0};
	    G4int k;

	    if((cent_dist-(rho1+rho2))>0.){


	      G4double theta12=atan2(cz2-cz1,cx2-cx1);
	      G4double centr=rho1+(cent_dist-(rho1+rho2))/2;

	      point1[0]=cos(theta12)*centr+cx1;
	      point1[1]=sin(theta12)*centr+cz1;
	      point1[2]=-1.*tpcData[i].tpcqq*ca1*rho1*(theta12-ct01)+cb1;

	      G4double theta21=atan2(cz1-cz2,cx1-cx2);
	      G4double centr1=rho2+(cent_dist-(rho1+rho2))/2;
	      point2[0]=cos(theta21)*centr1+cx2;
	      point2[1]=sin(theta21)*centr1+cz2;
	      point2[2]=-1.*tpcData[j].tpcqq*ca2*rho2*(theta21-ct02)+cb2;

	      vtxxfit[i]=point1[0];
	      vtxzfit[i]=point1[1];
	      vtxyfit[i]=(point1[2]+point2[2])/2.;
	      vtxxfit[j]=point1[0];
	      vtxzfit[j]=point1[1];
	      vtxyfit[j]=(point1[2]+point2[2])/2.;

	      vtx_flag[i]=5;
	      vtx_flag[j]=5;
	    }else  if((cent_dist+fmin(rho1,rho2))<fmax(rho1,rho2)){

	      if(rho1>=rho2){ //rho1>rho2
		G4double theta12=atan2(cz2-cz1,cx2-cx1);
		G4double centr=rho1-(rho1-cent_dist-rho2)/2; //rho1>rho2
		point1[0]=cos(theta12)*centr+cx1;
		point1[1]=sin(theta12)*centr+cz1;
		point1[2]=-1.*tpcData[i].tpcqq*ca1*rho1*(theta12-ct01)+cb1;

		G4double theta21=atan2(cz2-cz1,cx2-cx1);
		G4double centr1=rho2+(rho1-cent_dist-rho2)/2.; //rho1>rho2
		point2[0]=cos(theta21)*centr1+cx2;
		point2[1]=sin(theta21)*centr1+cz2;
		point2[2]=-1.*tpcData[j].tpcqq*ca2*rho2*(theta21-ct02)+cb2;
		//		G4cout<<"test1"<<G4endl;

	      }else if(rho2>rho1){ //rho1<rho2
		G4double theta12=atan2(cz1-cz2,cx1-cx2);
		G4double centr=rho2-(rho2-cent_dist-rho1)/2; //rho1<rho2
		point1[0]=cos(theta12)*centr+cx2;
		point1[1]=sin(theta12)*centr+cz2;
		point1[2]=-1.*tpcData[j].tpcqq*ca2*rho2*(theta12-ct02)+cb2;

		G4double theta21=atan2(cz1-cz2,cx1-cx2);
		G4double centr1=rho1+(rho2-cent_dist-rho1)/2; //rho1<rho2
		point2[0]=cos(theta21)*centr1+cx1;
		point2[1]=sin(theta21)*centr1+cz1;
		point2[2]=-1.*tpcData[i].tpcqq*ca1*rho1*(theta21-ct01)+cb1;
	      }

	      vtxxfit[i]=point1[0];
	      vtxzfit[i]=point1[1];
	      vtxyfit[i]=(point1[2]+point2[2])/2.;
	      vtxxfit[j]=point2[0];
	      vtxzfit[j]=point2[1];
	      vtxyfit[j]=(point1[2]+point2[2])/2.;
	      vtx_flag[i]=6;
	      vtx_flag[j]=6;
	    } else {

	      //k = CircleIntersect(cx1,cz1,rho1,cx2,cz2,rho2,point1,point2);
	      k = CircleIntersect(cx1,cz1,rho1,cx2,cz2,rho2,ca1,cb1,ct01,tpcData[i].tpcqq,ca2,cb2,ct02,tpcData[j].tpcqq,point1,point2);
	      if(k == 0) {
		G4cout << "no solution" << G4endl;
	      }else if(k>0){

		G4double dist1=sqrt(pow(point1[0]-tpcData[i].tpcvtxx,2)+pow(point1[1]-tpcData[i].tpcvtxz,2));
		G4double dist2=sqrt(pow(point2[0]-tpcData[i].tpcvtxx,2)+pow(point2[1]-tpcData[i].tpcvtxz,2));

		if(dist1<=dist2){//point1 is correct
		  vtxxfit[i]=point1[0];
		  vtxzfit[i]=point1[1];
		  vtxyfit[i]=point1[2];

		  vtxxfit[j]=point1[0];
		  vtxzfit[j]=point1[1];
		  vtxyfit[j]=point1[2];
		}else if(dist1>dist2){//point1 is correct
		  vtxxfit[i]=point2[0];
		  vtxzfit[i]=point2[1];
		  vtxyfit[i]=point2[2];

		  vtxxfit[j]=point2[0];
		  vtxzfit[j]=point2[1];
		  vtxyfit[j]=point2[2];
		}
		vtx_flag[i]=7;
		vtx_flag[j]=7;
	      }

	      mom_theta[i]=atan2(vtxzfit[i]-cz[i],vtxxfit[i]-cx[i])-acos(-1.)/2;
	      mom_theta[j]=atan2(vtxzfit[j]-cz[j],vtxxfit[j]-cx[j])-acos(-1.)/2;

	      vtxpxfit[i]=cos(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);
	      vtxpzfit[i]=sin(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);
	      vtxpxfit[j]=cos(mom_theta[j])*(cir_r[j])*(-0.299792458)*(env_helm_field)*(tpcData[j].tpcqq);
	      vtxpzfit[j]=sin(mom_theta[j])*(cir_r[j])*(-0.299792458)*(env_helm_field)*(tpcData[j].tpcqq);
	      //	    G4cout<<"env_helm_field:"<<env_helm_field<<G4endl;
	    }
	  }
	  /////vertex reconstruction with beam
	  /*	  else if(sh_paID[i]==0. && sh_paID[i] != sh_paID[j] ){
		  G4double rho1=rad[i];
		  G4double cx1=cx[i];

		  G4double cent_dist=sqrt(pow(cx1-cx2,2)+pow(cz1-cz2,2));

		  double point1[2]={0};
		  G4int k;

		  if((cent_dist-(rho1+rho2))>0.){


		  G4double theta12=atan2(cz2-cz1,cx2-cx1);
		  G4double centr=rho1+(cent_dist-(rho1+rho2))/2;

		  point1[0]=cos(theta12)*centr+cx1;
		  point1[1]=sin(theta12)*centr+cz1;

		  G4double theta21=atan2(cz1-cz2,cx1-cx2);
		  G4double centr1=rho2+(cent_dist-(rho1+rho2))/2;
		  point2[0]=cos(theta21)*centr1+cx2;
		  point2[1]=sin(theta21)*centr1+cz2;

		  vtxxfit[i]=point1[0];
		  vtxzfit[i]=point1[1];
		  vtxxfit[j]=point1[0];
		  vtxzfit[j]=point1[1];
		  }else  if((cent_dist+fmin(rho1,rho2))<fmax(rho1,rho2)){

		  if(rho1>=rho2){ //rho1>rho2
		  G4double theta12=atan2(cz2-cz1,cx2-cx1);
		  G4double centr=rho1-(rho1-cent_dist-rho2)/2; //rho1>rho2
		  point1[0]=cos(theta12)*centr+cx1;
		  point1[1]=sin(theta12)*centr+cz1;

		  G4double theta21=atan2(cz2-cz1,cx2-cx1);
		  G4double centr1=rho2+(rho1-cent_dist-rho2)/2.; //rho1>rho2
		  point2[0]=cos(theta21)*centr1+cx2;
		  point2[1]=sin(theta21)*centr1+cz2;
		  //		G4cout<<"test1"<<G4endl;

		  }else if(rho2>rho1){ //rho1<rho2
		  G4double theta12=atan2(cz1-cz2,cx1-cx2);
		  G4double centr=rho2-(rho2-cent_dist-rho1)/2; //rho1<rho2
		  point1[0]=cos(theta12)*centr+cx2;
		  point1[1]=sin(theta12)*centr+cz2;

		  G4double theta21=atan2(cz1-cz2,cx1-cx2);
		  G4double centr1=rho1+(rho2-cent_dist-rho1)/2; //rho1<rho2
		  point2[0]=cos(theta21)*centr1+cx1;
		  point2[1]=sin(theta21)*centr1+cz1;
		  }

		  vtxxfit[i]=point1[0];
		  vtxzfit[i]=point1[1];
		  vtxxfit[j]=point1[0];
		  vtxzfit[j]=point1[1];
		  } else {

		  k = CircleIntersect(cx1,cz1,rho1,cx2,cz2,rho2,point1,point2);
		  if(k == 0) {
		  G4cout << "no solution" << G4endl;
		  }else if(k>0){

		  G4double dist1=sqrt(pow(point1[0]-tpcData[i].tpcvtxx,2)+pow(point1[1]-tpcData[i].tpcvtxz,2));
		  G4double dist2=sqrt(pow(point2[0]-tpcData[i].tpcvtxx,2)+pow(point2[1]-tpcData[i].tpcvtxz,2));

		  if(dist1<=dist2){//point1 is correct
		  vtxxfit[i]=point1[0];
		  vtxzfit[i]=point1[1];

		  vtxxfit[j]=point1[0];
		  vtxzfit[j]=point1[1];
		  }else if(dist1>dist2){//point1 is correct
		  vtxxfit[i]=point2[0];
		  vtxzfit[i]=point2[1];

		  vtxxfit[j]=point2[0];
		  vtxzfit[j]=point2[1];
		  }
		  }

		  mom_theta[i]=atan2(vtxzfit[i]-cz[i],vtxxfit[i]-cx[i])-acos(-1.)/2;
		  mom_theta[j]=atan2(vtxzfit[j]-cz[j],vtxxfit[j]-cx[j])-acos(-1.)/2;

		  vtxpxfit[i]=cos(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);
		  vtxpzfit[i]=sin(mom_theta[i])*(cir_r[i])*(-0.299792458)*(env_helm_field)*(tpcData[i].tpcqq);
		  vtxpxfit[j]=cos(mom_theta[j])*(cir_r[j])*(-0.299792458)*(env_helm_field)*(tpcData[j].tpcqq);
		  vtxpzfit[j]=sin(mom_theta[j])*(cir_r[j])*(-0.299792458)*(env_helm_field)*(tpcData[j].tpcqq);

		  }
		  }
	  */

	}
      }
    }


    /*
    //------------------&^-^-------------------//
    ////rungekutta study
    //------------------&^-^-------------------//
    /////////////////////////////////////////////
    /////////////////////////////////////////////
    //////////// rungekutta tracking based on LEPS TPC analyzer
    /////////////////////////////////////////////
    /////////////////////////////////////////////
    int sector, lay;
    //    cir_r[kk]=rad[kk];
    //    cir_x[kk]=cx[kk];
    //    cir_z[kk]=cz[kk];
    double rkpar[5]={0};
    int iflag=0.;

    Switch sw;
    Track tracks[MAX_TRACK];
    initTrack(tracks);
    for( int kk=0; kk<tpctrNum; kk++){
    ///rkpara: x, y, u(px),v(py), q/p
    tracks[kk].rKInitPara[0]=x[kk][0];
    tracks[kk].rKInitPara[1]=y[kk][0];
    tracks[kk].rKInitPara[2]=vtxpxfit[kk]/sqrt(vtxpxfit[kk]*vtxpxfit[kk]+vtxpyfit[kk]*vtxpyfit[kk]+vtxpzfit[kk]*vtxpzfit[kk]);
    tracks[kk].rKInitPara[3]=vtxpyfit[kk]/sqrt(vtxpxfit[kk]*vtxpxfit[kk]+vtxpyfit[kk]*vtxpyfit[kk]+vtxpzfit[kk]*vtxpzfit[kk]);
    tracks[kk].rKInitPara[4]=tpcData[kk].tpcqq/sqrt(vtxpxfit[kk]*vtxpxfit[kk]+vtxpyfit[kk]*vtxpyfit[kk]+vtxpzfit[kk]*vtxpzfit[kk]);

    for( int j = 0; j < c[kk]; j++){
    tracks[kk].x[j][0] = x[kk][j];
    //	std::cout<<"j:"<<j<<"::"<<tracks[kk].x[j][0]<<std::endl;
    tracks[kk].x[j][1] = y[kk][j];
    tracks[kk].x[j][2] = z[kk][j];
    tracks[kk].numHits ++;
    //	std::cout<<"num_hits:"<<c[kk]<<std::endl;
    }
    }
    for( G4int kk=0; kk<tpctrNum; kk++){
    if(c[kk]>5.){
    RungeKuttaTracker rungekuttatrack(tracks+kk);
    }
    }

    */

    ///////////////////////vertex momentum for P_t
    G4int trn[MAX_TRACK];
    //// trancated mean --> now mean
    for(G4int i=0;i<tpctrNum;i++){
      trn[i]=c[i]*(truncated_mean_cut);
      G4double trtmp[MAX_TRACK]={0.000000000};
      for(G4int iii=0;iii<MAX_TRACK;iii++){
	trtmp[iii]=0.000000000;
      }

      for(G4int l=0;l<trn[i];l++){ //--> loop truncated number
	for(G4int k=0;k<c[i];k++){
	  if(l==0){
	    if(trtmp[l]<ede[i][k]){
	      trtmp[l]=ede[i][k];
	    }
	  }else if(l>0){
	    if(trtmp[l-1]>ede[i][k]){
	      if(trtmp[l]<ede[i][k]){
		trtmp[l]=ede[i][k];
	      }
	    }
	  }

	}//--loop end
      }
      for(G4int j=0;j<c[i];j++){
	if(trn[i]!=0.){
	  G4int sh_ch=1;
	  for(G4int jj=0;jj<trn[i];jj++){
	    if(ede[i][j]==trtmp[jj]){
	      sh_ch=-1;
	    }
	  }
	  if(sh_ch>0){
	    trmean[i]=trmean[i]+ede[i][j]/(c[i]-trn[i]);
	  }
	}else if(trn[i]==0.){
	  trmean[i]=trmean[i]+ede[i][j]/(c[i]-trn[i]);
	}

      }
      //      }
    }
    for( G4int i=0; i<HitNum; i++){
      event.ntrk[event.nttpc] = counterData[i].ntrk;
      hmap["Time"]->Fill( counterData[i].time );
      for( G4int j=0; j<G4ThreeVector::SIZE; ++j ){
	hmap[Form( "Pos%d", j )]->Fill( counterData[i].pos[j]/CLHEP::mm );
	hmap[Form( "Mom%d", j )]->Fill( counterData[i].mom[j]/CLHEP::GeV );
      }
      event.xtpc[event.nttpc] = counterData[i].pos[0]/CLHEP::mm;
      event.ytpc[event.nttpc] = counterData[i].pos[1]/CLHEP::mm;
      event.ztpc[event.nttpc] = counterData[i].pos[2]/CLHEP::mm;
      event.x0tpc[event.nttpc] = counterData[i].pos0[0]/CLHEP::mm;
      event.y0tpc[event.nttpc] = counterData[i].pos0[1]/CLHEP::mm;
      event.z0tpc[event.nttpc] = counterData[i].pos0[2]/CLHEP::mm;
      event.resoX[event.nttpc] = counterData[i].resoX;
      event.pxtpc[event.nttpc] = counterData[i].mom[0]/CLHEP::GeV;
      event.pytpc[event.nttpc] = counterData[i].mom[1]/CLHEP::GeV;
      event.pztpc[event.nttpc] = counterData[i].mom[2]/CLHEP::GeV;
      event.pptpc[event.nttpc] = sqrt(pow(counterData[i].mom[0], 2) +
				      pow(counterData[i].mom[1], 2) +
				      pow(counterData[i].mom[2], 2))/CLHEP::GeV;
      event.ititpc[event.nttpc] = counterData[i].trackID;
      event.idtpc[event.nttpc] = counterData[i].particleID;
      event.laytpc[event.nttpc] = counterData[i].iLay;
      event.rowtpc[event.nttpc] = counterData[i].iRow;
      event.betatpc[event.nttpc] = counterData[i].beta;
      event.edeptpc[event.nttpc] = counterData[i].edep;
      event.dedxtpc[event.nttpc] = counterData[i].dedx;
      event.slengthtpc[event.nttpc] = counterData[i].slength;
      event.nthlay[event.nttpc] = counterData[i].iLay;
      event.nthpad[event.nttpc] = counterData[i].iPad;
      event.laypad[event.nttpc][event.nthlay[event.nttpc]][event.nthpad[event.nttpc]]
	= event.laypad[event.nttpc][event.nthlay[event.nttpc]][event.nthpad[event.nttpc]]+1.;
      event.nttpc += 1;
    }

    //
    // TPC
    //
    for( G4int i=0; i<tpctrNum; i++){
      //      G4cout<<"abs"<<abs(env_helm_field)<<G4endl;
      //      G4cout<<"fabs"<<fabs(env_helm_field)<<G4endl;
      // anaRoot.FillTPCData(tpcData[i].tpcpx,
      // 			  tpcData[i].tpcpy,tpcData[i].tpcpz,
      // 			  tpcData[i].tpcpp,
      // 			  tpcData[i].tpcpid, tpcData[i].tpcparentid, tpcData[i].tpcparentid_pid,
      // 			  tpcData[i].tpcqq,
      // 			  tpcData[i].tpcpm,tpcData[i].tpcde,
      // 			  tpcData[i].tpclen,mean[i],trmean[i],
      // 			  tpcData[i].tpclay,
      // 			  tpcData[i].tpcvtxpx,tpcData[i].tpcvtxpy,tpcData[i].tpcvtxpz,
      // 			  tpcData[i].tpcvtxx,tpcData[i].tpcvtxy,tpcData[i].tpcvtxz,
      // 			  vtxxfit[i],vtxyfit[i],vtxzfit[i],
      // 			  vtxpxfit[i],Pz[i],vtxpzfit[i],cir_r[i]*(0.299792458)*fabs(env_helm_field),
      // 			  cir_r[i],cir_x[i],cir_z[i],test[i],
      // 			  vtx_flag[i], a_fory[i], b_fory[i]
      // 			  );
    }

  }//trigger parts

  tree->Fill();

  event.pb->SetMag( 0. );
  return 0;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetCounterData( G4int ntrk,G4double time, G4ThreeVector pos,
			       G4ThreeVector mom,
			       G4int track, G4int particle,
			       G4int iLay,  G4int iRow, G4double beta,
			       G4double edep, G4int parentid,
			       G4double /* tlength */, G4double slength )
{
  G4int hitnum = HitNum;
  G4bool flag=true;
  if (hitnum >= MaxTrack) {
    fprintf(stderr, "TPCAnaManager::SetCounterData Too Much multiplicity %d\n",
	    hitnum);
    return;
  }

  //  G4ThreeVector tar_pos(0.,0.*CLHEP::mm,-150.*CLHEP::mm);

  G4ThreeVector tar_pos(0.,0., -143);
  G4ThreeVector sh_pos(0.,0.,0.);
  sh_pos=pos-tar_pos;

  G4double sh_r = sh_pos.r();
  G4double sh_theta = sh_pos.theta();
  G4double sh_phi = sh_pos.phi();

  G4double sh_x = sh_r*sin(sh_theta)*cos(sh_phi);
  G4double sh_y = sh_r*sin(sh_theta)*sin(sh_phi);
  G4double sh_z = sh_r*cos(sh_theta);

  counterData[hitnum].particleID = particle;
  ////shhwang check, check a multiplicity of layers
  for(G4int i=0;i<hitnum;i++){
    if((counterData[i].iLay == iLay &&
  	counterData[i].ntrk == ntrk)){
      flag = false;
    }
  }
  flag=true;
  if(flag == true){
    counterData[hitnum].ntrk = ntrk;
    counterData[hitnum].time = time;
    counterData[hitnum].beta = beta;
    counterData[hitnum].dedx = edep/slength;
    counterData[hitnum].edep = edep;
    counterData[hitnum].slength = slength;

    //////shhwang position smearing////


    G4double sh_alpha =  atan2(sh_x,sh_z);
    G4double sh_rho =  sqrt(pow(sh_z,2)+pow(sh_x,2));
    //    G4double sh_dalpha = 0.300*CLHEP::mm/sh_rho; // rho * theta = arc --> sigma=300 um
    //    G4double sh_smear_alpha = CLHEP::RandGauss::shoot(sh_alpha, sh_dalpha);
    G4double sh_sigmaY = 0.500*CLHEP::mm; //--> smearing : 400 um

    G4double ang_sh=atan2(sh_pos.getY(),sh_pos.getX());
    // G4double ang_check=0;
    if(ang_sh>acos(-1.)){
      ang_sh=ang_sh-2*acos(-1.);
      // ang_check=-1;
    }else{
      // ang_check=1.;
    }
    // G4double arc_sh=pad_in[iLay]*ang_sh;
    // G4int ith_pad_in=arc_sh/pad_in_width;
    // G4int ith_pad_out=arc_sh/pad_out_width;


    // G4double delta_x=0.;

    // if( env_pad_config ==1 ){
    //   if(iLay<pad_in_num){   // const G4int pad_in_num=10;
    // 	delta_x=arc_sh-ith_pad_in*pad_in_width;
    // 	if(delta_x<0){
    // 	  delta_x=delta_x+pad_in_width;
    // 	}
    //   }else if(iLay>=pad_in_num){
    // 	delta_x=arc_sh-ith_pad_out*pad_out_width;
    // 	if(delta_x<0){
    // 	  delta_x=delta_x+pad_out_width;
    // 	}
    //   }
    // }else if( env_pad_config ==2 ){
    //   if(iLay<pad_in_num){   //

    // 	ith_pad_in=arc_sh/seg_width[iLay];
    // 	delta_x=arc_sh-ith_pad_in*seg_width[iLay];
    // 	if(delta_x<0){
    // 	  delta_x=delta_x+seg_width[iLay];
    // 	}

    //   }else if(iLay>=pad_in_num){
    // 	ith_pad_out=arc_sh/seg_width[iLay];
    // 	delta_x=arc_sh-ith_pad_out*seg_width[iLay];
    // 	if(delta_x<0){
    // 	  delta_x=delta_x+seg_width[iLay];
    // 	}

    //   }
    // }


    ///include saho-san's code

    // G4double compy=0.;
    G4double compx=0.;

    // G4int n_electron=0; //output
    // G4int n_pad=0;//output
    // G4double x_rms=0;//output
    // G4double x_track = delta_x;// pad edge
    // G4double y_track = double(250.+50.+pos.getY());// 50 is office set
    // if(y_track<0 || y_track>600) G4cout<<"Y_track estimation is wrong(y_track:y_pos)"<<y_track<<":"<<pos.getY()<<G4endl;


    // G4double dxdz_track=tan(acos((sh_pos.getX()*mom.getX()+sh_pos.getZ()*mom.getZ())/(sqrt(sh_pos.getX()*sh_pos.getX()+sh_pos.getZ()*sh_pos.getZ())*sqrt(mom.getX()*mom.getX()+mom.getZ()*mom.getZ()))));


    // //pad and particle angle


    // G4double dydz_track=0;

    // if( env_pad_config ==1){
    //   if(iLay<pad_in_num){
    // 	ResHypTPC reshyptpc(pad_in_width, pad_length_in, 0.1,0.18, 0);
    // 	compx = reshyptpc.getXDeviation(n_electron, n_pad, x_rms, x_track, y_track, dxdz_track, dydz_track);
    // 	compy = reshyptpc.getYDeviation(pos.getY());
    //   }else if(iLay>=pad_in_num){
    // 	ResHypTPC reshyptpc(pad_out_width, pad_length_out, 0.1,0.18, 0);
    // 	compx = reshyptpc.getXDeviation(n_electron, n_pad, x_rms, x_track, y_track, dxdz_track, dydz_track);
    // 	compy = reshyptpc.getYDeviation(pos.getY());
    //   }


    // }else if( env_pad_config ==2){
    //   if(iLay<pad_in_num){
    // 	ResHypTPC reshyptpc(double(seg_width[iLay]-0.5), pad_length_in, 0.1,0.18, 0);
    // 	compx = reshyptpc.getXDeviation(n_electron, n_pad, x_rms, x_track, y_track, dxdz_track, dydz_track);
    // 	compy = reshyptpc.getYDeviation(pos.getY());
    //   }else if(iLay>=pad_in_num){
    //ResHypTPC reshyptpc(double(seg_width[iLay]-0.5), pad_length_out, 0.1,0.18, 0);
    // 	compx = reshyptpc.getXDeviation(n_electron, n_pad, x_rms, x_track, y_track, dxdz_track, dydz_track);
    // 	compy = reshyptpc.getYDeviation(pos.getY());
    //   }
    // }
    compx = GetTransverseRes(sh_y);
    double s_compx = CLHEP::RandGauss::shoot(0.,compx);

    // std::cout<<"compx="<<compx<<", sh_sigmaY"<<sh_sigmaY<<std::endl;
    // getchar();
    //G4double sh_dalpha = compx/sh_rho; // rho * theta = arc --> from sako-san's code
    G4double sh_dalpha = s_compx/sh_rho; // rho * theta = arc --> from sako-san's code
    G4double sh_smear_alpha = sh_alpha+sh_dalpha;
    //    G4cout<<compx<<":"<<sh_dalpha<<G4endl;

    counterData[hitnum].resoX = compx;

    counterData[hitnum].pos[G4ThreeVector::Z] = sh_rho*cos(sh_smear_alpha)+tar_pos.getZ();
    counterData[hitnum].pos[G4ThreeVector::X] = sh_rho*sin(sh_smear_alpha);
    counterData[hitnum].pos[G4ThreeVector::Y] = CLHEP::RandGauss::shoot(sh_y,sh_sigmaY);

    counterData[hitnum].pos0[G4ThreeVector::X] = pos.getX();
    counterData[hitnum].pos0[G4ThreeVector::Y] = pos.getY();
    counterData[hitnum].pos0[G4ThreeVector::Z] = pos.getZ();

    counterData[hitnum].mom[G4ThreeVector::X] = mom.getX();
    counterData[hitnum].mom[G4ThreeVector::Y] = mom.getY();
    counterData[hitnum].mom[G4ThreeVector::Z] = mom.getZ();

    counterData[hitnum].trackID = track;
    counterData[hitnum].particleID = particle;
    counterData[hitnum].iLay = iLay;
    G4int iPad=0.;

    if( env_pad_config ==2 ){
      //    G4bool pass_check=false;
      G4bool pass_check=true;
      G4double cur_angle= (acos(-1.)-atan2(sh_x,sh_z))*180./acos(-1.);
      //    G4cout<<"--------------------"<<G4endl;
      //    G4cout<<"currrent angle:"<<cur_angle<<G4endl;
      //    G4cout<<"layer angle:"<<angle[iLay]<<G4endl;
      //    G4cout<<"seg angle:"<<seg_angle[iLay]<<G4endl;
      if(iLay<pad_in_num){
	G4double check_num_pads=(cur_angle)/seg_angle[iLay];
	//      G4cout<<check_num_pads<<G4endl;
	iPad=int(check_num_pads);
      }else if(iLay>=pad_in_num){
	G4double check_num_pads=(cur_angle-angle[iLay])/seg_angle[iLay];
	//      G4cout<<check_num_pads<<G4endl;
	iPad=int(check_num_pads);
      }
      if(iPad>numpads[iLay]){
	G4cout<<"this code has a error(iPad:numpads)-->"<<iPad<<":"<<numpads[iLay]<<G4endl;
      }
      if(pass_check){
	//    G4cout<<"iLay:"<<iLay<<G4endl;
	//        G4cout<<"iLay:"<<iLay<<G4endl;
	//        G4cout<<"iPad:"<<iPad<<G4endl;
	counterData[hitnum].iPad = iPad;
      }else{
	G4cout<<"wrong:"<<iLay<<G4endl;
      }
    }

    counterData[hitnum].iRow = iRow;
    counterData[hitnum].parentID = parentid;
    HitNum++;

    if(particle==321)
      HitNum_K++;

    if(particle==2212)
      HitNum_p++;
  }

  return;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetFTOFData( VHitInfo* hit )
{
  if( event.nhFtof > MaxHits ){
    G4cerr << FUNC_NAME << " too much nhit " << event.nhFtof << G4endl;
  } else {
    Int_t i = event.nhFtof;
    event.tofFtof[i] = hit->GetTime();
    event.xFtof[i] = hit->GetPosition().x();
    event.yFtof[i] = hit->GetPosition().y();
    event.zFtof[i] = hit->GetPosition().z();
    event.pidFtof[i] = hit->GetParticleID();
    event.nhFtof++;
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::SetHTOFData( VHitInfo* hit )
{
  if( event.nhHtof > MaxHits ){
    G4cerr << FUNC_NAME << " too much nhit " << event.nhHtof << G4endl;
  } else {
  // HTOF
    Int_t i = event.nhHtof;
    event.tofHtof[i] = hit->GetTime();
    event.xHtof[i] = hit->GetPosition().x();
    event.yHtof[i] = hit->GetPosition().y();
    event.zHtof[i] = hit->GetPosition().z();
    event.pidHtof[i] = hit->GetParticleID();
    event.nhHtof++;
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::SetSCHData( VHitInfo* hit )
{
  if( event.nhSch > MaxHits ){
    G4cerr << FUNC_NAME << " too much nhit " << event.nhSch << G4endl;
  } else {
    Int_t i = event.nhSch;
    event.tofSch[i] = hit->GetTime();
    event.xSch[i] = hit->GetPosition().x();
    event.ySch[i] = hit->GetPosition().y();
    event.zSch[i] = hit->GetPosition().z();
    event.pidSch[i] = hit->GetParticleID();
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::SetSDCData( VHitInfo* hit )
{
  if( event.nhSdc > MaxHits ){
    G4cerr << FUNC_NAME << " too much nhit " << event.nhSdc << G4endl;
  } else {
    Int_t i = event.nhSdc;
    event.tofSdc[i] = hit->GetTime();
    event.xSdc[i] = hit->GetPosition().x();
    event.ySdc[i] = hit->GetPosition().y();
    event.zSdc[i] = hit->GetPosition().z();
    event.pidSdc[i] = hit->GetParticleID();
    event.nhSdc++;
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::SetTPCData( G4int tpctr2, G4int tpcpid2, G4int tpcparentid2,
			   G4int tpcparentid_pid2, G4double tpcpx2,
			   G4double tpcpy2, G4double tpcpz2,
			   G4double /* tpcpp2 */,
			   G4int tpcqq2, G4double tpcpm2, G4double tpcde2,
			   G4double tpclen2, G4int tpclay2,
			   G4double vtxpxtpc2, G4double vtxpytpc2,
			   G4double vtxpztpc2,
			   G4double vtxxtpc2, G4double vtxytpc2,
			   G4double vtxztpc2, G4double vtxene2 )
{
  G4int hitnum = tpctr2;

  // G4double theta=acos(tpcpz2/tpcpp2);

  tpcData[hitnum].tpctr = tpctr2;
  tpcData[hitnum].tpcpid = tpcpid2;
  tpcData[hitnum].tpcparentid = tpcparentid2;
  tpcData[hitnum].tpcparentid_pid = tpcparentid_pid2;

  //// w/o smearing
  tpcData[hitnum].tpcpx = tpcpx2;
  tpcData[hitnum].tpcpy = tpcpy2;
  tpcData[hitnum].tpcpz = tpcpz2;
  //// with smearing
  //    tpcData[hitnum].tpcpx = px;
  //    tpcData[hitnum].tpcpz = pz;
  //    tpcData[hitnum].tpcpy = py;


  //kine E = sqrt(p^2+m^2)-m
  //p=sqrt((E+m)^2-m^2)
  G4double totalmom=sqrt(pow(vtxene2+tpcpm2,2)-pow(tpcpm2,2));
  tpcData[hitnum].tpcvtxpx = totalmom*vtxpxtpc2;
  tpcData[hitnum].tpcvtxpy = totalmom*vtxpytpc2;
  tpcData[hitnum].tpcvtxpz = totalmom*vtxpztpc2;

  tpcData[hitnum].tpcvtxx = vtxxtpc2;
  tpcData[hitnum].tpcvtxy = vtxytpc2;
  tpcData[hitnum].tpcvtxz = vtxztpc2;

  //// with smearing
  //  tpcData[hitnum].tpcpp = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));

  //// w/o smearing
  tpcData[hitnum].tpcpp = sqrt(pow(tpcpx2,2)+pow(tpcpy2,2)+pow(tpcpz2,2));
  //  tpcData[hitnum].tpcppfit = sqrt(pow(,2)+pow(tpcpy2,2));

  tpcData[hitnum].tpcqq = tpcqq2;
  tpcData[hitnum].tpcpm = tpcpm2;
  tpcData[hitnum].tpclen = tpclen2;
  tpcData[hitnum].tpcdedx = tpcde2;
  tpcData[hitnum].tpclay = tpclay2;
  tpctrNum++;
  return;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetPrimaryInfo( G4double mm_d, G4double mm_p,
			       G4double theta, G4double theta_scat,
			       G4double theta_CM )
{
  primaryInfo.mm_d = mm_d;
  primaryInfo.mm_p = mm_p;
  primaryInfo.theta = theta;
  primaryInfo.theta_scat = theta_scat;
  primaryInfo.theta_CM = theta_CM;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetNumberOfPrimaryParticle( G4int n )
{
  event.nhPrm = n;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetPrimaryParticle( G4double px, G4double py, G4double pz )
{
  Int_t id = 0;
  event.pxPrm[id] = px;
  event.pyPrm[id] = py;
  event.pzPrm[id] = pz;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetGeneratorID( G4int gen )
{
  event.gen = gen;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetModeID( G4int mode )
{
  event.mode = mode;
}

//_____________________________________________________________________________
void
TPCAnaManager::SetPrimaryParticle( G4int id, G4double px, G4double py,
				   G4double pz, G4double mass, G4int pid )
{
  if( id >= event.nhPrm ){
    G4cerr << FUNC_NAME << " Invalid Primary particle ID" << G4endl;
  } else {
    event.pxPrm[id] = px;
    event.pyPrm[id] = py;
    event.pzPrm[id] = pz;
    event.mPrm[id] = mass;
    event.pidPrm[id] = pid;
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::SetPrimaryVertex( G4int id, G4double x, G4double y, G4double z )
{
  if( id >= event.nhPrm ){
    G4cerr << FUNC_NAME << " Invalid Primary particle ID" << G4endl;
  } else {
    event.xPrm[id] = x;
    event.yPrm[id] = y;
    event.zPrm[id] = z;
  }
}

//_____________________________________________________________________________
void
TPCAnaManager::SetPrimaryBeam( const G4ThreeVector& p )
{
  event.pb->SetXYZ( p.x(), p.y(), p.z() );
}

//_____________________________________________________________________________
void
TPCAnaManager::SetPrimaryBeam( G4double px, G4double py, G4double pz )
{
  event.pb->SetXYZ( px, py, pz );
}

//_____________________________________________________________________________
void
TPCAnaManager::SetTargetData( VHitInfo* hit )
{
  if( event.nhTgt > MaxHits ){
    G4cerr << FUNC_NAME << " too much nhit " << event.nhTgt << G4endl;
  } else {
    Int_t i = event.nhTgt;
    event.tidTgt[i] = hit->GetTrackID();
    event.pidTgt[i] = hit->GetParticleID();
    event.prtTgt[i] = hit->GetParentID();
    event.xTgt[i] = hit->GetPosition().x();
    event.yTgt[i] = hit->GetPosition().y();
    event.zTgt[i] = hit->GetPosition().z();
    event.vtxTgt[i] = hit->GetVertexPosition().x();
    event.vtyTgt[i] = hit->GetVertexPosition().y();
    event.vtzTgt[i] = hit->GetVertexPosition().z();
    event.nhTgt++;
  }
}

/*************************************
 *************************************/
void initTrack(Track* tracks){
  static const std::string funcname = "[InitTrack]";
  int i,j;
  //  G4cout<<"init track"<<G4endl;
  for( i = 0; i < MAX_TRACK; i++){
    tracks[i].nout   =  0;
    tracks[i].ngood  =  0;
    tracks[i].igroup = -1;
    tracks[i].trkQual = -1;
    tracks[i].numHits = 0;
    ////    tracks[i].numSectors = 0;
    tracks[i].numLayers = 0;
    tracks[i].charge = 1000;
    tracks[i].totalLength = 0.0;
    tracks[i].totalLengthTOF = 0.0; /*NTPC TOF*/
    tracks[i].meanAdc = -1.0;
    tracks[i].chi2Pad = -1.0;
    tracks[i].chi2Z       = -1.0;
    tracks[i].chi2Prob = -1.0;
    tracks[i].chi2 = -1.0;
    tracks[i].radius = 0.0;
    tracks[i].center[0] = tracks[i].center[1] = 1000;
    tracks[i].rKNumIter = -1;
    tracks[i].CrossOuter = -1;
    tracks[i].mom[0] = tracks[i].mom[1] =
      tracks[i].mom[2] = 10000.0;
    tracks[i].mom[3] = -1.0;
    tracks[i].resVirtual[0] = tracks[i].resVirtual[1]
      =tracks[i].resVirtual[2] = tracks[i].resVirtual[3] = -100;
    tracks[i].xOnTrack[0] = tracks[i].xOnTrack[1]
      = tracks[i].xOnTrack[2] = 1000.0;
    tracks[i].RKPFinal[0] =
      tracks[i].RKPFinal[1] = tracks[i].RKPFinal[2]  = -1000;
    //    for( j=0 ; j<NUM_SECTOR ; j++ ){
    //      tracks[i].numLayersinSec[j] = 0;
    //    }
    for(j=0; j < MAX_ITERATION; j++){
      tracks[i].rKChi2[j] = 1000.;
    }

    for(j=0; j < NUM_PARA_RK; j++){
      tracks[i].rKInitPara[j] =
        tracks[i].rKFinalPara[j] = -1000.;
    }

    /*  tracks[i].hitPattern = 0;*/
    for( j = 0; j < MAX_HIT_IN_TRACK; j++){
      tracks[i].ibad[j]  = 10;
      tracks[i].zbad[j]  = 10;
      //      tracks[i].hit[j]    = NULL;
      tracks[i].resPad[j] = 1000;
      tracks[i].resPady[j] = 1000; /*NTPC*/
      tracks[i].resZ[j]   = 1000;
      tracks[i].sector[j] = 100;
      tracks[i].lay[j] = 100;
      tracks[i].x[j][0] = 1000.;/*originally 100.*/
      tracks[i].x[j][1] = 1000.;/*originally 100.*/
      tracks[i].x[j][2] = -10000.;
      tracks[i].err[j][0] = tracks[i].err[j][1]
        = tracks[i].err[j][2] = 1000.;
      tracks[i].arcLen[j] = -1.0;
      tracks[i].resPad[j] = tracks[i].resPady[j] = tracks[i].resZ[j] =
        tracks[i].rKresXYZ[j][0] = tracks[i].rKresXYZ[j][1] =
        tracks[i].rKresXYZ[j][2] = tracks[i].rKresXYZ[j][3] =
        tracks[i].initRes[j][0] = tracks[i].initRes[j][1] =
        tracks[i].initRes[j][2] = tracks[i].phi_local[j]
        = -10000.;

    }
  }
}


void initTrack_ku(Track* tracks){
  static const std::string funcname = "[InitTrack]";
  int i,j;
  //  G4cout<<"init track"<<G4endl;
  for( i = 0; i < MAX_TRACK; i++){
    tracks[i].nout   =  0;
    tracks[i].ngood  =  0;
    tracks[i].igroup = -1;
    tracks[i].trkQual = -1;
    tracks[i].numHits = 0;
    ////    tracks[i].numSectors = 0;
    tracks[i].numLayers = 0;
    tracks[i].charge = 1000;
    tracks[i].totalLength = 0.0;
    tracks[i].totalLengthTOF = 0.0; /*NTPC TOF*/
    tracks[i].meanAdc = -1.0;
    tracks[i].chi2Pad = -1.0;
    tracks[i].chi2Z       = -1.0;
    tracks[i].chi2Prob = -1.0;
    tracks[i].chi2 = -1.0;
    tracks[i].radius = 0.0;
    tracks[i].center[0] = tracks[i].center[1] = 1000;
    tracks[i].rKNumIter = -1;
    tracks[i].CrossOuter = -1;
    tracks[i].mom[0] = tracks[i].mom[1] =
      tracks[i].mom[2] = 10000.0;
    tracks[i].mom[3] = -1.0;
    tracks[i].resVirtual[0] = tracks[i].resVirtual[1]
      =tracks[i].resVirtual[2] = tracks[i].resVirtual[3] = -100;
    tracks[i].xOnTrack[0] = tracks[i].xOnTrack[1]
      = tracks[i].xOnTrack[2] = 1000.0;
    tracks[i].RKPFinal[0] =
      tracks[i].RKPFinal[1] = tracks[i].RKPFinal[2]  = -1000;
    //    for( j=0 ; j<NUM_SECTOR ; j++ ){
    //      tracks[i].numLayersinSec[j] = 0;
    //    }
    for(j=0; j < MAX_ITERATION; j++){
      tracks[i].rKChi2[j] = 1000.;
    }

    for(j=0; j < NUM_PARA_RK; j++){
      tracks[i].rKInitPara[j] =
        tracks[i].rKFinalPara[j] = -1000.;
    }

    /*  tracks[i].hitPattern = 0;*/
    for( j = 0; j < MAX_HIT_IN_TRACK; j++){
      tracks[i].ibad[j]  = 10;
      tracks[i].zbad[j]  = 10;
      //      tracks[i].hit[j]    = NULL;
      tracks[i].resPad[j] = 1000;
      tracks[i].resPady[j] = 1000; /*NTPC*/
      tracks[i].resZ[j]   = 1000;
      tracks[i].sector[j] = 100;
      tracks[i].lay[j] = 100;
      tracks[i].x[j][0] = 1000.;/*originally 100.*/
      tracks[i].x[j][1] = 1000.;/*originally 100.*/
      tracks[i].x[j][2] = -10000.;
      tracks[i].res[j] = 0.;
      tracks[i].err[j][0] = tracks[i].err[j][1]
        = tracks[i].err[j][2] = 1000.;
      tracks[i].arcLen[j] = -1.0;
      tracks[i].resPad[j] = tracks[i].resPady[j] = tracks[i].resZ[j] =
        tracks[i].rKresXYZ[j][0] = tracks[i].rKresXYZ[j][1] =
        tracks[i].rKresXYZ[j][2] = tracks[i].rKresXYZ[j][3] =
        tracks[i].initRes[j][0] = tracks[i].initRes[j][1] =
        tracks[i].initRes[j][2] = tracks[i].phi_local[j]
        = -10000.;

    }
  }
}

/*************************************
 *************************************/
/*void setTrack(Track* tracks,int ntrk, double* x, double* y, double* z, double* ede, double* nhit){
  static const std::string funcname = "[SetTrack]";
  int i,j;
  for( i = 0; i < ntrk; i++){
  for( j = 0; j < nhit[i]; j++){
  tracks[i].lay[j] = 100;
  tracks[i].x[j][0] = x[j];
  tracks[i].x[j][1] = y[j];
  tracks[i].x[j][2] = z[j];
  }
  }
  }
*/
/*************************************
 *************************************/
