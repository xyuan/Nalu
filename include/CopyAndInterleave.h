/*------------------------------------------------------------------------*/
/*  Copyright 2014 Sandia Corporation.                                    */
/*  This software is released under the license detailed                  */
/*  in the file, LICENSE, which is located in the top-level Nalu          */
/*  directory structure                                                   */
/*------------------------------------------------------------------------*/


#ifndef CopyAndInterleave_h
#define CopyAndInterleave_h

#include <ElemDataRequests.h>
#include <KokkosInterface.h>
#include <SimdInterface.h>
#include <ScratchViews.h>

namespace sierra{
namespace nalu{

template<typename DTYPE>
void interleave_3D(SharedMemView<DTYPE***>& dview, const SharedMemView<double***>& sview, int simdIndex)
{
    int dim0 = dview.dimension(0);
    int dim1 = dview.dimension(1);
    int dim2 = dview.dimension(2);

    for(int i=0; i<dim0; ++i) {
        for(int j=0; j<dim1; ++j) {
            for(int k=0; k<dim2; ++k) {
                stk::simd::set_data(dview(i,j,k), simdIndex, sview(i,j,k));
            }
        }
    }
}

template<typename DTYPE>
void interleave_2D(SharedMemView<DTYPE**>& dview, const SharedMemView<double**>& sview, int simdIndex)
{
    int dim0 = dview.dimension(0);
    int dim1 = dview.dimension(1);
    for(int i=0; i<dim0; ++i) {
        for(int j=0; j<dim1; ++j) {
            stk::simd::set_data(dview(i,j), simdIndex, sview(i,j));
        }
    }
}

template<typename DTYPE>
void interleave_1D(SharedMemView<DTYPE*>& dview, const SharedMemView<double*>& sview, int simdIndex)
{
    int dim = dview.dimension(0);

    for(int i=0; i<dim; ++i) {
        stk::simd::set_data(dview(i), simdIndex, sview(i));
    }
}

template<typename DTYPE>
void interleave_1D(SharedMemView<DTYPE*>& dview, const double* sviews[], int simdElems)
{
    int dim = dview.dimension(0);
    DoubleType* dptr = dview.data();
    for(int i=0; i<dim; ++i) {
        DoubleType& d = dptr[i];
        for(int simdIndex=0; simdIndex<simdElems; ++simdIndex) {
            stk::simd::set_data(d, simdIndex, sviews[simdIndex][i]);
        }
    }
}

template<typename DTYPE>
void interleave_2D(SharedMemView<DTYPE**>& dview, const double* sviews[], int simdElems)
{
    int len = dview.dimension(0)*dview.dimension(1);
    DoubleType* d = dview.data();
    for(int idx=0; idx<len; ++idx) {
        DoubleType& dv = d[idx];
        for(int simdIndex=0; simdIndex<simdElems; ++simdIndex) {
            stk::simd::set_data(dv, simdIndex, sviews[simdIndex][idx]);
        }
    }
}

template<typename DTYPE>
void interleave_3D(SharedMemView<DTYPE***>& dview, const double* sviews[], int simdElems)
{
    int len = dview.dimension(0)*dview.dimension(1)*dview.dimension(2);
    DoubleType* d = dview.data();
    for(int idx=0; idx<len; ++idx) {
        DoubleType& dv = d[idx];
        for(int simdIndex=0; simdIndex<simdElems; ++simdIndex) {
            stk::simd::set_data(dv, simdIndex, sviews[simdIndex][idx]);
        }
    }
}

inline
void interleave_1D(ViewHolder* dest, const ViewHolder* sviews[], int simdElems)
{
    const double* smemviews[stk::simd::ndoubles];
    SharedMemView<DoubleType*>& dmemview = static_cast<ViewT<SharedMemView<DoubleType*>>*>(dest)->view_;
    for(int i=0; i<simdElems; ++i) {
        smemviews[i] = static_cast<const ViewT<SharedMemView<double*>>*>(sviews[i])->view_.data();
    }

    interleave_1D(dmemview, smemviews, simdElems);
}

inline
void interleave_2D(ViewHolder* dest, const ViewHolder* sviews[], int simdElems)
{
    const double* smemviews[stk::simd::ndoubles];
    SharedMemView<DoubleType**>& dmemview = static_cast<ViewT<SharedMemView<DoubleType**>>*>(dest)->view_;
    for(int i=0; i<simdElems; ++i) {
        smemviews[i] = static_cast<const ViewT<SharedMemView<double**>>*>(sviews[i])->view_.data();
    }

    interleave_2D(dmemview, smemviews, simdElems);
}

inline
void interleave_3D(ViewHolder* dest, const ViewHolder* sviews[], int simdElems)
{
    const double* smemviews[stk::simd::ndoubles];
    SharedMemView<DoubleType***>& dmemview = static_cast<ViewT<SharedMemView<DoubleType***>>*>(dest)->view_;
    for(int i=0; i<simdElems; ++i) {
        smemviews[i] = static_cast<const ViewT<SharedMemView<double***>>*>(sviews[i])->view_.data();
    }

    interleave_3D(dmemview, smemviews, simdElems);
}

inline
void interleave_me_views(MasterElementViews<DoubleType>& dest,
                         const MasterElementViews<double>& src,
                         int simdIndex)
{
  interleave_2D(dest.scs_areav, src.scs_areav, simdIndex);
  interleave_3D(dest.dndx, src.dndx, simdIndex);
  interleave_3D(dest.dndx_shifted, src.dndx_shifted, simdIndex);
  interleave_3D(dest.dndx_fem, src.dndx_fem, simdIndex);
  interleave_3D(dest.deriv, src.deriv, simdIndex);
  interleave_3D(dest.deriv_fem, src.deriv_fem, simdIndex);
  interleave_1D(dest.det_j, src.det_j, simdIndex);
  interleave_1D(dest.det_j_fem, src.det_j_fem, simdIndex);
  interleave_1D(dest.scv_volume, src.scv_volume, simdIndex);
  interleave_3D(dest.gijUpper, src.gijUpper, simdIndex);
  interleave_3D(dest.gijLower, src.gijLower, simdIndex);
}

inline
void copy_and_interleave(const std::vector<ScratchViews<double>*>& data,
                         int simdElems,
                         ScratchViews<DoubleType>& simdData,
                         bool copyMEViews = true)
{
    const std::vector<ViewHolder*>& simdFieldViews = simdData.get_field_views();
    const ViewHolder* fViews[stk::simd::ndoubles] = {nullptr};

    for(size_t fieldViewsIndex=0; fieldViewsIndex<simdFieldViews.size(); ++fieldViewsIndex) {
      if (simdFieldViews[fieldViewsIndex] != nullptr) {
        for(int simdIndex=0; simdIndex<simdElems; ++simdIndex) {
          fViews[simdIndex] = data[simdIndex]->get_field_views()[fieldViewsIndex];
        }
        switch(simdFieldViews[fieldViewsIndex]->dim_) {
        case 1: interleave_1D(simdFieldViews[fieldViewsIndex], fViews, simdElems); break;
        case 2: interleave_2D(simdFieldViews[fieldViewsIndex], fViews, simdElems); break;
        case 3: interleave_3D(simdFieldViews[fieldViewsIndex], fViews, simdElems); break;
        default: ThrowRequireMsg(simdFieldViews[fieldViewsIndex]->dim_ > 0 &&
                                 simdFieldViews[fieldViewsIndex]->dim_ < 4,
                                 "ERROR, view dim out of range: "<<simdFieldViews[fieldViewsIndex]->dim_);
                 break;
        }
      }
    }

    if (copyMEViews)
    {
      for(int simdIndex=0; simdIndex<simdElems; ++simdIndex) {
        if (simdData.has_coord_field(CURRENT_COORDINATES)) {
          interleave_me_views(simdData.get_me_views(CURRENT_COORDINATES), data[simdIndex]->get_me_views(CURRENT_COORDINATES), simdIndex);
        }
        if (simdData.has_coord_field(MODEL_COORDINATES)) {
          interleave_me_views(simdData.get_me_views(MODEL_COORDINATES), data[simdIndex]->get_me_views(MODEL_COORDINATES), simdIndex);
        }
      }
    }
}

inline
void extract_vector_lane(const SharedMemView<DoubleType*>& simdrhs, int simdIndex, SharedMemView<double*>& rhs)
{
  int dim = simdrhs.dimension(0);
  const DoubleType* sr = simdrhs.data();
  double* r = rhs.data();
  for(int i=0; i<dim; ++i) {
    r[i] = stk::simd::get_data(sr[i], simdIndex);
  }
}

inline
void extract_vector_lane(const SharedMemView<DoubleType**>& simdlhs, int simdIndex, SharedMemView<double**>& lhs)
{
  int len = simdlhs.dimension(0)*simdlhs.dimension(1);
  const DoubleType* sl = simdlhs.data();
  double* l = lhs.data();
  for(int i=0; i<len; ++i) {
    l[i] = stk::simd::get_data(sl[i], simdIndex);
  }
}

} // namespace nalu
} // namespace Sierra

#endif
