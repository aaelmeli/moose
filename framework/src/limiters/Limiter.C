//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MooseEnum.h"
#include "VanLeerLimiter.h"
#include "UpwindLimiter.h"
#include "CentralDifferenceLimiter.h"
#include "MinModLimiter.h"
#include "SOULimiter.h"
#include "QUICKLimiter.h"
#include "MooseError.h"

#include "libmesh/auto_ptr.h"

namespace Moose
{
namespace FV
{
const MooseEnum
    moose_limiter_type("vanLeer=0 upwind=1 central_difference=2 min_mod=3 sou=4 quick=5", "upwind");

template <typename T>
std::unique_ptr<Limiter<T>>
Limiter<T>::build(const LimiterType limiter)
{
  switch (limiter)
  {
    case LimiterType::VanLeer:
      return libmesh_make_unique<VanLeerLimiter<T>>();

    case LimiterType::Upwind:
      return libmesh_make_unique<UpwindLimiter<T>>();

    case LimiterType::CentralDifference:
      return libmesh_make_unique<CentralDifferenceLimiter<T>>();

    case LimiterType::MinMod:
      return libmesh_make_unique<MinModLimiter<T>>();

    case LimiterType::SOU:
      return libmesh_make_unique<SOULimiter<T>>();

    case LimiterType::QUICK:
      return libmesh_make_unique<QUICKLimiter<T>>();

    default:
      mooseError("Unrecognized limiter type ", unsigned(limiter));
  }
}

// instantiations we care about
template std::unique_ptr<Limiter<Real>> Limiter<Real>::build(const LimiterType limiter);
template std::unique_ptr<Limiter<ADReal>> Limiter<ADReal>::build(const LimiterType limiter);
}
}
