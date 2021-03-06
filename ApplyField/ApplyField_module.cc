// ApplyField_module.cc
// A basic "skeleton" to read in art::Event records from a file,
// access their information, and do something with them. 

// See
// <https://cdcvs.fnal.gov/redmine/projects/larsoftsvn/wiki/Using_the_Framework>
// for a description of the ART classes used here.

// Almost everything you see in the code below may have to be changed
// by you to suit your task. The example task is to make histograms
// and n-tuples related to dE/dx of particle tracks in the detector.

// As you try to understand why things are done a certain way in this
// example ("What's all this stuff about 'auto const&'?"), it will help
// to read ADDITIONAL_NOTES.txt in the same directory as this file.

// Also note that, despite our efforts, the documentation and the practices in
// this code may fall out of date. In doubt, ask!
// The last revision of this code was done on July 2015 with LArSoft v04_17_00.

#ifndef ApplyField_Module
#define ApplyField_Module

// Always include headers defining everything you use, and only that.
// Starting from LArSoft and ending with standard C++ is a good check
// on LArSoft headers too -- if they can't be loaded by their own, it's a bug!

// LArSoft includes
#include "lardata/RawData/RawDigit.h"
#include "lardata/RawData/raw.h"
#include "lardata/RecoBaseArt/WireCreator.h"
#include "lardata/RecoBaseArt/HitCreator.h"
#include "lardata/RecoBase/Hit.h"
#include "lardata/RecoBase/Cluster.h"
#include "lardata/RecoBase/Wire.h"
#include "lardata/RecoBase/Track.h"
#include "larcore/Geometry/Geometry.h"
#include "larcore/Geometry/GeometryCore.h"
#include "larcore/SimpleTypesAndConstants/geo_types.h"

// Framework includes
#include "art/Utilities/Exception.h"
// #include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindManyP.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Persistency/Common/Ptr.h"

// uBooNE includes
#include "lardata/Utilities/AssociationUtil.h"
// #include "uboone/Utilities/SignalShapingServiceMicroBooNE.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"

#include "larevt/SpaceCharge/SpaceCharge.h"

// ROOT includes. Note: To look up the properties of the ROOT classes,
// use the ROOT web site; e.g.,
// <http://root.cern.ch/root/html532/ClassIndex.html>
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TFile.h"

// C++ Includes
#include <map>
#include <vector>
#include <array>
#include <string>
#include <cmath>
#include <typeinfo>
#include <utility>
#include <memory>
#include <iterator>

// Laser Module Classes
// #include "LaserObjects/LaserBeam.h"


namespace 
{
  // Applies field correction to to a track
  recob::Track TrackCorrector(const recob::Track& InputTrack);
} // local namespace


namespace ApplyField 
{

  class ApplyField : public art::EDProducer
  {
  public:
    /// Default constructor
    explicit ApplyField(fhicl::ParameterSet const& parameterSet);

    virtual void beginJob() override;
    
    virtual void endJob() override;

    virtual void reconfigure(fhicl::ParameterSet const& parameterSet) override;

    virtual void produce (art::Event& event) override;
    
  private:

    std::string fTrackModuleLabel;    ///< Track module label
    std::string fTrackInstanceLabel;    ///< Track instance label
    art::InputTag fTrackTag; ///< Track tags
    
  }; // class ApplyField
  
  DEFINE_ART_MODULE(ApplyField)


  //-----------------------------------------------------------------------
  //-----------------------------------------------------------------------
  // class implementation

  //-----------------------------------------------------------------------
  // Constructor
  ApplyField::ApplyField(fhicl::ParameterSet const& pset)
  {
    // Read in the parameters from the .fcl file.
    this->reconfigure(pset);
      
    produces< std::vector<recob::Track> >("FieldCorrection");
    
//     produces< std::vector<recob::Wire> >("blibla");
//     produces< std::vector<recob::Hit> >("UPlaneLaserHits");
//     produces< std::vector<recob::Hit> >("VPlaneLaserHits");
//     produces< std::vector<recob::Hit> >("YPlaneLaserHits");
  }

  
  //-----------------------------------------------------------------------
  void ApplyField::beginJob()
  {
    
  }
  
  
  
  void  ApplyField::endJob()
  {
    
  }

  //-----------------------------------------------------------------------
  void ApplyField::reconfigure(fhicl::ParameterSet const& parameterSet)
  {
    // Read parameter set
    fTrackModuleLabel = parameterSet.get< std::string >("TrackModuleLabel");
    fTrackInstanceLabel = parameterSet.get< std::string >("TrackInstanceLabel");
    
    fTrackTag = art::InputTag(fTrackModuleLabel,fTrackInstanceLabel);
  }

  //-----------------------------------------------------------------------
  void ApplyField::produce(art::Event& event) 
  {
    // Grab the space charge service handle which sets up the intorpolation polynomial
    art::ServiceHandle<spacecharge::SpaceCharge> SpaceChargeHandle;
    
    // Initialize output track (with applied correction)
    std::unique_ptr< std::vector<recob::Track> > CorrectedTracksPointer(new std::vector<recob::Track>); 
    
    // This is the handle to the raw data of this event (simply a pointer to std::vector<raw::RawDigit>)
    art::ValidHandle< std::vector<recob::Track> > TrackVecHandle = event.getValidHandle< std::vector<recob::Track> >(fTrackTag);
   
    for(const auto& Track : *TrackVecHandle)
    {
      CorrectedTracksPointer->push_back(TrackCorrector(Track));
//       recob::Track CorrectedTrack( std::vector< TVector3 > const &  	xyz,
// 		std::vector< TVector3 > const &  	dxdydz,
// // 		std::vector< std::vector< double > >  	dQdx = std::vector< std::vector<double> >(0),
// 		std::vector< double >  	fitMomentum = std::vector<double>(2, util::kBogusD),
// 		int  	ID = -1 )

      
      
      
      
    }
    
    event.put(std::move(CorrectedTracksPointer),"FieldCorrection");
    
    
    // Prepairing the wire signal vector. It will be just the raw signal with subtracted pedestial
//     std::unique_ptr< std::vector<recob::Wire> > WireVec(new std::vector<recob::Wire>);
    
    // Fill Hits of all planes into the new
//     event.put(std::move(UHitVec), "UPlaneLaserHits");
//     event.put(std::move(VHitVec), "VPlaneLaserHits");
//     event.put(std::move(YHitVec), "YPlaneLaserHits");
  } // ApplyField::produce()
  
} // namespace ApplyField

namespace
{
  recob::Track TrackCorrector(const recob::Track& InputTrack)
  {
    // Initialize corrected track points and directions
    std::vector<TVector3> TrackPointsCorrected;
    std::vector<TVector3> TrackDirectionsCorrected;
    
    // Fill track points and apply correction
    for(unsigned int point_no = 0; point_no < InputTrack.NumberTrajectoryPoints(); point_no++)
    {
      TrackPointsCorrected.push_back(InputTrack.LocationAtPoint(point_no)/*+ Correction*/);
      TrackDirectionsCorrected.push_back(InputTrack.DirectionAtPoint(point_no)/* + Correction */);
    }
    
    // Create and return output track
    return recob::Track(TrackPointsCorrected,TrackDirectionsCorrected);
  }
}

#endif // ApplyField_Module
