
#include "lcio.h"

#include "IO/LCWriter.h"
#include "EVENT/LCIO.h"

#include "IMPL/LCEventImpl.h" 
#include "IMPL/LCRunHeaderImpl.h" 
#include "IMPL/LCCollectionVec.h"
#include "IMPL/CalorimeterHitImpl.h"
#include "IMPL/MCParticleImpl.h" 
#include "IMPL/LCTOOLS.h"

#include <cstdlib>
#include <iostream>


using namespace std ;
using namespace lcio ;

static const int NRUN = 10 ;
static const int NEVENT = 10 ; // events
static const int NMCPART = 10 ;  // mc particles per event
static const int NHITS = 50 ;  // calorimeter hits per event

static const string FILEN = "simjob.sio" ;


/** Simple test program to demonstrate writing of data with lcio.
 */

int main(int argc, char** argv ){
  
  
  // create sio writer
  LCWriter* lcWrt = LCFactory::getInstance()->createLCWriter() ;

  int status ;
  if( (status = lcWrt->open( FILEN ))  != LCIO::SUCCESS ) {
    cout << " couldn't open file  : " << FILEN << " status: " << status << endl ;
    return 0 ;
  }

  // loop over runs
  for(int rn=0;rn<NRUN;rn++){
    
    LCRunHeaderImpl* runHdr = new LCRunHeaderImpl ; 
    runHdr->setRunNumber( rn ) ;

    string detName("D09TileHcal")  ;
    runHdr->setDetectorName( detName ) ;

    string description(" these are just dummy runs for testing lcio - no physics whatsoever !") ;
    runHdr->setDescription( description ) ;
    
    string ecalName("ECAL007") ;
    runHdr->addActiveSubdetector( ecalName ) ;
    
    lcWrt->writeRunHeader( runHdr ) ;

    // EventLoop - create some events and write them to the file
    for(int i=0;i<NEVENT;i++){
      
      // we need to use the implementation classes here 
      LCEventImpl*  evt = new LCEventImpl() ;
      
      
      evt->setRunNumber(  rn   ) ;
      evt->setEventNumber( i ) ;
      evt->setDetectorName( detName ) ;
      
      // create and add some mc particles 
      LCCollectionVec* mcVec = new LCCollectionVec( LCIO::MCPARTICLE )  ;
      
      MCParticle* mom = 0 ;
      for(int j=0;j<NMCPART;j++){
	MCParticleImpl* mcp = new MCParticleImpl ;
	
	mcp->setPDG( 101 + j*100  ) ;
	mcp->setParent(  mom  );
	
	float p[3] = { 2./1024. , 4./1024. , 8./1024. } ;
	mcp->setMomentum( p ) ;
	
	mom =  mcp ;  // one body decays :-)
	mcVec->push_back( mcp ) ;
      }
      
      // now add some calorimeter hits
      LCCollectionVec* calVec = new LCCollectionVec( LCIO::CALORIMETERHIT )  ;
      
      for(int j=0;j<NHITS;j++){
	
	CalorimeterHitImpl* hit = new CalorimeterHitImpl ;
	
	hit->setEnergy( 3.1415 * rand()/RAND_MAX   )  ;
	
	float pos[3] = { 1.1* rand()/RAND_MAX , 2.2* rand()/RAND_MAX , 3.3* rand()/RAND_MAX } ;
	
	hit->setPosition( pos ) ;
	
	calVec->push_back( hit ) ;
	
	// assign the hits randomly to MC particles
	float rn =  .99999*rand()/RAND_MAX ;
	int mcIndx = static_cast<int>( NMCPART * rn ) ;
	
	// in order to access a MCParticle,  we need a dynamic cast as the 
	// LCCollection returns an LCIOObject - this is like vectors in Java 
	hit->addMCParticleContribution(  dynamic_cast<const MCParticle*>(mcVec->getElementAt( mcIndx )) , 
					 0.314159, 0.1155 , 121212 ) ;
	
      }
      
      
      evt->addCollection( (LCCollection*) mcVec , "MCParticle" ) ;
      evt->addCollection( (LCCollection*) calVec , ecalName ) ;
      
      // dump the event to the screen 
      LCTOOLS::dumpEvent( evt ) ;
      
      // write the event to the file
      lcWrt->writeEvent( evt ) ;
      


      // we created the event so we need to take care of deleting it ...
      delete evt ;
      
      if( ! (i%100) ) cout << ". " << flush  ;
      
    } // evt loop
  } // run loop
    
  cout << endl 
       << " created  " << NRUN << " runs with  " << NRUN*NEVENT << " events"  
       << endl << endl ;



  lcWrt->close() ;
  
  return 0 ;
}
