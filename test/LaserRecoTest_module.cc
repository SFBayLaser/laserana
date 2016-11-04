////////////////////////////////////////////////////////////////////////
// Class:       LaserRecoTest
// Module Type: analyzer
// File:        LaserRecoTest_module.cc
//
// Generated at Fri Oct 28 13:49:04 2016 by matthias,,, using artmod
// from cetpkgsupport v1_10_01.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "LaserObjects/LaserHits.h"


class LaserRecoTest;

class LaserRecoTest : public art::EDAnalyzer {
public:
    explicit LaserRecoTest(fhicl::ParameterSet const &p);
    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    LaserRecoTest(LaserRecoTest const &) = delete;

    LaserRecoTest(LaserRecoTest &&) = delete;

    LaserRecoTest &operator=(LaserRecoTest const &) = delete;

    LaserRecoTest &operator=(LaserRecoTest &&) = delete;

    // Required functions.
    void analyze(art::Event const &e) override;

    void reconfigure(fhicl::ParameterSet const &pset) override;


private:

    std::string fHitModul, fHitLabel;
    std::string fTestConfigFile;
};


LaserRecoTest::LaserRecoTest(fhicl::ParameterSet const &pset)
        :
        EDAnalyzer(pset)  // ,
// More initializers here.
{
    this->reconfigure(pset);
}

void LaserRecoTest::analyze(art::Event const &event) {
    auto id = event.id().event();
    auto DigitTag = art::InputTag(fHitModul, fHitLabel );

    if (fTestConfigFile.compare("HitDefs-10000.txt") == 0) {

        std::cout << "==> Testing Single Track Reco" << std::endl;
        try {
            art::ValidHandle<std::vector<recob::Hit> > LaserHits = event.getValidHandle<std::vector<recob::Hit>>(DigitTag);
        }
        catch (...)
        {
            assert(false && "Event does not contain any hits");
        }
        switch(id) {
            case 0:

                break;
        }

        //for (auto const &hit : *LaserHits){
        //    std::cout << hit.PeakTime() << std::endl;
        //}

    }
}

void LaserRecoTest::reconfigure(fhicl::ParameterSet const &pset) {
    fHitModul = pset.get<std::string>("HitModul");
    fHitLabel = pset.get<std::string>("HitLabel");
    fTestConfigFile = pset.get<std::string>("TestConfigFile");
}


DEFINE_ART_MODULE(LaserRecoTest)
