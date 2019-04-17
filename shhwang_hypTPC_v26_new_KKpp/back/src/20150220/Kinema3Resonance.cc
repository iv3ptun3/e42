#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Kinema3Resonance.hh"
#include "Randomize.hh"
#include "G4ios.hh"

#define PI 3.141592654

Kinema3Resonance::Kinema3Resonance(double m1, double m2, double m3, double m4, double m5, double m_res, double width, double p1, double p2)
{
  double ECM;
  double vx_res, vy_res, vz_res;   /* unit vector */
  double vx3, vy3, vz3;            /* unit vector */
  double vx4, vy4, vz4;            /* unit vector */
  double vx5, vy5, vz5;            /* unit vector */
  double theta1, theta2;           /* tmpolary; theta1 represents 1st kinematics of 
	                                         m1,m2,m_res, and m5.
			             theta2 represents 2nd kinematics of m_res, m3, and m4*/

  double phi3;                     /* 2phi(CM system)*/ 
  double theta5,phi5;              /* log note p.43 */
  double theta_res;                /* log note p.43 */
  double Theta3,Phi3;              /*heta,Ph*/
  double Theta4,Phi4;              /* eta,Ph*/
  double Theta5,Phi5;              /* Ph*/
  double Theta_res,Phi_res;        /* Theta,Ph*/
  /*
    G4cout<<"m1"<<m1<<G4endl;
    G4cout<<"m2"<<m2<<G4endl;
    G4cout<<"m3"<<m3<<G4endl;
    G4cout<<"m4"<<m4<<G4endl;
    G4cout<<"m5"<<m5<<G4endl;
    G4cout<<"m_res: "<<m_res<<G4endl;
    G4cout<<"shhwang test test 1"<<G4endl;
  */
  kin3.M_1 = m1; //beam
  kin3.M_2 = m2; //target
  kin3.M_3 = m3; //decay from resonance
  kin3.M_4 = m4; //decay from resonance
  kin3.M_5 = m5; //beam

  kin3.p_1_lab = p1;
  kin3.p_2_lab = p2;

  kin3.E_1_lab = p2E(kin3.p_1_lab, kin3.M_1);
  kin3.E_2_lab = p2E(kin3.p_2_lab, kin3.M_2);

  ECM = sqrt((kin3.E_1_lab+kin3.E_2_lab)*(kin3.E_1_lab+kin3.E_2_lab)
	     -(kin3.p_1_lab+kin3.p_2_lab)*(kin3.p_1_lab+kin3.p_2_lab));
  do {
    kin3.M_res = CLHEP::RandBreitWigner::shoot(m_res, width);
  } while (kin3.M_3+kin3.M_4 > kin3.M_res || kin3.M_res > ECM-kin3.M_5);
  //  kin3.M_res = m_res;
  //  G4cout<<m_res<<G4endl;
  kin1 = Kinema2Body(m1, m2,kin3.M_res, m5);
  kin1.SetMomentum(1, p1);
  kin1.SetMomentum(2, p2);
  kin1.SetTheta(1,0.);
  kin1.SetTheta(2,0.);

  kin1.SetThetaCM((double)RandSin());
  //  kin1.SetThetaCM(acos(CLHEP::RandFlat::shoot(-1.,1.)));
  kin1.calc_kinema();
  phi5 = (-180.+360.0*(double)CLHEP::RandFlat::shoot());
  kin3.Theta1CM = kin1.GetThetaCM();
  kin3.Phi1     = phi5;
  /* calculate m_res */
  theta_res = kin1.GetThetaLab();
  vx_res = sin(deg2rad(theta_res))*cos(deg2rad(phi5));
  vy_res = sin(deg2rad(theta_res))*sin(deg2rad(phi5));
  vz_res = cos(deg2rad(theta_res));
  CalcDistoribution(vx_res, vy_res, vz_res, &Theta_res, &Phi_res);

  kin3.E_res_lab = kin1.GetEnergyLab(3);
  kin3.p_res_lab = kin1.GetMomentumLab(3);

  kin3.P_res_lab[0] = kin3.p_res_lab*vx_res;
  kin3.P_res_lab[1] = kin3.p_res_lab*vy_res;
  kin3.P_res_lab[2] = kin3.p_res_lab*vz_res;

  kin3.theta_res = Theta_res;
  kin3.phi_res = Phi_res;

  //  G4cout<<"3reso inside theta:"<<Theta_res<<":"<<theta_res<<G4endl;
  //  G4cout<<"3reso inside theta CM:"<<kin3.Theta1CM<<G4endl;
  //  G4cout<<"3reso inside phi:"<<Phi_res<<":"<<kin3.Phi1<<G4endl;
  //  G4cout<<"111111111111111:"<<Phi_res<<"::"<<phi5<<G4endl;
  /* calculate m5 */
  //  theta5 = -kin1.GetPhiLab();
  theta5 = -kin1.GetPhiLab();

  //  vx5 = sin(deg2rad(theta5))*cos(deg2rad(phi5+180));
  //  vy5 = sin(deg2rad(theta5))*sin(deg2rad(phi5+180));
  vx5 = sin(deg2rad(theta5))*cos(deg2rad(phi5));
  vy5 = sin(deg2rad(theta5))*sin(deg2rad(phi5));
  vz5 = cos(deg2rad(theta5));
  CalcDistoribution(vx5, vy5, vz5, &Theta5, &Phi5);
  //  G4cout<<"atan2"<<atan2(vy5,vx5)/PI*180<<G4endl;
  kin3.E_5_lab = kin1.GetEnergyLab(4);
  kin3.p_5_lab = kin1.GetMomentumLab(4);
  kin3.P_5_lab[0] = kin3.p_5_lab*vx5;
  kin3.P_5_lab[1] = kin3.p_5_lab*vy5;
  kin3.P_5_lab[2] = kin3.p_5_lab*vz5;
  kin3.theta5 = Theta5;
  kin3.phi5 = Phi5;
  //  G4cout<<"test2:"<<sqrt(vx5*vx5+vy5*vy5+vz5*vz5)<<G4endl;
  //  G4cout<<"test--> LAB:"<<acos(vz5/sqrt(vx5*vx5+vy5*vy5+vz5*vz5))*180/3.141592654 <<G4endl;
  //  G4cout<<"m5 inside theta:"<<Theta5<<":"<<theta5<<G4endl;//ok
  //  G4cout<<"m5 inside theta CM:"<<kin3.Theta1CM<<G4endl;//ok
  //  G4cout<<"m5 inside phi:"<<Phi5<<":"<<phi5+180<<G4endl;//ok
  //  G4cout<<Phi_res<<":"<<Phi5<<":"<<phi5<<G4endl;
  //  G4cout<<"3reso inside theta:"<<theta5<<":"<<Theta5<<G4endl;

  /* calculate m3,m4 */
  if (kin3.M_res == m4 && m3 == 0.0) {
    kin3.E_3_lab = 0.0;
    kin3.p_3_lab = 0.0;
    kin3.P_3_lab[0] = 0.0;
    kin3.P_3_lab[1] = 0.0;
    kin3.P_3_lab[2] = 0.0;
    kin3.theta3 = 0.0;
    kin3.phi3 = 0.0;

    kin3.E_4_lab = kin3.E_res_lab;
    kin3.p_4_lab =  kin3.p_res_lab;
    kin3.P_4_lab[0] = kin3.P_res_lab[0];
    kin3.P_4_lab[1] = kin3.P_res_lab[1];
    kin3.P_4_lab[2] = kin3.P_res_lab[2];
    kin3.theta4 = Theta_res;
    kin3.phi4 = Phi_res;
  } else {
    kin2 = Kinema2Body(kin3.M_res, 0.0, m3, m4);
    kin2.SetMomentum(1, kin1.GetMomentumLab(3));
    kin2.SetMomentum(2, 0.0);
    kin2.SetTheta(1,0.);
    kin2.SetTheta(2,0.);
    kin2.SetThetaCM((double)RandSin());
    kin2.calc_kinema();
    
    phi3 = (360.0*(double)CLHEP::RandFlat::shoot());
    //phi3 = 360.0*(double)rand()/(RAND_MAX+1.0);
    
    kin3.Theta2CM = kin2.GetThetaCM();
    kin3.Phi2     = phi3;
    
    /* m3 */
    theta1 = kin1.GetThetaLab();
    theta2 = kin2.GetThetaLab();

    vx3 = cos(deg2rad(phi5))*cos(deg2rad(theta2))*sin(deg2rad(theta1)) +
      cos(deg2rad(theta1))*cos(deg2rad(phi5))*cos(deg2rad(phi3))*sin(deg2rad(theta2)) -
      sin(deg2rad(phi5))*sin(deg2rad(phi3))*sin(deg2rad(theta2));
    
    vy3 = -sin(deg2rad(phi5))*cos(deg2rad(theta2))*sin(deg2rad(theta1)) -
      cos(deg2rad(theta1))*sin(deg2rad(phi5))*cos(deg2rad(phi3))*sin(deg2rad(theta2)) -
      cos(deg2rad(phi5))*sin(deg2rad(phi3))*sin(deg2rad(theta2));

    vz3 = cos(deg2rad(theta2))*cos(deg2rad(theta1)) - 
      sin(deg2rad(theta1))*cos(deg2rad(phi3))*sin(deg2rad(theta2));
    
    CalcDistoribution(vx3, vy3, vz3, &Theta3, &Phi3);
    
    kin3.E_3_lab = kin2.GetEnergyLab(3);
    kin3.p_3_lab = kin2.GetMomentumLab(3);
    kin3.P_3_lab[0] = kin3.p_3_lab*vx3;
    kin3.P_3_lab[1] = kin3.p_3_lab*vy3;
    kin3.P_3_lab[2] = kin3.p_3_lab*vz3;
    kin3.theta3 = Theta3;
    kin3.phi3 = Phi3;
    
    /* m4 */
    theta1 = kin1.GetThetaLab();
    theta2 = -kin2.GetPhiLab();
    
    vx4 = cos(deg2rad(phi5))*cos(deg2rad(theta2))*sin(deg2rad(theta1)) +
      cos(deg2rad(theta1))*cos(deg2rad(phi5))*cos(deg2rad(phi3))*sin(deg2rad(theta2)) -
      sin(deg2rad(phi5))*sin(deg2rad(phi3))*sin(deg2rad(theta2));
    
    vy4 = -sin(deg2rad(phi5))*cos(deg2rad(theta2))*sin(deg2rad(theta1)) -
      cos(deg2rad(theta1))*sin(deg2rad(phi5))*cos(deg2rad(phi3))*sin(deg2rad(theta2)) -
    cos(deg2rad(phi5))*sin(deg2rad(phi3))*sin(deg2rad(theta2));

    vz4 = cos(deg2rad(theta2))*cos(deg2rad(theta1)) - 
      sin(deg2rad(theta1))*cos(deg2rad(phi3))*sin(deg2rad(theta2));

    
    CalcDistoribution(vx4, vy4, vz4, &Theta4, &Phi4);
    
    kin3.E_4_lab = kin2.GetEnergyLab(4);
    kin3.p_4_lab = kin2.GetMomentumLab(4);
    kin3.P_4_lab[0] = kin3.p_4_lab*vx4;
    kin3.P_4_lab[1] = kin3.p_4_lab*vy4;
    kin3.P_4_lab[2] = kin3.p_4_lab*vz4;
    kin3.theta4 = Theta4;
    kin3.phi4 = Phi4;
  }
  //Dump();
}

double Kinema3Resonance::p2E(double p,double m)
{
  return sqrt(p*p + m*m);
}

void Kinema3Resonance::CalcDistoribution(double unitx, double unity, double unitz, double *theta, double *phi)
{
  *theta = rag2deg(acos(unitz));
  *phi=rag2deg(atan2(unity,unitx));
  /*  if (unity>=0.0 && unitz>0.0) 
    *phi = rag2deg(acos(unity/sin(deg2rad(*theta))));
  else if (unity<0.0 && unitz>=0.0) 
    *phi = rag2deg(acos(unity/sin(deg2rad(*theta))));
  else if (unity<=0.0 && unitz<0.0) 
    *phi = 360.0-rag2deg(acos(unity/sin(deg2rad(*theta))));
  else if (unity>0.0 && unitz<=0.0) 
    *phi = 360.0-rag2deg(acos(unity/sin(deg2rad(*theta))));
  else {

    fprintf(stderr,
	  "Kinema3Resonance::CalcDistribution No such reagion unity=%f, unitz=%f\n",
	    unity, unitz);

    Dump();
    exit(1);
  }
  */
  return;
}


double Kinema3Resonance::deg2rad(double theta) {
  return 3.141592654*theta/180.0;
}

double Kinema3Resonance::rag2deg(double rag)
{
  return 360.0 * rag/ (2.0 * 3.141592654);
}

    
double Kinema3Resonance::RandSin(void)
{
  int success=0;
  double x,fx;
  do {
    x = 180.0 * (double)CLHEP::RandFlat::shoot();
    //x = 180.0 * (double)rand()/(RAND_MAX+1.0);
    fx = sin(deg2rad(x));
    if (fx >= (double)CLHEP::RandFlat::shoot())
      success = 1;
  } while (success==0);
  //  G4cout<<x<<G4endl;
  return x;
}

void Kinema3Resonance::Dump(void)
{
  printf("======Kinema3Resonance Dump======\n");
  printf("--Particle1--\n");
  printf("mass=%f, p_lab=%f, E_lab=%f\n",kin3.M_1, kin3.p_1_lab, kin3.E_1_lab);
  printf("--Particle2--\n");
  printf("mass=%f, p_lab=%f, E_lab=%f\n",kin3.M_2, kin3.p_2_lab, kin3.E_2_lab);
  printf("--Resonance--\n");
  printf("mass=%f\n",kin3.M_res);
  printf("--Particle3--\n");
  printf("mass=%f, p_lab=%f, E_lab=%f\n",kin3.M_3, kin3.p_3_lab, kin3.E_3_lab);
  printf("momentum=(%f, %f, %f), (theta, phi)=(%f, %f)\n",
	 kin3.P_3_lab[0], kin3.P_3_lab[1], kin3.P_3_lab[2],
	 kin3.theta3, kin3.phi3);
  printf("--Particle4--\n");
  printf("mass=%f, p_lab=%f, E_lab=%f\n",kin3.M_4, kin3.p_4_lab, kin3.E_4_lab);
  printf("momentum=(%f, %f, %f), (theta, phi)=(%f, %f)\n",
	 kin3.P_4_lab[0], kin3.P_4_lab[1], kin3.P_4_lab[2],
	 kin3.theta4, kin3.phi4);
  printf("--Particle5--\n");
  printf("mass=%f, p_lab=%f, E_lab=%f\n",kin3.M_5, kin3.p_5_lab, kin3.E_5_lab);
  printf("momentum=(%f, %f, %f), (theta, phi)=(%f, %f)\n",
	 kin3.P_5_lab[0], kin3.P_5_lab[1], kin3.P_5_lab[2],
	 kin3.theta5, kin3.phi5);

  printf("Energy:E1+E2=%f, E1+E2+E3=%f\n", kin3.E_1_lab+kin3.E_2_lab,
	 kin3.E_3_lab+kin3.E_4_lab+kin3.E_5_lab);
  printf("Momentum: x-> p1+p2=%f, p3+p4+p5=%f\n", kin3.p_1_lab+kin3.p_2_lab,
	 kin3.P_3_lab[0]+kin3.P_4_lab[0]+kin3.P_5_lab[0]);
  printf("          y-> p3+p4+p5=%f\n",
	 kin3.P_3_lab[1]+kin3.P_4_lab[1]+kin3.P_5_lab[1]);
  printf("          z-> p3+p4+p5=%f\n",
	 kin3.P_3_lab[2]+kin3.P_4_lab[2]+kin3.P_5_lab[2]);

  return;
}

double Kinema3Resonance::GetEnergy(int i)
{
  switch (i) {
  case 1:
    return kin3.E_1_lab;
    break;
  case 2:
    return kin3.E_2_lab;
    break;
  case 3:
    return kin3.E_3_lab;
    break;
  case 4:
    return kin3.E_4_lab;
    break;
  case 5:
    return kin3.E_5_lab;
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::GetEnergy No such particle %d\n", i);
    exit(1);
  }
}

double Kinema3Resonance::GetMomentum(int i)
{
  switch (i) {
  case 1:
    return kin3.p_1_lab;
    break;
  case 2:
    return kin3.p_2_lab;
    break;
  case 3:
    return kin3.p_3_lab;
    break;
  case 4:
    return kin3.p_4_lab;
    break;
  case 5:
    return kin3.p_5_lab;
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::GetMomentum No such particle %d\n", i);
    exit(1);
  }
}

void Kinema3Resonance::GetMomentum(int i, double *mom)
{
  switch (i) {
  case 1:
    mom[0] = kin3.p_1_lab;
    mom[1] = 0.0;
    mom[2] = 0.0;
    break;
  case 2:
    mom[0] = kin3.p_2_lab;
    mom[1] = 0.0;
    mom[2] = 0.0;
    break;
  case 3:
    mom[0] = kin3.P_3_lab[0];
    mom[1] = kin3.P_3_lab[1];
    mom[2] = kin3.P_3_lab[2];
    break;
  case 4:
    mom[0] = kin3.P_4_lab[0];
    mom[1] = kin3.P_4_lab[1];
    mom[2] = kin3.P_4_lab[2];
    break;
  case 5:
    mom[0] = kin3.P_5_lab[0];
    mom[1] = kin3.P_5_lab[1];
    mom[2] = kin3.P_5_lab[2];
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::GetMomentum No such particle %d\n", i);
    exit(1);
  }
}

double Kinema3Resonance::GetTheta(int i)
{
  switch (i) {
  case 1:
    return 0.0;
    break;
  case 2:
    return 0.0;
    break;
  case 3:
    return kin3.theta3;
    break;
  case 4:
    return kin3.theta4;
    break;
  case 5:
    return kin3.theta5;
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::GetTheta No such particle %d\n", i);
    exit(1);
  }
}

double Kinema3Resonance::GetPhi(int i)
{
  switch (i) {
  case 1:
    return 0.0;
    break;
  case 2:
    return 0.0;
    break;
  case 3:
    return kin3.phi3;
    break;
  case 4:
    return kin3.phi4;
    break;
  case 5:
    return kin3.phi5;
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::GetPhi No such particle %d\n", i);
    exit(1);
  }
}

double Kinema3Resonance::GetThetaCM(int i)
{
  switch (i) {
  case 1:
    return kin3.Theta1CM;
    break;
  case 2:
    return kin3.Theta2CM;
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::GetThetaCM index should be 1 or 2 ->%d\n", i);
    exit(1);
  }
}

double Kinema3Resonance::GetPhiCM(int i)
{
  switch (i) {
  case 1:
    return kin3.Phi1;
    break;
  case 2:
    return kin3.Phi2;
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::GetPhiCM index should be 1 or 2 ->%d\n", i);
    exit(1);
  }
}


void Kinema3Resonance::RotateMom(int i, double deg, double *mom)
{
  double Sin,Cos;

  Sin=sin(deg2rad(deg));
  Cos=cos(deg2rad(deg));
  switch (i) {
  case 3:
    mom[0] = Cos*kin3.P_3_lab[0] - Sin*kin3.P_3_lab[1];
    mom[1] = Sin*kin3.P_3_lab[0] + Cos*kin3.P_3_lab[1];
    mom[2] = kin3.P_3_lab[2];
    break;
  case 4:
    mom[0] = Cos*kin3.P_4_lab[0] - Sin*kin3.P_4_lab[1];
    mom[1] = Sin*kin3.P_4_lab[0] + Cos*kin3.P_4_lab[1];
    mom[2] = kin3.P_4_lab[2];
    break;
  case 5:
    mom[0] = Cos*kin3.P_5_lab[0] - Sin*kin3.P_5_lab[1];
    mom[1] = Sin*kin3.P_5_lab[0] + Cos*kin3.P_5_lab[1];
    mom[2] = kin3.P_5_lab[2];
    break;
  default:
    fprintf(stderr, "Kinema3Resonance::RotateMom should be 3,4,5 ->%d\n",i);
    exit(1);
  }

}

double Kinema3Resonance::GetResMass(void)
{
  return kin3.M_res;
}
