/*------------------------------------------------------------------------*/
/*  Copyright 2014 Sandia Corporation.                                    */
/*  This software is released under the license detailed                  */
/*  in the file, LICENSE, which is located in the top-level Nalu          */
/*  directory structure                                                   */
/*------------------------------------------------------------------------*/


#ifndef NonConformalInfo_h
#define NonConformalInfo_h

//==============================================================================
// Includes and forwards
//==============================================================================

#include <master_element/MasterElement.h>

// stk
#include <stk_mesh/base/Part.hpp>
#include <stk_mesh/base/Ghosting.hpp>

#include <stk_search/BoundingBox.hpp>
#include <stk_search/IdentProc.hpp>
#include <stk_search/SearchMethod.hpp>

#include <vector>
#include <map>

namespace stk {
namespace mesh {
typedef std::vector<Part *> PartVector;
}
}

namespace sierra {
namespace nalu {

class Realm;
class DgInfo;

typedef stk::search::IdentProc<uint64_t,int>  theKey;
typedef stk::search::Point<double> Point;
typedef stk::search::Box<double> Box;
typedef std::pair<Point,theKey> boundingPoint;
typedef std::pair<Box,theKey> boundingElementBox;

//=============================================================================
// Class Definition
//=============================================================================
// NonConformalInfo
//=============================================================================
/**
 * * @par Description:
 * - class to manage dg information.
 *
 * @par Design Considerations:
 * -
 */
//=============================================================================
class NonConformalInfo {

 public:

  // constructor and destructor
  NonConformalInfo(
    Realm & realm,
    const stk::mesh::PartVector currentPartVec,
    const stk::mesh::PartVector opposingPartVec,
    const double expandBoxPercentage,
    const std::string &searchMethodName,
    const bool clipIsoParametricCoords,
    const double searchTolerance,
    const std::string debugName);

  ~NonConformalInfo();

  /* delete dgInfoVec_ */
  void delete_dgInfo();

  /* perform initialization such as dgInfoVec creation and search point/boxes */
  void initialize();
  
  /* perform the 'new' */
  void construct_dgInfo();

  void reset_dgInfo();
  void construct_bounding_points();
  void construct_bounding_boxes();
  void determine_elems_to_ghost();
  void complete_search();
  void provide_diagnosis();
  size_t error_check();

  Realm &realm_;
  const std::string name_;

  // master slave parts; slave part can be subsetted while master is not..
  const stk::mesh::PartVector currentPartVec_;
  const stk::mesh::PartVector opposingPartVec_;

  /* expand search box */
  double expandBoxPercentage_;

  stk::search::SearchMethod searchMethod_;

  /* clip isoparametric coordinates if they are out of bounds */
  const bool clipIsoParametricCoords_;

  /* allow for some finite search tolereance for bounding box */
  const double searchTolerance_;

  /* does the realm have mesh motion */
  const bool meshMotion_;

  /* can we possibly reuse */
  bool canReuse_;

  /* bounding box data types for stk_search */
  std::vector<boundingPoint>      boundingPointVec_;
  std::vector<boundingElementBox> boundingFaceElementBoxVec_;

  /* vector of DgInfo */
  std::vector<std::vector<DgInfo *> > dgInfoVec_;

  /* save off product of search */
  std::vector<std::pair<theKey, theKey> > searchKeyPair_;

};

} // end sierra namespace
} // end Acon namespace

#endif
