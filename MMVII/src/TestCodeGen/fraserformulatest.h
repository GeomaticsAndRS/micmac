#ifndef FRASERFORMAULATEST_H
#define FRASERFORMAULATEST_H

#include <vector>
#include <string>

class cFraserCamColinearDef
{
public :
    static const int TheNbUk  = 19;
    static const int TheNbObs = 11;
    static const  std::vector<std::string> TheVNamesUnknowns;
    static const  std::vector<std::string> TheVNamesObs;
    cFraserCamColinearDef() : mVUk(TheNbUk,0.0), mVObs (TheNbObs,0.0) {}

protected:

       /// Return unknowns vect after fixing XYZ (ground point)
    const std::vector<double> & VUk(double X,double Y,double Z);
       /// Return observation vect t after fixing I,J (pixel projection)
    const std::vector<double> & VObs(double I,double J);

    std::vector<double>        mVUk;  /// Buffer for computing the unknown
    std::vector<double>        mVObs; /// Buffer for computing the unknown
};

/*  Capital letter for 3D variable/formulas and small for 2D */
template <class TypeUk,class TypeObs> std::vector<TypeUk> FraserCamColinearEq
                  (
                      const std::vector<TypeUk> & aVUk,
                      const std::vector<TypeObs> & aVObs
                  )
{
    // TODO : Exception ?
    if (aVUk.size() != cFraserCamColinearDef::TheNbUk)
        throw std::range_error("Fraser: Bad Unk size");

    if (aVObs.size() != cFraserCamColinearDef::TheNbObs)
        throw std::range_error("Fraser: Bad Obs size");

    // 0 - Ground Coordinates of projected point
    const auto & XGround = aVUk[0];
    const auto & YGround = aVUk[1];
    const auto & ZGround = aVUk[2];

    // 1 - Pose / External parameter
        // 1.1  Coordinate of camera center
    const auto & C_XCam = aVUk[3];
    const auto & C_YCam = aVUk[4];
    const auto & C_ZCam = aVUk[5];

        // 1.2  Coordinate of Omega vector coding the unknown "tiny" rotation
    const auto & Wx = aVUk[6];
    const auto & Wy = aVUk[7];
    const auto & Wz = aVUk[8];

    // 2 - Intrinsic parameters
         // 2.1 Principal point  and Focal
    const auto & xPP = aVUk[ 9];
    const auto & yPP = aVUk[10];
    const auto & zPP = aVUk[11]; // also named as focal

         // Also in this model we confond Principal point and distorsion center, name
         // explicitely the dist center case we change our mind
    const auto & xCD = xPP;
    const auto & yCD = yPP;

         // 2.2  Radial  distortions coefficients
    const auto & k2D = aVUk[12];
    const auto & k4D = aVUk[13];
    const auto & k6D = aVUk[14];

         // 2.3  Decentric distorstion
    const auto & p1 = aVUk[15];
    const auto & p2 = aVUk[16];

         // 2.3  Affine distorsion
    const auto & b1 = aVUk[17];
    const auto & b2 = aVUk[18];

   // Vector P->Cam
    auto  XPC = XGround-C_XCam;
    auto  YPC = YGround-C_YCam;
    auto  ZPC = ZGround-C_ZCam;


    // Coordinate of points in  camera coordinate system, do not integrate "tiny" rotation
    auto  XCam0 = aVObs[0] * XPC +  aVObs[1]* YPC +  aVObs[2]*ZPC;
    auto  YCam0 = aVObs[3] * XPC +  aVObs[4]* YPC +  aVObs[5]*ZPC;
    auto  ZCam0 = aVObs[6] * XPC +  aVObs[7]* YPC +  aVObs[8]*ZPC;


    // Now "tiny" rotation
    //  Wx      X      Wy * Z - Wz * Y
    //  Wy  ^   Y  =   Wz * X - Wx * Z
    //  Wz      Z      Wx * Y - Wy * X

     //  P =  P0 + W ^ P0

    auto  XCam = XCam0 + Wy * ZCam0 - Wz * YCam0;
    auto  YCam = YCam0 + Wz * XCam0 - Wx * ZCam0;
    auto  ZCam = ZCam0 + Wx * YCam0 - Wy * XCam0;

    // Projection :  (xPi,yPi,1) is the bundle direction in camera coordinates

    auto xPi =  XCam/ZCam;
    auto yPi =  YCam/ZCam;

    // Coordinate relative to distorsion center
    auto xC =  xPi-xCD;
    auto yC =  yPi-yCD;
    auto x2C = square(xC);  // Use the indermediar value to (probably) optimize Jet
    auto y2C = square(yC);
    auto xyC = xC * yC;
    auto Rho2C = x2C + y2C;

   // Compute the distorsion
    auto rDist = k2D*Rho2C + k4D * square(Rho2C) + k6D*cube(Rho2C);
    auto affDist = b1 * xC + b2 * yC;
    auto decX = p1*(3.0*x2C + y2C) +  p2*(2.0*xyC);
    auto decY = p2*(3.0*y2C + x2C) +  p1*(2.0*xyC);

    auto xDist =  xPi + xC * rDist + decX + affDist;
    auto yDist =  yPi + yC * rDist + decY ;

   // Use principal point and focal
    auto xIm =  xPP  + zPP  * xDist;
    auto yIm =  yPP  + zPP  * yDist;

    auto x_Residual = xIm -  aVObs[ 9];
    auto y_Residual = yIm -  aVObs[10];

    return {x_Residual,y_Residual};
}



const std::vector<double> & cFraserCamColinearDef::VUk(double X,double Y,double Z)
{
   mVUk[0] = X;
   mVUk[1] = Y;
   mVUk[2] = Z;

   return mVUk;
}

const std::vector<double> & cFraserCamColinearDef::VObs(double I,double J)
{
     mVObs[ 9] = I;
     mVObs[10] = J;

    return mVObs;
}

const std::vector<std::string>
  cFraserCamColinearDef::TheVNamesUnknowns
  {
      "XGround","YGround","ZGround",            // Unknown 3D Point
      "XCam","YCam","ZCam", "Wx","Wy","Wz",     // External Parameters
      "ppX","ppY","ppZ",                        // Internal : principal point + focal
      "k2","k4","k6", "p1","p2","b1","b2"       // Distorsion (radiale/ Decentric/Affine)
  };

const std::vector<std::string>
  cFraserCamColinearDef::TheVNamesObs
  {
        "oR00","oR01","oR02","oR10","oR11","oR12","oR20","oR21","oR22",
        "oXIm","oYIm"
  };


#endif // FRASERFORMAULATEST_H
