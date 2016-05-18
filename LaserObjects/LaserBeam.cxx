#include "LaserBeam.h"

lasercal::LaserBeam::LaserBeam()
{
}

lasercal::LaserBeam::LaserBeam(const TVector3& LaserPosition, const TVector3& LaserDirection) : fLaserPosition(LaserPosition), fDirection(LaserDirection)
{
     // Calculate the intersection points with the TPC. This has to be done after SetPosition and SetDirection!
    SetIntersectionPoints();
}

lasercal::LaserBeam::LaserBeam(const TVector3& LaserPosition, const float& Phi, const float& Theta)
{
    TVector3 LaserDir;          // 3-Vector in laser coordinate system
    TVector3 LaserDirection;    // 3-Vector in uboone coordinate system 
    
    // Transform from measured angles to uboone coordinate system.
    LaserDir.SetMagThetaPhi(1., Theta, Phi);
    LaserDirection.SetX(LaserDir.Y());
    LaserDirection.SetY(LaserDir.Z());
    LaserDirection.SetZ(LaserDir.X());
    
    // Set position and direction
    SetPosition(LaserPosition);
    SetDirection(LaserDirection);
    
    // Calculate the intersection points with the TPC. This has to be done after SetPosition and SetDirection!
    SetIntersectionPoints();
    
}

void lasercal::LaserBeam::SetPosition(const TVector3& LaserPosition)
{
    fLaserPosition = LaserPosition;
}

void lasercal::LaserBeam::SetDirection(const TVector3& LaserDirection)
{
    fDirection = LaserDirection;
}
void lasercal::LaserBeam::SetPower(const float& AttenuatorPercentage) 
{
    fPower = AttenuatorPercentage;
}

void lasercal::LaserBeam::SetTime(const float& sec, const float& usec) 
{
    fTime.sec = (unsigned long) sec;
    fTime.usec = (unsigned long) usec;
}

// TODO: Fix class compilation if including GeometryCore.h and others!!!
void lasercal::LaserBeam::SetIntersectionPoints()
{   
    // Load geometry core
//     geo::GeometryCore const* Geometry = &*(art::ServiceHandle<geo::Geometry>());
    
//     // Get the TPCGeo volume and its intersection points
//     auto IntersectionPoints = Geometry->TPC().GetIntersections(fLaserPosition, fDirection);
// 
//     // Check if there is only an exit point (if laser head is in TPC volume)
//     if(IntersectionPoints.size() == 1)
//     {
// 	fEntryPoint = TVector3(-9999,-9999,-9999);
// 	fExitPoint = IntersectionPoints.back();
//     }
//     else
//     {
// 	fEntryPoint = IntersectionPoints.front();
// 	fExitPoint = IntersectionPoints.back();
//     }
}

void lasercal::LaserBeam::Print() const 
{
    std::cout   << "Laser System        " << fLaserID << "\n"
                << "Laser Event ID      " << fLaserEventID << "\n"
                << "Associate Event ID  " << fAssosiateEventID << "\n"
                << "Attenuator Position " << fPower * 100 << " %\n"
                << "Aperture Position   " << fAperturePosition << "\n"
                << "Mirror Position     ";// << std::endl; 
    fLaserPosition.Print();
    std::cout    << "Mirror Direction    ";// << std::endl;
    fDirection.Print();
    std::cout << "\n" << std::endl;
}

TVector3 lasercal::LaserBeam::GetEntryPoint() const
{
  return fEntryPoint;
}

TVector3 lasercal::LaserBeam::GetExitPoint() const
{
  return fExitPoint;
}